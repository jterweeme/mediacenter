#include <vector>
#include <complex>
#include <iostream>
#include <sstream>

class ComplexVector : public std::vector<std::complex<double> >
{
public:
    std::string toString();
    std::complex<double> sum();
};

class Complex1
{
public:
    ComplexVector onzin;
    int run(int argc, char **argv);
};

std::complex<double> ComplexVector::sum()
{
    std::complex<double> value;

    for (iterator it = begin(); it != end(); ++it)
        value += *it;

    return value;
}

std::string ComplexVector::toString()
{
    std::ostringstream ss;
    
    for (iterator it = begin(); it != end(); ++it)
        ss << *it << std::endl;

    return ss.str();
}

int Complex1::run(int argc, char **argv)
{
    onzin.push_back(std::complex<double>(4,4));
    onzin.push_back(std::complex<double>(5,5));
    std::cout << onzin.toString() << std::endl;
    std::cout << onzin.sum() << std::endl;

    return 0;
}

int main(int argc, char **argv)
{
    Complex1 cp1;
    return cp1.run(argc, argv);
}


