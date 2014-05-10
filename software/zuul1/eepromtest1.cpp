#include <system.h>
#include "misc.h"

class EEPromTest1
{
public:
    int run();
private:
    I2C *eep_i2c_bus;
    EEProm *eep;
};

int EEPromTest1::run()
{
    eep_i2c_bus = new I2C();
    eep_i2c_bus->init((volatile uint32_t *)EEP_I2C_SCL_BASE, (volatile uint32_t *)EEP_I2C_SDA_BASE);
    eep = new EEProm(eep_i2c_bus);
    return 0;
}

int main()
{
    EEPromTest1 ept1;
    return ept1.run();
}


