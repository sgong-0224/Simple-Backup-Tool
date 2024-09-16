#include "definition.h"
#include "MurmurHash3.h"
#include "adler32.h"
#include "delta.h"
#include <cstring>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

std::vector<delta_struct> diff_stream(std::fstream& oldfile, std::fstream& newfile)
{
    std::vector<delta_struct> delta_structs;
    uint64_t match_chunk_index=0;      // Matching old chunk BEFORE delta data
    bool diff = false;          // Indicator: Calculating delta?
    char MurmurHash[129], MurmurHash2[129];
    memset(MurmurHash,0,129);
    memset(MurmurHash2,0,129);
    char input_buf[CHUNK_SIZE];
    memset(input_buf,0,CHUNK_SIZE);
    uint32_t seed=0x114514;
    int len=0;
    std::vector<uint32_t> chunk_hash;
    // TODO: Hash&Compare
    // FIXME: chunk index
    while( !oldfile.eof() )
        chunk_hash.push_back(adler32(oldfile, CHUNK_SIZE));
    oldfile.clear();
    oldfile.seekg(std::ios::beg);
    newfile.clear();
    newfile.seekg(std::ios::beg);
    std::ios::pos_type match_start_pos = newfile.tellg(), prev_match_end_pos = newfile.tellg();
    uint64_t LOOP_CTR=1;
    while( !newfile.eof()){
        newfile.readsome(input_buf,CHUNK_SIZE);
        len = newfile.gcount();
        std::cout << LOOP_CTR << '\n';
        if( likely( chunk_hash[match_chunk_index]==adler32(input_buf, CHUNK_SIZE) ) ){
            // got a matching chunk ? 
            MurmurHash3_arch64_128 (input_buf,len,seed,MurmurHash);
            oldfile.seekg(match_chunk_index*CHUNK_SIZE);
            oldfile.readsome(input_buf,CHUNK_SIZE);
            len = oldfile.gcount();
            MurmurHash3_arch64_128 (input_buf,len,seed,MurmurHash2);
            if( unlikely(strcmp(MurmurHash,MurmurHash2)))
                goto START_DIFF;
            else if( unlikely(diff) ){
                // diff data ends
                delta_structs.push_back(delta_struct{
                    match_chunk_index+1,
                    match_start_pos - prev_match_end_pos
                });
                diff = false;
            }
            // update match&chunk pointer
            prev_match_end_pos = newfile.tellg();
            ++match_chunk_index;
        } else {
        START_DIFF:
            match_start_pos = (newfile.seekg(1-len, std::ios::cur)).tellg();
            diff = true;
        }
    }
    return delta_structs;
}
void print_diff(const std::vector<delta_struct>& delta_structs, std::fstream& out_str)
{
    // TODO: filename index
    out_str << "P " ;
    for(auto delta_struct:delta_structs)
        out_str << delta_struct.matching_chunk_cnt << ' ' << delta_struct.patch_length << ' ';
    out_str << '\n';
}
// TODO: write patch to binary file
void write_delta(const std::vector<delta_struct>& delta_structs, std::fstream& newfile, std::fstream& delta_file)
{
    for(auto delta:delta_structs){
        newfile.seekg(delta.matching_chunk_cnt*CHUNK_SIZE, std::ios::beg);
        char* deltabuf=new char(delta.patch_length);
        newfile.readsome(deltabuf, delta.patch_length);
        delta_file.write(deltabuf, newfile.gcount());
    }
}
void patch_file(std::fstream& oldfile, std::fstream& newfile, std::fstream& patch_data, const std::list<chunk_patchlen>& patch_info)
{
    // TODO: patch: write oldfile&patch_data to newfile
    // patch_structs: extract, parse, append
    char chunkbuf[CHUNK_SIZE];
    uint64_t last_chunk=0;
    std::streamoff patch_len=0;
    for(auto pair:patch_info){
        last_chunk = pair.first;
        patch_len = pair.second;
        for(uint64_t idx=0;idx<last_chunk;++idx){
            oldfile.readsome(chunkbuf, CHUNK_SIZE);
            newfile.write(chunkbuf, oldfile.gcount());
        }
        char* patchbuf = new char(patch_len);
        patch_data.readsome(patchbuf,patch_len);
        if(unlikely(patch_data.gcount()==patch_len)){
            delete patchbuf;
            oldfile.close();
            patch_data.close();
            newfile.close();
            throw "Error: Patch data corrupt.\n";
        }else{
            newfile.write(patchbuf,patch_len);
            delete patchbuf;
        }
    }
    oldfile.close();
    patch_data.close();
    newfile.close();
}