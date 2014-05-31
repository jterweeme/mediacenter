#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/alt_irq.h>
#include "misc.h"
#include <fcntl.h>

void Utility::toHex(uint8_t input, char *output)
{
    uint8_t foo = input & 0x0f;
    uint8_t bar = input >> 4;
    output[0] = bar < 10 ? (char)(bar + 48) : (char)(bar + 55);
    output[1] = foo < 10 ? (char)(foo + 48) : (char)(foo + 55);
}

// 0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f
const uint8_t lut[] = {0x40, 0xf9, 0x24, 0x30, 0x99, 0x92, 0x82, 0xf8,
                0x00, 0x10, 0x08, 0x83, 0xc6, 0xa1, 0x86, 0x8e};

void DuoSegment::setInt(unsigned int n)
{
    unsigned int a = n / 10 % 10;
    unsigned int b = n % 10;
    *base = lut[a] << 8 | lut[b];
}

void DuoSegment::setHex(uint8_t n)
{
    *base = lut[n >> 4] << 8 | lut[n % 16];
}

void QuadroSegment::setHex(uint16_t n)
{
    *base = lut[n >> 12] << 24 | lut[(n >> 8) % 16] << 16 | lut[(n >> 4) % 16] << 8 | lut[n % 16];
}

void CombinedSegment::setHex(uint32_t n)
{
    l->setHex(n >> 24);
    r->setHex(n >> 16);
    q->setHex((uint16_t)n);
}

void CombinedSegment::setInt(unsigned int n)
{
    l->setInt(n / 1000000);
    r->setInt(n / 10000);
    q->setInt(n);
}

void QuadroSegment::setInt(unsigned int n)
{
    unsigned int a = n / 1000 % 10;
    unsigned int b = n / 100 % 10;
    unsigned int c = n / 10 % 10;
    unsigned int d = n % 10;
    *base = lut[a] << 24 | lut[b] << 16 | lut[c] << 8 | lut[d];
}

void LCD::setPos(uint8_t x, uint8_t y)
{
}

alt_up_char_buffer_dev *VGA::openDev(const char *name)
{
    alt_up_char_buffer_dev *dev = (alt_up_char_buffer_dev *)alt_find_dev(name, &alt_dev_list);
    return dev;
}

void SDCard2::command(uint16_t cmd, uint32_t arg1)
{
    *argument_reg = arg1;
    *command_reg = cmd;
    while (*aux_status & 0x04) { }  // wait until complete

}

void InfraRood::init(volatile uint32_t * base, int irq, int ctl)
{
    this->base = base;
    alt_ic_isr_register(ctl, irq, isrBridge, 0, 0);
    base[0] = 0;
}

void InfraRood::isr(void *context)
{
    if (observer != 0)
        observer->update();

    base[3] = 0;
    base[0] = 0;    // reset interrupt
}

void InfraRood::setObserver(Observer *observer)
{
    this->observer = observer;
}

void InfraRood::isrBridge(void *context)
{
    InfraRood::getInstance()->isr(context);
}

InfraRood *InfraRood::getInstance()
{
    static InfraRood instance;
    return &instance;
}

Uart *Uart::getInstance()
{
    static Uart instance;
    return &instance;
}

void Uart::putc(const char c)
{
    while ((base[2] & (1<<6)) == 0)
    {
    }

    base[1] = c;
}

VGA::VGA(const char *devName)
{
    charBuffer = openDev(devName);
}

int VGA::draw(const char c, const int x, const int y)
{
    return ::alt_up_char_buffer_draw(charBuffer, c, x, y);
}

int VGA::draw(const char *s, const int x, const int y)
{
    return ::alt_up_char_buffer_string(charBuffer, s, x, y);
}

void VGATerminal::putc(const char c)
{
    if (c == 0x0a)
    {
        y++;
        return;
    }

    if (c == 0x0d)
    {
        x = 0;
        return;
    }

    this->draw(c, x++, y);

    if (x >= COLS)
    {
        x = 0;
        y++;
    }
}

bool SoundCard::init()
{
    regWrite(15, DATA_RESET);
    regWrite(9, DATA_INACTIVE_INTERFACE);
    regWrite(0, 0x0017);
    regWrite(1, 0x0017);
    regWrite(2, 0x005b);
    regWrite(3, 0x005b);
    regWrite(4, 0x0015 | 0x20 | 0x08 | 0x01);
    regWrite(5, 0x0000);
    regWrite(6, 0);
    regWrite(7, 0x0042);
    //regWrite(8, 0x0002);
    regWrite(8, 0x20);
    regWrite(9, ACTIVE);
    return true;
}

