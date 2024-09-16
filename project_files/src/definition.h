#ifndef DEFINITION_H
#define DEFINITION_H
#include <cstdint>
#include <vector>
#include <filesystem>
#include <ios>
#include <list>
// MACRO DEFINITION
// branch
#define likely(x)    __builtin_expect((x), 1)
#define unlikely(x)  __builtin_expect((x), 0)
// TYPE DEFINITION
typedef std::pair<std::filesystem::path,bool> backup_pair;
typedef std::pair<uint64_t,std::streamoff> chunk_patchlen;
typedef struct Delta_data {
    uint64_t matching_chunk_cnt;
    std::streamoff patch_length;
} delta_struct;
typedef struct Patch_data {
    std::string filename;
    std::list<chunk_patchlen> chunk_len; // <matching_chunk_cnt, patch_length>
} patch_struct;
// CONST DEFINITION
const size_t LARGE_FILE_SIZE_MiB=1;     // TODO: 1 MiB
const size_t CHUNK_SIZE = 16384;        // 16 KiB
#endif