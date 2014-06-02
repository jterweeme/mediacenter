#include <iostream>
#include <fstream>
#include <stdint.h>

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
    size_t f1read, f2read;

    do
    {
        sample = 0;
        wav1.read((char *)&buf, sizeof(uint16_t));
        sample += buf;
        wav2.read((char *)&buf, sizeof(uint16_t));
        sample += buf;
        ::fwrite(&sample, sizeof(uint16_t), 1, stdout);
    }
    while (wav1 && wav2);
}

int WavMixerMain::run(int argc, char **argv)
{
    wav1.open(argv[1]);
    wav2.open(argv[2]);
    combine(wav1, wav2);
    return 0;
}

int main(int argc, char **argv)
{
    WavMixerMain wmm;
    return wmm.run(argc, argv);
}


