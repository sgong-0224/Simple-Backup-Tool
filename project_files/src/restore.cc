#include "restore.h"
#include "utils.h"
#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
using namespace std;

size_t select_restore_index()
{
    size_t select;
    cout << "Select the backup file(s) to restore: ";
    cin >> select;
    return select;
}

void copy_restore_data(
    const filesystem::path& restore_src_dir, 
    const filesystem::path& restore_dest_dir,
    const set<filesystem::path>& deleted
)
{
    for (const auto &entry : filesystem::recursive_directory_iterator(restore_src_dir)) {
        const auto &path = entry.path();
        auto relativePath = path.lexically_relative(restore_src_dir);
        if( likely( deleted.count(path)==0 ) ){
            filesystem::copy( path, restore_dest_dir/relativePath,
                filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive);
            filesystem::last_write_time(restore_dest_dir/relativePath,
                filesystem::last_write_time(path));
        }
    }
}

void restore(const string& backup_home, const string& restore_dest)
{
    try {
        filesystem::path restoreSrc(backup_home);
        filesystem::path restoreDest(restore_dest);
        if( unlikely( !filesystem::exists(restoreDest) ) )
            filesystem::create_directories(restoreDest);
        
        // Select files to restore, skipping deleted files
        auto  restore_info = backup_info(restoreSrc, true);
        size_t idx = select_restore_index(), full=idx;
        fflush(stdin);
        set<filesystem::path> deleted;

        // restore base data
        if( likely( !restore_info[idx].second ) ){
            while(full!=0){
                if( unlikely(restore_info[full].second) )
                    break;
                --full;
            }
        }        
        string metadata_file, full_data, cur_line;
        stringstream ss;
        bool isfull = idx==full;
        if( likely(!isfull) )
            ss << idx << "_metadata-diff";
        else
            ss << idx << "_metadata-full";
        ss >> metadata_file;
        fstream metaFile(restoreSrc/metadata_file,ios::in);
        ss.clear();
        ss << full << "_data";
        ss >> full_data;
        while(getline(metaFile,cur_line)){
            if( unlikely(cur_line[0]=='-') )
                deleted.emplace( restoreSrc/full_data/cur_line.substr(2) );
            else
                continue;
        }
        metaFile.close();
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
        cout << "Restore from backup Index " << idx << " completed successfully.\n";
    } catch (const filesystem::filesystem_error &err) {
        cerr << "Filesystem error: " << err.what() << '\n';
    }
    
}