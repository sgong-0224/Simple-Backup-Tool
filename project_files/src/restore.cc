#include "restore.h"
#include "definition.h"
#include "utils.h"
#include "delta.h"
#include <iostream>
#include <set>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

size_t select_restore_index()
{
    size_t select;
    std::cout << "Select the backup file(s) to restore: ";
    std::cin >> select;
    return select;
}

void copy_restore_data(
    const std::filesystem::path& restore_src_dir, 
    const std::filesystem::path& restore_dest_dir,
    const std::set<std::filesystem::path>& deleted
)
{
    for (const auto &entry : std::filesystem::recursive_directory_iterator(restore_src_dir)) {
        const auto &path = entry.path();
        auto relativePath = path.lexically_relative(restore_src_dir);
        if( likely( deleted.count(path)==0 ) ){
            std::filesystem::copy( path, restore_dest_dir/relativePath,
                std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
            std::filesystem::last_write_time(restore_dest_dir/relativePath,
                std::filesystem::last_write_time(path));
        }
    }
}

void restore(const std::string& backup_home, const std::string& restore_dest)
{
    try {
        std::filesystem::path restoreSrc(backup_home);
        std::filesystem::path restoreDest(restore_dest);
        if( unlikely( !std::filesystem::exists(restoreDest) ) )
            std::filesystem::create_directories(restoreDest);
        
        // Select files to restore, skipping deleted files
        auto  restore_info = backup_info(restoreSrc, true);
        size_t idx = select_restore_index(), full=idx;
        fflush(stdin);
        std::set<std::filesystem::path> deleted; 
        // Parsing patch info
        std::list<patch_struct> patch;
        uint64_t old_chunk_index;
        std::streamoff length;

        // restore base data
        if( likely( !restore_info[idx].second ) ){
            while(full!=0){
                if( unlikely(restore_info[full].second) )
                    break;
                --full;
            }
        }        
        std::string metadata_file, full_data, diff_data, cur_line, cur_file;
        std::stringstream ss;
        bool isfull = idx==full;
        if( likely(!isfull) )
            ss << idx << "_metadata-diff";
        else
            ss << idx << "_metadata-full";
        ss >> metadata_file;
        std::fstream metaFile(restoreSrc/metadata_file,std::ios::in);
        ss.clear();
        ss.str("");
        ss << full << "_data";
        ss >> full_data;
        ss.clear();
        ss.str("");
        ss << idx << "_data";
        ss >> diff_data;

        while(getline(metaFile,cur_line)){
            if( unlikely(cur_line[0]=='-') )
                deleted.emplace( restoreSrc/full_data/cur_line.substr(2) );
            else if( unlikely(cur_line[0]=='M') ){
                cur_file = cur_line.substr(2);
                deleted.emplace( restoreSrc/full_data/cur_line.substr(2) ); // not to copy
            }else if( unlikely(cur_line[0]=='P') ){
                std::stringstream ss(cur_line.substr(2));
                std::list<chunk_patchlen> patches;
                while(!ss.eof()){
                    ss >> old_chunk_index >> length;
                    patches.push_back(std::make_pair(old_chunk_index,length));
                }
                // Patch: filename + list<idx,len>
                patch.push_back(patch_struct{cur_file, patches});
            }else
                continue;
        }
        copy_restore_data(
            restoreSrc/restore_info[full].first, 
            restoreDest, deleted
        );
        if( likely(!isfull) ){
            copy_restore_data(
                restoreSrc/restore_info[idx].first, 
                restoreDest, deleted
            );
        }
        // TODO: patch
        std::fstream oldfile, newfile, patch_filename;
        for(auto file:patch){
            // patch filename = file.filename
            oldfile.open(restoreSrc/full_data/file.filename,std::ios::in|std::ios::binary);
            patch_filename.open( restoreSrc/diff_data/file.filename, std::ios::in|std::ios::binary );
            newfile.open(restoreDest/file.filename, std::ios::out|std::ios::binary);
            patch_file( oldfile, newfile, patch_filename, file.chunk_len );
            patch_filename.close();
            oldfile.close();
            newfile.close();
        }
        metaFile.close();
        std::cout << "Restore from backup Index " << idx << " completed successfully.\n";
    } catch (const std::filesystem::filesystem_error &err) {
        std::cerr << "Filesystem error: " << err.what() << '\n';
    }
    
}
