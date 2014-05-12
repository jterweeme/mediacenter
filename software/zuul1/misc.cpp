#define SYSTEM_BUS_WIDTH 32

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <altera_avalon_pio_regs.h>
#include <sys/alt_irq.h>
#include <io.h>
#include "misc.h"
#include <fcntl.h>

void InfraRood::init(volatile uint32_t *base, int irq, int ctl)
{
    this->base = base;
    alt_ic_isr_register(ctl, irq, isrBridge, 0, 0);
    IOWR(base, 0, 0);
}

void InfraRood::isr(void *context)
{
    if (observer != 0)
        observer->update();

    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(base, 0);
    IOWR(base, 0, 0);
}

void InfraRood::setObserver(Observer *observer)
{
    this->observer = observer;
}

void InfraRood::isrBridge(void *context)
{
    InfraRood::getInstance()->isr(context);
}

InfraRood *InfraRood::getInstance()
{
    static InfraRood instance;
    return &instance;
}

Uart *Uart::getInstance()
{
    static Uart instance;
    return &instance;
}

void Uart::init(volatile uint32_t *base)
{
    this->base = base;
}

void Uart::putc(const char c)
{
    while ((base[2] & (1<<6)) == 0)
    {
    }

    base[1] = c;
}

void Uart::puts(const char *s)
{
    while (*s)
        putc(*s++);
}

void SDCard::init(const char *name)
{
    sd_card_dev = ::alt_up_sd_card_open_dev(name);
}

bool SDCard::write(int sd_fileh, char c)
{
    return ::alt_up_sd_card_write(sd_fileh, c);
}

bool SDCard::fclose(int sd_fileh)
{
    return ::alt_up_sd_card_fclose(sd_fileh);
}

VGA::VGA(const char *devName)
{
    charBuffer = ::alt_up_char_buffer_open_dev(devName);
}

int VGA::draw(const char c, const int x, const int y)
{
    return ::alt_up_char_buffer_draw(charBuffer, c, x, y);
}

int VGA::draw(const char *s, const int x, const int y)
{
    return ::alt_up_char_buffer_string(charBuffer, s, x, y);
}

bool SoundCard::init()
{
    regWrite(15, DATA_RESET);
    regWrite(9, DATA_INACTIVE_INTERFACE);
    regWrite(9, ACTIVE);
}

bool SoundCard::regWrite(uint8_t index, uint16_t data)
{
    i2cBus->write(ADDR, 1, 1);
}

void I2C::init(volatile uint32_t *scl, volatile uint32_t *sda)
{
    this->scl = scl;
    this->sda = sda;
}

void I2C::write(uint8_t devAddr, uint8_t ctlAddr, uint8_t ctlData)
{
    start();
    stop();
    ::usleep(7*1000);
}

void I2C::start()
{
    IOWR_ALTERA_AVALON_PIO_DIRECTION(sda, 1);
    IOWR_ALTERA_AVALON_PIO_DATA(sda, 1);
    IOWR_ALTERA_AVALON_PIO_DATA(scl, 1);
    ::usleep(1);
    IOWR_ALTERA_AVALON_PIO_DATA(sda, 0);
    ::usleep(1);
    IOWR_ALTERA_AVALON_PIO_DATA(scl, 0);
    ::usleep(1);
}

void I2C::stop()
{
    IOWR_ALTERA_AVALON_PIO_DIRECTION(sda, 1);
    IOWR_ALTERA_AVALON_PIO_DATA(sda, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(scl, 1);
    ::usleep(1);
    IOWR_ALTERA_AVALON_PIO_DATA(sda, 1);
    ::usleep(1);
}

bool I2C::private_write(uint8_t data)
{
    uint8_t mask = 0x80;
    IOWR_ALTERA_AVALON_PIO_DIRECTION(sda, 1);

    for (int i = 0; i < 8; i++)
    {
        IOWR_ALTERA_AVALON_PIO_DATA(scl, 0);
        
        if (data & mask)
            IOWR_ALTERA_AVALON_PIO_DATA(sda, 1);
        else
            IOWR_ALTERA_AVALON_PIO_DATA(sda, 0);

        mask >>= 1;
        IOWR_ALTERA_AVALON_PIO_DATA(scl, 1);
        ::usleep(1);
        IOWR_ALTERA_AVALON_PIO_DATA(scl, 0);
        ::usleep(1);
    }
    
    IOWR_ALTERA_AVALON_PIO_DIRECTION(sda, 0);
    IOWR_ALTERA_AVALON_PIO_DATA(scl, 1);
    ::usleep(1);
    IOWR_ALTERA_AVALON_PIO_DATA(scl, 0);
    ::usleep(1);
    return 0;
}



