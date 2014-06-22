#include <stdlib.h>
#include "mystl.h"
#include "misc.h"

class DuoSegmentLinks : public ::DuoSegment
{
public:
    DuoSegmentLinks() : DuoSegment((volatile uint16_t *)VA_S1_BASE) { }
};

class DuoSegmentRechts : public ::DuoSegment
{
public:
    DuoSegmentRechts() : DuoSegment((volatile uint16_t *)VA_S2_BASE) { }
};

class Fourier1
{
public:
    Fourier1();
    int run();
    ::Uart uart;
    ::DuoSegmentLinks segLinks;
    ::DuoSegmentRechts segRechts;
};

Fourier1::Fourier1()
  :
    uart((uint32_t *)UART_BASE)
{
}

int Fourier1::run()
{
    typedef Sample cx;
    uart.puts("Fourier test\r\n");
    segLinks.setInt(10);
    mstd::complex<double> x(5, 4);
    uart.printf("%u\r\n", 5);

    cx a[] = {cx(0,0), cx(1,1), cx(3,3), cx(4,4), cx(4,4), cx(3,3), cx(1,1), cx(0,0) };
    Signaal s1(20);

    for (int i = 0; i < 8; i++)
        s1.push_back(a[i]);

    Signaal s2 = s1.fft(3);
    return 0;
}

int main()
{
    Fourier1 fourier1;
    return fourier1.run();
}


