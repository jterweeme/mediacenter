#include <system.h>
#include "misc.h"
#include "sdcard.h"

class TestText1
{
public:
    int run();
    void init();
    Uart *uart;
    SDCardEx *sd;
};

void TestText1::init()
{
    sd = new SDCardEx();
    sd->init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
}

int TestText1::run()
{
    return 0;
}

int main()
{
    TestText1 tt1;
    return tt1.run();
}


