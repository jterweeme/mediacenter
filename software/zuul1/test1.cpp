#define SYSTEM_BUS_WIDTH 32

#include <stdio.h>
#include <system.h>
#include <stdint.h>
#include <altera_avalon_lcd_16207.h>
#include <sys/alt_irq.h>
#include <io.h>
#include <altera_avalon_pio_regs.h>
#include "segment.h"
#include "misc.h"

class DuoSegmentLinks : public DuoSegment
{
public:
    DuoSegmentLinks() : DuoSegment((volatile uint16_t *)DUOSEGMENTLINKS_BASE) { }
};

class DuoSegmentRechts : public DuoSegment
{
public:
    DuoSegmentRechts() : DuoSegment((volatile uint16_t *)DUOSEGMENTRECHTS_BASE) { }
};

class Beam : public Observer
{
public:
    void update();
};

QuadroSegment segmentQuadro((volatile uint32_t *)QUADROSEGMENT_BASE);
int i = 0;

void Beam::update()
{
    Uart::getInstance()->puts("Zap!\r\n");
    segmentQuadro.setInt(++i);
}

class Test1
{
};

int main()
{
    DuoSegmentLinks segmentLinks;
    DuoSegmentRechts segmentRechts;
    segmentLinks.write(0x2430);
    segmentRechts.write(0x9992);
    segmentQuadro.write(0x82f80010);
    segmentQuadro.setInt(9876);
    InfraRood *ir = InfraRood::getInstance();

    ir->init((volatile uint32_t *)INFRARED_0_BASE, INFRARED_0_IRQ,
                        INFRARED_0_IRQ_INTERRUPT_CONTROLLER_ID);

    ir->setObserver(new Beam());
    Uart *uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Opstarten\r\n");

    while (true) { }

    return 0;
}


