#include <system.h>
#include "misc.h"
#include <fcntl.h>

class JtagTest1
{
private:
    JtagUart ju;
public:
    int run();
};

int JtagTest1::run()
{
    ju.init(::open(JTAG_UART_0_NAME, O_WRONLY));

    while (true)
    {
        ju.write("Hallo Niceberg\r\n");

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

