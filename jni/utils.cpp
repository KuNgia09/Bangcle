#include "utils.h"
#include <jni.h>
#include <dlfcn.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "elfGotHook/logger.h"


unsigned int search_symbol_fromelf(char *lib_path, unsigned int lib_base_addr, char *target)
{
    if (!lib_base_addr) {
        return 0;
    }

    int fd = open(lib_path, O_RDONLY);
    if (!fd) {
        return 0;
    }

    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    int buffer = (int)mmap(0, size, 1, 2, fd, 0);
    if (!buffer) {
        close(fd);
        return 0;
    }
    short e_phnum = *(short *)(buffer + 0x2C);
    LOGD("[+]%s->e_phnum:%d", lib_path, e_phnum);
    int          program_table     = buffer + 0x34;
    int          program_table_end = program_table + 0x20 * e_phnum;
    unsigned int ptr          = program_table;
    unsigned int pt_load_base = 0xfffffff;
    bool         find_pt_load = false;
    unsigned int result       = 0;

#define PT_LOAD       1
#define PT_DYNAMIC    2
#define DT_STRTAB     5
#define DT_SYMTAB     6
#define DT_STRSZ      10
    do {
        if (*(int *)ptr != PT_LOAD) {
            ptr += 0x20;
            continue;
        }
        find_pt_load = 1;
        if (pt_load_base > *(unsigned int *)(ptr + 8)) {
            pt_load_base = *(unsigned int *)(ptr + 8);
        }
        ptr += 0x20;
    } while (ptr != program_table_end);

    if (find_pt_load) {
        if (pt_load_base) {
            pt_load_base = pt_load_base & 0xfffff000;
        }
        LOGD("[+]pt_load p_vaddr:%08x", pt_load_base);
    } else {
        return 0;
    }
    ptr = program_table;
    unsigned int d_ptr;
    unsigned int strtab_addr = 0;
    unsigned int symtab_addr = 0;
    unsigned int strsz_addr  = 0;
    do {
        if (*(int *)ptr == PT_DYNAMIC) {
            unsigned int pt_dynamic_vaddr   = *(unsigned int *)(ptr + 8);
            unsigned int file_to_mem_offset = lib_base_addr - pt_load_base;
            // b3d9c000
            unsigned int dynamic_addr = file_to_mem_offset + pt_dynamic_vaddr;// 我的result:b4205ae4
            LOGD("[+]pt_dynamic_vaddr:%08x,lib_base_addr:%08x,pt_load_based:%08x,ynamic_addr:%08x", pt_dynamic_vaddr,
                 lib_base_addr, pt_load_base, dynamic_addr);
            if (dynamic_addr) {
                int d_tag = *(int *)dynamic_addr;
                int *next = (int *)(dynamic_addr + 8);
                do {
                    switch (d_tag) {
                      case DT_SYMTAB:
                          symtab_addr = file_to_mem_offset + *(next - 1);
                          // LOGD("[+]symtab_addr offset:%08x",*(next - 1));//0xB1C0
                          break;

                      case DT_STRSZ:
                          strsz_addr = *(next - 1);
                          // LOGD("[+]strsz_addr offset:%08x",*(next - 1));//0x61797
                          break;

                      case DT_STRTAB:
                          strtab_addr = file_to_mem_offset + *(next - 1);   // 0x22f00
                          // LOGD("[+]strtab_addr offset:%08x",*(next - 1));
                          break;
                    }
                    d_tag = *next;
                    next += 2;
                } while (d_tag);
                LOGD("[+]find strtab:%08x,symtab,%08x", strtab_addr, symtab_addr);
                int p_sym = symtab_addr;
                while (memcmp((char *)(strtab_addr + *(int *)(p_sym)), target, strlen(target)) != 0) {
                    if ((unsigned int)p_sym >= strtab_addr) {
                        LOGE("[-]Unexcepted symtab>=strtab");
                        result = 0;
                        goto label;
                    }
                    p_sym += 16;
                }
                result = file_to_mem_offset + *(unsigned int *)(p_sym + 4);
                LOGD("[+]get %s addr:%08x", target, result);
                break;
            }
        }
        ptr += 0x20;
    } while ((int)ptr != program_table_end);

label:
    munmap((void *)buffer, (size_t)size);
    close(fd);
    return result;
} // search_symbol_fromelf


void *get_addr_symbol(char *module_name, char *target_symbol)
{
    void *module_base = get_module_base(-1, module_name);

    if (!module_base) {
        LOGE("[-]get module %s base failed", module_name);
        return 0;
    }
    void *result = (void *)search_symbol_fromelf(module_name, (unsigned int)module_base, target_symbol);
    if (!result) {
        LOGE("[-]search symbol %s from %s failed", target_symbol, module_name);
        return NULL;
    }
    return result;
}


int extract_file(JNIEnv *env, jobject ctx, const char *dir, const char *szDexPath, const char *fileName)
{
    if (access(dir, F_OK) != 0) {
        mkdir(dir, 505);
        chmod(dir, 505);
    }

    if (access(szDexPath, F_OK) == 0) {
        LOGD("[+]File %s have existed", szDexPath);
        return 0;
    }
    // jiami.dat不存在，开始提取
    else{
        AAssetManager *mgr;
        jclass        ApplicationClass = env->GetObjectClass(ctx);
        jmethodID     getAssets        =
            env->GetMethodID(ApplicationClass, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject Assets_obj = env->CallObjectMethod(ctx, getAssets);
        mgr = AAssetManager_fromJava(env, Assets_obj);
        if (mgr == NULL) {
            LOGE("[-]getAAssetManager failed");
            return 0;
        }
        AAsset *asset     = AAssetManager_open(mgr, fileName, AASSET_MODE_STREAMING);
        FILE   *file      = fopen(szDexPath, "wb");
        int    bufferSize = AAsset_getLength(asset);
        LOGD("[+]Asset FileName:%s,extract path:%s,size:%d\n", fileName, szDexPath, bufferSize);
        void *buffer = malloc(4096);
        while (true) {
            int numBytesRead = AAsset_read(asset, buffer, 4096);
            if (numBytesRead <= 0) {
                break;
            }
            fwrite(buffer, numBytesRead, 1, file);
        }
        free(buffer);
        fclose(file);
        AAsset_close(asset);
        chmod(szDexPath, 493);
    }
} // extract_file


void *get_module_base(pid_t pid, const char *module_name)
{
    FILE *fp;
    long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];

    if (pid < 0) {
        /* self process */
        snprintf(filename, sizeof(filename), "/proc/self/maps", pid);
    } else {
        snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
    }

    fp = fopen(filename, "r");

    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, module_name)) {
                pch  = strtok(line, "-");
                addr = strtoull(pch, NULL, 16);

                if (addr == 0x8000) {
                    addr = 0;
                }

                break;
            }
        }

        fclose(fp);
    }

    return (void *)addr;
} // get_module_base
