#include "utils.h"
#include <ctime>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
using namespace std;

// return how many backups have been performed before
size_t find_backup_cnt(filesystem::path& path)
{
    size_t cnt=0;
    for( auto iter: filesystem::directory_iterator(path) )
        ++cnt;
    return cnt>>1;
}

// print and return backup info
vector<filesystem::path> backup_info(filesystem::path& path)
{
    size_t cnt=0;
    vector<filesystem::path> backup_info(1);
    fstream meta;
    string line, time, pathstr;
    cout << "Index" << " | " << "Backup Type" << "  | "  << "Backup Time\n";
    for( auto iter: filesystem::directory_iterator(path) ){
        if(!iter.is_directory()){
            meta.open(iter.path().c_str(),ios::in);
            getline(meta,line,'@');
            getline(meta,time,'\n');
            cout << cout.width(4) << ++cnt 
                 << " | " << line
                 << " | " << time
                 << '\n';
            meta.close();
            // push back relative paths to restore
            pathstr = iter.path().lexically_relative(path).string();
            backup_info.push_back(filesystem::path(pathstr.substr(0,2).append("data")));
        }
    }
    return backup_info;
}
