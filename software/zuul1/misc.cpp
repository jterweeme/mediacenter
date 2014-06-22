#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/alt_irq.h>
#include "misc.h"
#include <fcntl.h>
#include <math.h>

void Utility::toHex(const uint8_t input, char *output)
{
    const uint8_t foo = input & 0x0f;
    const uint8_t bar = input >> 4;
    output[0] = bar < 10 ? (char)(bar + 48) : (char)(bar + 55);
    output[1] = foo < 10 ? (char)(foo + 48) : (char)(foo + 55);
}

void Utility::to_bytes(const uint32_t val, uint8_t *bytes)
{
    bytes[0] = (uint8_t) val;
    bytes[1] = (uint8_t) (val >> 8);
    bytes[2] = (uint8_t) (val >> 16);
    bytes[3] = (uint8_t) (val >> 24);
}

uint32_t Utility::to_int32(const uint8_t * const bytes)
{
    return (uint32_t) bytes[0]
        | ((uint32_t) bytes[1] << 8)
        | ((uint32_t) bytes[2] << 16)
        | ((uint32_t) bytes[3] << 24);
}

/*
void Utility::itoa(int n, char *s)
{
    int i, sign;

    if ((sign = n) < 0)
        n = -n;

    i = 0;

    do
    {
        s[i++] = n % 10 + '0';
    }
    while ((n /= 10) > 0);

    if (
}*/

const Sample& jexp(const Sample& s)
{
    //return Sample(::exp(s.r), ::exp(s.i));
    return Sample(s.r, s.i);
}

Signaal Signaal::fft(const unsigned log2n)
{
    Signaal uitvoer(8);
    uitvoer.size = 8;
    
    const Sample J(0, -1);
    const unsigned n = 1 << log2n;

    for (unsigned i = 0; i < n; ++i)
        uitvoer[Utility::bitReverse(i, log2n)] = buffer[i];

    Uart *uart = Uart::getInstance();

    for (unsigned s = 1; s <= log2n; ++s)
    {
        const unsigned m = 1 << s;
        const unsigned m2 = m >> 1;
        Sample w(1,0);
        const double pim = Utility::PI / m2;
        Sample foo = J * pim;
        //Sample wm = ::jexp(foo);
        Sample wm = Sample(::exp(foo.r), ::exp(foo.i));
        uart->puts(wm.toString());
        uart->puts("\r\n");
        
        for (unsigned j = 0; j < m2; ++j)
        {
            for (unsigned k = j; k < n; k += m)
            {
                Sample t = w * uitvoer[k + m2];
                Sample u = uitvoer[k];
                uitvoer[k] = u + t;
                uitvoer[k + m2] = u - t;
            }

            w *= wm;
        }
    }

    return uitvoer;
}

void JtagUart::putc(const char c)
{
    while (*ctl & 0xffff0000 == 0) { }
    base[0] = c;
}

JtagUart *JtagUart::instance;

unsigned int Utility::bitReverse(unsigned int x, const int log2n)
{
    int n = 0;
    
    for (int i = 0; i < log2n; i++)
    {
        n <<= 1;
        n |= (x & 1);
        x >>= 1;
    }
    
    return n;
}

// 0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f
const uint8_t lut[] = {0x40, 0xf9, 0x24, 0x30, 0x99, 0x92, 0x82, 0xf8,
                0x00, 0x10, 0x08, 0x83, 0xc6, 0xa1, 0x86, 0x8e};

void DuoSegment::setInt(const unsigned int n)
{
    unsigned int a = n / 10 % 10;
    unsigned int b = n % 10;
    *base = lut[a] << 8 | lut[b];
}

void DuoSegment::setHex(const uint8_t n)
{
    *base = lut[n >> 4] << 8 | lut[n % 16];
}

void QuadroSegment::setHex(const uint16_t n)
{
    *base = lut[n >> 12] << 24 | lut[(n >> 8) % 16] << 16 | lut[(n >> 4) % 16] << 8 | lut[n % 16];
}

void CombinedSegment::setHex(const uint32_t n)
{
    l->setHex(n >> 24);
    r->setHex(n >> 16);
    q->setHex((uint16_t)n);
}

void CombinedSegment::setInt(const unsigned n)
{
    l->setInt(n / 1000000);
    r->setInt(n / 10000);
    q->setInt(n);
}

