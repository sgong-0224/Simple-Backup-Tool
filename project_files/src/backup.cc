#include "backup.h"
#include "utils.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
using namespace std;

void full_backup(const string& backup_src, const string& backup_home) {
    try {
        filesystem::path backupSrc(backup_src);
        filesystem::path backupHome(backup_home);
        if(!filesystem::exists(backupHome))
            filesystem::create_directories(backupHome);

        // Create backup directory & metadata
        size_t cur_backup_idx=find_backup_cnt(backupHome)+1;
        stringstream ss;
        ss << cur_backup_idx  << "_data";
        string cur_backup_path;
        ss >> cur_backup_path;
        filesystem::create_directories(backupHome/cur_backup_path);
        ss.clear();
        ss << cur_backup_idx  << "_metadata";
        string cur_backup_metadata;
        ss >> cur_backup_metadata;
        ofstream metaFile(backupHome/cur_backup_metadata);

        // Copy files
        for (const auto &entry : filesystem::recursive_directory_iterator(backupSrc)) {
            const auto &path = entry.path();
            auto relativePath = path.lexically_relative(backupSrc);
            filesystem::copy( path, backupHome/cur_backup_path / relativePath,
        filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive);
        }


        // Write metadata
        time_t now = time(0);
        char* dt = ctime(&now);
        metaFile << "full backup @ " << dt << "\n";
        for (const auto &entry : filesystem::recursive_directory_iterator(backupSrc)) {
            const auto &path = entry.path();
            metaFile << path.string() << "\n";
        }
        metaFile.close();
        cout << "Full backup completed successfully.\n";
    } catch (const filesystem::filesystem_error &err) {
        cerr << "Filesystem error: " << err.what() << '\n';
    }
}