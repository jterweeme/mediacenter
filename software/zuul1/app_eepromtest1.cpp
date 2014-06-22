#include <system.h>
#include "misc.h"

class EEPromTest1
{
    I2C *eep_i2c_bus;
    EEProm *eep;
    VGATerminal *vga;
    Uart uart;
public:
    EEPromTest1();
    int run();
};

EEPromTest1::EEPromTest1()
  :
    uart((uint32_t *)UART_BASE)
{
}

int EEPromTest1::run()
{
    uart.puts("+++))))AAA][0sdf");
    return 0;
}

int main()
{
    EEPromTest1 ept1;
    return ept1.run();
}


