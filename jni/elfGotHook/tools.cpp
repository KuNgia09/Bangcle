#include "tools.h"

void *ElfHooker::get_module_base(pid_t pid, const char *module)
{
    char buffer[1024];

    if (pid <= 0)
    {
        snprintf(buffer, 1024, "/proc/self/maps");
    }
    else
    {
        snprintf(buffer, 1024, "/proc/%d/maps", pid);
    }

    unsigned long module_start;
    unsigned long module_end;
    char          so_name[SO_NAME_LEN];
    FILE          *mapsFile = fopen(buffer, "r");
    if (NULL != mapsFile)
    {
        while (fgets(buffer, 1024, mapsFile))
        {
            // 7ad995c000-7ad9a18000 r-xp 00000000 103:09 1623                          /system/lib64/libc.so
            if (sscanf(buffer, "%lx-%lx %*4s %*x %*x:%*x %*d %s", &module_start, &module_end, so_name) != 3)
            {
                continue;
            }

            if (0 == strncmp(so_name, module, strlen(module)))
            {
                fclose(mapsFile);
                return reinterpret_cast<void *>(module_start);
            }
        }
    }
    fclose(mapsFile);
    LOGE("failed to find module %s (%d)", module, pid);
    return NULL;
}


uint32_t ElfHooker::elf_hash(const char *symbol)
{
    const uint8_t *name = reinterpret_cast<const uint8_t *>(symbol);
    uint32_t      h = 0, g;

    while (*name)
    {
        h  = (h << 4) + *name++;
        g  = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }

    return h;
}


uint32_t ElfHooker::gnu_hash(const char *symbol)
{
    uint32_t      h     = 5381;
    const uint8_t *name = reinterpret_cast<const uint8_t *>(symbol);

    while (0 != *name)
    {
        h += (h << 5) + *name++; // h*33 + c = h + h * 32 + c = h + h << 5 + c
    }
    return h;
}


void ElfHooker::clear_cache(void *addr, size_t len)
{
    uint8_t *end = reinterpret_cast<uint8_t *>(addr) + len;

    LOGD("clear cache %lx  , %lx", addr, end);

    syscall(0xf0002, addr, end);
}
