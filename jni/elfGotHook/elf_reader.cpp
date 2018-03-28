#include "elf_reader.h"
#include "tools.h"

static const char *ELF_CLASS_MAP[] = {
    "None",
    "ELF32",
    "ELF64",
    "Unknown"
};

static const char *ELF_DATA_MAP[] = {
    "None",
    "little endian",
    "big endian",
    "Unknown"
};

static const char *ELF_VERSION_MAP[] = {
    "None",
    "Current",
    "Unknown"
};

static const struct Elf_Program_Type_Map
{
    const char *name;
    ElfW(Word) value;
}
ELF_PROGRAM_TYPE_MAP[] = {
    { "PT_NULL",         PT_NULL         },
    { "PT_LOAD",         PT_LOAD         },
    { "PT_DYNAMIC",      PT_DYNAMIC      },
    { "PT_INTERP",       PT_INTERP       },
    { "PT_NOTE",         PT_NOTE         },
    { "PT_SHLIB",        PT_SHLIB        },
    { "PT_PHDR",         PT_PHDR         },
    { "PT_TLS",          PT_TLS          },
    { "PT_LOOS",         PT_LOOS         },
    { "PT_HIOS",         PT_HIOS         },
    { "PT_LOPROC",       PT_LOPROC       },
    { "PT_HIPROC",       PT_HIPROC       },
    { "PT_GNU_EH_FRAME", PT_GNU_EH_FRAME },
    { "PT_GNU_STACK",    PT_GNU_STACK    },
    { "PT_GNU_RELRO",    PT_GNU_RELRO    },
    { "PT_ARM_EXIDX",    PT_ARM_EXIDX    }
};

static const struct Elf_Dynamic_Type_Map
{
    const char *name;
    ElfW(Xword) value;
}
ELF_DYNAMIC_TYPE_MAP[] = {
    { "DT_NULL",            DT_NULL            },
    { "DT_NEEDED",          DT_NEEDED          },
    { "DT_PLTRELSZ",        DT_PLTRELSZ        },
    { "DT_PLTGOT",          DT_PLTGOT          },
    { "DT_HASH",            DT_HASH            },
    { "DT_STRTAB",          DT_STRTAB          },
    { "DT_SYMTAB",          DT_SYMTAB          },
    { "DT_RELA",            DT_RELA            },
    { "DT_RELASZ",          DT_RELASZ          },
    { "DT_RELAENT",         DT_RELAENT         },
    { "DT_STRSZ",           DT_STRSZ           },
    { "DT_SYMENT",          DT_SYMENT          },
    { "DT_INIT",            DT_INIT            },
    { "DT_FINI",            DT_FINI            },
    { "DT_SONAME",          DT_SONAME          },
    { "DT_RPATH",           DT_RPATH           },
    { "DT_SYMBOLIC",        DT_SYMBOLIC        },
    { "DT_REL",             DT_REL             },
    { "DT_RELSZ",           DT_RELSZ           },
    { "DT_RELENT",          DT_RELENT          },
    { "DT_PLTREL",          DT_PLTREL          },
    { "DT_DEBUG",           DT_DEBUG           },
    { "DT_TEXTREL",         DT_TEXTREL         },
    { "DT_JMPREL",          DT_JMPREL          },
    { "DT_BIND_NOW",        DT_BIND_NOW        },
    { "DT_INIT_ARRAY",      DT_INIT_ARRAY      },
    { "DT_FINI_ARRAY",      DT_FINI_ARRAY      },
    { "DT_INIT_ARRAYSZ",    DT_INIT_ARRAYSZ    },
    { "DT_FINI_ARRAYSZ",    DT_FINI_ARRAYSZ    },
    { "DT_RUNPATH",         DT_RUNPATH         },
    { "DT_FLAGS",           DT_FLAGS           },
    { "DT_PREINIT_ARRAY",   DT_PREINIT_ARRAY   },
    { "DT_PREINIT_ARRAYSZ", DT_PREINIT_ARRAYSZ },
    { "DT_ANDROID_REL",     DT_ANDROID_REL     },
    { "DT_ANDROID_RELSZ",   DT_ANDROID_RELSZ   },
    { "DT_ANDROID_RELA",    DT_ANDROID_RELA    },
    { "DT_ANDROID_RELASZ",  DT_ANDROID_RELASZ  },
    { "DT_HIOS",            DT_HIOS            },
    { "DT_VALRNGLO",        DT_VALRNGLO        },
    { "DT_VALRNGHI",        DT_VALRNGHI        },
    { "DT_ADDRRNGLO",       DT_ADDRRNGLO       },
    { "DT_ADDRRNGHI",       DT_ADDRRNGHI       },
    { "DT_VERSYM",          DT_VERSYM          },
    { "DT_RELACOUNT",       DT_RELACOUNT       },
    { "DT_RELCOUNT",        DT_RELCOUNT        },
    { "DT_FLAGS_1",         DT_FLAGS_1         },
    { "DT_VERDEF",          DT_VERDEF          },
    { "DT_VERDEFNUM",       DT_VERDEFNUM       },
    { "DT_VERNEED",         DT_VERNEED         },
    { "DT_VERNEEDNUM",      DT_VERNEEDNUM      },
    { "DT_GNU_HASH",        DT_GNU_HASH        },
    { "DT_LOPROC",          DT_LOPROC          },
    { "DT_HIPROC",          DT_HIPROC          }
};

