#include <stdio.h>
#include <system.h>
#include "segment.h"
#include "misc.h"

int main()
{
    QuadroSegment seg((volatile uint32_t *)QUADROSEGMENT_BASE);
    Uart *uart = Uart::getInstance();
    //uart->init((volatile uint32_t *)JTAG_UART_0_BASE);
    //uart->puts("Bladiebla\r\n");
    JtagUart *jtagUart = JtagUart::getInstance();
    jtagUart->puts("Bladiebla\r\n");
    printf("Opstarten\r\n");

    int aantal = 0;
    seg.setInt(0);
    
    for (int i = 2; i < 0xffff; i++)
    {
        bool priem = true;
        
        for (int j = 2; j < i; j++)
            if (i % j == 0)
                priem = false;

        if (priem)
            seg.setInt(++aantal);
    }

    printf("%u\r\n", aantal);
    return 0;
}


