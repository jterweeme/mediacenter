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
};

int EEPromTest1::run()
{
    vga = new VGATerminal("/dev/video_character_buffer_with_dma_0");
    vga->clear();
    vga->puts("EEPromTest1\r\n");
    eep_i2c_bus = new I2C();
    eep_i2c_bus->init((volatile uint32_t *)EEP_I2C_SCL_BASE, (volatile uint32_t *)EEP_I2C_SDA_BASE);
    eep = new EEProm(eep_i2c_bus);
    eep->write('E');
    char c = eep->read();
    vga->putc(c);
    return 0;
}

int main()
{
    EEPromTest1 ept1;
    return ept1.run();
}


