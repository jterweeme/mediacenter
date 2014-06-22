/*
Alex Aalbertsberg
Jasper ter Weeme
*/

#include <system.h>
#include "misc.h"
#include "sdcard.h"

int delay = 20;

class WavPlay2
{
    I2C i2c;
    SoundCard soundCard;
    SDCardEx *sdCard;
    Uart uart;
    LCD *lcd;
    MyFile *myFile;
    CombinedSegment *cs;
    InfraRood ir;
public:
    WavPlay2();
    SoundCard& getSoundCard() { return soundCard; }
    void init();
    int run();
};


class Beam : public Observer
{
private:
    WavPlay2 *wavPlay;
public:
    Beam(WavPlay2 *wavPlay) : wavPlay(wavPlay) { }
    void update();
};

WavPlay2::WavPlay2() :
    i2c((uint32_t *)SND_I2C_SCL_BASE, (uint32_t *)SND_I2C_SDA_BASE),
    soundCard(&i2c, (volatile uint32_t * const)AUDIO_IF_0_BASE),
    uart((uint32_t *)UART_BASE),
    ir((uint32_t *)INFRARED_0_BASE, INFRARED_0_IRQ, INFRARED_0_IRQ_INTERRUPT_CONTROLLER_ID)
{
}

void Beam::update()
{
    uint32_t button = InfraRood::getInstance()->read();
    static unsigned volume = 100;
    
    switch (button >> 16)
    {
    case ::TerasicRemote::LEFT:
        ::delay++;
        break;
    case ::TerasicRemote::RIGHT:
        ::delay--;
        break;
    case ::TerasicRemote::VOL_UP:
        wavPlay->getSoundCard().setOutputVolume(++volume);
        break;
    case ::TerasicRemote::VOL_DOWN:
        wavPlay->getSoundCard().setOutputVolume(--volume);
        break;
    case ::TerasicRemote::MUTE:
        wavPlay->getSoundCard().setOutputVolume(0);
        break;
    }
}

void WavPlay2::init()
{
    volatile void * const sdbase = (void *)ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE;
    sdCard = new SDCardEx(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME, sdbase);
    soundCard.init();
    soundCard.setOutputVolume(100);
    //soundCard->setSampleRate(SoundCard::RATE_ADC44K1_DAC44K1);
    //soundCard->setSampleRate(3);
    lcd = new LCD((volatile uint8_t *)LCD_BASE);
    lcd->clear();
    //lcd->home();
    lcd->puts("CROCKETS.WAV");
    //ir = InfraRood::getInstance();
    //int ctl = VA_S3_IRQ_INTERRUPT_CONTROLLER_ID;
    //ir->init((volatile uint32_t *)INFRARED_0_BASE, INFRARED_0_IRQ, ctl);
    ir.setObserver(new Beam(this));
}

int WavPlay2::run()
{
    uint8_t buf[900000];
    uint16_t sample, sample_r;

    if (sdCard->isPresent() && sdCard->isFAT16())
    {
        char fn[13] = {0};
        t_file_record fr;
        int foo = sdCard->alt_up_sd_card_find_first("/.", fn);

        while ((foo = sdCard->findNext(fn, &fr)) >= 0)
        {
            uint8_t wav[3] = {'W','A','V'};
            uint8_t wav2[8] = {'C', 'R', 'O', 'C','K','E','T','S'};
            
            if (::memcmp(fr.extension, wav, 3) == 0 && ::memcmp(fr.name, wav2, 8) == 0)
            {
                MyFileRecord record(fr);
                //const char *fileName = record.toString();
                myFile = sdCard->openFile((char *)"CROCKETS.WAV");
        
                if (myFile <= 0)
                    return 0;
        
                for (int i = 0; i < 44; i++)
                    myFile->read();      // skip header

                while (true)
                {
                    for (size_t i = 0; i < sizeof(buf); i++)
                        buf[i] = myFile->read();

                    for (size_t i = 0; i < sizeof(buf);)
                    {
                        sample = 0;
                        sample_r = 0;
                        sample += buf[i++];
                        sample += buf[i++] << 8;
                        //usleep(10);
                        //sample_r += buf[i++];
                        //sample_r += buf[i++] << 8;

                        for (volatile int j = 0; j < ::delay; j++)
                            soundCard.writeDacOut(sample, sample);
                    }
                }
            }
        }
        
        uart.puts("done\r\n");
    }
    return 0;
}

int main()
{
    WavPlay2 wavPlay2;
    wavPlay2.init();
    wavPlay2.run();
    Uart::getInstance()->puts("All done\r\n");
    return 0;
}


