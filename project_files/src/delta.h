#ifndef DELTA_H
#define DELTA_H
#include "definition.h"
#include "utils.h"
#include <fstream>
#include <vector>
#include <list>
std::vector<delta_struct> diff_stream(std::fstream& oldfile, std::fstream& newfile);
void print_diff(const std::vector<delta_struct>& delta_structs, std::fstream& out_str);
void patch_file(std::fstream& oldfile, std::fstream& newfile, std::fstream& patch_data, const std::list<chunk_patchlen>& patch_info);
void write_delta(const std::vector<delta_struct>& delta_structs, std::fstream& newfile, std::fstream& delta_file);
#endif