/*
Jasper ter Weeme
*/

#include <system.h>
#include <stdio.h>
#include "misc.h"
#include "sdcard.h"

class SDCardTest
{
    SDCardEx *sdCard;
    int bestand;
    Uart uart;
public:
    SDCardTest();
    int run();
    void init();
};

SDCardTest::SDCardTest()
  :
    uart((uint32_t *)UART_BASE)
{
}

char buffer[512] = "WELCOME TO THE INTERFACE!!\r\n\0";

void SDCardTest::init()
{
    sdCard = new SDCardEx(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME,
        (void *)ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE);

    //sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME,
    //    (volatile void *)ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE);
}

int SDCardTest::run()
{
    if (!sdCard->isPresent() || !sdCard->isFAT16())
    {
        uart.puts("Geen FAT16\r\n");
        return -1;
    }

    uart.puts("Kaart is FAT16\r\n");
    bestand = sdCard->fopen("lucky.txt");
        
    if (bestand < 0)
    {
        ::printf("Probleem creating file\r\n");
        return 1;
    }

    ::printf("SD Succes\r\n");
    int index = 0;

    while (buffer[index] != '\0')
    {
        sdCard->write(bestand, buffer[index]);
        index++;
    }

    ::printf("Done\r\n");
    ::printf("Closing File\r\n");
    sdCard->fclose(bestand);
    ::printf("All done\r\n");
    return 0;
}

int main()
{
    SDCardTest test;
    test.init();
    return test.run();
}


