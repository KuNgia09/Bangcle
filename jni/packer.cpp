#include "packer.h"
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <errno.h>
#include <assert.h>
#include <sys/system_properties.h>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <vector>
#include <bits/unique_ptr.h>


#include "common.h"
#include "dex_header.h"
#include "utils.h"
#include "byte_load.h"
#include "hook_instance.h"
#include "elfGotHook/tools.h"
#include "elfGotHook/elf_reader.h"

#define PAGE_MASK    0xfffff000
#define PAGE_SIZE    4096
#define PAGE_START(x)     ((x) & PAGE_MASK)
#define PAGE_OFFSET(x)    ((x) & ~PAGE_MASK)
#define PAGE_END(x)       PAGE_START((x) + (PAGE_SIZE - 1))

using namespace std;

#define LIB_ART_PATH    "/system/lib/libart.so"
#define JIAMI_MAGIC     "jiami.dat"


bool       g_isArt   = false;
int        g_sdk_int = 0;
const char *g_file_dir;
const char *g_NativeLibDir;
char       *g_PackageResourcePath;
char       *g_pkgName;
int        g_dexSize            = 0; // main.dex的真正大小
int        g_pageSize           = 0; // main.dex进行页面对齐后的大小
char       g_fake_dex_path[256] = { 0 };
void       *g_maindex_base      = NULL;
void       *g_ArtHandle         = NULL;


void native_attachBaseContext(JNIEnv *env, jobject obj, jobject ctx);

unsigned char MINIDEX[292] = {
    0x64, 0x65, 0x78, 0x0A, 0x30, 0x33, 0x35, 0x00, 0xD9, 0x24, 0x14, 0xFD, 0x2F, 0x81, 0x4D, 0x8B,
    0x50, 0x48, 0x13, 0x1D, 0x8D, 0xA9, 0xCF, 0x1F, 0xF1, 0xF2, 0xDD, 0x06, 0xB4, 0x67, 0x70, 0xA1,
    0x24, 0x01, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xD8, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x00,
    0xA4, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0E, 0x4C, 0x63, 0x6F, 0x6D, 0x2F, 0x6D, 0x69, 0x78, 0x43, 0x6C, 0x61,
    0x73, 0x73, 0x3B, 0x00, 0x12, 0x4C, 0x6A, 0x61, 0x76, 0x61, 0x2F, 0x6C, 0x61, 0x6E, 0x67, 0x2F,
    0x4F, 0x62, 0x6A, 0x65, 0x63, 0x74, 0x3B, 0x00, 0x0D, 0x6D, 0x69, 0x78, 0x43, 0x6C, 0x61, 0x73,
    0x73, 0x2E, 0x6A, 0x61, 0x76, 0x61, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
    0x70, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0xD8, 0x00, 0x00, 0x00
};

void writemixdex(const char *minidex)
{
    // If the file exists,skip
    if (access(minidex, F_OK) == -1) {
        FILE *file = fopen(minidex, "wb");
        fwrite(MINIDEX, 292, 1, file);
        fclose(file);
    }
}


static JNINativeMethod methods[] = {
    { "attachBaseContext", "(Landroid/content/Context;)V", (void *)native_attachBaseContext },
};

int jniRegisterNativeMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethods, int numMethods)
{
    jclass clazz;
    int    tmp;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return -1;
    }
    if ((tmp = env->RegisterNatives(clazz, gMethods, numMethods)) < 0) {
        LOGE("[-]RegisterNatives failed");
        return -1;
    }
    return 0;
}


int registerNativeMethods(JNIEnv *env)
{
    // return jniRegisterNativeMethods(env, "com/storm/StubApplication/Native", methods, sizeof(methods) / sizeof(methods[0]));
    return jniRegisterNativeMethods(env, "com/storm/fengyue/Native", methods, sizeof(methods) / sizeof(methods[0]));
}


