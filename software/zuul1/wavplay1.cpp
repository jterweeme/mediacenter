/*
Alex Aalbertsberg
Jasper ter Weeme
*/

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
    QuadroSegment *quadroSegment;
};

void WavPlay1::init()
{
    quadroSegment = new QuadroSegment((volatile uint32_t *)MYSEGDISP2_0_BASE);
    quadroSegment->setInt(0);
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    i2c = new I2C((volatile uint32_t *)SND_I2C_SCL_BASE, (volatile uint32_t *)SND_I2C_SDA_BASE);
    sdCard = new SDCard();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    soundCard = new SoundCard(i2c, (volatile uint32_t *)AUDIO_IF_0_BASE);
    soundCard->init();
    soundCard->setOutputVolume(100);
    soundCard->setSampleRate(SoundCard::RATE_ADC44K1_DAC44K1);
}

uint8_t buf[5120000];
uint16_t sample;
uint16_t sample_r;

int WavPlay1::run()
{
    if (sdCard->isPresent())
    {
        if (sdCard->isFAT16())
        {
            myFile = sdCard->openFile("TEST.WAV");
            
            for (int i = 0; i < 44; i++)
                myFile->read();      // skip header


            for (int i = 0; i < 5120000; i++)
                buf[i] = myFile->read();

            uart->puts("Muziek begint\r\n");

            for (int i = 0; i < 5120000;)
            {
                sample = 0;
                sample_r = 0;
                sample += buf[i++];
                sample += buf[i++] << 8;
                //sample_r += buf[i++];
                //sample_r += buf[i++] << 8;
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