const static struct Elf_Dynamic_Rel_Type_Map
{
    const char *name;
    ElfW(Xword) value;
}
ELF_DYNAMIC_REL_TYPE_MAP[] = {
#if defined(__arm__)
    {
        "R_ARM_NONE", R_ARM_NONE
    },
    { "R_ARM_PC24",                    R_ARM_PC24                    },
    { "R_ARM_ABS32",                   R_ARM_ABS32                   },
    { "R_ARM_REL32",                   R_ARM_REL32                   },
    { "R_ARM_PC13",                    R_ARM_PC13                    },
    { "R_ARM_ABS16",                   R_ARM_ABS16                   },
    { "R_ARM_ABS12",                   R_ARM_ABS12                   },
    { "R_ARM_THM_ABS5",                R_ARM_THM_ABS5                },
    { "R_ARM_ABS8",                    R_ARM_ABS8                    },
    { "R_ARM_SBREL32",                 R_ARM_SBREL32                 },
    { "R_ARM_THM_PC22",                R_ARM_THM_PC22                },
    { "R_ARM_THM_PC8",                 R_ARM_THM_PC8                 },
    { "R_ARM_AMP_VCALL9",              R_ARM_AMP_VCALL9              },
    { "R_ARM_SWI24",                   R_ARM_SWI24                   },
    { "R_ARM_THM_SWI8",                R_ARM_THM_SWI8                },
    { "R_ARM_XPC25",                   R_ARM_XPC25                   },
    { "R_ARM_THM_XPC22",               R_ARM_THM_XPC22               },
    { "R_ARM_TLS_DTPMOD32",            R_ARM_TLS_DTPMOD32            },
    { "R_ARM_TLS_DTPOFF32",            R_ARM_TLS_DTPOFF32            },
    { "R_ARM_TLS_TPOFF32",             R_ARM_TLS_TPOFF32             },
    { "R_ARM_COPY",                    R_ARM_COPY                    },
    { "R_ARM_GLOB_DAT",                R_ARM_GLOB_DAT                },
    { "R_ARM_JUMP_SLOT",               R_ARM_JUMP_SLOT               },
    { "R_ARM_RELATIVE",                R_ARM_RELATIVE                },
    { "R_ARM_GOTOFF",                  R_ARM_GOTOFF                  },
    { "R_ARM_GOTPC",                   R_ARM_GOTPC                   },
    { "R_ARM_GOT32",                   R_ARM_GOT32                   },
    { "R_ARM_PLT32",                   R_ARM_PLT32                   },
    { "R_ARM_CALL",                    R_ARM_CALL                    },
    { "R_ARM_JUMP24",                  R_ARM_JUMP24                  },
    { "R_ARM_THM_JUMP24",              R_ARM_THM_JUMP24              },
    { "R_ARM_BASE_ABS",                R_ARM_BASE_ABS                },
    { "R_ARM_ALU_PCREL_7_0",           R_ARM_ALU_PCREL_7_0           },
    { "R_ARM_ALU_PCREL_15_8",          R_ARM_ALU_PCREL_15_8          },
    { "R_ARM_ALU_PCREL_23_15",         R_ARM_ALU_PCREL_23_15         },
    { "R_ARM_ALU_SBREL_11_0",          R_ARM_ALU_SBREL_11_0          },
    { "R_ARM_ALU_SBREL_19_12",         R_ARM_ALU_SBREL_19_12         },
    { "R_ARM_ALU_SBREL_27_20",         R_ARM_ALU_SBREL_27_20         },
    { "R_ARM_TARGET1",                 R_ARM_TARGET1                 },
    { "R_ARM_SBREL31",                 R_ARM_SBREL31                 },
    { "R_ARM_V4BX",                    R_ARM_V4BX                    },
    { "R_ARM_TARGET2",                 R_ARM_TARGET2                 },
    { "R_ARM_PREL31",                  R_ARM_PREL31                  },
    { "R_ARM_MOVW_ABS_NC",             R_ARM_MOVW_ABS_NC             },
    { "R_ARM_MOVT_ABS",                R_ARM_MOVT_ABS                },
    { "R_ARM_MOVW_PREL_NC",            R_ARM_MOVW_PREL_NC            },
    { "R_ARM_MOVT_PREL",               R_ARM_MOVT_PREL               },
    { "R_ARM_THM_MOVW_ABS_NC",         R_ARM_THM_MOVW_ABS_NC         },
    { "R_ARM_THM_MOVT_ABS",            R_ARM_THM_MOVT_ABS            },
    { "R_ARM_THM_MOVW_PREL_NC",        R_ARM_THM_MOVW_PREL_NC        },
    { "R_ARM_THM_MOVT_PREL",           R_ARM_THM_MOVT_PREL           },
    { "R_ARM_GNU_VTENTRY",             R_ARM_GNU_VTENTRY             },
    { "R_ARM_GNU_VTINHERIT",           R_ARM_GNU_VTINHERIT           },
    { "R_ARM_THM_PC11",                R_ARM_THM_PC11                },
    { "R_ARM_THM_PC9",                 R_ARM_THM_PC9                 },
    { "R_ARM_TLS_GD32",                R_ARM_TLS_GD32                },
    { "R_ARM_TLS_LDM32",               R_ARM_TLS_LDM32               },
    { "R_ARM_TLS_LDO32",               R_ARM_TLS_LDO32               },
    { "R_ARM_TLS_IE32",                R_ARM_TLS_IE32                },
    { "R_ARM_TLS_LE32",                R_ARM_TLS_LE32                },
    { "R_ARM_TLS_LDO12",               R_ARM_TLS_LDO12               },
    { "R_ARM_TLS_LE12",                R_ARM_TLS_LE12                },
    { "R_ARM_TLS_IE12GP",              R_ARM_TLS_IE12GP              },
    { "R_ARM_IRELATIVE",               R_ARM_IRELATIVE               },
    { "R_ARM_RXPC25",                  R_ARM_RXPC25                  },
    { "R_ARM_RSBREL32",                R_ARM_RSBREL32                },
    { "R_ARM_THM_RPC22",               R_ARM_THM_RPC22               },
    { "R_ARM_RREL32",                  R_ARM_RREL32                  },
    { "R_ARM_RABS32",                  R_ARM_RABS32                  },
    { "R_ARM_RPC24",                   R_ARM_RPC24                   },
    { "R_ARM_RBASE",                   R_ARM_RBASE                   },
#elif defined(__aarch64__)
    {
        "R_AARCH64_NONE", R_AARCH64_NONE
    },
    { "R_AARCH64_ABS64",               R_AARCH64_ABS64               },
    { "R_AARCH64_ABS32",               R_AARCH64_ABS32               },
    { "R_AARCH64_ABS16",               R_AARCH64_ABS16               },
    { "R_AARCH64_PREL64",              R_AARCH64_PREL64              },
    { "R_AARCH64_PREL32",              R_AARCH64_PREL32              },
    { "R_AARCH64_PREL16",              R_AARCH64_PREL16              },
    { "R_AARCH64_MOVW_UABS_G0",        R_AARCH64_MOVW_UABS_G0        },
    { "R_AARCH64_MOVW_UABS_G0_NC",     R_AARCH64_MOVW_UABS_G0_NC     },
    { "R_AARCH64_MOVW_UABS_G1",        R_AARCH64_MOVW_UABS_G1        },
    { "R_AARCH64_MOVW_UABS_G1_NC",     R_AARCH64_MOVW_UABS_G1_NC     },
    { "R_AARCH64_MOVW_UABS_G2",        R_AARCH64_MOVW_UABS_G2        },
    { "R_AARCH64_MOVW_UABS_G2_NC",     R_AARCH64_MOVW_UABS_G2_NC     },
    { "R_AARCH64_MOVW_UABS_G3",        R_AARCH64_MOVW_UABS_G3        },
    { "R_AARCH64_MOVW_SABS_G0",        R_AARCH64_MOVW_SABS_G0        },
    { "R_AARCH64_MOVW_SABS_G1",        R_AARCH64_MOVW_SABS_G1        },
    { "R_AARCH64_MOVW_SABS_G2",        R_AARCH64_MOVW_SABS_G2        },
    { "R_AARCH64_LD_PREL_LO19",        R_AARCH64_LD_PREL_LO19        },
    { "R_AARCH64_ADR_PREL_LO21",       R_AARCH64_ADR_PREL_LO21       },
    { "R_AARCH64_ADR_PREL_PG_HI21",    R_AARCH64_ADR_PREL_PG_HI21    },
    { "R_AARCH64_ADR_PREL_PG_HI21_NC", R_AARCH64_ADR_PREL_PG_HI21_NC },
    { "R_AARCH64_ADD_ABS_LO12_NC",     R_AARCH64_ADD_ABS_LO12_NC     },
    { "R_AARCH64_LDST8_ABS_LO12_NC",   R_AARCH64_LDST8_ABS_LO12_NC   },
    { "R_AARCH64_TSTBR14",             R_AARCH64_TSTBR14             },
    { "R_AARCH64_CONDBR19",            R_AARCH64_CONDBR19            },
    { "R_AARCH64_JUMP26",              R_AARCH64_JUMP26              },
    { "R_AARCH64_CALL26",              R_AARCH64_CALL26              },
    { "R_AARCH64_LDST16_ABS_LO12_NC",  R_AARCH64_LDST16_ABS_LO12_NC  },
    { "R_AARCH64_LDST32_ABS_LO12_NC",  R_AARCH64_LDST32_ABS_LO12_NC  },
    { "R_AARCH64_LDST64_ABS_LO12_NC",  R_AARCH64_LDST64_ABS_LO12_NC  },
    { "R_AARCH64_LDST128_ABS_LO12_NC", R_AARCH64_LDST128_ABS_LO12_NC },
    { "R_AARCH64_MOVW_PREL_G0",        R_AARCH64_MOVW_PREL_G0        },
    { "R_AARCH64_MOVW_PREL_G0_NC",     R_AARCH64_MOVW_PREL_G0_NC     },
    { "R_AARCH64_MOVW_PREL_G1",        R_AARCH64_MOVW_PREL_G1        },
    { "R_AARCH64_MOVW_PREL_G1_NC",     R_AARCH64_MOVW_PREL_G1_NC     },
    { "R_AARCH64_MOVW_PREL_G2",        R_AARCH64_MOVW_PREL_G2        },
    { "R_AARCH64_MOVW_PREL_G2_NC",     R_AARCH64_MOVW_PREL_G2_NC     },
    { "R_AARCH64_MOVW_PREL_G3",        R_AARCH64_MOVW_PREL_G3        },
    { "R_AARCH64_COPY",                R_AARCH64_COPY                },
    { "R_AARCH64_GLOB_DAT",            R_AARCH64_GLOB_DAT            },
    { "R_AARCH64_JUMP_SLOT",           R_AARCH64_JUMP_SLOT           },
    { "R_AARCH64_RELATIVE",            R_AARCH64_RELATIVE            },
    { "R_AARCH64_TLS_TPREL64",         R_AARCH64_TLS_TPREL64         },
    { "R_AARCH64_TLS_DTPREL32",        R_AARCH64_TLS_DTPREL32        },
    { "R_AARCH64_IRELATIVE",           R_AARCH64_IRELATIVE           },
#endif
};