int lookup(JNINativeMethod *table, const char *name, const char *sig, void **fnPtrout)
{
    int i = 0;

    while (table[i].name != NULL) {
        // LOGD("[+]lookup %d %s" ,i,table[i].name);
        if ((strcmp(name, table[i].name) == 0) &&
            (strcmp(sig, table[i].signature) == 0)) {
            *fnPtrout = table[i].fnPtr;
            return 1;
        }
        i++;
    }

    return 0;
}


char *mmap_dex(const char *szDexPath)
{
    int         fd;
    struct stat buf = {
        0
    };

    fd = open(szDexPath, 0);
    if (!fd) {
        LOGE("[-]open %s failed", szDexPath);
        return NULL;
    }
    int status = stat(szDexPath, &buf);
    if (status == -1) {
        LOGE("[-]fstat %s failed", szDexPath);
        return NULL;
    }

    // 设置页面对齐
    g_dexSize  = buf.st_size;
    g_pageSize = PAGE_END(buf.st_size);

    char *dexBase = (char *)mmap(0, g_dexSize, 3, 2, fd, 0);
    close(fd);
    return dexBase;
}


jstring new_obj_string(JNIEnv *env, const char *path)
{
    int    len          = strlen(path);
    jclass string_clazz = env->FindClass("java/lang/String");

    if (!env->ExceptionCheck() && string_clazz) {
        jmethodID init = env->GetMethodID(string_clazz, "<init>", "([BLjava/lang/String;)V");
        if (!env->ExceptionCheck() && init) {
            jbyteArray array = env->NewByteArray(len);
            env->SetByteArrayRegion(array, 0, len, (jbyte *)path);
            jstring utf = env->NewStringUTF("utf-8");
            return (jstring)env->NewObject(string_clazz, init, array, utf);
        } else {
            LOGE("[-]find <init> method failed");
            return NULL;
        }
    } else {
        LOGE("[-]find java/lang/String class failed");
        return NULL;
    }
}


jint mem_loadDex_dvm(JNIEnv *env, char *szPath)
{
    void (*openDexFile)(const u4 *args, union  JValue *pResult);
    void *ldvm = (void *)dlopen("libdvm.so", 1);
    if (!ldvm) {
        LOGE("[-]could not dlopen dvm:%s", dlerror());
    }
    JNINativeMethod *dvm_dalvik_system_DexFile = (JNINativeMethod *)dlsym(ldvm, "dvm_dalvik_system_DexFile");
    if (0 == lookup(dvm_dalvik_system_DexFile, "openDexFile", "([B)I", (void **)&openDexFile)) {
        LOGE("[-]dvm_dalvik_system_DexFile method does not found ");
        return 0;
    }

    char *arr;
    arr = (char *)malloc(16 + g_dexSize);
    ArrayObject *ao = (ArrayObject *)arr;
    // LOGD("sizeof ArrayObject:%d",sizeof(ArrayObject));
    ao->length = g_dexSize;
    memcpy(arr + 16, (char *)g_maindex_base, g_dexSize);
    munmap((char *)g_maindex_base, g_dexSize);

    u4           args[] = { (u4)ao };
    union JValue pResult;
    jint         result;
    if (openDexFile != NULL) {
        openDexFile(args, &pResult);
        jint mCookie = (jint)pResult.l;
        return mCookie;
    } else {
        LOGE("[-]cannot get dvm_dalvik_system_DexFile addr");
        return 0;
    }
}


