#ifndef _BYTE_LOADER_H
#define _BYTE_LOADER_H

#include <vector>
#include <bits/unique_ptr.h>


#include "dex_header.h"

typedef void * (*org1_artDexFileOpenMemory19)(unsigned char const *base, unsigned int size,
                                              std::string const& location,
                                              unsigned int location_checksum, void *mem_map);
typedef void * (*org_artDexFileOpenMemory21)(const uint8_t *base, size_t size,
                                             const std::string& location, uint32_t location_checksum,
                                             void *mem_map, std::string *error_msg);

typedef void * (*org_artDexFileOpenMemory22)(const uint8_t *base, size_t size,
                                             const std::string& location, uint32_t location_checksum,
                                             void *mem_map, const void *oat_file, std::string *error_msg);

typedef void * (*org_artDexFileOpenMemory23)(void *retcookie, const uint8_t *base,
                                             size_t size, const std::string& location,
                                             uint32_t location_checksum, void *mem_map, const void *oat_dex_file, std::string *error_msg);

typedef void * (*orig_OpenCommon)(void *retcookie, const uint8_t *base, size_t size,
                                  const std::string& location, uint32_t location_checksum,
                                  const void *oat_dex_file, bool verify,
                                  bool verify_checksum, std::string *error_msg, void *verify_result);

void *mem_loadDex_byte19(void *arthandler, const char *base, size_t size);

void *mem_loadDex_byte21(void *arthandler, const char *base, size_t size);
void *mem_loadDex_byte22(void *arthandler, const char *base, size_t size);

void *mem_loadDex_byte23(void *arthandler, const char *base, size_t size);

void *mem_loadDex_byte24(void *arthandler, const char *base, size_t size);

void *mem_loadDex_byte26(void *arthandler, const char *base, size_t size);


#endif
