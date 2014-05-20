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
    DuoSegment *segLinks;
    static void isrBridge(void *context);
};

void KeyBoardTest2::isrBridge(void *context)
{
    ::printf("Interrupt\r\n");
    IOWR(TOETSENBORD1_0_BASE, 0, 0);
}

void KeyBoardTest2::init()
{
    segLinks = new DuoSegment((volatile uint16_t *)DUOSEGMENTRECHTS_BASE);
    segLinks->setHex(0);
    alt_ic_isr_register(TOETSENBORD1_0_IRQ_INTERRUPT_CONTROLLER_ID, TOETSENBORD1_0_IRQ, isrBridge, 0, 0); 
}

int KeyBoardTest2::run()
{
    volatile uint8_t *keyboard = (volatile uint8_t *)TOETSENBORD1_0_BASE;

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