static const Elf_Program_Type_Map *map_elf_program_type(ElfW(Word)type)
{
    for (size_t i = 0; i < sizeof(ELF_PROGRAM_TYPE_MAP) / sizeof(Elf_Program_Type_Map); i++)
    {
        if (ELF_PROGRAM_TYPE_MAP[i].value == type)
        {
            return &ELF_PROGRAM_TYPE_MAP[i];
        }
    }
    return NULL;
}


static const Elf_Dynamic_Type_Map *map_elf_dynamic_type(ElfW(Xword)type)
{
    for (size_t i = 0; i < sizeof(ELF_DYNAMIC_TYPE_MAP) / sizeof(Elf_Dynamic_Type_Map); i++)
    {
        if (ELF_DYNAMIC_TYPE_MAP[i].value == type)
        {
            return &ELF_DYNAMIC_TYPE_MAP[i];
        }
    }
    return NULL;
}


static const Elf_Dynamic_Rel_Type_Map *map_elf_dynamic_rel_type(ElfW(Xword)type)
{
    for (size_t i = 0; i < sizeof(ELF_DYNAMIC_REL_TYPE_MAP) / sizeof(Elf_Dynamic_Rel_Type_Map); i++)
    {
        if (ELF_DYNAMIC_REL_TYPE_MAP[i].value == type)
        {
            return &ELF_DYNAMIC_REL_TYPE_MAP[i];
        }
    }
    return NULL;
}


