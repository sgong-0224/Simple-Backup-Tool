#ifndef ADLER32_H
#define ADLER32_H
#include <cstdint>
#include <fstream>
uint32_t adler32(std::fstream& data, uint32_t len);
uint32_t adler32(char* data, uint32_t len);
#endif