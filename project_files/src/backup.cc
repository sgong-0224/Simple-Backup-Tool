#include "backup.h"
#include "utils.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

void full_backup(const string& backup_src, const string& backup_home) 
{
    try {
        filesystem::path backupSrc(backup_src);
        filesystem::path backupHome(backup_home);
        if( likely( !filesystem::exists(backupHome) ) )
            filesystem::create_directories(backupHome);

        // Create backup directory & metadata
        size_t cur_backup_idx=find_backup_cnt(backupHome)+1;
        stringstream ss;
        ss << cur_backup_idx  << "_data";
        string cur_backup_path;
        ss >> cur_backup_path;
        filesystem::create_directories(backupHome/cur_backup_path);
        ss.clear();
        ss << cur_backup_idx  << "_metadata-full";
        string cur_backup_metadata;
        ss >> cur_backup_metadata;
        fstream metaFile(backupHome/cur_backup_metadata, ios::out|ios::trunc);

        // Copy files
        for (const auto &entry : filesystem::recursive_directory_iterator(backupSrc)) {
            const auto &path = entry.path();
            auto relativePath = path.lexically_relative(backupSrc);
            filesystem::copy( path, backupHome/cur_backup_path/relativePath,
        filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive);
            // fix metadata
            filesystem::last_write_time(
                backupHome/cur_backup_path/relativePath,
                filesystem::last_write_time(path)
            );
        }

        // Write metadata
        for (const auto &entry : filesystem::recursive_directory_iterator(backupSrc)) {
            const auto &path = entry.path().lexically_relative(backupSrc);
            metaFile << "+ " << path.string() << "\n";
        }
        metaFile.close();
        cout << "Full backup completed successfully.\n";
    } catch (const filesystem::filesystem_error &err) {
        cerr << "Filesystem error: " << err.what() << '\n';
    }
}

void diff_backup(const string& backup_src, const string& backup_home) 
{
    try {
        filesystem::path backupSrc(backup_src);
        filesystem::path backupHome(backup_home);

        // Create backup directory & metadata
        if( unlikely( !filesystem::exists(backupHome) ) ){
            filesystem::create_directories(backupHome);
            full_backup(backup_src, backup_home);
            return;
        }
        size_t prev_backup_idx= find_backup_cnt(backupHome);
        if( unlikely(prev_backup_idx==0) ){
            full_backup(backup_src, backup_home);
            return;
        }
        size_t cur_backup_idx=prev_backup_idx+1;
        stringstream ss;
        ss << cur_backup_idx  << "_data";
        string cur_backup_path, full_backup_path, cur_backup_metadata;
        ss >> cur_backup_path;
        filesystem::create_directories(backupHome/cur_backup_path);
        ss.clear();
        ss << cur_backup_idx  << "_metadata-diff";
        ss >> cur_backup_metadata;
        fstream metaFile(backupHome/cur_backup_metadata, ios::out|ios::trunc );
        
        // Find base backup
        size_t full_backup_idx=cur_backup_idx;
        auto backups = backup_info(backupHome);
        if( likely( !backups[full_backup_idx].second ) ){
            while(full_backup_idx!=0){
                if( unlikely(backups[full_backup_idx].second) )
                    break;
                --full_backup_idx;
            }
        }
        ss.clear();
        ss << full_backup_idx << "_data";
        ss >> full_backup_path;

        // Copy files
        #pragma omp parallel num_threads(4)
        {
        for (const auto &entry : filesystem::recursive_directory_iterator(backupSrc)) {
            const auto &path = entry.path();
            auto relativePath = path.lexically_relative(backupSrc);
            auto prevPath = backupHome/full_backup_path/relativePath;
            if( unlikely(
                !filesystem::exists(prevPath)
            ||  filesystem::last_write_time(path)!=filesystem::last_write_time(prevPath)
            ) ){
                filesystem::copy( path, backupHome/cur_backup_path/relativePath,
        filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive);
                // fix metadata
                filesystem::last_write_time(
                    backupHome/cur_backup_path/relativePath,
                filesystem::last_write_time(path)
                );
            }
        }
        }

        // Write metadata && identify changes
        #pragma omp parallel num_threads(4)
        {
        for (const auto &entry : filesystem::recursive_directory_iterator(backupHome/cur_backup_path)) {
            const auto &path = entry.path().lexically_relative(backupHome/cur_backup_path);
            metaFile << "+ " << path.string() << "\n";
        }
        }
        
        #pragma omp parallel num_threads(4)
        {
        for (const auto &entry : filesystem::recursive_directory_iterator(backupHome/full_backup_path)) {
            const auto &path = entry.path();
            auto relativePath=path.lexically_relative(backupHome/full_backup_path);
            if( unlikely( !filesystem::exists(backupSrc/relativePath) ))
                metaFile << "- "  << relativePath.string() << "\n";
        }
        }
        metaFile.close();
        cout << "Differential backup completed successfully.\n";
    } catch (const filesystem::filesystem_error &err) {
        cerr << "Filesystem error: " << err.what() << '\n';
    }
}