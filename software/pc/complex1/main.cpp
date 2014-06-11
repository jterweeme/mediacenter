#include <vector>
#include <complex>

class Complex1
{
public:
    std::vector<std::complex<double> > onzin;
    int run();
};

int Complex1::run(int argc, char **argv)
{
    
    return 0;
}

int main(int argc, char **argv)
{
    Complex1 cp1;
    return cp1.run(argc, argv);
}


