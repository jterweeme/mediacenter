#include <stdio.h>
#include <system.h>
#include <stdint.h>
#include <altera_avalon_lcd_16207.h>
#include "lcd.h"

int main()
{
    volatile uint16_t *segLinks = (volatile uint16_t *)DUOSEGMENTLINKS_BASE;
    volatile uint32_t *quadro = (volatile uint32_t *)QUADROSEGMENT_BASE;
    *segLinks = 0x0010;
    *quadro = 0x00000010;

    while (true)
        ::printf("Hallo wereld\r\n");

    return 0;
}


