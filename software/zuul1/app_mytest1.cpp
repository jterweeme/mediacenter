#define SYSTEM_BUS_WIDTH 32

#include <stdio.h>
#include <system.h>
#include <stdbool.h>
#include <sys/alt_irq.h>
#include <io.h>
#include "misc.h"

class MyTimerTest
{
public:
    void init();
    int run();
    static void isr(void *context);
};

void MyTimerTest::init()
{
    Uart *uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("init\r\n");
    ::alt_ic_isr_register(MYTIMER1_0_IRQ_INTERRUPT_CONTROLLER_ID, MYTIMER1_0_IRQ, isr, 0, 0);
}

int MyTimerTest::run()
{
    while (true)
    {
    }

    return 0;
}

void MyTimerTest::isr(void *context)
{
    Uart::getInstance()->puts("Interrupt\r\n");
    IOWR(MYTIMER1_0_BASE, 0, 0);
}

int main()
{
    MyTimerTest mtt;
    mtt.init();
    return mtt.run();
}