void make_dex_elements(JNIEnv *env, jobject classLoader, jobject dexFileobj)
{
    jclass PathClassLoader    = env->GetObjectClass(classLoader);
    jclass BaseDexClassLoader = env->GetSuperclass(PathClassLoader);
    // get pathList fieldid
    jfieldID pathListid = env->GetFieldID(BaseDexClassLoader, "pathList", "Ldalvik/system/DexPathList;");
    jobject  pathList   = env->GetObjectField(classLoader, pathListid);

    // get DexPathList Class
    jclass DexPathListClass = env->GetObjectClass(pathList);
    // get dexElements fieldid
    jfieldID dexElementsid = env->GetFieldID(DexPathListClass, "dexElements", "[Ldalvik/system/DexPathList$Element;");

    // ��ȡelements���� get dexElement array value
    jobjectArray dexElement = static_cast<jobjectArray>(env->GetObjectField(pathList, dexElementsid));


    jint len = env->GetArrayLength(dexElement);

    LOGD("[+]Elements size:%d", len);

    jclass    ElementClass = env->FindClass("dalvik/system/DexPathList$Element"); // dalvik/system/DexPathList$Element
    jmethodID Elementinit  = env->GetMethodID(ElementClass, "<init>",
                                              "(Ljava/io/File;ZLjava/io/File;Ldalvik/system/DexFile;)V");
    jboolean isDirectory = JNI_FALSE;

    // ����һ���µ�dalvik/system/DexPathList$Element�� dexFileobjΪ�µ�dexFileobj
    jobject element_obj = env->NewObject(ElementClass, Elementinit, NULL, isDirectory, NULL, dexFileobj);

    // Get dexElement all values and add  add each value to the new array
    jobjectArray new_dexElement = env->NewObjectArray(len + 1, ElementClass, NULL);
    for (int i = 0; i < len; ++i) {
        env->SetObjectArrayElement(new_dexElement, i, env->GetObjectArrayElement(dexElement, i));
    }

    env->SetObjectArrayElement(new_dexElement, len, element_obj);
    env->SetObjectField(pathList, dexElementsid, new_dexElement);

    env->DeleteLocalRef(element_obj);
    env->DeleteLocalRef(ElementClass);
    env->DeleteLocalRef(dexElement);
    env->DeleteLocalRef(DexPathListClass);
    env->DeleteLocalRef(pathList);
    env->DeleteLocalRef(BaseDexClassLoader);
    env->DeleteLocalRef(PathClassLoader);
} // make_dex_elements


// For Nougat
void replace_cookie_N(JNIEnv *env, jobject minidexfileobj, jlong value)
{
    jclass   DexFileClass = env->FindClass("dalvik/system/DexFile"); // "dalvik/system/DexPathList$Element"
    jfieldID field_mCookie;
    jobject  mCookie;

    field_mCookie = env->GetFieldID(DexFileClass, "mCookie", "Ljava/lang/Object;");
    mCookie       = env->GetObjectField(minidexfileobj, field_mCookie);

    jboolean is_data_copy = 1;
    jsize    arraylen     = env->GetArrayLength((jarray)mCookie);
    LOGD("[+]g_sdk_int:%d,cookie arrayLen:%d", g_sdk_int, arraylen);

    jlong *mix_element = env->GetLongArrayElements((jlongArray)mCookie, &is_data_copy);
    jlong *dex_info    = (jlong *)value;

    jlong *tmp = mix_element;
    *(tmp + 1) = value;
    // 更新mCookie
    env->ReleaseLongArrayElements((jlongArray)mCookie, mix_element, 0);
    if (env->ExceptionCheck()) {
        LOGE("[-]g_sdk_int:%d Update cookie failed", g_sdk_int);
        return;
    }

    jlong *second_mix_element = env->GetLongArrayElements((jlongArray)mCookie, &is_data_copy);
    jlong ptr     = *(second_mix_element + 1);
    int   dexbase = *(int *)(ptr + 4);
    int   dexsize = *(int *)(ptr + 8);
    LOGD("[+]Nougat after replace cookie dex base:%x,dexsize:%x", dexbase, dexsize);
}


