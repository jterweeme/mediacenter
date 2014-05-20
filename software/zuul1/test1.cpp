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
    uint32_t button = IORD(INFRARED_0_BASE, 0);
    cs->setHex(button);
    Uart *uart = Uart::getInstance();

    switch (button >> 16)
    {
    case TerasicRemote::A:
        uart->puts("A\r\n");
        break;
    case TerasicRemote::B:
        uart->puts("B\r\n");
        break;
    case TerasicRemote::C:
        uart->puts("C\r\n");
        break;
    case TerasicRemote::POWER:
        uart->puts("Power\r\n");
        break;
    case TerasicRemote::NUM_0:
        uart->puts("0\r\n");
        break;
    case TerasicRemote::NUM_1:
        uart->puts("1\r\n");
        break;
    case TerasicRemote::NUM_2:
        uart->puts("2\r\n");
        break;
    case TerasicRemote::NUM_3:
        uart->puts("3\r\n");
        break;
    case TerasicRemote::NUM_4:
        uart->puts("4\r\n");
        break;
    case TerasicRemote::NUM_5:
        uart->puts("5\r\n");
        break;
    case TerasicRemote::NUM_6:
        uart->puts("6\r\n");
        break;
    case TerasicRemote::NUM_7:
        uart->puts("7\r\n");
        break;
    case TerasicRemote::NUM_8:
        uart->puts("8\r\n");
        break;
    case TerasicRemote::NUM_9:
        uart->puts("9\r\n");
        break;
    case TerasicRemote::CH_UP:
        uart->puts("Channel Up\r\n");
        break;
    case TerasicRemote::CH_DOWN:
        uart->puts("Channel Down\r\n");
        break;
    case TerasicRemote::VOL_UP:
        uart->puts("Volume Up\r\n");
        break;
    case TerasicRemote::VOL_DOWN:
        uart->puts("Volume Down\r\n");
        break;
    case TerasicRemote::MENU:
        uart->puts("Menu\r\n");
        break;
    case TerasicRemote::RETURN:
        uart->puts("Return\r\n");
        break;
    case TerasicRemote::PLAY:
        uart->puts("Play\r\n");
        break;
    case TerasicRemote::LEFT:
        uart->puts("Left\r\n");
        break;
    case TerasicRemote::RIGHT:
        uart->puts("Right\r\n");
        break;
    case TerasicRemote::MUTE:
        uart->puts("Mute\r\n");
        break;
    default:
        uart->puts("Onbekende knop ingeduwd\r\n");
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
    VGATerminal *vgaTerminal;
    GreenLeds *gl;
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
    vgaTerminal = new VGATerminal("/dev/video_character_buffer_with_dma_0");
    vgaTerminal->clear();
    segmentQuadro = new QuadroSegment((volatile uint32_t *)MYSEGDISP2_0_BASE);
    segmentQuadro->setInt(12345);
    segmentQuadro->setHex(0xabcd);
    segmentLinks.write(0x3024);
    segmentLinks.write(0x2430);
    segmentLinks.setInt(12345);
    segmentRechts.write(0x9992);
    segmentRechts.setHex(0x3f);
    ir = InfraRood::getInstance();
    int ctl = INFRARED_0_IRQ_INTERRUPT_CONTROLLER_ID;
    ir->init((volatile uint32_t *)INFRARED_0_BASE, INFRARED_0_IRQ, ctl);
    ir->setObserver(new Beam(new CombinedSegment(&segmentLinks, &segmentRechts, segmentQuadro)));
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    vgaTerminal->puts("Opstarten\r\n");
    gl = new GreenLeds((volatile uint8_t *)LEDG_BASE);
    gl->set(0x01);
}


