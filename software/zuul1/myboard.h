#ifndef _MYBOARD_H_
#define _MYBOARD_H_

#include <system.h>

class MyBoard
{
public:
    static const volatile uint32_t *uarT_BASE = (static volatile uint32_t *)UART_BASE;
};

#endif


