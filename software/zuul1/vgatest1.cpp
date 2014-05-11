#include <system.h>
#include "misc.h"

class VGATest
{
public:
    int run();
private:
    VGA *vga;
};

int VGATest::run()
{
    vga = new VGA("/dev/video_character_buffer_with_dma_0");
    vga->draw('D', 31, 30);
}

int main()
{
    VGATest vt;
    return vt.run();
}