static void getProgramFlagString(ElfW(Word)flags, char *buffer)
{
    int        index = 0;
    const char RWX[] = { 'R', 'W', 'X' };

    while (index < 3)
    {
        if (flags & 0x1)
        {
            buffer[index] = RWX[index];
        }
        else
        {
            buffer[index] = ' ';
        }
        flags >>= 1;
        index++;
    }
    buffer[index] = '\0';
}


ElfReader::ElfReader(const char *module, void *start) : moduleName(module), start(reinterpret_cast<ElfW(Addr)>(start))
{
}


int ElfReader::parse()
{
    this->ehdr = reinterpret_cast<ElfW(Ehdr) *>(this->start);
    if (0 != verifyElfHeader())
    {
        return -1;
    }

    this->phdrNum = ehdr->e_phnum;
    this->phdr    = reinterpret_cast<ElfW(Phdr) *>(this->start + ehdr->e_phoff);
    this->bias    = getSegmentBaseAddress();
    if (0 == this->bias)
    {
        LOGE("failed to get segment base address");
        return -1;
    }
    if (0 != parseDynamicSegment())
    {
        LOGE("failed to parse dynamic segment");
        return -1;
    }
    return 0;
}


int ElfReader::hook(const char *func_name, void *new_func, void **old_func)
{
    uint32_t symidx = 0;

    ElfW(Sym) * sym = NULL;

    if (0 == findSymbolByName(func_name, &sym, &symidx))
    {
        void *addr;
        ElfW(Xword) r_info  = 0;
        ElfW(Addr) r_offset = 0;
#if defined(USE_RELA)
        ElfW(Rela) * curr;
        ElfW(Rela) * rel;
#else
        ElfW(Rel) * curr;
        ElfW(Rel) * rel;
#endif
        rel = this->pltRel;
        for (uint32_t i = 0; i < this->pltRelCount; i++)
        {
            curr     = rel + i;
            r_info   = curr->r_info;
            r_offset = curr->r_offset;

            if ((ELF_R_SYM(r_info) == symidx) && (ELF_R_TYPE(r_info) == R_GENERIC_JUMP_SLOT))
            {
                addr = reinterpret_cast<void *>((this->bias + r_offset));
                if (0 == hookInternally(addr, new_func, old_func))
                {
                    LOGD("hook %s successfully in %s", func_name, this->moduleName);
                    return 0;
                }
                break;
            }
        }

        rel = this->rel;
        for (uint32_t i = 0; i < this->relCount; i++)
        {
            r_info   = rel->r_info;
            r_offset = rel->r_offset;

            if ((ELF_R_SYM(r_info) == symidx) && ((ELF_R_TYPE(r_info) == R_GENERIC_ABS) || (ELF_R_TYPE(r_info) == R_GENERIC_GLOB_DAT)))
            {
                addr = reinterpret_cast<void *>((this->bias + r_offset));
                if (0 == hookInternally(addr, new_func, old_func))
                {
                    LOGD("hook %s successfully in %s", func_name, this->moduleName);
                    return 0;
                }
                break;
            }
        }
    }
    LOGD("hook %s failure in %s", func_name, this->moduleName);
    return -1;
}


