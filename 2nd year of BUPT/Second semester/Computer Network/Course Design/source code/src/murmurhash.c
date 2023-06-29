#include "..\include\murmurhash.h"

#define FMIX32(h) \
    do { \
        (h) ^= (h) >> 16; \
        (h) *= 0x85ebca6b; \
        (h) ^= (h) >> 13; \
        (h) *= 0xc2b2ae35; \
        (h) ^= (h) >> 16; \
    } while (0)

uint32_t MurmurHash(const void *key, size_t len, uint32_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;
    const uint8_t *data = (const uint8_t *)key;
    uint32_t h = seed ^ len;
    while (len >= 4)
    {
        uint32_t k = *(uint32_t *)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        len -= 4;
    }
    switch (len)
    {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    }
    FMIX32(h);
    return h;
}