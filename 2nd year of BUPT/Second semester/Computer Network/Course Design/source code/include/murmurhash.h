#ifndef _MURMURHASH_H_
#define _MURMURHASH_H_

#include <stdint.h>
#include <stddef.h>

uint32_t MurmurHash(const void *key, size_t len, uint32_t seed);

#endif // _MURMURHASH_H_