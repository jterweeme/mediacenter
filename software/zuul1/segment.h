/*
Jasper ter Weeme
Alex Aalbertsberg
*/

#ifndef _SEGMENT_H_
#define _SEGMENT_H_
#include <stdint.h>

template <class T> class Segment
{
private:
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
};

class CombinedSegment
{
public:
    CombinedSegment();
};
#endif

