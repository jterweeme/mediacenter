#include <system.h>
#include "misc.h"

class VGATest
{
public:
    int run();
private:
    VGATerminal *vga;
};

int VGATest::run()
{
    vga = new VGATerminal("/dev/video_character_buffer_with_dma_0");
    vga->clear();
    vga->puts("The Court consists of a chief justice and eight associate ");
    vga->puts("justices who are nominated by the President and confirmed by ");
    vga->puts("the Senate. Once appointed, justices have life tenure unless\r\nthey resign, retire, take senior status, or are removed after impeachment (though none has ever been removed). In modern discourse, the justices are often categorized as having conservative, moderate, or liberal philosophies of law and of judicial interpretation. Each justice has one vote, and while many cases are decided unanimously, many of the highest profile cases often expose ideological beliefs that track with those philosophical or political categories. The Court meets in the United States Supreme Court Building in Washington, D.C.");
}

int main()
{
    VGATest vt;
    return vt.run();
}


