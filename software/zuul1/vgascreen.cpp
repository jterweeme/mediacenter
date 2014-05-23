#define SYSTEM_BUS_WIDTH 32

#include "vgascreen.h"
#include "alt_types.h"
#include <io.h>
#include "system.h"
VgaScreen::VgaScreen(unsigned int base)
{
    this->base = base;
}

void VgaScreen::clear()
{
    alt_u16 x_col,y_col;

    for(y_col=0;y_col < 480; y_col++)
        for(x_col=0;x_col< 640; x_col++)
            Vga_Clr_Pixel(base,x_col,y_col);
}

void VgaScreen::write_ctrl(unsigned char value)
{
    IOWR(base, OSD_MEM_ADDR, value);
}

void VgaScreen::Set_Pixel_On_Color(unsigned int R,unsigned int G,unsigned int B)
{
    Vga_Pixel_On_Color_R(VPG_BASE,R);
    Vga_Pixel_On_Color_G(VPG_BASE,G);
    Vga_Pixel_On_Color_B(VPG_BASE,B);
}

void VgaScreen::Set_Pixel_Off_Color(unsigned int R,unsigned int G,unsigned int B)
{
    Vga_Pixel_Off_Color_R(VPG_BASE,R);
    Vga_Pixel_Off_Color_G(VPG_BASE,G);
    Vga_Pixel_Off_Color_B(VPG_BASE,B);
}

void VgaScreen::Set_Cursor_Color(unsigned int R,unsigned int G,unsigned int B)
{
    Vga_Cursor_Color_R(VPG_BASE,R);
    Vga_Cursor_Color_G(VPG_BASE,G);
    Vga_Cursor_Color_B(VPG_BASE,B);
}



