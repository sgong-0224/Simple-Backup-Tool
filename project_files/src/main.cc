#include "backup.h"
#include "restore.h"
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

void usage(){
    cout << "Usage:\n";
    cout << "  backup_tool backup -s <src_dir> -d <backup_home_dir>\n";
    cout << "  backup_tool full_backup -s <src_dir> -d <backup_home_dir>\n";
    cout << "  backup_tool restore -s <backup_home_dir> -d <target_dir>\n";
}

int main(int argc, char** argv){
    if( argc!=6 ){
        usage();
        return 1;
    }
    if( strcmp(argv[1], "full_backup")==0 ){
        if( strcmp(argv[2], "-s")==0 &&  strcmp(argv[4], "-d")==0 )
            full_backup(argv[3],argv[5]);
        else if( strcmp(argv[2], "-d")==0 &&  strcmp(argv[4], "-s")==0 )
            full_backup(argv[5],argv[3]);
        else{
            usage();
            return 1;
        }
    }else if( strcmp(argv[1], "backup")==0 ){
        if( strcmp(argv[2], "-s")==0 &&  strcmp(argv[4], "-d")==0 )
            diff_backup(argv[3],argv[5]);
        else if( strcmp(argv[2], "-d")==0 &&  strcmp(argv[4], "-s")==0 )
            diff_backup(argv[5],argv[3]);
        else{
            usage();
            return 1;
        }
    }else if( strcmp(argv[1], "restore")==0 ){
        if( strcmp(argv[2], "-s")==0 &&  strcmp(argv[4], "-d")==0 )
            restore(argv[3],argv[5]);
        else if( strcmp(argv[2], "-d")==0 &&  strcmp(argv[4], "-s")==0 )
            restore(argv[5],argv[3]);
        else{
            usage();
            return 1;
        }
    }else{
        usage();
        return 1;
    }
    return 0;
}
