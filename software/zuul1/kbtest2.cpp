#define SYSTEM_BUS_WIDTH 32

#include <system.h>
#include <io.h>
#include <stdint.h>
#include <stdio.h>

int main()
{
    uint8_t keystroke;

    while (true)
    {
        keystroke = IORD(TOETSENBORD1_0_BASE, 0);
        ::printf("%u\r\n", keystroke);
    }

    return 0;
}


