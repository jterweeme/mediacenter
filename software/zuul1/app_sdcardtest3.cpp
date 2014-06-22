#include <system.h>
#include "misc.h"
#include "sdcard.h"

class SDCardTest3
{
    SDCard2 *sd;
    Uart uart;
public:
    SDCardTest3();
    int run();
};

SDCardTest3::SDCardTest3()
  :
    uart((uint32_t *)UART_BASE)
{
}

int SDCardTest3::run()
{
    sd = new SDCard2((void *)ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE);
    uart.puts("Init SDCardTest3\r\n");
    sd->waitForInsert();

    for (int i = 0; i < 28800; i++)
        sd->read(i);
    
    for (int i = 0; i < 512; i++)
    {
        char x[3] = {0};
        Utility::toHex(sd->data[i], x);
        uart.puts(x);
    }

    uart.puts("\r\n");
    uart.puts("done\r\n");
    return 0;
}

int main()
{
    SDCardTest3 sd3;
    sd3.run();
    return 0;
}


