#ifndef UTILS_H
#define UTILS_H
#include "definition.h"
#include <vector>
#include <string>
#include <filesystem>
size_t find_backup_cnt(std::filesystem::path& path);
size_t find_base_full_backup(std::vector<backup_pair> infos);
std::vector<backup_pair>  backup_info(std::filesystem::path& path, bool print=false);
std::string to_time_t(std::filesystem::file_time_type tp);
#endif // UTILS_H