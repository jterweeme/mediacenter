#include <system.h>
#include "misc.h"

class MD5Test1
{
public:
    void init();
private:
    Uart *uart;
    SDCardEx *sdCard;
};

void MD5Test1::init()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Initialize MD5Test1\r\n");
    sdCard = new SDCardEx();
    sdCard->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    uint8_t buf[5120000];
    MyFile *myFile;

    if (sdCard->isPresent())
    {
        if (sdCard->isFAT16())
        {
            myFile = sdCard->openFile("ENIKDA~1.WAV");

            for (int i = 0; i < 5120000; i++)
                buf[i] = myFile->read();

            uart->puts("done\r\n");
        }
    }
}

int main()
{
    MD5Test1 md5test1;
    md5test1.init();
    return 0;
}


