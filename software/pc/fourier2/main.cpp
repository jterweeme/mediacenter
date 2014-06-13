#include <iostream>
#include <complex>
#include <cmath>
#include <iterator>
#include <vector>

typedef std::complex<double> Sample;

class Signaal
{
public:
    std::vector<Sample> data;
    Signaal(std::vector<Sample> &data) : data(data) { }
    std::vector<Sample> fft(unsigned int log2n);
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

std::vector<Sample> Signaal::fft(unsigned int log2n)
{
    const Sample J(0,1);
    unsigned int n = 1 << log2n;
    std::vector<Sample> uitvoer(data.size());

    for (unsigned int i = 0; i < n; ++i)
        uitvoer[bitReverse(i, log2n)] = data[i];

    for (unsigned int s = 1; s <= log2n; ++s)
    {
        unsigned int m = 1 << s;
        unsigned int m2 = m >> 1;
        Sample w(1,0);
        Sample wm = exp(-J * (PI / m2));

        for (unsigned int j = 0; j < m2; ++j)
        {
            for (unsigned int k = j; k < n; k += m)
            {
                Sample t = w * uitvoer[k + m2];
                Sample u = uitvoer[k];
                uitvoer[k] = u + t;
                uitvoer[k + m2] = u - t;
            }

            w *= wm;
        }
    }

    return uitvoer;
}

int main()
{
    typedef std::complex<double> cx;
    cx a[] = { cx(0,0), cx(1,1), cx(3,3), cx(4,4), cx(4, 4), cx(3, 3), cx(1,1), cx(0,0) };

    std::vector<Sample> domein1(a, a + sizeof(a) / sizeof(a[0]));
    Signaal domein1s(domein1);
    std::vector<Sample> uitvoer = domein1s.fft(3);

    for (std::vector<Sample>::iterator it = uitvoer.begin(); it < uitvoer.end(); it++)
        std::cout << *it << std::endl;
    
    return 0;
}
