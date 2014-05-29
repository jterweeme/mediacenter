#include <stdio.h>
#include "md5.h"

void to_bytes(uint32_t val, uint8_t *bytes)
{
    bytes[0] = (uint8_t) val;
    bytes[1] = (uint8_t) (val >> 8);
    bytes[2] = (uint8_t) (val >> 16);
    bytes[3] = (uint8_t) (val >> 24);
}

uint32_t to_int32(const uint8_t *bytes)
{
    return (uint32_t) bytes[0]
        | ((uint32_t) bytes[1] << 8)
        | ((uint32_t) bytes[2] << 16)
        | ((uint32_t) bytes[3] << 24);
}

Hasher::Hasher()
{
    h[0] = 0x67452301;
    h[1] = 0xefcdab89;
    h[2] = 0x98badcfe;
    h[3] = 0x10325476;
}

const char *MD5Hash::toString()
{
    return "hello";
}

const char *Chunk::toString()
{
    return "hello";
}

void Hasher::readChunk(Chunk *chunk)
{
    uint32_t a, b, c, d, f, g, temp, x, c2;

    for (int i = 0; i < 512; i += 64)
    {
        for (int j = 0; j < 16; j++)
            w[j] = ::to_int32(chunk->c + i + j * 4);

        a = h[0];
        b = h[1];
        c = h[2];
        d = h[3];

        for(uint32_t i = 0; i<64; i++)
        {
            if (i < 16)
            {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5*i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3*i + 5) % 16;
            } else {
                f = c ^ (b | (~d));
                g = (7*i) % 16;
            }

            temp = d;
            d = c;
            c = b;
            x = a + f + k[i] + w[g];
            c2 = r[i];
            b += (((x) << (c2)) | ((x) >> (32 - (c2))));
            a = temp;
        }

        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
    }

    uint8_t digest[16];
    to_bytes(h[0], digest);
    to_bytes(h[1], digest + 4);
    to_bytes(h[2], digest + 8);
    to_bytes(h[3], digest + 12);

    for (int i = 0; i < 16; i++)
        ::printf("%2.2x", digest[i]);

    ::printf("\r\n");
}

