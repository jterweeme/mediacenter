#include <system.h>
#include <stdio.h>
#include "misc.h"

class SDCardTest
{
public:
    int run();
private:
    SDCard *sdCard;
    int bestand;
};

char buffer[512] = "WELCOME TO THE INTERFACE!!\r\n\0";

int SDCardTest::run()
{
    sdCard = new SDCard();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    
    if (sdCard->isPresent())
    {
        if (sdCard->isFAT16())
            ::printf("Kaart is FAT16\r\n");
        else
            ::printf("Kaart is niet FAT16\r\n");

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
    return test.run();
}


