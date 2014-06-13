#include <iostream>
#include <complex>
#include <cmath>
#include <iterator>
#include <vector>

typedef std::complex<double> Sample;
typedef std::complex<double> complex;
typedef std::complex<double> cx;

class Signaal : public std::vector<Sample>
{
public:
    Signaal() : std::vector<Sample>() { }
    Signaal(size_t size) : std::vector<Sample>(size) { }
    Signaal(Sample *in, Sample *length) : std::vector<Sample>(in, length) { }
    Signaal fft(int log2n);
};



unsigned int bitReverse(unsigned int x, int log2n)
{
    int n = 0;

    for (int i=0; i < log2n; i++)
    {
        n <<= 1;
        n |= (x & 1);
        x >>= 1;
    }

    return n;
}

const double PI = 3.1415926536;

Signaal Signaal::fft(int log2n)
{
    const Sample J(0,1);
    unsigned int n = 1 << log2n;
    Signaal uitvoer(this->size());

/*
    for (iterator it = begin(); it < at(n); it++)
    {
        
    }*/

    for (unsigned int i = 0; i < n; ++i)
        uitvoer[bitReverse(i, log2n)] = *(this[i]);
        
    return *this;
}

void fft(Signaal &a, Signaal &b, unsigned int log2n)
{
    const cx J(0, 1);
    unsigned int n = 1 << log2n;

    for (unsigned int i=0; i < n; ++i)
        b[bitReverse(i, log2n)] = a[i];
 
    for (unsigned int s = 1; s <= log2n; ++s)
    {
        unsigned int m = 1 << s;
        unsigned int m2 = m >> 1;
        cx w(1, 0);
        cx wm = exp(-J * (PI / m2));

        for (unsigned int j=0; j < m2; ++j)
        {
            for (unsigned int k=j; k < n; k += m)
            {
                cx t = w * b[k + m2];
                cx u = b[k];
                b[k] = u + t;
                b[k + m2] = u - t;
            }

            w *= wm;
        }
    }
}

int main()
{
    
    cx a[] = { cx(0,0), cx(1,1), cx(3,3), cx(4,4), cx(4, 4), cx(3, 3), cx(1,1), cx(0,0) };

    Signaal domein1(a, a + sizeof(a) / sizeof(a[0]));
    Signaal domein2 = domein1.fft(3);
    fft(domein1, domein2, 3);

    for (Signaal::iterator it = domein2.begin(); it < domein2.end(); ++it)
        std::cout << *it << std::endl;
}