void ElfReader::dumpElfHeader()
{
    LOGD("elf header:");

    size_t len;
    size_t size = 128;
    char   buffer[size];
    snprintf(buffer, size, "magic: ");
    len = strlen(buffer);
    for (int i = 0; i < EI_NIDENT; i++)
    {
        snprintf(&buffer[len], size - len, "%02x ", ehdr->e_ident[i]);
        len = strlen(buffer);
    }
    snprintf(&buffer[len], size - len, "\n");
    LOGD("%s", buffer);

    uint8_t class_type = ehdr->e_ident[EI_CLASS];
    if (class_type > ELFCLASSNUM)
    {
        class_type = ELFCLASSNUM;
    }
    LOGD("class: %d (%s)", ehdr->e_ident[EI_CLASS], ELF_CLASS_MAP[class_type]);

    uint8_t data_type = ehdr->e_ident[EI_DATA];
    if (data_type > ELFDATA2MSB)
    {
        data_type = ELFDATA2MSB + 1;
    }
    LOGD("data: %d (%s)", ehdr->e_ident[EI_DATA], ELF_DATA_MAP[data_type]);

    uint8_t version_type = ehdr->e_ident[EI_VERSION];
    if (version_type > EV_CURRENT)
    {
        version_type = EV_CURRENT + 1;
    }
    LOGD("version: %d (%s)", ehdr->e_ident[EI_VERSION], ELF_VERSION_MAP[version_type]);

    LOGD("type: %s", ET_DYN == ehdr->e_type ? "DYN" : "Unknown");
    LOGD("machine: %s", EM_ARM == elfTargetMachine() ? "arm" : "arm64");
    LOGD("entry point address: %p", reinterpret_cast<void *>(ehdr->e_entry));
#if defined(__LP64__)
    LOGD("start of program header: %llx", ehdr->e_phoff);
    LOGD("start of section header: %llx", ehdr->e_shoff);
#else
    LOGD("start of program header: %x", ehdr->e_phoff);
    LOGD("start of section header: %x", ehdr->e_shoff);
#endif
    LOGD("flags: %x", ehdr->e_flags);
    LOGD("size of this header: %d", ehdr->e_ehsize);
    LOGD("size of program header: %d", ehdr->e_phentsize);
    LOGD("number of program header: %d", ehdr->e_phnum);
    LOGD("size of section header: %d", ehdr->e_shentsize);
    LOGD("number of section header: %d", ehdr->e_shnum);
    LOGD("section header string table index: %d", ehdr->e_shstrndx);
}


void ElfReader::dumpProgramHeaders()
{
    if (phdrNum > 0)
    {
        LOGD("program header dump:");
#if defined(__LP64__)
        LOGD("Type                 Offset   VirtAddr          PhysAddr         FileSize MemSize  Props Align");
#else
        LOGD("Type                 Offset   VirtAddr PhysAddr FileSize MemSize  Props Align");
#endif

        ElfW(Phdr) * phdr;
        const Elf_Program_Type_Map *type;
        char buffer[4];
        for (ElfW(Half) i = 0; i < phdrNum; i++)
        {
            phdr = this->phdr + i;
            type = map_elf_program_type(phdr->p_type);
            getProgramFlagString(phdr->p_flags, buffer);
#if defined(__LP64__)
            LOGD("%-20s %08llx %016llx %016llx %08llx %08llx %s   %llx", NULL != type ? type->name : "Unknown", phdr->p_offset,
                 phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz, buffer, phdr->p_align);
#else
            LOGD("%-20s %08x %08x %08x %08x %08x %s   %x", NULL != type ? type->name : "Unknown", phdr->p_offset,
                 phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz, buffer, phdr->p_align);
#endif
        }
    }
}


void ElfReader::dumpDynamicSegment()
{
    ElfW(Phdr) * phdr = findSegmentByType(PT_DYNAMIC);
    if (NULL != phdr)
    {
        const Elf_Dynamic_Type_Map *type;
        ElfW(Dyn) * dyn = reinterpret_cast<ElfW(Dyn) *>(this->bias + phdr->p_vaddr);
        LOGD("dump dynamic segment:");
        LOGD("Type                 Value/Addr");
        for ( ; DT_NULL != dyn->d_tag; dyn++)
        {
            type = map_elf_dynamic_type(dyn->d_tag);
#if defined(__LP64__)
            LOGD("%-20s %llx", NULL != type ? type->name : "Unknown", dyn->d_un.d_val);
#else
            LOGD("%-20s %x", NULL != type ? type->name : "Unknown", dyn->d_un.d_val);
#endif
        }
    }
}


