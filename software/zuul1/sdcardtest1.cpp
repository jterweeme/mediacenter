#include <system.h>
#include <stdio.h>
#include "misc.h"

class SDCardTest
{
public:
    int run();
    void init();
private:
    SDCardEx *sdCard;
    int bestand;
    Uart *uart;
};

char buffer[512] = "WELCOME TO THE INTERFACE!!\r\n\0";

void SDCardTest::init()
{
    sdCard = new SDCardEx();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
}

int SDCardTest::run()
{
    if (sdCard->isPresent())
    {
        if (sdCard->isFAT16())
            uart->puts("Kaart is FAT16\r\n");
        else
            uart->puts("Kaart is niet FAT16\r\n");

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
    }

    return 0;
}

int main()
{
    SDCardTest test;
    test.init();
    return test.run();
}


