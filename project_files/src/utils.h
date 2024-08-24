#ifndef UTILS_H
#define UTILS_H
#include <vector>
#include <filesystem>

// MACRO DEFINITION
// branch
#define likely(x)    __builtin_expect((x), 1)
#define unlikely(x)  __builtin_expect((x), 0)

using namespace std;
typedef pair<filesystem::path,bool> backup_pair;

size_t find_backup_cnt(filesystem::path& path);
size_t find_base_full_backup(vector<backup_pair> infos);
vector<backup_pair>  backup_info(filesystem::path& path, bool print=false);
string to_time_t(filesystem::file_time_type tp);

#endif // UTILS_H