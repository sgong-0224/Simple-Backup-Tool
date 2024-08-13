#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <filesystem>
using namespace std;
size_t find_backup_cnt(filesystem::path& path);
vector<filesystem::path> backup_info(filesystem::path& path);
#endif // UTILS_H