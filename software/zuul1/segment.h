/*
Jasper ter Weeme
Alex Aalbertsberg
*/

#ifndef _SEGMENT_H_
#define _SEGMENT_H_
#include <stdint.h>

uint8_t lut[] = {0x40, 0xf9, 0x24, 0x30, 0x99, 0x92, 0x82, 0xf8, 0x00, 0x10};

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
};

class QuadroSegment : public Segment<uint32_t>
{
public:
    QuadroSegment(volatile uint32_t *base) : Segment<uint32_t>(base) { }
    void setInt(int n)
    {
        int a = n / 1000 % 10;
        int b = n / 100 % 10;
        int c = n / 10 % 10;
        int d = n % 10;
        *base = lut[a] << 24 | lut[b] << 16 | lut[c] << 8 | lut[d];
    }
};

class CombinedSegment
{
public:
    CombinedSegment();
};
#endif

