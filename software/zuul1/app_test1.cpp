#include <system.h>
#include <stdint.h>
#include <sys/alt_irq.h>
#include "misc.h"

class Beam : public Observer
{
    CombinedSegment *cs;
public:
    Beam(CombinedSegment *cs) : cs(cs) { }
    void update();
};

void Beam::update()
{
    uint32_t button = InfraRood::getInstance()->read();
    cs->setHex(button);
    Uart *uart = Uart::getInstance();

    switch (button >> 16)
    {
    case ::TerasicRemote::A:
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
    ::DuoSegment segmentLinks;
    ::DuoSegment segmentRechts;
    ::QuadroSegment segmentQuadro;
    ::InfraRood ir;
    ::Uart uart;
    ::VGATerminal vgaTerminal;
    ::GreenLeds gl;
    ::LCD *lcd;
    ::EEProm *eeprom;
    ::I2C *i2cBus1;
public:
    void init();
    Test1();
};

Test1::Test1()
  :
    segmentLinks(VA_S1_BASE),
    segmentRechts(VA_S2_BASE),
    segmentQuadro(MYSEGDISP2_0_BASE),
    ir((uint32_t *)INFRARED_0_BASE, INFRARED_0_IRQ, INFRARED_0_IRQ_INTERRUPT_CONTROLLER_ID),
    uart((uint32_t *)UART_BASE),
    vgaTerminal("/dev/video_character_buffer_with_dma_0"),
    gl((volatile uint8_t *)LEDG_BASE)
{
    vgaTerminal.clear();
    vgaTerminal.puts("Opstarten\r\n");
    gl.set(0x03);
}

int main()
{
    ::Test1 test1;
    test1.init();

    while (true) { }

    return 0;
}

void Test1::init()
{
    ir.setObserver(new Beam(new CombinedSegment(&segmentLinks, &segmentRechts, &segmentQuadro)));
    lcd = new LCD((volatile uint8_t *)LCD_BASE);
    lcd->puts("Xargon");
}


