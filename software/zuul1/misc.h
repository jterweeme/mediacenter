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
private:
    void init();
    InfraRood();
    Observer *rh;
    static void isr(void *context);
};

class Uart
{
public:
    Uart(volatile uint32_t *base);
    void putc(const char);
    void puts(const char *);
private:
    volatile uint32_t *base;
};
#endif

