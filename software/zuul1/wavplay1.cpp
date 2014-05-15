#include <system.h>
#include "misc.h"

class WavPlay1
{
public:
    void init();
    int run();
private:
    I2C *i2c;
    SoundCard *soundCard;
    SDCard *sdCard;
    Uart *uart;
    MyFile *myFile;
};

void WavPlay1::init()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    i2c = new I2C((volatile uint32_t *)SND_I2C_SCL_BASE, (volatile uint32_t *)SND_I2C_SDA_BASE);
    sdCard = new SDCard();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
}

int WavPlay1::run()
{
    if (sdCard->isPresent())
    {
        if (sdCard->isFAT16())
        {
            myFile = sdCard->openFile("TITEL.WAV");
        }
    }
    return 0;
}

int main()
{
    WavPlay1 wavPlay1;
    wavPlay1.init();
    return wavPlay1.run();
}


