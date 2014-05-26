#include <system.h>
#include "misc.h"

class Karaoke1
{
private:
    VGATerminal *vgaTerminal;
    Uart *uart;
    QuadroSegment *qs;
    SDCard *sdCard;
    MyFile *myFile;
    KarFile *karFile;
public:
    void init();
    int run();
};

void Karaoke1::init()
{
    vgaTerminal = new VGATerminal("/dev/video_character_buffer_with_dma_0");
    vgaTerminal->clear();
    vgaTerminal->puts("Karaoke 1");
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Uart\r\n");
    sdCard = new SDCard();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    
    if (sdCard->isPresent())
    {
    if (sdCard->isFAT16())
    {
        myFile = sdCard->openFile("LUCKY.TXT");
vgaTerminal->puts("Onzin");
    uint8_t c = 'd';
    vgaTerminal->putc((char)c);
    
    do
    {
        c = myFile->read();
        vgaTerminal->putc(c);
    }
    while (c > 0);
    }
    }
    else
    {
        vgaTerminal->puts("\r\nGeen SD Kaart aanwezig!");
    }
    
    
}

int Karaoke1::run()
{
    
    return 0;
}

int main()
{
    Karaoke1 kar1;
    kar1.init();
    return kar1.run();
}