void replace_cookie_M(JNIEnv *env, jobject minidexfileobj, jlong value)
{
    jclass   DexFileClass = env->FindClass("dalvik/system/DexFile"); // "dalvik/system/DexPathList$Element"
    jfieldID field_mCookie;
    jobject  mCookie;

    field_mCookie = env->GetFieldID(DexFileClass, "mCookie", "Ljava/lang/Object;");
    mCookie       = env->GetObjectField(minidexfileobj, field_mCookie);

    jboolean is_data_copy = 1;
    jsize    arraylen     = env->GetArrayLength((jarray)mCookie);
    LOGD("[+]g_sdk_int:%d,cookie arrayLen:%d", g_sdk_int, arraylen);

    jlong *mix_element = env->GetLongArrayElements((jlongArray)mCookie, &is_data_copy);

    // 这里的mix_element 等价于openMemory的返回值
    int ptr     = *(int *)mix_element;
    int dexbase = *(int *)(ptr + 4);
    int dexsize = *(int *)(ptr + 8);
    LOGD("[+]mini dex array len :%d,dex magic:%x,dexsize:%x", arraylen, *(int *)dexbase, dexsize);
    // very important
    // jlong* tmp=mix_element;
    // *tmp=*dex_info;
    *mix_element = value;
    // 更新mCookie
    env->ReleaseLongArrayElements((jlongArray)mCookie, mix_element, 0);
    if (env->ExceptionCheck()) {
        LOGE("[-]g_sdk_int:%d Update cookie failed", g_sdk_int);
        return;
    }
    jlong *second_mix_element = env->GetLongArrayElements((jlongArray)mCookie, &is_data_copy);
    ptr     = *(int *)second_mix_element;
    dexbase = *(int *)(ptr + 4);
    dexsize = *(int *)(ptr + 8);
    LOGD("[+]second minidex dex dex magic:%x,dexsize:%x", *(int *)dexbase, dexsize);
    LOGD("[+]replace_cookie_one successful");
} // replace_cookie_M


char *get_path_frommaps(const char *pkgName, char *filter1, char *filter2)
{
    int  pid      = getpid();
    char map[256] = { 0 };

    sprintf(map, "/proc/%d/maps", pid);
    FILE *fd = fopen(map, "r");
    if (!fd) {
        LOGE("[-]open %s failed", map);
        return NULL;
    }

    while (true) {
        char line_buffer[256] = { 0 };
        if (!fgets(line_buffer, 255, fd)) {
            break;
        }
        // 寻找带有包名和.dex或.odex的行
        if (strstr(line_buffer, pkgName) && (strstr(line_buffer, filter1) || strstr(line_buffer, filter2))) {
            LOGD("[+]targt line buffer:%s", line_buffer);
            char *p = strchr(line_buffer, '/');
            // 获取需要复制的长度
            // line_buffer结尾是一个换行符
            int copy_len = strlen(line_buffer) - (p - (char *)line_buffer) - 1;

            memcpy((void *)g_fake_dex_path, p, copy_len);
            LOGD("[+]find dex path:%s from maps", g_fake_dex_path);
            return g_fake_dex_path;
        }
    }
    fclose(fd);
    return NULL;
}


jobject load_dex_fromfile(JNIEnv *env, const char *inPath, const char *outPath)
{
    jclass DexFileClass = env->FindClass("dalvik/system/DexFile"); // "dalvik/system/DexPathList$Element"
    // new DexFile==loadDex
    // loadDex方法比<init>方法通用性更好
    // jmethodID init = env->GetMethodID(DexFileClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;I)V");
    jmethodID init = env->GetStaticMethodID(DexFileClass, "loadDex",
                                            "(Ljava/lang/String;Ljava/lang/String;I)Ldalvik/system/DexFile;");

    if (env->ExceptionCheck()) {
        LOGE("[-]get loadDex methodID  error");
        return 0;
    }
    jstring apk  = env->NewStringUTF(inPath);
    jstring odex = env->NewStringUTF(outPath);

    jobject dexobj = env->CallStaticObjectMethod(DexFileClass, init, apk, odex, 0);
    if (env->ExceptionCheck()) {
        LOGE("[-]loadDex %s dex failed", inPath);
        return 0;
    }

    env->DeleteLocalRef(DexFileClass);
    env->DeleteLocalRef(apk);
    env->DeleteLocalRef(odex);

    // try delete mixdexobj localRef
    // env->DeleteLocalRef(mixdexobj);
    return dexobj;
}


void *get_lib_handle(const char *lib_path)
{
    void *handle_art = dlopen(lib_path, RTLD_NOW);

    if (!handle_art) {
        LOGE("[-]get %s handle failed:%s", dlerror());
        return NULL;
    }
    return handle_art;
}


