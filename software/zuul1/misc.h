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
    bool isPresent();
    bool isFAT16();
    int fopen(char *);
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
    uint8_t read(uint8_t devAddr, uint8_t ctlAddr);
public:
    void init(volatile uint32_t *scl, volatile uint32_t *sda);
    I2C() { }
    I2C(volatile uint32_t *scl, volatile uint32_t *sda) { init(scl, sda); }
    void write(uint8_t devAddr, uint8_t ctlAddr, uint8_t ctlData);

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

