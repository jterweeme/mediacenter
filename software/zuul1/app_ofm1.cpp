/*
Jasper ter Weeme
*/

#include <system.h>
#include <stdlib.h>
#include "misc.h"
#include "sdcard.h"

class Box
{
    VGA *vga;
public:
    Box(VGA *vga) : vga(vga) { }
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
    VGATerminal vga;
    Box box;
    SDCardEx sdCard;
    Uart uart;
    static const uint32_t SD_BASE = ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE;
public:
    OrthodoxFileManager1();
    int run();
};

OrthodoxFileManager1::OrthodoxFileManager1()
  :
    vga("/dev/video_character_buffer_with_dma_0"),
    box(&vga),
    sdCard(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME, (void *)SD_BASE),
    uart((uint32_t *)UART_BASE)
{
    uart.puts("StartUp OFM1...\r\n");
    vga.clear();
}

void Box::draw()
{
vga->draw("+--------------------------------------++--------------------------------------+",
0, 0);
vga->draw("|                                      ||                                      |",
0, 1);
vga->draw("|                                      ||                                      |",
0, 2);
vga->draw("|                                      ||                                      |",
0, 3);
vga->draw("|                                      ||                                      |",
0, 4);
}

int OrthodoxFileManager1::run()
{
    box.draw();

    if (sdCard.isPresent() && sdCard.isFAT16())
    {
        char fn[13] = {0};
        t_file_record fr;
        int foo = sdCard.alt_up_sd_card_find_first("/.", fn);
        char dinges[255];

        while ((foo = sdCard.findNext(fn, &fr)) >= 0)
        {
            uint8_t wav[3] = {'K','A','R'};

            if (::memcmp(fr.extension, wav, 3) == 0)
            {
                MyFileRecord record(fr);
                vga.puts(record.toString());
                vga.puts("\r\n");
                uart.puts("\r\n");
            }
        }
    }
    else
    {
        uart.puts("Geen FAT16\r\n");
    }

    return 0;
}

int main()
{
    OrthodoxFileManager1 ofm1;
    return ofm1.run();
}


