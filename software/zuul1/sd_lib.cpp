#define SYSTEM_BUS_WIDTH 32


#include <unistd.h>  // usleep
#include <stdio.h>
#include "sd_lib.h"
#include "sd_protocol.h"
#include "misc.h"


#ifdef DEBUG_SDCARD
    #define SDCARD_TRACE(x)    {DEBUG(("[SD_LIB]")); DEBUG(x);}  
    #define SDCARD_DEBUG(x)    {DEBUG(("[SD_LIB]")); DEBUG(x);}  
    #define SDCARD_ERROR(x)    {DEBUG(("[SD_LIB]ERR:")); DEBUG(x);}  
#else
    #define SDCARD_DEBUG(x)
    #define SDCARD_TRACE(x)
    #define SDCARD_ERROR(x)
#endif


#define DEBUG_SDCARD_HEX //DEBUG_HEX_PRINTF


//alt_u8 gszCSD[17];
//alt_u8 gszCID[17];
//bool gbActive = FALSE;
//void DumpCsdInfo(alt_u8 szCSD[]);
//-------------------------------------------------------------------------

// sdcard info
typedef struct{
    bool bSupport4Bits;
    bool bVer2;
    bool bSDHC;
    alt_u32 HostOCR32;
    alt_u32 OCR32;
    alt_u16 RCA16;
    alt_u8  szCID[16+1];
    alt_u8  szCSD[16+1];
}SD_INFO;

static SD_INFO gSdInfo;


