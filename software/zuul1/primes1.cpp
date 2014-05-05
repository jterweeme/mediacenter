#include <stdio.h>
#include <system.h>
#include "misc.h"

int main()
{
    Uart *uart = Uart::getInstance();
    printf("Opstarten\r\n");

    int aantal = 0;
    
    for (int i = 2; i < 0x1f; i++)
    {
        bool priem = true;
        
        for (int j = 2; j < i; j++)
            if (i % j == 0)
                priem = false;

        if (priem)
            aantal++;
    }

    printf("%u\r\n", aantal);
    return 0;
}


