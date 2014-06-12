#include <system.h>
#include "misc.h"
#include "sdcard.h"

class SDCardTest3
{
public:
    void init();
private:
    SDCard2 *sd;
    Uart *uart;
};

void SDCardTest3::init()
{
    sd = new SDCard2((void *)ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE);
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Init SDCardTest3\r\n");
    sd->waitForInsert();

    for (int i = 0; i < 28800; i++)
        sd->read(i);
    
    for (int i = 0; i < 512; i++)
    {
        char x[3] = {0};
        Utility::toHex(sd->data[i], x);
        uart->puts(x);
    }

    uart->puts("\r\n");
    uart->puts("done\r\n");
}

int main()
{
    SDCardTest3 sd3;
    sd3.init();
    return 0;
}


