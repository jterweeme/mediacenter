/*
Jasper ter Weeme
*/

#define SYSTEM_BUS_WIDTH 32

#include <system.h>
#include <io.h>
#include <priv/alt_legacy_irq.h>
#include <altera_up_avalon_ps2.h>
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
    Uart::getInstance()->puts("Interrupt2\r\n");
    IOWR(PS2_0_BASE, 8, 0);
}
    alt_up_ps2_dev *ps2dev = alt_up_ps2_open_dev("/dev/ps2_0");

void KeyboardTest::init()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    alt_up_ps2_init(ps2dev);
    ::printf("%d\r\n", ps2dev->device_type);
    alt_up_ps2_enable_read_interrupt(ps2dev);
    alt_irq_register(PS2_0_IRQ, 0, isrBridge);
    //::alt_ic_isr_register(PS2_0_IRQ_INTERRUPT_CONTROLLER_ID, PS2_0_IRQ, isrBridge, 0, 0);
}

int KeyboardTest::run()
{
    uart->puts("\033[1;31mbold red text\033[0m\r\n");
    uint8_t ps2data;
    uint8_t previous;

    while (true)
    {
        alt_up_ps2_read_data_byte(ps2dev, &ps2data);
        
        if (ps2data != previous)
            ::printf("%d\r\n", ps2data);

        previous = ps2data;
    }
}

int main()
{
    KeyboardTest kbt;
    kbt.init();
    return kbt.run();
}


