/*
Oude rommel nu
*/

#define SYSTEM_BUS_WIDTH 32

#include <io.h>
#include <system.h>
#include <stdint.h>
#include "misc.h"

uint8_t *base = (uint8_t *)ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE;
int *command_argument_register = ((int *)(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE + 0x22c));
short int *command_register = ((short int *)(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE + 0x230));
short int *aux_status_register = ((short int *)(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_BASE + 0x234));
short int status;

int main()
{
    do
    {
        status = (short int)IORD_16DIRECT(aux_status_register, 0);
    }
    while ((status & 0x02) == 0);


    IOWR_32DIRECT(command_argument_register, 0, (9) * 512);
    IOWR_16DIRECT(command_register, 0, 17);

    do
    {
        status = (short int)IORD_16DIRECT(aux_status_register, 0);
    }
    while ((status & 0x04) != 0);

    Uart *uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);

    for (int i = 0; i < 512; i++)
    {
        char x[3] = {0};
        Utility::toHex(base[i], x);
        uart->puts(x);
    }

    uart->puts("\r\n");
    return 0;
}


