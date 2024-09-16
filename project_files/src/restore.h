// restore.h
#ifndef RESTORE_H
#define RESTORE_H
#include <string>
#include <set>
#include <filesystem>
size_t select_restore_index();
void copy_restore_data(const std::filesystem::path& restore_src_dir,const std::filesystem::path& restore_dest_dir, const std::set<std::filesystem::path>& deleted);
void restore(const std::string& backup_home, const std::string& restore_dest);
#endif // RESTORE_H