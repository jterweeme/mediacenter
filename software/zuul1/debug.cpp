#define SYSTEM_BUS_WIDTH 32

#include <stdio.h> 
#include <stdarg.h> 
#include "terasic_includes.h"
#include "debug.h"





#ifdef ENABLE_UART_DEBUG
#include "uart.h"

void debug_output(char *pMessage){
    if (!UART_IsOpened())
        UART_Open();
    UART_WriteString(pMessage);  // UART debug
}

#else

void debug_output(char *pMessage){
    
    printf(pMessage);
}

#endif





int myprintf(char *format, ...){
    int rc;
    char szText[512];
    
    va_list paramList;
    va_start(paramList, format);
    rc = vsnprintf(szText, 512, format, paramList);
    va_end(paramList);
    
    debug_output(szText);

    return rc;    
}


int myprintf_hexarray(unsigned char *pHex, int len){
    int i;
    unsigned char szText[16];
    for(i=0;i<len;i++){
        sprintf((char *)szText, "[%02X]", *(pHex+i));
        DEBUG(((char *)szText));
    }
    return len;
}

int  myprintf_dwordarray(unsigned int *pArray, int nElementCount){
    int i;
    char szText[16];
    for(i=0;i<nElementCount;i++){
        sprintf(szText, "[%08X]", *(pArray+i));
        DEBUG((szText));
    }
    return nElementCount;
}
