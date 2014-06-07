#include <system.h>
#include "misc.h"

class EEPromTest1
{
public:
    int run();
private:
    I2C *eep_i2c_bus;
    EEProm *eep;
    VGATerminal *vga;
    Uart *uart;
};

int EEPromTest1::run()
{
/*
    vga = new VGATerminal("/dev/video_character_buffer_with_dma_0");
    vga->clear();
    vga->puts("EEPromTest1\r\n");
    eep_i2c_bus = new I2C();
    eep_i2c_bus->init((volatile uint32_t *)EEP_I2C_SCL_BASE, (volatile uint32_t *)EEP_I2C_SDA_BASE);
    eep = new EEProm(eep_i2c_bus);
    eep->write('E');
    char c = eep->read();
    vga->putc(c);
*/

    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    const char a[] = {0, 0xd0, 0x61, 0, 0, 0x61, 0x00};
    //uart->putc((char)0x62);

    for (size_t i = 0; i < sizeof(a); i++)
    {
        ::usleep(10);
        uart->putc(a[i]);
    }

/*
    uart->puts("Abcdef123456");
    ::usleep(9*100);
    uart->puts("3A456");
    uart->puts("bcdef123456");
    ::usleep(9*100);
    uart->puts("34A56");
    uart->puts("bcdef123456");
    ::usleep(9*100);
    uart->puts("34A56");
    uart->puts("bcdAef123456");
    ::usleep(9*100);
    uart->puts("3456");
    uart->puts("bcdeAf123456");
    ::usleep(9*100);
    uart->puts("34A56");*/

    return 0;
}

int main()
{
    EEPromTest1 ept1;
    return ept1.run();
}


