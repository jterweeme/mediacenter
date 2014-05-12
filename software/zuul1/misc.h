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
#include <Altera_UP_SD_Card_Avalon_Interface.h>
#include <altera_up_avalon_video_character_buffer_with_dma.h>

class Observer
{
public:
    virtual void update() = 0;
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

class SDCard
{
public:
    void init(const char *);
    bool isPresent() { ::alt_up_sd_card_is_Present(); }
    bool isFAT16() { ::alt_up_sd_card_is_FAT16(); }
    int fopen(char *fn) { ::alt_up_sd_card_fopen(fn, true); }
    bool write(int, char);
    bool fclose(int);
private:
    alt_up_sd_card_dev *sd_card_dev;
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
public:
    SoundCard(I2C *i2cBus) { this->i2cBus = i2cBus; }
    bool init();
    bool regWrite(uint8_t index, uint16_t data);
    static const uint8_t ADDR = 0x34;
    static const uint16_t DATA_RESET = 0;
    static const uint16_t DATA_INACTIVE_INTERFACE = 0;
    static const uint16_t DATA_LEFT_LINE_IN_VOLUME = 0x0017;
    static const uint16_t DATA_RIGHT_LINE_IN_VOLUME = 0x0017;
    static const uint16_t DATA_LEFT_HEADPHONE_OUT_VOLUME = 0x005b;
    static const uint16_t DATA_RIGHT_HEADPHONE_OUT_VOLUME = 0x005b;
    static const uint16_t ACTIVE = 1;
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

#endif

