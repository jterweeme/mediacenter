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
    soundCard = new SoundCard(i2c, (volatile uint32_t *)AUDIO_IF_0_BASE);
    soundCard->init();
    soundCard->setOutputVolume(100);
}

uint8_t buf[512];
uint16_t sample;

int WavPlay1::run()
{
    if (sdCard->isPresent())
    {
        if (sdCard->isFAT16())
        {
            myFile = sdCard->openFile("TEST.WAV");
            
            for (int i = 0; i < 44; i++)
                myFile->read();      // skip header

            uart->puts("Muziek begint\r\n");

            for (int i = 0; i < 512000; i++)
            {
                uint8_t foo = myFile->read();
                sample = 0;
                sample += foo;
                foo = myFile->read();
                sample += foo << 8;
                soundCard->writeDacOut(sample, sample);    
            }

            

            uart->puts("done\r\n");
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


