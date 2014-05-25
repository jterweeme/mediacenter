#ifndef _MYBOARD_H_
#define _MYBOARD_H_

#include <system.h>

namespace MyBoard
{
    class DUart
    {
    public:
        static volatile uint32_t *base() { return (volatile uint32_t *)UART_BASE; }
        static volatile uint32_t *base2 = UART_BASE;
    };
}

#endif


