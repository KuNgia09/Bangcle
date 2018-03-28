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

#include "elfGotHook/logger.h"
#include "packer.h"

#define  kDexMagic    "dex\n"


int (*old_open)(const char *pathname, int flags, mode_t mode);
int     (*old_fstat)(int fildes, struct stat *buf);
ssize_t (*old_read_chk)(int fd, void *buf, size_t nbytes, size_t buflen);
ssize_t (*old_read)(int fd, void *buf, size_t count);
void    * (*old_mmap)(void *start, size_t length, int prot, int flags, int fd, off_t offset);
int     (*old_munmap)(void *start, size_t length);
pid_t   (*old_fork)(void);


int new_open(const char *pathname, int flags, mode_t mode)
{
    int result = old_open(pathname, flags, mode);

    if (!strcmp(pathname, g_fake_dex_path)) {
        LOGD("[+]new open pathname:%s,result:%d", pathname, result);
        if (result == -1) {
            LOGE("[-]open failed error:%s", strerror(errno));
        }
    }
    return result;
}


int new_fstat(int fd, struct stat *buf)
{
    int result = old_fstat(fd, buf);

    char fdlinkstr[128] = { 0 };
    char linkPath[256]  = { 0 };

    memset(fdlinkstr, 0, 128);
    memset(linkPath, 0, 256);
    int pid = getpid();
    snprintf(fdlinkstr, 128, "/proc/%ld/fd/%d", pid, fd);

    if (readlink(fdlinkstr, linkPath, 256) >= 0) {
        if (!strcmp(linkPath, g_fake_dex_path)) {
            buf->st_size = g_dexSize;
            LOGD("[+]fstat linkPath:%s,buf.size:%d", linkPath, buf->st_size);
        }
    } else {
        LOGD("[-]fun my fstat readlink error");
    }
    return result;
}


ssize_t new_read(int fd, void *buf, size_t count)
{
    char fdlinkstr[128] = { 0 };
    char linkPath[256]  = { 0 };

    memset(fdlinkstr, 0, 128);
    memset(linkPath, 0, 256);
    int pid = getpid();
    snprintf(fdlinkstr, 128, "/proc/%ld/fd/%d", pid, fd);
    if (readlink(fdlinkstr, linkPath, 256) >= 0) {
        LOGD("[+]read linkpath:%s", linkPath);
        if (!strcmp(g_fake_dex_path, linkPath)) {
            LOGD("[+]fun my read memcpy magic");
            memcpy(buf, kDexMagic, 4);
            return 4;
        }
    } else {
        LOGD("[-]fun my read readlink error");
    }
    return old_read(fd, buf, count);
}


ssize_t new_read_chk(int fd, void *buf, size_t nbytes, size_t buflen)
{
    char fdlinkstr[128] = { 0 };
    char linkPath[256]  = { 0 };

    memset(fdlinkstr, 0, 128);
    memset(linkPath, 0, 256);
    int pid = getpid();
    snprintf(fdlinkstr, 128, "/proc/%ld/fd/%d", pid, fd);
    if (readlink(fdlinkstr, linkPath, 256) >= 0) {
        LOGD("[+]read_chk linkpath:%s", linkPath);
        if (!strcmp(g_fake_dex_path, linkPath)) {
            LOGD("[+]fun my read_chk memcpy magic");
            memcpy(buf, kDexMagic, 4);
            return 4;
        }
    } else {
        LOGD("[-]fun my read_chk readlink error");
    }
    return old_read_chk(fd, buf, nbytes, buflen);
}


void *new_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
    char fdlinkstr[128] = { 0 };
    char linkPath[256]  = { 0 };

    memset(fdlinkstr, 0, 128);
    memset(linkPath, 0, 256);
    int pid = getpid();
    snprintf(fdlinkstr, 128, "/proc/%ld/fd/%d", pid, fd);
    if (readlink(fdlinkstr, linkPath, 256) < 0) {
        LOGD("[-]fun my mmap readlink error");
        return old_mmap(start, length, prot, flags, fd, offset);
    }

    if (strcmp(linkPath, g_fake_dex_path) == 0) {
        LOGD("[+]mmap linkpath:%s,size:%d", linkPath, length);
        return g_maindex_base;
    }
    return old_mmap(start, length, prot, flags, fd, offset);
}


int new_munmap(void *start, size_t length)
{
    if ((start == g_maindex_base) || (length == g_pageSize)) {
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
