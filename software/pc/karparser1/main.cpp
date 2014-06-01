#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#ifdef __GNUC__
#include <endian.h>
#endif

struct Header
{
    char chunkID[4];
    uint32_t chunkSize;
    uint16_t formatType;
    uint16_t tracks;
    uint16_t timeDivision;
};

class Utility
{
public:
    static inline uint32_t be_32_toh(uint32_t x);
    static inline uint16_t be_16_toh(uint16_t x);
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
private:
    uint32_t chunkID;
    uint32_t chunkSize;
    uint8_t *data;
public:
    void read(FILE *file);
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

void CTrack::read(FILE *file)
{
    ::fread(&chunkID, sizeof(chunkID), 1, file);
    ::fread(&chunkSize, sizeof(chunkSize), 1, file);
    ::fread(data, ::Utility::be_32_toh(chunkSize), 1, file);
}

uint16_t Utility::be_16_toh(uint16_t x)
{
#ifdef __GNUC__
    return be16toh(x);
#endif
	return (x & 0x00ff) << 8 | (x & 0xff00) >> 8;
}

uint32_t Utility::be_32_toh(uint32_t x)
{
#ifdef __GNUC__
    return be32toh(x);
#endif
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
	ss << "Chunk Size: " << ::Utility::be_32_toh(header.chunkSize) << std::endl;
	ss << "Format Type: " << ::Utility::be_16_toh(header.formatType) << std::endl;
	ss << "Tracks: " << ::Utility::be_16_toh(header.tracks) << std::endl;
	ss << "Time Division: " << ::Utility::be_16_toh(header.timeDivision);
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


