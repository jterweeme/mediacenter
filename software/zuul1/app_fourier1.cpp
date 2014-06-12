class Fourier1
{
public:
    Fourier1() { }
    int run();
}

int Fourier1::run()
{
    typedef complex<double> cx;
    cx a[] = {cx(0,0), cx(1,1), cx(3,3), cx(4,4), cx(4,4), cx(3,3), cx(1,1), cx(0,0) };
    cx b[8];
    Utility::fft(a, b, 3);
    return 0;
}

int main()
{
    Fourier1 fourier1;
    return fourier1.run();
}