void SoundCard::setOutputVolume(int vol)
{
    if(vol < 128)
    {
        regWrite(2, vol);
        regWrite(3, vol);
    }
}

void KarFile::readToBuf()
{
    buf = new uint8_t[256*1024];    // zodat de grootste erin past
    
    //for (int i = 0; 
}

/*
const char *KarFile::getText()
{
    uint8_t c;
    static char returnString[100] = {0};
    
    int i = 0;
    
    do
    {
        c = myFile->read();
        returnString[i++] = c;
    }
    while (c > 10);

    return returnString;
}*/

bool SoundCard::regWrite(uint8_t index, uint16_t data)
{
    uint8_t foo = data & 0xff;
    uint8_t control = (index << 1) & 0xfe;
    control |= ((foo >> 8) & 0x01);
    i2cBus->write(I2C_ADDR, control, data);
    ::usleep(50 * 1000);
    return true;
}

void SoundCard::writeDacOut(uint16_t left, uint16_t right)
{
    base[0] = left;
    base[1] = right;
}

void SoundCard::setSampleRate(uint8_t ssr)
{
    uint16_t control;

    switch (ssr)
    {
        case RATE_ADC44K1_DAC44K1:
            control = (0x8) << 2;
            break;
    }

    control |= 0x02;
    regWrite(8, control);
}

void VGA::shiftLeft(int n)
{
    
}

void EEProm::write(uint8_t c)
{
    bus->start();
    bus->private_write(I2C_ADDR);
    bus->private_write('E');
    bus->stop();
    ::usleep(7*1000);
    //bus.write(I2C_ADDR, 
}

uint8_t EEProm::read()
{
    uint8_t value;
    bus->start();
    bus->private_write(I2C_ADDR);
    bus->private_read(&value, false);
    bus->stop();
    return value;
}

void I2C::init(volatile uint32_t *scl, volatile uint32_t *sda)
{
    this->scl = scl;
    this->sda = sda;
}

void I2C::write(uint8_t devAddr, uint8_t ctlAddr, uint8_t ctlData)
{
    start();
    private_write(devAddr);
    private_write(ctlAddr);
    private_write(ctlData);
    stop();
    ::usleep(7*1000);
}

void I2C::start()
{
    sda[DIRECTION] = PIO_OUTPUT;
    sda[DATA] = 1;
    scl[DATA] = 1;
    ::usleep(1);
    sda[DATA] = 0;
    ::usleep(1);
    scl[DATA] = 0;
    ::usleep(1);
}

void I2C::stop()
{
    sda[DIRECTION] = PIO_OUTPUT;
    sda[DATA] = 0;
    scl[DATA] = 1;
    ::usleep(1);
    sda[DATA] = 1;
    ::usleep(1);
}

void I2C::private_read(uint8_t *data, bool ack)
{
    uint8_t foo = 0;
    sda[DIRECTION] = PIO_INPUT;
    scl[DATA] = 0;
    ::usleep(1);
    
    for (int i = 0; i < 8; i++)
    {
        foo <<= 1;
        ::usleep(1);
        
        if (sda[DATA])
            foo |= 1;

        scl[DATA] = 0;
        ::usleep(1);
    }

    scl[DATA] = 0;
    sda[DIRECTION] = PIO_OUTPUT;
    sda[DATA] = ack ? 0 : 1;
    scl[DATA] = 1;
    ::usleep(1);
    scl[DATA] = 0;
    ::usleep(1);
    sda[DATA] = 0;
    ::usleep(1);
    *data = foo;
}

bool I2C::private_write(uint8_t data)
{
    uint8_t mask = 0x80;
    sda[DIRECTION] = PIO_OUTPUT;

    for (int i = 0; i < 8; i++)
    {
        scl[DATA] = 0;
        sda[DATA] = (data & mask) ? 1 : 0;
        mask >>= 1;
        scl[DATA] = 1;
        ::usleep(1);
        scl[DATA] = 0;
        ::usleep(1);
    }
    
    sda[DIRECTION] = PIO_INPUT;
    scl[DATA] = 1;
    ::usleep(1);
    scl[DATA] = 0;
    ::usleep(1);
    return 0;
}



