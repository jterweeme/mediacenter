#define SYSTEM_BUS_WIDTH 32

#include <io.h>
#include "vgascreen.h"

int main()
{
    VGA_Ctrl_Reg vga_ctrl_set;
    vga_ctrl_set.VGA_Ctrl_Flags.RED_ON = 1;
    vga_ctrl_set.VGA_Ctrl_Flags.GREEN_ON = 1;
    vga_ctrl_set.VGA_Ctrl_Flags.BLUE_ON = 1;
    vga_ctrl_set.VGA_Ctrl_Flags.CURSOR_ON = 1;
    VgaScreen vgaScreen(VPG_BASE);
    vgaScreen.write_ctrl(vga_ctrl_set.Value);
    vgaScreen.clear();

    vgaScreen.Set_Pixel_On_Color(0, 0, 512);
    vgaScreen.Set_Pixel_Off_Color(0, 0, 0);
    vgaScreen.Set_Cursor_Color(0, 1023, 0);


    for (int i = 650; i < 15700; i++)
        IOWR(VPG_BASE, i, 1);


    return 0;
}


