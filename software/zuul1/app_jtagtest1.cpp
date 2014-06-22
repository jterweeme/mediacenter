#include <system.h>
#include "misc.h"
#include <fcntl.h>

class JtagTest1
{
    JtagUart ju;
public:
    JtagTest1();
    int run();
};

JtagTest1::JtagTest1()
  :
    ju((uint32_t *)JTAG_UART_0_BASE)
{
}

int JtagTest1::run()
{
    while (true)
    {
        ju.puts("Hallo Niceberg\r\n");

        for (volatile int i = 0; i < 0xffff; i++)
        {
        }
    }

    return 0;
}

int main()
{
    JtagTest1 test1;
    return test1.run();
}

