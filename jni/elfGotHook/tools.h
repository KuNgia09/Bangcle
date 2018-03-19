#ifndef INJECTDEMO_TOOLS_H
#define INJECTDEMO_TOOLS_H

#include <unistd.h>
#include <stdio.h>
#include <cctype>
#include <fcntl.h>
#include <elf.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include "logger.h"
#include "def.h"

namespace ElfHooker {
    void* get_module_base(pid_t pid, const char* module);
    uint32_t elf_hash(const char* symbol);
    uint32_t gnu_hash(const char* symbol);
    void clear_cache(void *addr, size_t len);
};

#endif //INJECTDEMO_TOOLS_H
