#include <stdio.h>
#include <system.h>
#include <stdint.h>
#include <altera_avalon_lcd_16207.h>
#include <sys/alt_irq.h>
#include "lcd.h"
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

InfraRood::InfraRood()
{
    init();
}

void InfraRood::init()
{
    alt_ic_isr_register(INFRARED_0_IRQ_INTERRUPT_CONTROLLER_ID, INFRARED_0_IRQ, isr, 0, 0);
}

void InfraRood::isr(void *context)
{
    ::printf("InfraRood\r\n");
}

InfraRood *InfraRood::getInstance()
{
    static InfraRood instance;
    return &instance;
}

Uart::Uart(volatile uint32_t *base)
{
    this->base = base;
}

void Uart::putc(const char c)
{
    while ((base[2] & (1<<6)) == 0)
    {
    }

    base[1] = c;
}

void Uart::puts(const char *s)
{
    while (*s)
        putc(*s++);
}

int main()
{
    DuoSegmentLinks segmentLinks;
    DuoSegmentRechts segmentRechts;
    QuadroSegment segmentQuadro((volatile uint32_t *)QUADROSEGMENT_BASE);

    segmentLinks.write(0xf940);
    segmentRechts.write(0xf910);
    segmentQuadro.write(0x10829230);

    InfraRood *ir = InfraRood::getInstance();
    Uart uart((volatile uint32_t *)UART_BASE);
    uart.puts("Opstarten\r\n");

    while (true) { }

    return 0;
}