void QuadroSegment::setInt(const unsigned n)
{
    unsigned int a = n / 1000 % 10;
    unsigned int b = n / 100 % 10;
    unsigned int c = n / 10 % 10;
    unsigned int d = n % 10;
    *base = lut[a] << 24 | lut[b] << 16 | lut[c] << 8 | lut[d];
}

/*
template<class T> void Utility::fft(T a, T b, const int log2n)
{
    typedef typename mstd::iterator_traits<T>::value_type complex;
    const complex J(0, 1);
    int n = 1 << log2n;
    
    for (unsigned int i = 0; i < n; ++i)
        b[Utility::bitReverse(i, log2n)] = a[i];

    for (int s = 1; s <= log2n; ++s)
    {
        int m = 1 << s;
        int m2 = m >> 1;
        complex w(1, 0);
        complex wm = exp(-J * (PI / m2));

        for (int j = 0; j < m2; ++j)
        {
            for (int k = j; k < n; k += m)
            {
                complex t = w * b[k + m2];
                complex u = b[k];
                b[k] = u + t;
                b[k + m2] = u - t;
            }

            w *= wm;
        }
    }
}*/

void LCD::setPos(uint8_t x, uint8_t y)
{
}

alt_up_char_buffer_dev *VGA::openDev(const char *name)
{
    alt_up_char_buffer_dev *dev = (alt_up_char_buffer_dev *)alt_find_dev(name, &alt_dev_list);
    return dev;
}

uint16_t Utility::be_16_toh(uint16_t x)
{
    return (x & 0x00ff) << 8 | (x & 0xff00) >> 8;
}

uint32_t Utility::be_32_toh(uint32_t x)
{
    x = (x & 0x0000ffff) << 16 | (x & 0xffff0000) >> 16;
    return (x & 0x00ff00ff) << 8 | (x & 0xff00ff00) >> 8;
}

InfraRoodBase::InfraRoodBase(volatile void * const base, const int irq,
    const int ctl, alt_isr_func isr)
  :
    base(base),
    base32((volatile uint32_t * const)base)
{
    ::alt_ic_isr_register(ctl, irq, isr, 0, 0);
    base32[0] = 0;
}

InfraRood::InfraRood(volatile void * const base, const int irq, const int ctl) :
    InfraRoodBase(base, irq, ctl, isrBridge)
{
    instance = this;
}

void InfraRood::isr(void *context)
{
    if (observer != 0)
        observer->update();

    base32[3] = 0;
    base32[0] = 0;    // reset interrupt
}

InfraRood *InfraRood::instance;

Uart *Uart::getInstance()
{
    if (!instance)
    {
        static Uart inst;
        instance = &inst;
    }

    return instance;
}

Uart *Uart::instance;

Uart::Uart() : Uart2()
{
    if (instance)
        throw "Bestaat al";

    instance = this;
}

Uart::Uart(volatile uint32_t * const base) : Uart2(base)
{
    if (instance)
        throw "Bestaat al";

    instance = this;
}

void Uart2::printf(const char *format, ...)
{
    va_list argp;
    va_start(argp, format);

    for (const char *p = format; *p != '\0'; p++)
    {
        if (*p != '%')
        {
            putc(*p);
            continue;
        }

        switch (*++p)
        {
        case 'u':   // unsigned int
        {
            //int i = va_arg(argp, int);
            //this->puts(itoa(i));
            putc('9');
        }
            break;
        case 's':   // string
        {
            char *s = va_arg(argp, char *);
            this->puts(s);
        }
            break;
        case 'x':
            //char c[3] = {0};
            break;
        }
    }

    va_end(argp);
}

void Uart2::putc(const char c)
{
    while ((base[2] & (1<<6)) == 0) { }
    base[1] = c;
}

VGA::VGA(const char *devName)
{
    for (size_t i = 0; i < sizeof(buffer); i++)
        buffer[i] = 0;

    charBuffer = openDev(devName);
}

int VGA::draw(const char c, int x, int y)
{
    if (x >= 80 || y >= 60)
        return -1;

    buffer[y * 80 + x] = c;
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

    if (y >= 60)
    {
        shiftLeft(80);
        y--;
        putc(c);
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
    ::alt_up_char_buffer_clear(charBuffer);

    for (size_t i = 0; i < sizeof(buffer); i++)
        buffer[i] = (i + n) <= sizeof(buffer) ? buffer[i+n] : 0;
        
   
    for (int row = 0; row < 50; row++)
        for (int column = 0; column < 80; column++)
            draw(buffer[row * 80 + column], column, row);
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



