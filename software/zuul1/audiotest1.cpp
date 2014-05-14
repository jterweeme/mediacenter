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
    Uart *uart;
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
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("AudioTest1 StartUp...\r\n");
    i2cBus = new I2C((volatile uint32_t *)SND_I2C_SCL_BASE, (volatile uint32_t *)SND_I2C_SDA_BASE);
    soundCard = new SoundCard(i2cBus, (volatile uint32_t *)AUDIO_IF_0_BASE);
    soundCard->init();
}

int main()
{
    AudioTest1 at1;
    at1.init();
    return at1.run();
}


