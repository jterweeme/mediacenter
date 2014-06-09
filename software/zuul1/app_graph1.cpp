#include <system.h>
#include <stdint.h>

int main()
{
    volatile uint32_t *pixelBuffer = (volatile uint32_t *)SRAM_BASE;
    
    static const uint16_t WIDTH = 840;
    static const uint16_t HEIGHT = 480;

    for (uint16_t i = 0; i < WIDTH * HEIGHT; i++)
        pixelBuffer[i] = 0;

    return 0;
}


