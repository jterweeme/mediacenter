#include <stdio.h>
#include <system.h>
#include <unistd.h>
#include <fcntl.h>
#include "misc.h"

class AudioTest2
{
private:
    Audio2 audio;
public:
    int run();
};

class AudioTest1
{
private:
    Audio audio;
public:
    int run();
};

int AudioTest1::run()
{
    audio.init(::open(AUDIO_IF_0_NAME, O_WRONLY));
    return 0;
}

int AudioTest2::run()
{
    audio.init((volatile uint32_t *)AUDIO_IF_0_BASE, AUDIO_IF_0_NAME);
}

int main()
{
    //AudioTest1 at;
    AudioTest1 at1;
    return at1.run();
}


