#include "utils.h"
#include <ctime>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <filesystem>

// return how many backups have been performed before
size_t find_backup_cnt(std::filesystem::path& path)
{
    size_t cnt=0;
    for(const auto& iter: std::filesystem::directory_iterator(path) )
        ++cnt;
    return cnt>>1;
}

/* 
 * print and return backup info: <PATH, ISFULL>
 */ 
std::vector<backup_pair> backup_info(std::filesystem::path& path, bool print)
{
    std::vector<backup_pair> backup_info(1);
    std::fstream meta;
    std::string type, time, pathstr, index;
    std::stringstream path_in;
    if( unlikely(print) )
        std::cout << "Index" << " | " << "Backup Type" << "  | "  << "Backup Time\n";
    std::list<std::filesystem::path> filenames;
    for(const auto& iter: std::filesystem::directory_iterator(path) ){
        if( likely( iter.is_regular_file() ) )
           filenames.push_back(iter.path());
    }
    filenames.sort(
        [](const std::filesystem::path& p1, const std::filesystem::path& p2){
            return std::filesystem::last_write_time(p1) < std::filesystem::last_write_time(p2);
        }
    );
    
    for(const auto& filename:filenames){
            pathstr = filename.lexically_relative(path).string();
            path_in.clear();
            path_in << pathstr;
            getline(path_in,index,'_');
            getline(path_in,type,'-');
            getline(path_in,type);
            if( unlikely(print) ){
                std::cout.width(5);
                std::cout << index;
                std::cout << " |     " << type
                     << "     | " << to_time_t(last_write_time(filename))
                     << '\n';
            }
            meta.close();
            // push back relative paths to restore
            backup_info.push_back( 
                std::make_pair(std::filesystem::path(index+"_data"), type.compare("full")==0) 
            );
    }
    return backup_info;
}

/*
 * Time utility
 */
std::string to_time_t(std::filesystem::file_time_type tp)
{
	using namespace std::chrono;
	auto fsysclk = time_point_cast<system_clock::duration>(tp - std::filesystem::file_time_type::clock::now() + system_clock::now());
	auto systimet = system_clock::to_time_t(fsysclk);
    tm* gmt = localtime(&systimet); 
    std::stringstream buffer;
	buffer << std::put_time(gmt, "%Y-%m-%d %H:%M:%S");
	return buffer.str();
}