void ElfReader::dumpDynamicRel()
{
#if defined(USE_RELA)
    ElfW(Rela) * curr;
    ElfW(Rela) * rel    = this->rel;
    ElfW(Rela) * pltRel = this->pltRel;
#else
    ElfW(Rel) * curr;
    ElfW(Rel) * rel    = this->rel;
    ElfW(Rel) * pltRel = this->pltRel;
#endif
    ElfW(Addr) r_offset;
    ElfW(Xword) r_info;
    ElfW(Xword) symIdx;
    ElfW(Sym) * sym;
    const char *name;
    const Elf_Dynamic_Rel_Type_Map *map;
    if (NULL != rel)
    {
#if defined(USE_RELA)
        LOGD("dump rela info:");
#else
        LOGD("dump rel info:");
#endif
#if defined(__LP64__)
        LOGD("Offset           Info             Type                                Sym.value        Sym.name");
#else
        LOGD("Offset   Info     Type                                Sym.value Sym.name");
#endif
        for (ElfW(Word) i = 0; i < this->relCount; i++)
        {
            curr     = rel + i;
            r_offset = curr->r_offset;
            r_info   = curr->r_info;
            symIdx   = ELF_R_SYM(r_info);
            sym      = this->symTable + symIdx;
            name     = this->strTable + sym->st_name;
            map      = map_elf_dynamic_rel_type(ELF_R_TYPE(r_info));
#if defined(__LP64__)
            LOGD("%016llx %016llx %-35s %016llx %s", r_offset, r_info, NULL != map ? map->name : "Unknown", sym->st_value, name);
#else
            LOGD("%08x %08x %-35s %08x %s", r_offset, r_info, NULL != map ? map->name : "Unknown", sym->st_value, name);
#endif
        }
    }

    if (NULL != pltRel)
    {
#if defined(USE_RELA)
        LOGD("dump plt.rela info:");
#else
        LOGD("dump plt.rel info:");
#endif
#if defined(__LP64__)
        LOGD("Offset           Info             Type                                Sym.value        Sym.name");
#else
        LOGD("Offset   Info     Type                                Sym.value Sym.name");
#endif
        for (ElfW(Word) i = 0; i < this->pltRelCount; i++)
        {
            curr     = pltRel + i;
            r_offset = curr->r_offset;
            r_info   = curr->r_info;
            symIdx   = ELF_R_SYM(r_info);
            sym      = this->symTable + symIdx;
            name     = this->strTable + sym->st_name;
            map      = map_elf_dynamic_rel_type(ELF_R_TYPE(r_info));
#if defined(__LP64__)
            LOGD("%016llx %016llx %-35s %016llx %s", r_offset, r_info, NULL != map ? map->name : "Unknown", sym->st_value, name);
#else
            LOGD("%08x %08x %-35s %08x %s", r_offset, r_info, NULL != map ? map->name : "Unknown", sym->st_value, name);
#endif
        }
    }
}


int ElfReader::verifyElfHeader()
{
    if (0 != memcmp(ehdr->e_ident, ELFMAG, SELFMAG))
    {
        LOGE("wrong elf format for magic");
        return -1;
    }

    int elf_class = ehdr->e_ident[EI_CLASS];
#if defined(__LP64__)
    if (ELFCLASS64 != elf_class)
    {
        LOGE("wrong elf bit format for 64bit elf (%d)", elf_class);
        return -1;
    }
#else
    if (ELFCLASS32 != elf_class)
    {
        LOGE("wrong elf bit format for 32bit elf (%d)", elf_class);
        return false;
    }
#endif

    if (ELFDATA2LSB != ehdr->e_ident[EI_DATA])
    {
        LOGE("wrong elf format for not little-endian (%d)", ehdr->e_ident[EI_DATA]);
        return -1;
    }

    if (ET_DYN != ehdr->e_type)
    {
        LOGE("wrong elf format for e_type (%d)", ehdr->e_type);
        return -1;
    }

    if (EV_CURRENT != ehdr->e_version)
    {
        LOGE("wrong elf format for e_version (%d)", ehdr->e_version);
        return -1;
    }

    if (ehdr->e_machine != elfTargetMachine())
    {
        LOGE("wrong elf format for e_machine (%d)", ehdr->e_machine);
        return -1;
    }

    return 0;
}


int ElfReader::elfTargetMachine()
{
#if defined(__arm__)
    return EM_ARM;
#elif defined(__aarch64__)
    return EM_AARCH64;
#else
#error "unknown architecture!"
#endif
}


ElfW(Addr) ElfReader::getSegmentBaseAddress()
{
    ElfW(Phdr) * phdr = findSegmentByType(PT_LOAD);
    if (NULL != phdr)
    {
        return this->start + phdr->p_offset - phdr->p_vaddr;
    }
    return 0;
}

