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
    SDCardEx *sdCard;
    Uart *uart;
    MyFile *myFile;
    CombinedSegment *cs;
};

void WavPlay1::init()
{
    volatile uint16_t *l = (volatile uint16_t *)DUOSEGMENTLINKS_BASE;
    volatile uint16_t *r = (volatile uint16_t *)DUOSEGMENTRECHTS_BASE;
    volatile uint32_t *q = (volatile uint32_t *)MYSEGDISP2_0_BASE;
    cs = new CombinedSegment(new DuoSegment(l), new DuoSegment(r), new QuadroSegment(q));
    cs->setInt(0);
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    i2c = new I2C((volatile uint32_t *)SND_I2C_SCL_BASE, (volatile uint32_t *)SND_I2C_SDA_BASE);
    sdCard = new SDCardEx();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    soundCard = new SoundCard(i2c, (volatile uint32_t *)AUDIO_IF_0_BASE);
    soundCard->init();
    soundCard->setOutputVolume(120);
    //soundCard->setSampleRate(SoundCard::RATE_ADC44K1_DAC44K1);
    //soundCard->setSampleRate(3);
}

uint8_t buf[5120000];
uint16_t sample;
uint16_t sample_r;

int WavPlay1::run()
{
    if (sdCard->isPresent() && sdCard->isFAT16())
    {
            myFile = sdCard->openFile("CROCKETS.WAV");
            
            if (myFile <= 0)
                return 0;
            
            for (int i = 0; i < 44; i++)
                myFile->read();      // skip header


            for (int i = 0; i < sizeof(buf); i++)
            {
                buf[i] = myFile->read();
            }

            uart->puts("Muziek begint\r\n");

            for (int i = 0; i < sizeof(buf);)
            {
                sample = 0;
                sample_r = 0;
                sample += buf[i++];
                sample += buf[i++] << 8;
                //usleep(10);
                //sample_r += buf[i++];
                //sample_r += buf[i++] << 8;
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
                soundCard->writeDacOut(sample, sample);
            }
            

            uart->puts("done\r\n");
    }
    return 0;
}

int main()
{
    WavPlay1 wavPlay1;
    wavPlay1.init();
    wavPlay1.run();
    Uart::getInstance()->puts("All done\r\n");
    return 0;
}


