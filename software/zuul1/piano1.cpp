#include <system.h>
#include "misc.h"

class Piano1
{
public:
    void init();
private:
    I2C *i2c;
    SoundCard *soundCard;
};

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

void Piano1::init()
{
    i2c = new I2C((volatile uint32_t *)SND_I2C_SCL_BASE, (volatile uint32_t *)SND_I2C_SDA_BASE);
    soundCard = new SoundCard(i2c, (volatile uint32_t *)AUDIO_IF_0_BASE);
    soundCard->init();

    for (int i = 0; i < 9999; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            soundCard->writeDacOut(geluid[j], geluid[j]);
            //usleep(2);
        }
        //usleep(200);
    }

    for (int i = 0; i < 9999; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            soundCard->writeDacOut(geluid[j], geluid[j]);
            usleep(2);
        }
        //usleep(200);
    }
}

int main()
{
    Piano1 piano1;
    piano1.init();
    return 0;
}


