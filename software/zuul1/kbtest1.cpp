#include <system.h>
#include "misc.h"

class KeyboardTest
{
public:
    void init();
    int run();
private:
    Uart *uart;
};

void KeyboardTest::init()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
}

int KeyboardTest::run()
{
    uart->puts("\033[1;31mbold red text\033[0m\n");
}

int main()
{
    KeyboardTest kbt;
    kbt.init();
    return kbt.run();
}


