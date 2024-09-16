//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
// https://github.com/aappleby/smhasher
#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_
#include <stdint.h>
void MurmurHash3_arch32_32  (const void* key, int len, uint32_t seed, void* out);
void MurmurHash3_arch32_128 (const void* key, int len, uint32_t seed, void* out);
void MurmurHash3_arch64_128 (const void* key, int len, uint32_t seed, void* out);
#endif // _MURMURHASH3_H_