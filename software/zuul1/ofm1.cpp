#include <system.h>
#include <stdlib.h>
#include "misc.h"

class Box
{
private:
    VGA *vga;
public:
    Box(VGA *vga) { this->vga = vga; }
    void draw();
};

class Left
{
public:
    Left() { }
    char *toArray();
};

class OrthodoxFileManager1
{
public:
    OrthodoxFileManager1() { }
    void init();
    int run();
private:
    VGA *vga;
    Box *box;
    SDCard *sdCard;
    Uart *uart;
    QuadroSegment *quadroSegment;
};

void OrthodoxFileManager1::init()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("StartUp OFM1...\r\n");
    quadroSegment = new QuadroSegment((volatile uint32_t *)QUADROSEGMENT_BASE);
    sdCard = new SDCard();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    vga = new VGA("/dev/video_character_buffer_with_dma_0");
    box = new Box(vga);
    vga->clear();
    
    if (sdCard->isPresent())
    {
        if (sdCard->isFAT16())
        {
            char fn[13];
            int foo = ::alt_up_sd_card_find_first("/.", fn);
            foo = sdCard->findNext(fn);
            uart->puts(fn);
            //MyFile *file = sdCard->openFile(fn);
            int fd = sdCard->fopen(fn);
            quadroSegment->setHex(fd);
            int bar = ::alt_up_sd_card_read(fd);
            quadroSegment->setHex(bar);
            uart->puts("Test\r\n");


            short int bijt;
            //bijt = file->read();

            uart->puts("Onzin\r\n");

        }
        else
        {
            uart->puts("Geen FAT16\r\n");
        }
    }
}

void Box::draw()
{
    vga->draw("+--------------------------------------++--------------------------------------+", 0, 0);
}



int OrthodoxFileManager1::run()
{
    box->draw();
    return 0;
}

int main()
{
    OrthodoxFileManager1 ofm1;
    ofm1.init();
    return ofm1.run();
}


