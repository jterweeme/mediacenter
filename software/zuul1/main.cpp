#include <stdio.h>
#include <system.h>
#include <stdint.h>
#include <altera_avalon_lcd_16207.h>
#include "lcd.h"

class DuoSegmentLinks
{
public:
    DuoSegmentLinks(volatile uint16_t *base);
};

class DuoSegmentRechts
{
public:
    DuoSegmentRechts(volatile uint16_t *base);
private:
    volatile uint16_t *base;
};

class QuadroSegment
{
public:
    QuadroSegment(volatile uint32_t *base);
private:
    volatile uint32_t *base;
};

class CombinedSegment
{
public:
    CombinedSegment();
};

QuadroSegment::QuadroSegment(volatile uint32_t *base)
{
    this->base = base;
}

int main()
{
    volatile uint16_t *segLinks = (volatile uint16_t *)DUOSEGMENTLINKS_BASE;
    volatile uint16_t *segRechts = (volatile uint16_t *)DUOSEGMENTRECHTS_BASE;
    volatile uint32_t *quadro = (volatile uint32_t *)QUADROSEGMENT_BASE;
    *segLinks = 0x40f9;
    *segRechts = 0x3010;
    *quadro = 0x92820010;

    while (true)
        ::printf("Hallo wereld\r\n");

    return 0;
}


