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

    void write(const char *s)
    {
        ::write(fd, s, ::strlen(s));
    }
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
    void write(const char *);
};

class Audio
{
public:
    void init(volatile uint32_t *base, const char *name);
    void write(const char *s);
private:
    volatile uint32_t *base;
    const char *name;
    int fd;
};

class Audio2
{
private:
    int fd;
public:
    void init(int fd) { this->fd = fd; }

    void write(const char *s)
    {
        ::write(fd, s, ::strlen(s));
    }
};

#endif

