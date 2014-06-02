#include <iostream>
#include <fstream>
#include <stdint.h>

struct RIFFHeader
{
    uint32_t chunkID_BE;
    uint32_t chunkSize_LE;
    uint32_t format_BE;
    uint32_t subChunk1ID_BE;
    uint32_t subChunk1Size_LE;
    uint16_t audioFormat_LE;
    uint16_t numChannels_LE;
    uint32_t sampleRate_LE;
    uint32_t byteRate_LE;
    uint16_t blockAlign_LE;
    uint16_t bitsPerSample_LE;
    uint32_t subChunk2ID_BE;
    uint32_t subChunk2Size_LE;
};

class WavMixerMain
{
public:
    int run(int argc, char **argv);
    void combine(std::ifstream &wav1, std::ifstream &wav2);
    std::ifstream wav1;
    std::ifstream wav2;
};

void WavMixerMain::combine(std::ifstream &wav1, std::ifstream &wav2)
{
    uint16_t buf, sample;

    do
    {
        sample = 0;
        wav1.read((char *)&buf, sizeof(uint16_t));
        sample += buf;
        wav2.read((char *)&buf, sizeof(uint16_t));
        sample += buf;
        std::cout.write((const char *)&sample, sizeof(sample));
    }
    while (wav1 && wav2);
}

int WavMixerMain::run(int argc, char **argv)
{
    wav1.open(argv[1]);
    wav2.open(argv[2]);
    RIFFHeader wav1header;
    RIFFHeader wav2header;
    wav1.read((char *)&wav1header, sizeof(RIFFHeader));
    wav2.read((char *)&wav2header, sizeof(RIFFHeader));
    std::cout.write((const char *)&wav1header, sizeof(wav1header));
    std::cerr << wav2header.sampleRate_LE << std::endl;
    combine(wav1, wav2);
    return 0;
}

int main(int argc, char **argv)
{
    WavMixerMain wmm;
    return wmm.run(argc, argv);
}


