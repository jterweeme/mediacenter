/*
Jasper ter Weeme
Alex Aalbertsberg
*/

#ifndef _MISC_H_
#define _MISC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <altera_up_avalon_video_character_buffer_with_dma.h>
#include <priv/alt_file.h>
#include "mystl.h"

typedef mstd::complex<double> Sample;

class Signaal : public mstd::vector<Sample>
{
public:
    Signaal(const int capacity) : mstd::vector<Sample>(capacity) { }
    Signaal fft(const unsigned log2n);
};

class Utility
{
public:
    static void toHex(uint8_t input, char *output);
    static void to_bytes(uint32_t val, uint8_t *bytes);
    static uint32_t to_int32(const uint8_t *bytes);
    static uint16_t be_16_toh(uint16_t x);
    static uint32_t be_32_toh(uint32_t x);
    static const double PI = 3.1415926536;
    static unsigned int bitReverse(unsigned int x, const int log2n);
    static void itoa(int n, char *s);

/*
    static void fft(ComplexDouble *a, ComplexDouble *b, int log2n)
    {
        const ComplexDouble J(0, 1);
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
};

extern const uint8_t lut[];



template <class T> class Segment
{
protected:
    volatile T *base;
public:
    Segment(volatile T * const base) : base(base) { }
    void write(T value) { *base = value; }
};

class DuoSegment : public Segment<uint16_t>
{
public:
    DuoSegment(volatile uint16_t * const base) : Segment<uint16_t>(base) { }
    void setInt(unsigned int n);
    void setHex(uint8_t n);
};

class QuadroSegment : public Segment<uint32_t>
{
public:
    QuadroSegment(volatile uint32_t * const base) : Segment<uint32_t>(base) { }
    void setInt(unsigned int n);
    void setHex(uint16_t n);
};

class CombinedSegment
{
private:
    DuoSegment *l;
    DuoSegment *r;
    QuadroSegment *q;
public:
    CombinedSegment(DuoSegment *l, DuoSegment *r, QuadroSegment *q)
        : l(l), r(r), q(q)
    { }

#ifdef CPP11
    CombinedSegment(volatile uint16_t *l, volatile uint16_t *r, volatile uint32_t *q)
        : CombinedSegment(new DuoSegment(l), new DuoSegment(r), new QuadroSegment(q))
    { }
#endif

    void setInt(unsigned int n);
    void setHex(uint32_t n);
};

class GreenLeds
{
private:
    volatile uint8_t *base;
public:
    GreenLeds(volatile uint8_t *base) : base(base) { }
    void set(uint8_t value) { *base = value; }
};

class Observer
{
public:
    virtual void update() = 0;
    virtual ~Observer() { }
};

class KeyBoard
{
private:
    volatile uint8_t *base;
public:
    void init(volatile uint8_t *base) { this->base = base; }
};

class TerasicRemote
{
public:
    static const uint16_t A = 0xf00f;
    static const uint16_t B = 0xec13;
    static const uint16_t C = 0xef10;
    static const uint16_t POWER = 0xed12;
    static const uint16_t NUM_0 = 0xff00;
    static const uint16_t NUM_1 = 0xfe01;
    static const uint16_t NUM_2 = 0xfd02;
    static const uint16_t NUM_3 = 0xfc03;
    static const uint16_t NUM_4 = 0xfb04;
    static const uint16_t NUM_5 = 0xfa05;
    static const uint16_t NUM_6 = 0xf906;
    static const uint16_t NUM_7 = 0xf807;
    static const uint16_t NUM_8 = 0xf708;
    static const uint16_t NUM_9 = 0xf609;
    static const uint16_t CH_UP = 0xe51a;
    static const uint16_t CH_DOWN = 0xe11e;
    static const uint16_t VOL_UP = 0xe41b;
    static const uint16_t VOL_DOWN = 0xe01f;
    static const uint16_t MENU = 0xee11;
    static const uint16_t RETURN = 0xe817;
    static const uint16_t PLAY = 0xe916;
    static const uint16_t LEFT = 0xeb14;
    static const uint16_t RIGHT = 0xe718;
    static const uint16_t MUTE = 0xf30c;
};

class InfraRood
{
private:
    InfraRood() { }
    Observer *observer;
    void isr(void *context);
    static void isrBridge(void *context);
    volatile uint32_t * base;
public:
    static InfraRood *getInstance();
    void setObserver(Observer *);
    void init(volatile uint32_t * const base, int irq, int ctl);
    uint32_t read() { return base[0]; }
};

class Uart2
{
protected:
    volatile uint32_t *base;
public:
    Uart2() { }
    Uart2(volatile uint32_t * const base) : base(base) { }
    void putc(const char);
    void puts(const char *s) { while (*s) putc(*s++); }
    void printf(const char *format, ...);
};

class Uart : public Uart2  // singleton versie
{
private:
    static Uart *instance;
public:
    Uart();
    Uart(volatile uint32_t *const base);
    static Uart *getInstance();
    void init(volatile uint32_t *base) { this->base = base; }
};

class JtagUart
{
private:
    int fd;
public:
    void init(int fd) { this->fd = fd; }
    void write(const char *s) { ::write(fd, s, ::strlen(s)); }
};

class LCD
{
private:
    volatile uint8_t *base;
    static const uint8_t COMMAND_REG = 0;
    static const uint8_t DATA_REG = 1;
    static const uint8_t CMDCLEAR = 1;
    static const uint8_t CMDHOME = 2;
public:
    LCD(volatile uint8_t *base) : base(base) { }
    inline void putc(const char c) { base[DATA_REG] = c; }
    inline void puts(const char *s) { while (*s) putc(*s++); }
    inline void cmd(uint8_t c) { base[COMMAND_REG] = c; }
    inline void clear() { cmd(CMDCLEAR); }
    inline void home() { cmd(CMDHOME); }
    void setPos(uint8_t x, uint8_t y);
};

class I2C
{
private:
    volatile uint32_t *scl;
    volatile uint32_t *sda;
public:
    void start();
    void stop();
    bool private_write(uint8_t data);
    void private_read(uint8_t *data, bool ack);
private:
    static const uint8_t DATA = 0;
    static const uint8_t DIRECTION = 1;
    static const uint8_t PIO_INPUT = 0;
    static const uint8_t PIO_OUTPUT = 1;
public:
    void init(volatile uint32_t *scl, volatile uint32_t *sda);
    I2C() { }
    I2C(volatile uint32_t *scl, volatile uint32_t *sda) { init(scl, sda); }
    void write(uint8_t devAddr, uint8_t ctlAddr, uint8_t ctlData);
    uint8_t read(uint8_t devAddr, uint8_t ctlAddr);
};

class SoundCard
{
private:
    I2C *i2cBus;
    volatile uint32_t *base;
public:
    SoundCard(I2C *i2cBus, volatile uint32_t *base) { this->i2cBus = i2cBus; this->base = base; }
    bool init();
    bool regWrite(uint8_t index, uint16_t data);
    void writeDacOut(uint16_t left, uint16_t right);
    void setOutputVolume(int vol);
    void setSampleRate(uint8_t sr);
    static const uint8_t I2C_ADDR = 0x34;
    static const uint8_t ADDR = I2C_ADDR;
    static const uint16_t DATA_RESET = 0;
    static const uint16_t DATA_INACTIVE_INTERFACE = 0;
    static const uint16_t DATA_LEFT_LINE_IN_VOLUME = 0x0017;
    static const uint16_t DATA_RIGHT_LINE_IN_VOLUME = 0x0017;
    static const uint16_t DATA_LEFT_HEADPHONE_OUT_VOLUME = 0x005b;
    static const uint16_t DATA_RIGHT_HEADPHONE_OUT_VOLUME = 0x005b;
    static const uint16_t ACTIVE = 1;
    static const uint8_t RATE_ADC48K_DAC48K = 0;
    static const uint8_t RATE_ADC48K_DAC8K = 1;
    static const uint8_t RATE_ADC8K_DAC48K = 2;
    static const uint8_t RATE_ADC8K_DAC8K = 3;
    static const uint8_t RATE_ADC32K_DAC32K = 4;
    static const uint8_t RATE_ADC96K_DAC96K = 5;
    static const uint8_t RATE_ADC44K1_DAC44K1 = 6;
};

class EEProm
{
private:
    I2C *bus;
public:
    EEProm(I2C *bus) { this->bus = bus; }
    uint8_t read();
    void write(uint8_t);
    static const uint8_t I2C_ADDR = 0x50;
};

class PixelBuffer
{
public:
};

class VGA
{
private:
    alt_up_char_buffer_dev *charBuffer;
    alt_up_char_buffer_dev *openDev(const char *name);
public:
    VGA(const char *devName);
    int clear() { return ::alt_up_char_buffer_clear(charBuffer); }
    int draw(const char, const int, const int);
    int draw(const char *, const int, const int);
    void shiftLeft(int n);
};

class VGATerminal : public VGA
{
private:
    int x, y;
    static const uint8_t COLS = 80;
public:
    VGATerminal(const char *devName) : VGA(devName) { x = y = 0; }
    void putc(const char c);
    void puts(const char *s) { while (*s) putc(*s++); }
};

#endif

