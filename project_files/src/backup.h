#ifndef BACKUP_H
#define BACKUP_H
#include <string>
void full_backup(const std::string& backup_src, const std::string& backup_home);
void diff_backup(const std::string& backup_src, const std::string& backup_home);
#endif // BACKUP_H