/*
Jasper ter Weeme
*/

#include <system.h>
#include "misc.h"
#include "sdcard.h"

class App
{
    ::Uart uart;
    ::VGATerminal vgaTerminal;
    static const uint32_t SD_BASE = ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE;
    ::SDCardEx sdCard;
    ::DuoSegment segLinks;
    ::DuoSegment segRechts;
    ::QuadroSegment segQuad;
    ::CombinedSegment segCombi;
public:
    App();
    int run();
};

App::App()
  :
    uart((uint32_t *)UART_BASE),
    vgaTerminal("/dev/video_character_buffer_with_dma_0"),
    sdCard(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME, (void *)SD_BASE),
    segLinks(VA_S1_BASE),
    segRechts(VA_S2_BASE),
    segQuad(MYSEGDISP2_0_BASE),
    segCombi(&segLinks, &segRechts, &segQuad)
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
    //MyFile *output = sdCard.fopen("OUTPUT.WAV");
    RIFFHeader wav1header;
    RIFFHeader wav2header;
    input1->fread((char *)&wav1header, sizeof(RIFFHeader), 1);
    input2->fread((char *)&wav2header, sizeof(RIFFHeader), 1);
    segCombi.setHex(wav1header.chunkID_BE);
    
    do
    {
        uint16_t sample = 0;
        uint16_t buf = 0;
        input1->fread((char *)&buf, sizeof(uint16_t), 1);
        
        
    } while (true);
}

int main()
{
    App app;
    return app.run();
}


