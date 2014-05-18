#ifndef _MD5_H_
#define _MD5_H_
#include <string.h>
#include <stdint.h>

class MD5Hash
{
private:
    uint8_t hash[16];
public:
    MD5Hash(uint8_t *hash) { ::memcpy(this->hash, hash, 16); }
    const char *toString();
};

class Chunk
{
public:
    uint8_t c[512];
    size_t size;
    Chunk(char *src, size_t size);
    const char *toString();
};

class Hasher
{
public:
    Hasher();
    MD5Hash *getCurrentHash();
    void readChunk(Chunk *chunk);
private:
    uint32_t h[4];
    uint32_t w[16];
};

#endif


