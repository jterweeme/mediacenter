#define SYSTEM_BUS_WIDTH 32

#include <system.h>
#include <io.h>
#include "misc.h"

class Beam : public Observer
{
private:
    SoundCard *soundCard;
public:
    Beam(SoundCard *soundCard) { this->soundCard = soundCard; }
    void update();
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

class Piano1
{
public:
    void init();
private:
    I2C *i2c;
    SoundCard *soundCard;
    InfraRood *ir;
};

void Beam::update()
{
    uint32_t button = IORD(INFRARED_0_BASE, 0);
    
    switch (button >> 16)
    {
    case TerasicRemote::A:
        for (int i = 0; i < 9999; i++)
        {
            for (int j = 0; j < 12; j++)
            {
                soundCard->writeDacOut(geluid[j], geluid[j]);
            }
        }
        break;
    case TerasicRemote::B:
        for (int i = 0; i < 9999; i++)
        {
            for (int j = 0; j < 12; j++)
            {
                soundCard->writeDacOut(geluid[j], geluid[j]);
                usleep(3);
            }
        }
        break;
    default:
        for (int i = 0; i < 5999; i++)
        {
            for (int j = 0; j < 12; j++)
            {
                soundCard->writeDacOut(geluid[j], geluid[j]);
                usleep(2);
            }
            //usleep(200);
        }
        break;
    }
}



void Piano1::init()
{
    i2c = new I2C((volatile uint32_t *)SND_I2C_SCL_BASE, (volatile uint32_t *)SND_I2C_SDA_BASE);
    soundCard = new SoundCard(i2c, (volatile uint32_t *)AUDIO_IF_0_BASE);
    soundCard->init();
    soundCard->setOutputVolume(100);
    ir = InfraRood::getInstance();
    volatile uint32_t *irBase = (volatile uint32_t *)INFRARED_0_BASE;
    ir->init(irBase, INFRARED_0_IRQ, INFRARED_0_IRQ_INTERRUPT_CONTROLLER_ID);
    ir->setObserver(new Beam(soundCard));

}

int main()
{
    Piano1 piano1;
    piano1.init();
    return 0;
}


