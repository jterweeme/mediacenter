#define SYSTEM_BUS_WIDTH 32

#include <system.h>
#include <io.h>
#include <stdint.h>
#include <stdio.h>
#include "misc.h"

class KeyBoardTest2
{
public:
    void init();
    int run();
    uint8_t keystroke;
    DuoSegment *segLinks;
};

void KeyBoardTest2::init()
{
    segLinks = new DuoSegment((volatile uint16_t *)DUOSEGMENTLINKS_BASE);
    segLinks->setHex(0);
}

int KeyBoardTest2::run()
{
    while (true)
    {
        keystroke = IORD(TOETSENBORD1_0_BASE, 0);
        segLinks->setHex(keystroke);
    }
    return 0;
}

int main()
{
    KeyBoardTest2 kbt2;
    kbt2.init();
    return kbt2.run();
}


