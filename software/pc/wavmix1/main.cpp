#include <iostream>
#include <fstream>
#include <stdint.h>

class MyFile
{
private:
    MyFile(FILE *fp);
    FILE *fp;
public:
    static const int READ_ONLY = 267;
    static MyFile *open(const char *fn, const int mode);
    FILE *getFP() { return fp; }
    size_t fread(void *ptr, size_t size, size_t nmemb);
};

class WavMixerMain
{
public:
    int run(int argc, char **argv);
    void combine(MyFile *f1, MyFile *f2);
    MyFile *wav1;
    MyFile *wav2;
};

MyFile::MyFile(FILE *fp)
{
    this->fp = fp;
}

size_t MyFile::fread(void *ptr, size_t size, size_t nmemb)
{
    return ::fread(ptr, size, nmemb, fp);
}

void WavMixerMain::combine(MyFile *f1, MyFile *f2)
{
    uint16_t buf, sample;
    size_t f1read, f2read;

    do
    {
        sample = 0;
        f1read = f1->fread(&buf, sizeof(uint16_t), 1);
        sample += buf;
        f2read = f2->fread(&buf, sizeof(uint16_t), 1);
        sample += buf;
        ::fwrite(&sample, sizeof(uint16_t), 1, stdout);
    }
    while (f1read > 0 && f2read > 0);
}

MyFile *MyFile::open(const char *fn, const int mode)
{
    FILE *fp = fopen(fn, "r");

    if (fp != NULL)
        return new MyFile(fp);

    return NULL;
}

int WavMixerMain::run(int argc, char **argv)
{
    wav1 = MyFile::open(argv[1], MyFile::READ_ONLY);
    wav2 = MyFile::open(argv[2], MyFile::READ_ONLY);
    combine(wav1, wav2);
    return 0;
}

int main(int argc, char **argv)
{
    WavMixerMain wmm;
    return wmm.run(argc, argv);
}


