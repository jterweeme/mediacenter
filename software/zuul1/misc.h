/*
Jasper ter Weeme
Alex Aalbertsberg
*/

#ifndef _MISC_H_
#define _MISC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
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
public:
    void init(FILE *fp);
    void write(char *);
private:
    FILE *fp;
};

class Audio
{
public:
    Audio();
    void init(volatile uint32_t *base);
private:
    volatile uint32_t *base;
};
#endif

