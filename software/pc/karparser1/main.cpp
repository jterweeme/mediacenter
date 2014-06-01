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
    uint32_t chunkID;
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
    uint32_t chunkIDBE;
    uint32_t chunkSizeBE;
    uint8_t *data;
public:
    void read(FILE *file);
    std::string toString();
};

class KarFile
{
public:
    void read(FILE *file);
    CHeader header;
    CTrack track;
};

class KarParser1
{
public:
    int run();
};

void CTrack::read(FILE *file)
{
    ::fread(&chunkIDBE, 4, 1, file);
    ::fread(&chunkSizeBE, sizeof(chunkSizeBE), 1, file);
    size_t chunkSize = ::Utility::be_32_toh(chunkSizeBE);
    data = new uint8_t[chunkSize];
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

std::string CTrack::toString()
{
    std::stringstream ss;
    ss << "[Track]" << std::endl;
    ss << "Signature: 0x" << std::hex << Utility::be_32_toh(chunkIDBE) << std::endl;
    ss << "Chunk Size: " << std::dec << Utility::be_16_toh(chunkSizeBE);
    return ss.str();
}

std::string CHeader::toString()
{
    std::stringstream ss;
    ss << "[Header]" << std::endl;
    ss << "Signature: " << std::hex << header.chunkID << std::endl;
    ss << "Chunk Size: " << std::dec << ::Utility::be_32_toh(header.chunkSize) << std::endl;
	ss << "Format Type: " << ::Utility::be_16_toh(header.formatType) << std::endl;
	ss << "Tracks: " << ::Utility::be_16_toh(header.tracks) << std::endl;
	ss << "Time Division: " << ::Utility::be_16_toh(header.timeDivision);
    return ss.str();
}

void KarFile::read(FILE *file)
{
    header.read(file);
    track.read(file);
}

int KarParser1::run()
{
    KarFile karFile;
    karFile.read(stdin);
    std::cout << karFile.header.toString() << std::endl << std::endl;
    std::cout << karFile.track.toString() << std::endl;
    return 0;
}

int main()
{
    KarParser1 kp1;
    return kp1.run();
}


