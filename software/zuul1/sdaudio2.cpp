#define SYSTEM_BUS_WIDTH 32

#include <stdint.h>
#include <io.h>
#include "AUDIO.h"
#include "AUDIO_REG.h"
#include "misc.h"

class Main
{
public:
    Main() { }
    int run();
private:
    Audio *audio;
};

int main()
{
    Uart::getInstance()->init((volatile uint32_t *)UART_BASE);
    Main *main = new Main();
    return main->run();
}

uint16_t geluid[] = {
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
0x0246,
0x0c36,
0x0cfc,
0x0c17,
0x0aee,
0x0aa0
};

int Main::run()
{
    audio = new Audio();

    if (!audio->AUDIO_Init())
        return 0;
    
    Uart::getInstance()->puts("Audio Init success\r\n");
    audio->AUDIO_SetLineOutVol(120, 120);
    Uart::getInstance()->puts("Volume set\r\n");

    for (int i = 0; i < 9999; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            AUDIO_DAC_WRITE_L(geluid[j]);
            AUDIO_DAC_WRITE_R(geluid[j]);
            //usleep(2);
        }
        //usleep(200);
    }

    for (int i = 0; i < 9999; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            AUDIO_DAC_WRITE_L(geluid[j]);
            AUDIO_DAC_WRITE_R(geluid[j]);
            usleep(2);
        }
        //usleep(200);
    }

    Uart::getInstance()->puts("Le fin\r\n");
    return 0;
}


