#include <stdio.h>
#include <system.h>
#include <stdint.h>
#include <altera_avalon_lcd_16207.h>
#include "lcd.h"
#include "segment.h"

class InfraRood
{
public:
    InfraRood();
};

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

int main()
{
    DuoSegmentLinks segmentLinks;
    DuoSegmentRechts segmentRechts;
    QuadroSegment segmentQuadro((volatile uint32_t *)QUADROSEGMENT_BASE);

    segmentLinks.write(0xf940);
    segmentRechts.write(0xf910);
    segmentQuadro.write(0x10829230);

    while (true)
        ::printf("Hallo wereld\r\n");

    return 0;
}