ElfW(Phdr) * ElfReader::findSegmentByType(ElfW(Word) type)
{
    if (phdrNum > 0)
    {
        ElfW(Phdr) * phdr = NULL;
        for (ElfW(Half) i = 0; i < phdrNum; i++)
        {
            phdr = this->phdr + i;
            if (phdr->p_type == type)
            {
                return phdr;
            }
        }
    }
    return NULL;
}

ElfW(Phdr) * ElfReader::findSegmentByAddress(void *addr)
{
    if (phdrNum > 0)
    {
        ElfW(Addr) start;
        ElfW(Addr) end;
        ElfW(Addr) target = reinterpret_cast<ElfW(Addr)>(addr);
        ElfW(Phdr) * phdr = NULL;
        for (ElfW(Half) i = 0; i < phdrNum; i++)
        {
            phdr  = this->phdr + i;
            start = PAGE_START(this->bias + phdr->p_vaddr);
            end   = PAGE_END(this->bias + phdr->p_vaddr + phdr->p_memsz);
            if ((target >= start) && (target <= end))
            {
                return phdr;
            }
        }
    }
    return NULL;
}

int ElfReader::parseDynamicSegment()
{
    ElfW(Phdr) * phdr = findSegmentByType(PT_DYNAMIC);
    if (NULL != phdr)
    {
        ElfW(Dyn) * dyn = reinterpret_cast<ElfW(Dyn) *>(this->bias + phdr->p_vaddr);
        for ( ; DT_NULL != dyn->d_tag; dyn++)
        {
            switch (dyn->d_tag)
            {
            case DT_STRTAB:
                this->strTable = reinterpret_cast<const char *>(this->bias + dyn->d_un.d_ptr);
                break;

            case DT_SYMTAB:
                this->symTable = reinterpret_cast<ElfW(Sym) *>(this->bias + dyn->d_un.d_ptr);
                break;

#if !defined(USE_RELA)
            case DT_PLTREL:
                if (DT_RELA == dyn->d_un.d_val)
                {
                    LOGE("unsupported DT_PLTREL in %s, expected DT_REL", this->moduleName);
                    return -1;
                }
                break;

            case DT_RELA:
                LOGE("unsupported DT_RELA in %s", this->moduleName);
                return -1;

            case DT_RELASZ:
                LOGE("unsupported DT_RELASZ in %s", this->moduleName);
                return -1;

            case DT_RELSZ:
                this->relCount = dyn->d_un.d_val / sizeof(ElfW(Rel));
                break;

            case DT_PLTRELSZ:
                this->pltRelCount = dyn->d_un.d_val / sizeof(ElfW(Rel));
                break;

            case DT_REL:
                this->rel = reinterpret_cast<ElfW(Rel) *>(this->bias + dyn->d_un.d_ptr);
                break;

            case DT_JMPREL:
                this->pltRel = reinterpret_cast<ElfW(Rel) *>(this->bias + dyn->d_un.d_ptr);
                break;
#endif
#if defined(USE_RELA)
            case DT_PLTREL:
                if (DT_REL == dyn->d_un.d_val)
                {
                    LOGE("unsupported DT_PLTREL in %s, expected DT_RELA", this->moduleName);
                    return -1;
                }
                break;

            case DT_REL:
                LOGE("unsupported DT_REL in %s", this->moduleName);
                return -1;

            case DT_RELSZ:
                LOGE("unsupported DT_RELSZ in %s", this->moduleName);
                return -1;

            case DT_RELASZ:
                this->relCount = dyn->d_un.d_val / sizeof(ElfW(Rela));
                break;

            case DT_PLTRELSZ:
                this->pltRelCount = dyn->d_un.d_val / sizeof(ElfW(Rela));
                break;

            case DT_RELA:
                this->rel = reinterpret_cast<ElfW(Rela) *>(this->bias + dyn->d_un.d_ptr);
                break;

            case DT_JMPREL:
                this->pltRel = reinterpret_cast<ElfW(Rela) *>(this->bias + dyn->d_un.d_ptr);
                break;
#endif
            case DT_HASH:
                this->nbucket = reinterpret_cast<uint32_t *>(this->bias + dyn->d_un.d_ptr)[0];
                this->nchain  = reinterpret_cast<uint32_t *>(this->bias + dyn->d_un.d_ptr)[1];
                this->bucket  = reinterpret_cast<uint32_t *>(this->bias + dyn->d_un.d_ptr + 8);
                this->chain   = reinterpret_cast<uint32_t *>(this->bias + dyn->d_un.d_ptr + 8 + this->nbucket * 4);
                break;

            case DT_GNU_HASH:
                this->gnuNBucket     = reinterpret_cast<uint32_t *>(this->bias + dyn->d_un.d_ptr)[0];
                this->gnuSymndx      = reinterpret_cast<uint32_t *>(this->bias + dyn->d_un.d_ptr)[1];
                this->gnuMaskwords   = reinterpret_cast<uint32_t *>(this->bias + dyn->d_un.d_ptr)[2];
                this->gnuShift2      = reinterpret_cast<uint32_t *>(this->bias + dyn->d_un.d_ptr)[3];
                this->gnuBloomFilter = reinterpret_cast<ElfW(Addr) *>(this->bias + dyn->d_un.d_ptr + 16);
                this->gnuBucket      = reinterpret_cast<uint32_t *>(this->gnuBloomFilter + this->gnuMaskwords);
                this->gnuChain       = this->gnuBucket + this->gnuNBucket - this->gnuSymndx;

                if (!powerof2(this->gnuMaskwords))
                {
                    LOGE("invalid maskwords for gnu_hash in %s: 0x%x, expecting power to two", this->moduleName, this->gnuMaskwords);
                    return -1;
                }
                this->isGNUHash = true;
                --this->gnuMaskwords;
                break;

            default:
                break;
            }
        }

        if ((NULL == this->strTable) || (NULL == this->symTable))
        {
            LOGE("no DT_STRTAB or DT_SYMTAB found in %s", this->moduleName);
            return -1;
        }

        return 0;
    }
    return -1;
}


