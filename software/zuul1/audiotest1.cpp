#include <stdio.h>
#include <system.h>
#include <unistd.h>
#include <fcntl.h>
#include "misc.h"

class AudioTest1
{
private:
    I2C *i2cBus;
    SoundCard *soundCard;
public:
    void init();
    int run();
};

int AudioTest1::run()
{
    return 0;
}

void AudioTest1::init()
{
    i2cBus = new I2C((volatile uint32_t *)SND_I2C_SCL_BASE, (volatile uint32_t *)SND_I2C_SDA_BASE);
    soundCard = new SoundCard(i2cBus);
    soundCard->init();
}

int main()
{
    AudioTest1 at1;
    return at1.run();
}


