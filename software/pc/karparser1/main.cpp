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

std::string CHeader::toString()
{
    std::stringstream ss;
    ss << "Signature: ";
    
    for (int i = 0; i < 4; i++)
        ss << header.chunkID[i];

    ss << std::endl;
	ss << "Chunk Size: " << _byteswap_ulong(header.chunkSize) << std::endl;
	ss << "Format Type: " << _byteswap_ushort(header.formatType) << std::endl;
	ss << "Tracks: " << _byteswap_ushort(header.tracks) << std::endl;
	ss << "Time Division: " << _byteswap_ushort(header.timeDivision);
    //ss << "Chunk Size: " << __builtin_bswap32(header.chunkSize) << std::endl;
    //ss << "Format Type: " << __builtin_bswap16(header.formatType) << std::endl;
    //ss << "Tracks: " << __builtin_bswap16(header.tracks) << std::endl;
    //ss << "Time Division: " << __builtin_bswap16(header.timeDivision);
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