int ElfReader::findSymbolByName(const char *symbol, ElfW(Sym) **sym, uint32_t *symidx)
{
    if (this->isGNUHash)
    {
        if (0 == gnuLookup(symbol, sym, symidx))
        {
            return 0;
        }

        ElfW(Sym) * curr;
        for (uint32_t i = 0; i < this->gnuSymndx; i++)
        {
            curr = this->symTable + i;
            if (0 == strcmp(this->strTable + curr->st_name, symbol))
            {
                *symidx = i;
                *sym    = curr;
                return 0;
            }
        }
        LOGE("not found %s in %s before gnu symbol index %d", symbol, this->moduleName, this->gnuSymndx);
        return -1;
    }
    return elfLookup(symbol, sym, symidx);
}


int ElfReader::elfLookup(const char *symbol, ElfW(Sym) **sym, uint32_t *symidx)
{
    if ((NULL == this->bucket) || (NULL == this->chain))
    {
        LOGE("elfLookup: no hash info found");
        return -1;
    }

    uint32_t hash = ElfHooker::elf_hash(symbol);
    ElfW(Sym) * ret;
    for (uint32_t n = this->bucket[hash % this->nbucket]; 0 != n; n = this->chain[n])
    {
        ret = this->symTable + n;
        if (0 == strcmp(this->strTable + ret->st_name, symbol))
        {
            *symidx = n;
            *sym    = ret;
            return 0;
        }
    }
    LOGE("elfLookup: not found symbol %s in %s", symbol, this->moduleName);
    return -1;
}


int ElfReader::gnuLookup(const char *symbol, ElfW(Sym) **sym, uint32_t *symidx)
{
    uint32_t hash = ElfHooker::gnu_hash(symbol);
    uint32_t h2   = hash >> this->gnuShift2;

    uint32_t bloom_mask_bits = sizeof(ElfW(Addr)) * 8;
    uint32_t word_num        = (hash / bloom_mask_bits) & this->gnuMaskwords;

    ElfW(Addr) bloom_word = this->gnuBloomFilter[word_num];

    *sym    = NULL;
    *symidx = 0;
    if ((1 & (bloom_word >> (hash % bloom_mask_bits)) & (bloom_word >> (h2 % bloom_mask_bits))) == 0)
    {
        LOGE("gnuLookup: not found symbol %s in %s", symbol, this->moduleName);
        return -1;
    }

    uint32_t n = this->gnuBucket[hash % this->gnuNBucket];
    if (n == 0)
    {
        LOGE("gnuLookup: not found symbol %s in %s", symbol, this->moduleName);
        return -1;
    }

    do
    {
        ElfW(Sym) * s = this->symTable + n;
        if ((((this->gnuChain[n] ^ hash) >> 1) == 0) && (strcmp((this->strTable + s->st_name), symbol) == 0))
        {
            *symidx = n;
            *sym    = s;
            return 0;
        }
    } while ((this->gnuChain[n++] & 1) == 0);

    LOGE("gnuLookup: not found symbol %s in %s", symbol, this->moduleName);
    return -1;
}


int ElfReader::hookInternally(void *addr, void *new_func, void **old_func)
{
    if (*(void **)addr == new_func)
    {
        LOGD("already been hooked");
        return 0;
    }


    ElfW(Phdr) * phdr = findSegmentByAddress(addr);
    if (NULL == phdr)
    {
        LOGE("failed to find segment for address %p in %s", addr, this->moduleName);
        return -1;
    }

    int prot = PFLAGS_TO_PROT(phdr->p_flags);
    LOGD("prot p_flags = %d", prot);
    prot &= ~PROT_EXEC;
    prot |= PROT_WRITE;
    ElfW(Addr) target = reinterpret_cast<ElfW(Addr)>(addr);
    void *start = reinterpret_cast<void *>(PAGE_START(target));
    if (0 != mprotect(start, PAGE_SIZE, prot))
    {
        LOGE("failed to mprotect %p in %s", start, this->moduleName);
        return -1;
    }
    *old_func      = *(void **)addr;
    *(void **)addr = new_func;

    LOGD("plt addr = %lx", addr);
    ElfHooker::clear_cache(start, PAGE_SIZE);
    return 0;
}
