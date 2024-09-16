#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <filesystem>

// MACRO DEFINITION
// branch
#define likely(x)    __builtin_expect((x), 1)
#define unlikely(x)  __builtin_expect((x), 0)

typedef std::pair<std::filesystem::path,bool> backup_pair;

size_t find_backup_cnt(std::filesystem::path& path);
size_t find_base_full_backup(std::vector<backup_pair> infos);
std::vector<backup_pair>  backup_info(std::filesystem::path& path, bool print=false);
std::string to_time_t(std::filesystem::file_time_type tp);

#endif // UTILS_H