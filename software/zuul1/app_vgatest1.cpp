#include <system.h>
#include <altera_up_avalon_video_pixel_buffer_dma.h>
#include "misc.h"

class VGATest
{
public:
    int run();
private:
    VGATerminal *vga;
    Uart *uart;
};

int VGATest::run()
{
    uart = Uart::getInstance();
    uart->init((volatile uint32_t *)UART_BASE);
    Uart::getInstance()->puts("onzin\r\n");
    vga = new VGATerminal("/dev/video_character_buffer_with_dma_0");
    vga->clear();
    vga->puts("The Court consists of a chief justice and eight associate ");
    vga->puts("justices who are nominated by the President and confirmed by ");
    vga->puts("the Senate. Once appointed, justices have life ");
    vga->puts("tenure unless\r\nthey resign, retire, take senior status, or ");
    vga->puts("are removed after impeachment (though none has ever been ");
    vga->puts("removed). In modern discourse, the justices are often ");
    vga->puts("categorized as having conservative, moderate, or liberal ");
    vga->puts("philosophies of law and of judicial interpretation. Each ");
    vga->puts("justice has one vote, and while many cases are decided ");
    vga->puts("unanimously, many of the highest profile cases often expose ");
    vga->puts("ideological beliefs that track with those philosophical or ");
    vga->puts("political categories. The Court meets in the United States ");
    vga->puts("Supreme Court Building in Washington, D.C.\r\n");
    vga->puts("In modern discourse\r\n");
    vga->puts("nmodern discourse\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("The Court consists of a chief justice and eight associate ");
    vga->puts("justices who are nominated by the President and confirmed by ");
    vga->puts("the Senate. Once appointed, justices have life ");
    vga->puts("tenure unless\r\nthey resign, retire, take senior status, or ");
    vga->puts("are removed after impeachment (though none has ever been ");
    vga->puts("Nieuwe regel\r\n");
    vga->puts("Nieuwe regel\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("Appelscha\r\n");
    vga->puts("Amsterdam\r\n");
    vga->puts("Nederland\r\n");
    vga->puts("The Court consists of a chief justice and eight associate ");
    vga->puts("justices who are nominated by the President and confirmed by ");
    vga->puts("the Senate. Once appointed, justices have life ");
    vga->puts("tenure unless\r\nthey resign, retire, take senior status, or ");
    vga->puts("are removed after impeachment (though none has ever been ");
    vga->puts("removed). In modern discourse, the justices are often ");
    vga->puts("categorized as having conservative, moderate, or liberal ");
    vga->puts("philosophies of law and of judicial interpretation. Each ");
    vga->puts("justice has one vote, and while many cases are decided ");
    vga->puts("unanimously, many of the highest profile cases often expose ");
    vga->puts("ideological beliefs that track with those philosophical or ");
    vga->puts("political categories. The Court meets in the United States ");
    vga->puts("Supreme Court Building in Washington, D.C.\r\n");
    //vga->shiftLeft(6);
    Uart::getInstance()->puts("onzin\r\n");
    return 0;
}

int main()
{
    alt_up_pixel_buffer_dma_dev *pixel_buf_dev = ::alt_up_pixel_buffer_dma_open_dev("/dev/video_pixel_buffer_dma_0");
    alt_up_pixel_buffer_dma_clear_screen(pixel_buf_dev, 0);
    VGATest vt;
    return vt.run();
}


