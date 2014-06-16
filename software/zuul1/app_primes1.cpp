#include <system.h>
#include "misc.h"

class PrimesTest1
{
    QuadroSegment seg;
public:
    PrimesTest1();
    int run();
};

PrimesTest1::PrimesTest1() :
    seg(MYSEGDISP2_0_BASE)
{
    seg.setInt(0);
}

int PrimesTest1::run()
{
    for (int i = 2, aantal = 0; i < 0xffff; i++)
    {
        bool priem = true;

        for (int j = 2; j < i; j++)
            if (i % j == 0)
                priem = false;

        if (priem)
            seg.setInt(++aantal);
    }

    return 0;
}

int main()
{
    PrimesTest1 test1;
    return test1.run();
}


