#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

struct Header
{
    char chunkID[4];
    uint32_t chunkSize;
    uint16_t formatType;
    uint16_t tracks;
    uint16_t timeDivision;
};

struct Track
{
    char chunkID[4];
    uint32_t chunkSize;
    uint8_t data[];
};

class Utility
{
public:
	static inline uint16_t byteswap16(uint16_t x);
	static inline uint32_t byteswap32(uint32_t x);
};

class CHeader
{
private:
    Header header;
public:
    CHeader() { }
    CHeader(Header header) { this->header = header; }
    CHeader(FILE *file) { this->read(file); }
    void read(FILE *file) { ::fread((Header *)&header, sizeof(Header), 1, file); }
    std::string toString();
};

class CTrack
{
public:
};

class KarFile
{
public:
    void read(FILE *file);
private:
    CHeader header;
};

class KarParser1
{
public:
    int run();
};

uint16_t Utility::byteswap16(uint16_t x)
{
	return (x & 0x00ff) << 8 | (x & 0xff00) >> 8;
}

uint32_t Utility::byteswap32(uint32_t x)
{
	x = (x & 0x0000FFFF) << 16 | (x & 0xFFFF0000) >> 16;
	return (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
}

std::string CHeader::toString()
{
    std::stringstream ss;
    ss << "Signature: ";
    
    for (int i = 0; i < 4; i++)
        ss << header.chunkID[i];

    ss << std::endl;
	ss << "Chunk Size: " << ::Utility::byteswap32(header.chunkSize) << std::endl;
	ss << "Format Type: " << ::Utility::byteswap16(header.formatType) << std::endl;
	ss << "Tracks: " << ::Utility::byteswap16(header.tracks) << std::endl;
	ss << "Time Division: " << ::Utility::byteswap16(header.timeDivision);
    return ss.str();
}

void KarFile::read(FILE *file)
{
    header.read(file);
    std::cout << header.toString() << std::endl;
}

int KarParser1::run()
{
    KarFile karFile;
    karFile.read(stdin);
    return 0;
}

int main()
{
    KarParser1 kp1;
    return kp1.run();
}


