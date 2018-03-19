#ifndef INJECTDEMO_ELF_READER_H
#define INJECTDEMO_ELF_READER_H

#include <string.h>
#include <stdio.h>

#include "def.h"
#include "logger.h"

class ElfReader {
public:
    ElfReader(const char* module, void *start);
    int parse();
    int hook(const char *func_name, void *new_func, void **old_func);
    void dumpElfHeader();
    void dumpProgramHeaders();
    void dumpDynamicSegment();
    void dumpDynamicRel();

private:
    int verifyElfHeader();
    int elfTargetMachine();
    ElfW(Addr) getSegmentBaseAddress();
    ElfW(Phdr)* findSegmentByType(ElfW(Word) type);
    ElfW(Phdr)* findSegmentByAddress(void *addr);
    int parseDynamicSegment();
    int findSymbolByName(const char *symbol, ElfW(Sym) **sym, uint32_t *symidx);
    int elfLookup(const char *symbol, ElfW(Sym) **sym, uint32_t *symidx);
    int gnuLookup(const char *symbol, ElfW(Sym) **sym, uint32_t *symidx);
    int hookInternally(void *addr, void *new_func, void **old_func);

private:
    const char* moduleName;
    ElfW(Addr) start;
    ElfW(Addr) bias;
    ElfW(Ehdr) *ehdr;
    ElfW(Half) phdrNum;
    ElfW(Phdr) *phdr;

    const char *strTable = NULL;
    ElfW(Sym) *symTable = NULL;
    ElfW(Xword) relCount = 0;
    ElfW(Xword) pltRelCount = 0;
#if defined(USE_RELA)
    ElfW(Rela) *rel = NULL;
    ElfW(Rela) *pltRel = NULL;
#else
    ElfW(Rel) *rel = NULL;
    ElfW(Rel) *pltRel = NULL;
#endif

    //for elf hash
    uint32_t nbucket;
    uint32_t nchain;
    uint32_t *bucket;
    uint32_t *chain;

    //for gnu hash
    bool isGNUHash = false;
    uint32_t gnuNBucket;
    uint32_t gnuSymndx;
    uint32_t gnuMaskwords;
    uint32_t gnuShift2;
    uint32_t *gnuBucket;
    uint32_t *gnuChain;
    ElfW(Addr) *gnuBloomFilter;
};


#endif //INJECTDEMO_ELF_READER_H
