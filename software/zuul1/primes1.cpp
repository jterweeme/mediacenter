#include <stdio.h>
#include <system.h>
#include <fcntl.h>
#include <unistd.h>
#include "segment.h"
#include "misc.h"

class PrimesTest1
{
public:
    int run();
private:
    QuadroSegment *seg;
    Uart *uart;
    LCD *lcd;
};

int PrimesTest1::run()
{
    seg = new QuadroSegment((volatile uint32_t *)QUADROSEGMENT_BASE);
    uart = Uart::getInstance();

    int aantal = 0;
    seg->setInt(0);
    lcd = new LCD();
    lcd->init(::open(CHARACTER_LCD_0_NAME, O_WRONLY));
    lcd->write("Niceberg");

    for (int i = 2; i < 0xffff; i++)
    {
        bool priem = true;

        for (int j = 2; j < i; j++)
            if (i % j == 0)
                priem = false;

        if (priem)
            seg->setInt(++aantal);
    }

    ::printf("%u\r\n", aantal);
    return 0;
}

int main()
{
    PrimesTest1 test1;
    return test1.run();
}


