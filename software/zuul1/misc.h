/*
Jasper ter Weeme
Alex Aalbertsberg
*/

#ifndef _MISC_H_
#define _MISC_H_

#include <stdint.h>
#include <stdbool.h>
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

class JtagUart
{
public:
    static JtagUart *getInstance();
    void init(volatile uint32_t *base);
    void puts(const char *);
};

class SDCard
{
public:
    void init(const char *);
    bool isPresent();
    bool isFAT16();
    int fopen(char *fn);
    bool write(int sd_fileh, char c);
    bool fclose(int sd_fileh);
private:
    alt_up_sd_card_dev *sd_card_dev;
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
#endif

