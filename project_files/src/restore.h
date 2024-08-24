// restore.h
#ifndef RESTORE_H
#define RESTORE_H
#include <string>
#include <set>
#include <filesystem>
using namespace std;
size_t select_restore_index();
void copy_restore_data(const filesystem::path& restore_src_dir,const filesystem::path& restore_dest_dir, const set<filesystem::path>& deleted);
void restore(const string& backup_home, const string& restore_dest);
#endif // RESTORE_H