void mem_loadDex(JNIEnv *env, jobject ctx, const char *szDexPath)
{
    char     inPath[256]  = { 0 };
    char     outPath[256] = { 0 };
    jobject  mini_dex_obj;
    void     *dex_info = NULL;
    jfieldID cookie_field;

    jclass    ApplicationClass = env->GetObjectClass(ctx);
    jmethodID getClassLoader   = env->GetMethodID(ApplicationClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject   classLoader      = env->CallObjectMethod(ctx, getClassLoader);
    jclass    DexFileClass     = env->FindClass("dalvik/system/DexFile");


    //这里可以mmap 加密后的dex，在内存中解密
    g_maindex_base = mmap_dex(szDexPath);
    LOGD("[+]mmap dex base:0x%p,size:%d,page_size:%d", g_maindex_base, g_dexSize, g_pageSize);

    sprintf(inPath, "%s/.jiagu/mini.dex", g_file_dir);
    sprintf(outPath, "%s/.jiagu/fengyue.oat", g_file_dir);
    writemixdex(inPath);
    mini_dex_obj = load_dex_fromfile(env, inPath, outPath);
    LOGD("[+]Start mem load dex");
    if (!g_isArt) {
        jint mCookie = mem_loadDex_dvm(env, (char *)szDexPath);
        LOGD("[+]Dalvik dex cookie :0x%x", mCookie);
        cookie_field = env->GetFieldID(DexFileClass, "mCookie", "I");
        //replace cookie
        env->SetIntField(mini_dex_obj, cookie_field, mCookie);
    } else {
        g_ArtHandle = get_lib_handle("libart.so");
        switch (g_sdk_int) {
          //android 4.4 art mode
          case 19:
              dex_info = mem_loadDex_byte19(g_ArtHandle, (char *)g_maindex_base, (size_t)g_dexSize);
              break;

          case 21:
              dex_info = mem_loadDex_byte21(g_ArtHandle, (char *)g_maindex_base, (size_t)g_dexSize);
              break;

          case 22:
              dex_info = mem_loadDex_byte22(g_ArtHandle, (char *)g_maindex_base, (size_t)g_dexSize);
              break;

          case 23:
              dex_info = mem_loadDex_byte23(g_ArtHandle, (char *)g_maindex_base, (size_t)g_dexSize);
              break;

          // 7.0 and 7.1
          case 24:
          case 25:
              dex_info = mem_loadDex_byte24(g_ArtHandle, (char *)g_maindex_base, (size_t)g_dexSize);
              break;

          //8.0
          case 26:
          case 27:
              //reserved
              dex_info = NULL;
              break;

          default:
              break;
        }
        if (dex_info) {
            unique_ptr<vector<const void *> > dex_files(new vector<const void *>());
            if (g_sdk_int == 19) {
                cookie_field = env->GetFieldID(DexFileClass, "mCookie", "I");
            } else if ((g_sdk_int == 21) || (g_sdk_int == 22)) {
                cookie_field = env->GetFieldID(DexFileClass, "mCookie", "J");
                //将dex_info转换为java层的cookie
                //jlong realcookie=get_real_cookie(env,dex_info);
                dex_files.get()->push_back(dex_info);
                jlong mCookie = static_cast<jlong>(reinterpret_cast<uintptr_t>(dex_files.release()));
                env->SetLongField(mini_dex_obj, cookie_field, mCookie);
            } else if (g_sdk_int == 23) {
                // cookie_field = env->GetFieldID(DexFileClass, "mCookie", "Ljava/lang/Object;");
                // dex_files.get()->push_back(dex_info);
                // jlong mCookie = static_cast<jlong>(reinterpret_cast<uintptr_t>(dex_files.release()));
                // LOGD("dex_info:%x,mCookie:%x",dex_info,mCookie);
                // env->SetObjectField(mini_dex_obj, cookie_field, (jobject)mCookie);
                replace_cookie_M(env, mini_dex_obj, (jlong)dex_info);
            } else if (g_sdk_int >= 24) {
                //cookie_field = env->GetFieldID(DexFileClass, "mCookie", "Ljava/lang/Object;");
                replace_cookie_N(env, mini_dex_obj, (jlong)dex_info);
            }
            make_dex_elements(env, classLoader, mini_dex_obj);
            if (g_ArtHandle) {
                dlclose(g_ArtHandle);
            }
            return;
        } else {
            char *buffer = get_path_frommaps(g_pkgName, (char *)".dex", (char *)".odex");
            memset(inPath, 0, sizeof(inPath));
            //memcpy(inPath,szDexPath,strlen(szDexPath));
            memcpy(inPath, buffer, strlen(buffer));
            sprintf(outPath, "%s/.jiagu/libfaked.so", g_file_dir);
            LOGD("[+]Load fake dex inPath:%s,outPath:%s", inPath, outPath);

            void *art_base = get_module_base(getpid(), LIB_ART_PATH);
            if (!art_base) {
                LOGE("[-]get lib %s base failed", LIB_ART_PATH);
                return;
            }
            //art got hook
            ElfReader elfReader(LIB_ART_PATH, art_base);
            if (0 != elfReader.parse()) {
                LOGE("failed to parse %s in %d maps at %p", LIB_ART_PATH, getpid(), art_base);
                return;
            }
            elfReader.hook("open", reinterpret_cast<void *>(new_open), reinterpret_cast<void **>(&old_open));
            elfReader.hook("read", reinterpret_cast<void *>(new_read), reinterpret_cast<void **>(&old_read));
            elfReader.hook("mmap", reinterpret_cast<void *>(new_mmap), reinterpret_cast<void **>(&old_mmap));
            elfReader.hook("munmap", reinterpret_cast<void *>(new_munmap), reinterpret_cast<void **>(&old_munmap));
            elfReader.hook("__read_chk", reinterpret_cast<void *>(new_read_chk), reinterpret_cast<void **>(&old_read_chk));
            elfReader.hook("fstat", reinterpret_cast<void *>(new_fstat), reinterpret_cast<void **>(&old_fstat));
            elfReader.hook("fork", reinterpret_cast<void *>(new_fork), reinterpret_cast<void **>(&old_fork));

            jobject faked_dex_obj = load_dex_fromfile(env, inPath, outPath);
            //恢复fork和fstat的hook
            elfReader.hook("fork", reinterpret_cast<void *>(old_fork), reinterpret_cast<void **>(&old_fork));
            elfReader.hook("fstat", reinterpret_cast<void *>(old_fstat), reinterpret_cast<void **>(&old_fstat));
            make_dex_elements(env, classLoader, faked_dex_obj);
            if (g_ArtHandle) {
                dlclose(g_ArtHandle);
            }
            return;
        }
    }
}


void native_attachBaseContext(JNIEnv *env, jobject obj, jobject ctx)
{
    jclass    ApplicationClass = env->GetObjectClass(ctx);
    jmethodID getFilesDir      = env->GetMethodID(ApplicationClass, "getFilesDir", "()Ljava/io/File;");
    jobject   File_obj         = env->CallObjectMethod(ctx, getFilesDir);
    jclass    FileClass        = env->GetObjectClass(File_obj);

    jmethodID getAbsolutePath = env->GetMethodID(FileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring   data_file_dir   = static_cast<jstring>(env->CallObjectMethod(File_obj, getAbsolutePath));

    g_file_dir = env->GetStringUTFChars(data_file_dir, NULL);
    LOGD("[+]FilesDir:%s", g_file_dir);
    env->DeleteLocalRef(data_file_dir);
    env->DeleteLocalRef(File_obj);
    env->DeleteLocalRef(FileClass);

    // NativeLibraryDir
    jmethodID getApplicationInfo = env->GetMethodID(ApplicationClass, "getApplicationInfo",
                                                    "()Landroid/content/pm/ApplicationInfo;");
    jobject  ApplicationInfo_obj    = env->CallObjectMethod(ctx, getApplicationInfo);
    jclass   ApplicationInfoClass   = env->GetObjectClass(ApplicationInfo_obj);
    jfieldID nativeLibraryDir_field = env->GetFieldID(ApplicationInfoClass, "nativeLibraryDir", "Ljava/lang/String;");
    jstring  nativeLibraryDir       = (jstring)(env->GetObjectField(ApplicationInfo_obj, nativeLibraryDir_field));

    g_NativeLibDir = env->GetStringUTFChars(nativeLibraryDir, NULL);
    LOGD("[+]NativeLibDir:%s", g_NativeLibDir);

    env->DeleteLocalRef(nativeLibraryDir);
    env->DeleteLocalRef(ApplicationInfoClass);
    env->DeleteLocalRef(ApplicationInfo_obj);

    jmethodID getPackageResourcePath = env->GetMethodID(ApplicationClass, "getPackageResourcePath",
                                                        "()Ljava/lang/String;");

    jstring    mPackageFilePath   = static_cast<jstring>(env->CallObjectMethod(ctx, getPackageResourcePath));
    const char *cmPackageFilePath = env->GetStringUTFChars(mPackageFilePath, NULL);
    g_PackageResourcePath = const_cast<char *>(cmPackageFilePath);
    LOGD("[+]PackageResourcePath:%s", g_PackageResourcePath);
    env->DeleteLocalRef(mPackageFilePath);


    jmethodID  getPackageName = env->GetMethodID(ApplicationClass, "getPackageName", "()Ljava/lang/String;");
    jstring    PackageName    = static_cast<jstring>(env->CallObjectMethod(ctx, getPackageName));
    const char *packagename   = env->GetStringUTFChars(PackageName, NULL);
    g_pkgName = (char *)packagename;
    LOGD("[+]g_pkgName :%s", g_pkgName);
    env->DeleteLocalRef(PackageName);

    char jiaguPath[256] = { 0 }; // 加密dex的存储路径
    char jiaguDir[256]  = { 0 }; // 加密dex的所在目录
    sprintf(jiaguDir, "%s/.jiagu", g_file_dir);
    sprintf(jiaguPath, "%s/.jiagu/%s", g_file_dir, JIAMI_MAGIC);
    LOGD("[+]jiaguDir:%s,jiaguPath:%s", jiaguDir, jiaguPath);
    extract_file(env, ctx, jiaguDir, jiaguPath, JIAMI_MAGIC);

    // mem loadDex
    mem_loadDex(env, ctx, jiaguPath);
} // native_attachBaseContext


void init(JNIEnv *env)
{
    jclass   jclazz  = env->FindClass("android/os/Build$VERSION");
    jfieldID SDK_INT = env->GetStaticFieldID(jclazz, "SDK_INT", "I");

    g_sdk_int = env->GetStaticIntField(jclazz, SDK_INT);
    LOGD("[+]sdk_int:%d", g_sdk_int);
    if (g_sdk_int > 13) {
        jclass    System             = env->FindClass("java/lang/System");
        jmethodID System_getProperty = env->GetStaticMethodID(System, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");

        jstring vm_version_name  = env->NewStringUTF("java.vm.version");
        jstring vm_version_value = (jstring)(env->CallStaticObjectMethod(System, System_getProperty, vm_version_name));

        char   *cvm_version_value = (char *)env->GetStringUTFChars(vm_version_value, NULL);
        double version            = atof(cvm_version_value);
        g_isArt = version >= 2 ? true : false;
        LOGD("[+]Android VmVersion:%f", version);

        env->ReleaseStringUTFChars(vm_version_value, cvm_version_value);
        env->DeleteLocalRef(System);
        env->DeleteLocalRef(vm_version_name);
        env->DeleteLocalRef(vm_version_value);
    } else {
        LOGE("[-]unsupported Android version");
        assert(false);
    }

    env->DeleteLocalRef(jclazz);
    registerNativeMethods(env);
}


JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;

    if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    init(env);
    return JNI_VERSION_1_6;
}