//-------------------------------------------------------------------------
bool SDLIB_Init()
{
    Uart::getInstance()->puts("SDLIB init\r\n");
    bool bSuccess = FALSE, bTimeout = FALSE;
    alt_u8  szThisCID[16];
    const int nTimeout = alt_ticks_per_second();
    alt_u32 TimeStart;
    memset(&gSdInfo, 0, sizeof(gSdInfo));
    gSdInfo.bSupport4Bits = SD_IsSupport4Bits();
    Uart::getInstance()->puts("--- Power On, Card Identification Mode, Idle State\r\n");
    //Uart::getInstance()->puts("sd %d-bit mode\r\n", gSdInfo.bSupport4Bits?4:1);
  
    usleep(74*10);
    SD_Init();
    SD_CMD0();
    usleep(100*1000);
    
    if (SD_CMD8(SD_VHS_2V7_3V6, CMD8_DEFAULT_TEST_PATTERN))
    {
        gSdInfo.HostOCR32 = 0x40FF8000;
        gSdInfo.bVer2 = TRUE;
        gSdInfo.bSDHC = TRUE;
        Uart::getInstance()->puts(" 2.0 \r\n");
    }
    else
    {
        gSdInfo.HostOCR32 = 0x00040000;
        Uart::getInstance()->puts("CMD8 not supported\r\n");
        SD_CMD0();
        usleep(100*1000);        
    }

    TimeStart = alt_nticks();
    while(!bSuccess && !bTimeout){
        if (!SD_CMD55(gSdInfo.RCA16, SD_STATE_IDLE)){  // execute cmd55 before execute and ACMD
            SDCARD_ERROR(("CMD55 NG\r\n"));
            return FALSE;
        }else{    
            bSuccess = SD_ACMD41(gSdInfo.HostOCR32, &gSdInfo.OCR32);
            if (bSuccess && gSdInfo.bSDHC && ((gSdInfo.OCR32 & 0x40000000) == 0x00)){
                gSdInfo.bSDHC = FALSE;
            }    
            // check timeout
            if (!bSuccess){
                if ((alt_nticks() - TimeStart) > nTimeout){
                    SDCARD_ERROR(("ACMD41[SD_APP_OP_COND] fail\r\n"));
                    bTimeout = TRUE;
                }else{
                    usleep(10*1000);
                }    
            }
        }
    }
    if (!bSuccess)
    {
        return FALSE;
    }
   
    if (!SD_CMD2(gSdInfo.szCID, sizeof(gSdInfo.szCID))){
        SDCARD_ERROR(("CMD2 fail\r\n"));
        return FALSE;
    }

    Uart::getInstance()->puts("--- Power On, Card Identification Mode, Identification State\r\n");

    if (!SD_CMD3(&gSdInfo.RCA16))
    {
        Uart::getInstance()->puts("CMD3 fail\r\n");
        return FALSE;
    }    
    
    SDCARD_DEBUG(("--- enter data-transfer mode, Standby state\r\n"));

    if (!SD_CMD9(gSdInfo.RCA16, gSdInfo.szCSD, sizeof(gSdInfo.szCSD))){
        SDCARD_DEBUG(("CMD9 fail\r\n"));
        return FALSE;
    }
    
    if (!SD_CMD10(gSdInfo.RCA16, szThisCID, sizeof(szThisCID))){
        SDCARD_DEBUG(("CMD10 fail\r\n"));
        return FALSE;
    }    

    // can issue cmd 4, 9, 10, in (stdandby state)         
         
    // issue cmd9 with given RCA & wait response
    
    //========================== CMD7 ========================
    // richard: issue cmd7 to enter transfer state
    // cmd7: toggle between Standy-by and Trasfer State
    if (!SD_CMD7(gSdInfo.RCA16)){
        SDCARD_DEBUG(("CMD7 fail\r\n"));
        return FALSE;
    }      
    //### Transfer state in Data-transfer mode     
    SDCARD_DEBUG(("--- enter data-transfer mode, Transfer state\r\n"));
       
    //========================== CMD16 ========================   
    // issue cmd16 (select a block length) & wait response
    if (!SD_CMD16(512)){  // 512 bytes
        SDCARD_DEBUG(("CMD16 fail\r\n"));
        return FALSE;
    }       
    
    if (gSdInfo.bSupport4Bits){ 
        // richard add: set bus width
        // Note. This command is valid only in "transfer state", i.e. after CMD7 is issued

        if (!SD_CMD55(gSdInfo.RCA16, SD_STATE_TRAN)){  // execute cmd55 before execute and ACMD, RCA = 0
            SDCARD_DEBUG(("CMD55 NG\r\n"));
            return FALSE;
        }       
    
        SDCARD_DEBUG(("ACMD6[SET_BUS_WIDTH], 4-bit\r\n"));
        if (!SD_ACMD6(TRUE, SD_STATE_TRAN)){  // 4-bit mode
            SDCARD_DEBUG(("ACMD6 NG\r\n"));
            return FALSE;
        }         
    
    
        //
        if (!SD_CMD55(gSdInfo.RCA16, SD_STATE_TRAN)){  // execute cmd55 before execute and ACMD, RCA = 0
            SDCARD_DEBUG(("CMD55 NG\r\n"));
            return FALSE;
        }    
    
        Uart::getInstance()->puts("ACMD42[SET_CLR_CARD_DETECT], connect card\r\n");

        if (!SD_ACMD42(TRUE, SD_STATE_TRAN))
        {
            Uart::getInstance()->puts("ACMD42 NG\r\n");
            return FALSE;
        }   
    }
    
    
    //
#if 0
    int i;
    bool test_ok;
    alt_u8 szWrite[512];
    for(i=0;i<512;i++)
        szWrite[i] = i;
    test_ok = SDLIB_WriteBlock512(100, szWrite);
    if (!test_ok)
        printf("!!!!!SDLIB_WriteBlock512 ng\r\n");
    if (test_ok){
        test_ok = SDLIB_ReadBlock512(100, szWrite);
        if (!test_ok)
            printf("!!!!!SDLIB_ReadBlock512 ng\r\n");
    }            
        
    if (test_ok && szWrite[1] == 1)
        printf("!!!!!write success\r\n");
    else     
        printf("!!!!!write ng\r\n");
#endif
    Uart::getInstance()->puts("SDLIB_Init success\r\n");
    return true;
}


bool SDLIB_ReadBlock512(alt_u32 block_number, alt_u8 *buff)
{
    bool bSuccess;  
    alt_u32  addr;
  
    if (gSdInfo.bSDHC)
        addr = block_number;
    else 
        addr = block_number * 512;
    if (!SD_CMD17(addr, SD_STATE_TRAN)){  // 4-bit mode
        SDCARD_DEBUG(("CMD17 NG\r\n"));
        return FALSE;
    }
    //

    bSuccess = SD_ReadData(buff, 512);  // read 512 bytes
    
    //
    return bSuccess;
}

bool SDLIB_WriteBlock512(alt_u32 block_number, alt_u8 *szDataWrite)
{
    // buffer size muse be 512 byte
    bool bSuccess;  
    alt_u32  addr;
  
    // issue cmd24 for 'Single Block write'. parameter: block address
    if (gSdInfo.bSDHC)
        addr = block_number; // note. for SDHC, argument for addr of CMD7 is block number
    else 
        addr = block_number * 512;
    if (!SD_CMD24(addr, SD_STATE_TRAN)){ 
        SDCARD_DEBUG(("CMD24 NG\r\n"));
        return FALSE;
    }
    //

    bSuccess = SD_WriteData(szDataWrite, 512);  // Write 512 bytes
    
    //
    return bSuccess;
}



