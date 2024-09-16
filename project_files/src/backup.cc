#include "backup.h"
#include "utils.h"
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

void full_backup(const std::string& backup_src, const std::string& backup_home) 
{
    try {
        std::filesystem::path backupSrc(backup_src);
        std::filesystem::path backupHome(backup_home);
        if( likely( !std::filesystem::exists(backupHome) ) )
            std::filesystem::create_directories(backupHome);

        // Create backup directory & metadata
        size_t cur_backup_idx=find_backup_cnt(backupHome)+1;
        std::stringstream ss;
        ss << cur_backup_idx  << "_data";
        std::string cur_backup_path;
        ss >> cur_backup_path;
        std::filesystem::create_directories(backupHome/cur_backup_path);
        ss.clear();
        ss << cur_backup_idx  << "_metadata-full";
        std::string cur_backup_metadata;
        ss >> cur_backup_metadata;
        std::fstream metaFile(backupHome/cur_backup_metadata, std::ios::out|std::ios::trunc);

        // Copy files
        for (const auto &entry : std::filesystem::recursive_directory_iterator(backupSrc)) {
            const auto &path = entry.path();
            auto relativePath = path.lexically_relative(backupSrc);
            std::filesystem::copy( path, backupHome/cur_backup_path/relativePath,
        std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
            // fix metadata
            std::filesystem::last_write_time(
                backupHome/cur_backup_path/relativePath,
                std::filesystem::last_write_time(path)
            );
        }

        // Write metadata
        for (const auto &entry : std::filesystem::recursive_directory_iterator(backupSrc)) {
            const auto &path = entry.path().lexically_relative(backupSrc);
            metaFile << "+ " << path.string() << "\n";
        }
        metaFile.close();
        std::cout << "Full backup completed successfully.\n";
    } catch (const std::filesystem::filesystem_error &err) {
        std::cerr << "Filesystem error: " << err.what() << '\n';
    }
}

void diff_backup(const std::string& backup_src, const std::string& backup_home) 
{
    try {
        std::filesystem::path backupSrc(backup_src);
        std::filesystem::path backupHome(backup_home);

        // Create backup directory & metadata
        if( unlikely( !std::filesystem::exists(backupHome) ) ){
            std::filesystem::create_directories(backupHome);
            full_backup(backup_src, backup_home);
            return;
        }
        size_t prev_backup_idx= find_backup_cnt(backupHome);
        if( unlikely(prev_backup_idx==0) ){
            full_backup(backup_src, backup_home);
            return;
        }
        size_t cur_backup_idx=prev_backup_idx+1;
        std::stringstream ss;
        ss << cur_backup_idx  << "_data";
        std::string cur_backup_path, full_backup_path, cur_backup_metadata;
        ss >> cur_backup_path;
        std::filesystem::create_directories(backupHome/cur_backup_path);
        ss.clear();
        ss << cur_backup_idx  << "_metadata-diff";
        ss >> cur_backup_metadata;
        std::fstream metaFile(backupHome/cur_backup_metadata, std::ios::out|std::ios::trunc );
        
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
        ss.str("");
        ss << full_backup_idx << "_data";
        ss >> full_backup_path;

        // Copy files
        for (const auto &entry : std::filesystem::recursive_directory_iterator(backupSrc)) {
            const auto &path = entry.path();
            auto relativePath = path.lexically_relative(backupSrc);
            auto prevPath = backupHome/full_backup_path/relativePath;
            if( unlikely(
                !std::filesystem::exists(prevPath)
            ||  std::filesystem::last_write_time(path)!=std::filesystem::last_write_time(prevPath)
            ) ){
                std::filesystem::copy( path, backupHome/cur_backup_path/relativePath,
        std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
                // fix metadata
                std::filesystem::last_write_time(
                    backupHome/cur_backup_path/relativePath,
                std::filesystem::last_write_time(path)
                );
            }
        }

        // Write metadata && identify changes
        for (const auto &entry : std::filesystem::recursive_directory_iterator(backupHome/cur_backup_path)) {
            const auto &path = entry.path().lexically_relative(backupHome/cur_backup_path);
            metaFile << "+ " << path.string() << "\n";
        }
        for (const auto &entry : std::filesystem::recursive_directory_iterator(backupHome/full_backup_path)) {
            const auto &path = entry.path();
            auto relativePath=path.lexically_relative(backupHome/full_backup_path);
            if( unlikely( !std::filesystem::exists(backupSrc/relativePath) ))
                metaFile << "- "  << relativePath.string() << "\n";
        }
        metaFile.close();
        std::cout << "Differential backup completed successfully.\n";
    } catch (const std::filesystem::filesystem_error &err) {
        std::cerr << "Filesystem error: " << err.what() << '\n';
    }
}
