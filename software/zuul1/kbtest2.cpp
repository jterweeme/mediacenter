#define SYSTEM_BUS_WIDTH 32

#include <system.h>
#include <io.h>
#include <stdint.h>
#include <stdio.h>
#include "misc.h"
#include <sys/alt_irq.h>

class KeyBoardTest2
{
public:
    void init();
    int run();
    uint8_t keystroke;
    static void isrBridge(void *context);
private:
    Uart *uart;
    DuoSegment *segLinks;
};

void KeyBoardTest2::isrBridge(void *context)
{
    Uart::getInstance()->puts("Interrupt\r\n");
    IOWR(ITOETSENBORD_BASE, 0, 0);
}

void KeyBoardTest2::init()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Init KBTest2\r\n");
    ::printf("Init\r\n");
    segLinks = new DuoSegment((volatile uint16_t *)DUOSEGMENTLINKS_BASE);
    segLinks->setHex(0);
    alt_ic_isr_register(ITOETSENBORD_IRQ_INTERRUPT_CONTROLLER_ID, ITOETSENBORD_IRQ, isrBridge, 0, 0); 
}

int KeyBoardTest2::run()
{
    volatile uint8_t *keyboard = (volatile uint8_t *)ITOETSENBORD_BASE;

    while (true)
    {
        keystroke = keyboard[0];
        segLinks->setHex(keystroke);
    }
    return 0;
}

int main()
{
    KeyBoardTest2 kbt2;
    kbt2.init();
    return kbt2.run();
    
}


