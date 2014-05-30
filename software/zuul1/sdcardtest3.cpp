#include <system.h>
#include "misc.h"

class SDCardTest3
{
public:
    void init();
private:
    SDCard2 *sd;
    Uart *uart;
};

void toHex(uint8_t input, char *output)
{
    uint8_t foo = input & 0x0f;
    uint8_t bar = input >> 4;
    output[1] = foo < 10 ? (char)(foo + 48) : (char)(foo + 55);
    output[0] = bar < 10 ? (char)(bar + 48) : (char)(bar + 55);
}

void SDCardTest3::init()
{
    sd = new SDCard2((void *)ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE);
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Init SDCardTest3\r\n");
    sd->waitForInsert();

    for (int i = 0; i < 28800; i++)
        sd->read(i);
    
    char x[3] = {0};
    for (int i = 0; i < 512; i++)
    {
        toHex(sd->data[i], x);
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


