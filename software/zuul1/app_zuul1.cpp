#include "misc.h"
#include <unistd.h>

class Zuul1
{
    Uart uart;
    VGATerminal vga;
public:
    Zuul1();
};

Zuul1::Zuul1()
  :
    uart((uint32_t *)UART_BASE),
    vga("/dev/video_character_buffer_with_dma_0")
{
    vga.clear();
    vga.puts("Zuul1");

    for (int i = 0; i < 100; i++)
    {
        ::usleep(1000*1000);
        vga.putc(uart.read());
    }
}

int main()
{
    Zuul1 zuul1;
    return 0;
}


