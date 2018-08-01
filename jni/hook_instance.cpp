#include <dlfcn.h>
#include <string.h>
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
#include <unistd.h>

#include "elfGotHook/logger.h"
#include "packer.h"

#define kDexMagic "dex\n"


int (*old_open)(const char *pathname, int flags, mode_t mode);
int (*old_fstat)(int fildes, struct stat *buf);
ssize_t (*old_read_chk)(int fd, void *buf, size_t nbytes, size_t buflen);
ssize_t (*old_read)(int fd, void *buf, size_t count);
void *(*old_mmap)(void *start, size_t length, int prot, int flags, int fd, off_t offset);
int (*old_munmap)(void *start, size_t length);
pid_t (*old_fork)(void);


int new_open(const char *pathname, int flags, mode_t mode)
{
    int result = old_open(pathname, flags, mode);

    if (strstr(pathname, g_fake_dex_magic))
    {
        LOGD("[+]my open pathname:%s,result:%d", pathname, result);
        if (result == -1)
        {
            LOGE("[-]my open failed error:%s", strerror(errno));
        }
    }
    return result;
}

int new_fstat(int fd, struct stat *buf)
{
    int result = old_fstat(fd, buf);

    char fdlinkstr[128] = {0};
    char linkPath[256] = {0};

    memset(fdlinkstr, 0, 128);
    memset(linkPath, 0, 256);

    int pid = getpid();
    snprintf(fdlinkstr, 128, "/proc/%ld/fd/%d", pid, fd);
    
    if (readlink(fdlinkstr, linkPath, 256) >= 0)
    {
        // LOGD("[+]new fstat file:%s",linkPath);
        if (strstr(linkPath,(char*)g_fake_dex_magic))
        {
            buf->st_size = g_dex_size;
            LOGD("[+]fstat linkPath:%s,buf.size:%d", linkPath, buf->st_size);
        }
    }
    else
    {
        LOGD("[-]fun my fstat readlink error");
    }
    return result;
}

ssize_t new_read(int fd, void *buf, size_t count)
{
    char fdlinkstr[128] = {0};
    char linkPath[256] = {0};

    memset(fdlinkstr, 0, 128);
    memset(linkPath, 0, 256);
    int pid = getpid();
    snprintf(fdlinkstr, 128, "/proc/%ld/fd/%d", pid, fd);
    if (readlink(fdlinkstr, linkPath, 256) >= 0)
    {
        // LOGD("[+]my read file:%s,count:%d",linkPath,count);
        if (strstr(linkPath,(char*)g_fake_dex_magic))
        {
            LOGD("[+]my read memcpy dex magic");
            memcpy(buf, kDexMagic, 4);
            return 4;
        }
    }
    else
    {
        LOGD("[-]my read readlink error");
    }
    return old_read(fd, buf, count);
}

ssize_t new_read_chk(int fd, void *buf, size_t nbytes, size_t buflen)
{
    char fdlinkstr[128] = {0};
    char linkPath[256] = {0};

    memset(fdlinkstr, 0, 128);
    memset(linkPath, 0, 256);
    int pid = getpid();
    snprintf(fdlinkstr, 128, "/proc/%ld/fd/%d", pid, fd);
    if (readlink(fdlinkstr, linkPath, 256) >= 0)
    {
        // LOGD("[+]my read_chk file:%s,nbytes:%d,buflen:%d",linkPath,nbytes,buflen);

        // [+]my read_chk file:/data/data/com.ai
        // speech.weiyu/files/.jiagu/mini.dex,nbytes:4,buflen:-1
        // [+]my read_chk g_faked_dex_path:/data
        // /user/0/com.aispeech.weiyu/files/.jiagu/mini.dex

        // 这里不能使用strcmp比较 如果使用mini.dex作为fake_dex,mini_dex的位置为
        // /data/user/0/pkg_name/files/.jiagu/mini.dex
        // 但是使用readlink获取到的mini.dex路径为
        // /data/data/pkg_name/files/.jiagu/mini.dex
        if (strstr(linkPath,(char*)g_fake_dex_magic))
        {
            LOGD("[+]fun my read_chk memcpy dex magic");
            memcpy(buf, kDexMagic, 4);
            return 4;
        }
    }
    else
    {
        LOGD("[-]fun my read_chk readlink error");
    }
    return old_read_chk(fd, buf, nbytes, buflen);
}

void *new_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    char fdlinkstr[128] = {0};
    char linkPath[256] = {0};

    memset(fdlinkstr, 0, 128);
    memset(linkPath, 0, 256);
    int pid = (int)getpid();
    snprintf(fdlinkstr, 128, "/proc/%ld/fd/%d", pid, fd);

    if (readlink(fdlinkstr, linkPath, 256) < 0)
    {
        LOGD("[-]my mmap readlink error");
        return old_mmap(start, length, prot, flags, fd, offset);
    }

    if (strstr(linkPath,(char*)g_fake_dex_magic))
    {
        LOGD("[+]mmap linkpath:%s,size:%d", linkPath, length);
        return g_decrypt_base;
    }
    return old_mmap(start, length, prot, flags, fd, offset);
}

int new_munmap(void *start, size_t length)
{
    if ((start == g_decrypt_base) || (length == g_page_size))
    {
        LOGD("[+]munmap start:%p,length:%d", start, length);
        return 0;
    }
    return old_munmap(start, length);
}

pid_t new_fork(void)
{
    LOGD("[+]fun my fork called");
    return -1;
}
