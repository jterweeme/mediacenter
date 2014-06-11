#include <iostream>

class Options
{
public:
    Options() { }
};

int main(int argc, char **argv)
{
    int i = 0;

    for (;std::cin && i < 0x44; std::cin.get())
    {
        i++;
    }

    short int sample;

    while (std::cin)
    {
        std::cin.read((char *)&sample, sizeof(sample));
        std::cout << sample << std::endl;
    }

    std::cout << i << std::endl;

    return 0;
}


