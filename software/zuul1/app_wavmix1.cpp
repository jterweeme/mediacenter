#include <system.h>
#include "misc.h"
#include "sdcard.h"

class App
{
    Uart uart;
    VGATerminal vgaTerminal;
    static const uint32_t SD_BASE = ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE;
    SDCardEx sdCard;
public:
    App();
    int run();
};

App::App()
  :
    uart((uint32_t *)UART_BASE),
    vgaTerminal("/dev/video_character_buffer_with_dma_0"),
    sdCard(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME, (void *)SD_BASE)
{
    uart.puts("WavMixer1\r\n");
    vgaTerminal.clear();
    vgaTerminal.puts("WavMixer1\r\n");
}

int App::run()
{
    if (!sdCard.isPresent())
        return -1;

    if (!sdCard.isFAT16())
        return -2;

    MyFile *input1 = sdCard.openFile("CROCKETS.WAV");
    MyFile *input2 = sdCard.openFile("DINGES.WAV");

    
}

int main()
{
    App app;
    return app.run();
}


