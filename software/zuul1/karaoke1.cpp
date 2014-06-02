#include <system.h>
#include "misc.h"

class Karaoke1
{
private:
    VGATerminal *vgaTerminal;
    Uart *uart;
    QuadroSegment *qs;
    SDCardEx *sdCard;
    MyFile *myFile;
    KarFile *karFile;
public:
    void init();
    int run();
};

void Karaoke1::init()
{
    qs = new QuadroSegment((volatile uint32_t *)MYSEGDISP2_0_BASE);
    qs->setInt(0);
    vgaTerminal = new VGATerminal("/dev/video_character_buffer_with_dma_0");
    vgaTerminal->clear();
    vgaTerminal->puts("Karaoke 1");
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Uart\r\n");
    sdCard = new SDCardEx();

    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME,
            (volatile void *)ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE);
    
    if (sdCard->isPresent() && sdCard->isFAT16())
    {
        myFile = sdCard->openFile("VOGUE.KAR");
        karFile = new KarFile(myFile);
        karFile->read();
        qs->setInt(::Utility::be_32_toh(karFile->header.header.chunkSizeBE));
        //uart->printf("He%ulo Wo%rld\r\n");
/*
        for (int i = 0; i < 100; i++)
        {
            uint16_t foo;
            foo = myFile->read();
            
        }*/
            
        //karFile = new KarFile(myFile);
        
    }
    else
    {
        vgaTerminal->puts("\r\nGeen SD Kaart aanwezig!");
    }
    
    uart->printf("Hello %s\r\n", "World");
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


