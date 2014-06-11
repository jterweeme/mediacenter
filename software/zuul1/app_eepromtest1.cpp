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

/*
    const char a[] = {0x1c, 0xfc, 0x1c, 0, 0xfc, 0, 0x1c, 0, 0, 0xe0, 0xfc, 0x1c, 0x7c, 0xfc, 0};

    for (size_t i; i < sizeof(a); i++)
        uart->putc(a[i]);
*/

    
    //uart->putc((char)0);
//    uart->putc((char)0xfc);   
//    uart->putc((char)0);
    uart->puts("+++))))AAA][0sdf");
    //::usleep(9*100);
/*
    uart->puts("3Asdf@#%456");
    uart->puts("bcdef123456");
    ::usleep(9*100);
    uart->puts("34A56");
    uart->puts("bc235@#%def123456");
    ::usleep(9*100);
    uart->puts("34A5#$5%6");
    uart->puts("bcdAef$5345123456");
    ::usleep(9*100);
    uart->puts("345&(*&(&^6");
    uart->puts("bcde^(*Af123456");
    ::usleep(9*100);
    uart->puts("absadf34A56");*/

    return 0;
}

int main()
{
    EEPromTest1 ept1;
    return ept1.run();
}


