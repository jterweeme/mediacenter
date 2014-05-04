/*
Jasper ter Weeme
Alex Aalbertsberg
*/

#ifndef _MISC_H_
#define _MISC_H_
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
    InfraRood();
    Observer *observer;
    void isr(void *context);
    static void isrBridge(void *context);
};

class Uart
{
public:
    static Uart *getInstance();
    void putc(const char);
    void puts(const char *);
private:
    Uart(volatile uint32_t *base);
    volatile uint32_t *base;
};
#endif

