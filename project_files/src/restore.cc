#include "restore.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
using namespace std;

size_t select_restore_index()
{
    size_t select;
    cout << "Select the backup file(s) to restore: ";
    cin >> select;
    return select;
}

void full_restore(const string& backup_home, const string& restore_dest)
{
    try {
        filesystem::path restoreSrc(backup_home);
        filesystem::path restoreDest(restore_dest);
        if(!filesystem::exists(restoreDest))
            filesystem::create_directories(restoreDest);
        // Select files to restore
        vector<filesystem::path> restore_path = backup_info(restoreSrc);
        size_t idx = select_restore_index();
        fflush(stdin);
        // Copy files to <path/relativepath>
        for (const auto &entry : filesystem::recursive_directory_iterator(restoreSrc/restore_path[idx])) {
            const auto &path = entry.path();
            auto relativePath = path.lexically_relative(restoreSrc/restore_path[idx]);
            filesystem::copy( path, restoreDest / relativePath,
            filesystem::copy_options::overwrite_existing | filesystem::copy_options::recursive);
        }
        cout << "Full restore from backup Index " << idx << " completed successfully.\n";
    } catch (const filesystem::filesystem_error &err) {
        cerr << "Filesystem error: " << err.what() << '\n';
    }
    
}