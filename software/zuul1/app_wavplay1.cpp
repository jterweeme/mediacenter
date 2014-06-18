/*
Alex Aalbertsberg
Jasper ter Weeme
*/

#include <system.h>
#include "misc.h"
#include "sdcard.h"

class WavPlay1
{
    I2C i2c;
    SoundCard soundCard;
    SDCardEx sdCard;
    Uart uart;
    LCD lcd;
    MyFile *myFile;
    MPlayer mplayer;
    InfraRood ir;
    static const uint32_t SD_BASE = ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE;
public:
    WavPlay1();
    SoundCard& getSoundCard() { return soundCard; }
    MPlayer& getMPlayer() { return mplayer; }
    int run();
};

class Beam : public Observer
{
    WavPlay1 *wavPlay;
public:
    Beam(WavPlay1 *wavPlay) : wavPlay(wavPlay) { }
    void update();
};

WavPlay1::WavPlay1()
  :
    i2c((uint32_t *)SND_I2C_SCL_BASE, (uint32_t *)SND_I2C_SDA_BASE),
    soundCard(&i2c, (uint32_t *)AUDIO_IF_0_BASE),
    sdCard(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME, (void *)SD_BASE),
    uart((uint32_t *)UART_BASE),
    lcd((uint8_t *)LCD_BASE),
    mplayer(&soundCard),
    ir((uint32_t *)INFRARED_0_BASE, INFRARED_0_IRQ, INFRARED_0_IRQ_INTERRUPT_CONTROLLER_ID)
{
    soundCard.init();
    soundCard.setOutputVolume(100);
    soundCard.setSampleRate(SoundCard::RATE_ADC44K1_DAC44K1);
    soundCard.setSampleRate(3);
    lcd.clear();
    lcd.home();
    lcd.puts("CROCKETS.WAV");
    ir.setObserver(new Beam(this));
}

void Beam::update()
{
    uint32_t button = InfraRood::getInstance()->read();
    static unsigned volume = 100;
    
    switch (button >> 16)
    {
    case ::TerasicRemote::LEFT:
        wavPlay->getMPlayer().delay++;
        break;
    case ::TerasicRemote::RIGHT:
        wavPlay->getMPlayer().delay--;
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

int WavPlay1::run()
{
    if (!sdCard.isPresent())
        return -1;

    if (!sdCard.isFAT16())
        return -2;

    myFile = sdCard.openFile("CROCKETS.WAV");
        
    if (myFile <= 0)
        return -3;

    mplayer.enqueue(myFile);
    mplayer.play();
    uart.puts("done\r\n");
    return 0;
}

int main()
{
    WavPlay1 wavPlay1;
    wavPlay1.run();
    Uart::getInstance()->puts("All done\r\n");
    return 0;
}


