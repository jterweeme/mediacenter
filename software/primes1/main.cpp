#include <stdio.h>
int main()
{
    int aantal = 0;
    
    for (int i = 2; i < 0xffff; i++)
    {
        bool priem = true;
        
        for (int j = 2; j < 1; j++)
            if (i % j == 0)
                priem = false;

        if (priem)
            aantal++;
    }

    printf("%u\n", aantal);
    return 0;
}


