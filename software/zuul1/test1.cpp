#define SYSTEM_BUS_WIDTH 32

#include <stdio.h>
#include <system.h>
#include <stdint.h>
#include <sys/alt_irq.h>
#include <io.h>
#include <altera_avalon_pio_regs.h>
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
private:
    CombinedSegment *cs;
public:
    Beam(CombinedSegment *cs) { this->cs = cs; }
    void update();
};

void Beam::update()
{
    unsigned int button = IORD(INFRARED_0_BASE, 0);
    cs->setInt(button);

    switch (button % 10000)
    {
    case TerasicRemote::A:
        Uart::getInstance()->puts("A\r\n");
        break;
    case TerasicRemote::B:
        Uart::getInstance()->puts("B\r\n");
        break;
    case TerasicRemote::C:
        Uart::getInstance()->puts("C\r\n");
        break;

    }
}

class Test1
{
public:
    void init();
private:
    DuoSegmentLinks segmentLinks;
    DuoSegmentRechts segmentRechts;
    QuadroSegment *segmentQuadro;
    InfraRood *ir;
    Uart *uart;
};

int main()
{
    Test1 test1;
    test1.init();

    while (true) { }

    return 0;
}

void Test1::init()
{
    segmentQuadro = new QuadroSegment((volatile uint32_t *)QUADROSEGMENT_BASE);
    segmentQuadro->setInt(12345);
    segmentLinks.write(0x3024);
    segmentLinks.write(0x2430);
    segmentLinks.setInt(12345);
    segmentRechts.write(0x9992);
    segmentRechts.setHex(0x32);
    ir = InfraRood::getInstance();

    ir->init((volatile uint32_t *)INFRARED_0_BASE, INFRARED_0_IRQ,
                        INFRARED_0_IRQ_INTERRUPT_CONTROLLER_ID);

    ir->setObserver(new Beam(new CombinedSegment(&segmentLinks, &segmentRechts, segmentQuadro)));
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    uart->puts("Opstarten\r\n");
}


