#include <system.h>
#include <priv/alt_legacy_irq.h>
#include "misc.h"

class KeyboardTest
{
public:
    void init();
    int run();
    static void isrBridge(void *context);
private:
    Uart *uart;
};

void KeyboardTest::isrBridge(void *context)
{
    Uart::getInstance()->puts("Interrupt\r\n");
}

void KeyboardTest::init()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    alt_irq_register(PS2_0_IRQ, 0, isrBridge);
    //::alt_ic_isr_register(PS2_0_IRQ_INTERRUPT_CONTROLLER_ID, PS2_0_IRQ, isrBridge, 0, 0);
}

int KeyboardTest::run()
{
    uart->puts("\033[1;31mbold red text\033[0m\r\n");

    while (true)
    {
        
    }
}

int main()
{
    KeyboardTest kbt;
    kbt.init();
    return kbt.run();
}


