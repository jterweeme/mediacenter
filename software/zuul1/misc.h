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
#include "sdcard.h"

extern const uint8_t lut[];

template <class T> class Segment
{
protected:
    volatile T *base;
public:
    Segment(volatile T *base) { this->base = base; }
    void write(T value) { *base = value; }
};

class DuoSegment : public Segment<uint16_t>
{
public:
    DuoSegment(volatile uint16_t *base) : Segment<uint16_t>(base) { }
    void setInt(unsigned int n);
    void setHex(uint8_t n);
};

class QuadroSegment : public Segment<uint32_t>
{
public:
    QuadroSegment(volatile uint32_t *base) : Segment<uint32_t>(base) { }
    void setInt(unsigned int n);
    void setHex(uint16_t n);
};

class CombinedSegment
{
public:
    CombinedSegment(DuoSegment *l, DuoSegment *r, QuadroSegment *q);
    void setInt(unsigned int n);
    void setHex(uint32_t n);
private:
    DuoSegment *l;
    DuoSegment *r;
    QuadroSegment *q;
};

class Observer
{
public:
    virtual void update() = 0;
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
public:
    static InfraRood *getInstance();
    void setObserver(Observer *);
    void init(volatile uint32_t *base, int irq, int ctl);
private:
    InfraRood() { }
    Observer *observer;
    void isr(void *context);
    static void isrBridge(void *context);
    volatile uint32_t *base;
};

class Uart
{
public:
    static Uart *getInstance();
    void init(volatile uint32_t *base);
    void putc(const char);
    void puts(const char *);
private:
    Uart() { }
    volatile uint32_t *base;
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
    int fd;
public:
    void init(int fd) { this->fd = fd; }
    void write(const char *s) { ::write(fd, s, ::strlen(s)); }
};

class I2C
{
private:
    volatile uint32_t *scl;
    volatile uint32_t *sda;
    void start();
    void stop();
    bool private_write(uint8_t data);
    void private_read(uint8_t *data, bool ack);
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
    static const uint8_t ADDR = 0x34;
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
};

class VGA
{
private:
    alt_up_char_buffer_dev *charBuffer;
public:
    VGA(const char *devName);
    int clear() { ::alt_up_char_buffer_clear(charBuffer); }
    int draw(const char, const int, const int);
    int draw(const char *, const int, const int);
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

