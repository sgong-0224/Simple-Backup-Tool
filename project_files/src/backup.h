#ifndef BACKUP_H
#define BACKUP_H
#include <string>
using namespace std;
void full_backup(const string& backup_src, const string& backup_home);
void diff_backup(const string& backup_src, const string& backup_home);
#endif // BACKUP_H