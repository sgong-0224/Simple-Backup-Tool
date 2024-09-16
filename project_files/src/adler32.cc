#include "adler32.h"
const uint32_t MOD_ADLER = 65521;
uint32_t adler32(std::fstream& data, uint32_t len)
{
    uint32_t a=1, b=0, index=0;
    char data_buf[len];
    data.readsome(data_buf,len);
    len=data.gcount();
    for (index = 0; index < len; ++index){
        a = (a + data_buf[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }
    return (b << 16) | a;
}
uint32_t adler32(char* data, uint32_t len)
{
    uint32_t a=1, b=0, index=0;
    for (index = 0; index < len; ++index){
        a = (a + data[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }
    return (b << 16) | a;
}