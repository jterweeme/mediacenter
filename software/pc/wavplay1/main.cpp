/*
./main hw:0 test.wav
*/

#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <alsa/asoundlib.h>

class MyStringBuilder
{
public:
    std::string build(const char *, ...);
};

class SoundCard
{
public:
    ~SoundCard();
    void init(const char *card);
    void play(int *buf, size_t nread);
private:
    snd_pcm_t *playback_handle;
    snd_pcm_hw_params_t *hw_params;
};

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

class WavFile
{
private:
};

class WavPlayMain
{
public:
    int run(int argc, char **argv);
private:
    SoundCard *sndCard;
    FILE *fin;
};

SoundCard::~SoundCard()
{
    ::snd_pcm_drain(playback_handle);
    ::snd_pcm_close(playback_handle);
}

std::string MyStringBuilder::build(const char *s, ...)
{
    // argumenten moeten nog geimplementeerd worden
    return std::string(s);
}

MyFile::MyFile(FILE *fp)
{
    this->fp = fp;
}

size_t MyFile::fread(void *ptr, size_t size, size_t nmemb)
{
    return ::fread(ptr, size, nmemb, fp);
}

MyFile *MyFile::open(const char *fn, const int mode)
{
    FILE *fp = fopen(fn, "r");

    if (fp != NULL)
        return new MyFile(fp);

    return NULL;
}

void SoundCard::play(int *buf, size_t nread)
{
    int err;
    MyStringBuilder msb;

    if ((err = ::snd_pcm_writei(playback_handle, buf, nread)) != nread)
    {
        throw msb.build("write to audio interface failed (%s)", snd_strerror(err));
        ::snd_pcm_prepare(playback_handle);
    }
}

void SoundCard::init(const char *card)
{
    int err;
    unsigned int rate = 44100;
    MyStringBuilder msb;

    if ((err = snd_pcm_open(&playback_handle, card, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
        throw msb.build("cannot open audio device %s (%s)\n", card, snd_strerror(err));
           
    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
        throw msb.build("cannot allocate hardware parameter structure (%s)\n", snd_strerror(err));
                 
    if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0)
        throw msb.build("cannot initialize hardware parameter structure (%s)\n", snd_strerror(err));
    
    if ((err = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        ::fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
        ::exit(1);
    }
    
    if ((err = snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        ::fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
        ::exit(1);
    }


    if ((err = ::snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &rate, 0)) < 0)
    {
        ::fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
        ::exit(1);
    }

    printf("Rate set to %d\n", rate);
    
    if ((err = ::snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2)) < 0)
    {
        ::fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
        ::exit(1);
    }
    
    if ((err = ::snd_pcm_hw_params(playback_handle, hw_params)) < 0)
    {
        ::fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
        ::exit(1);
    }
    
    ::snd_pcm_hw_params_free(hw_params);
}

int WavPlayMain::run(int argc, char **argv)
{
    if (argc != 3)
    {
        ::fprintf(stderr, "Usage: %s card file\n", argv[0]);
        ::exit(1);
    }

    sndCard = new SoundCard();
    sndCard->init(argv[1]);
    MyFile *mf = MyFile::open(argv[2], MyFile::READ_ONLY);

    size_t nread;
    int buf[512];

    
    if (mf == NULL)
    {
        ::fprintf(stderr, "Can't open %s for reading\n", argv[2]);
        ::exit(1);
    }

    {
        size_t nread;
        int buf[512];

        do
        {
            nread = mf->fread(buf, sizeof(int), 512);
            sndCard->play(buf, nread);
        }
        while (nread > 0);
    }

    delete sndCard;
    exit(0);
}

int main(int argc, char **argv)
{
    WavPlayMain wpm;
    return wpm.run(argc, argv);
}


