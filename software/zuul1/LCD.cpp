#define SYSTEM_BUS_WIDTH 32

#include "terasic_includes.h"
#include "LCD.h"

// (\b): Move the cursor to the left by one character
// (\r): Move the cursor to the start of the current line
// (\n): Move the cursor to the start of the line and move it donw one line

static FILE *fp=0;

bool LCD_Open(void){
    fp = fopen(CHARACTER_LCD_0_NAME, "w");
    if (fp)
        return TRUE;
    return FALSE;        
}
bool LCD_TextOut(char *pText){
    if (!fp)
        return FALSE;
    fwrite(pText, strlen(pText), 1, fp);        
    return TRUE;        
}

bool LCD_Clear(void){
    char szText[32]="\n\n";
    if (!fp)
        return FALSE;
    fwrite(szText, strlen(szText), 1, fp);        
    return TRUE;        
}


void LCD_Close(void){
    if (fp)
        fclose(fp);
    fp = 0;        
}
