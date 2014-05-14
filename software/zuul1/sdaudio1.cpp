#define SYSTEM_BUS_WIDTH 32

#include "sd_lib.h"
#include <stdint.h>
#include "terasic_includes.h"
#include "LED.h"
#include "FatFileSystem.h"
#include "FatInternal.h"
#include "WaveLib.h"
#include "I2C.h"
#include "AUDIO.h"
#include "misc.h"


#ifdef DEBUG_APP
    #define APP_DEBUG(x)    DEBUG(x)
#else
    #define APP_DEBUG(x)
#endif

#define DEMO_PRINTF printf

#define LCD_DISPLAY
//#define SEG7_DISPLAY
//#define DISPLAY_WAVE_POWER
#define SUPPORT_PLAY_MODE
#define xENABLE_DEBOUNCE

#ifdef LCD_DISPLAY
    #include "LCD.h"
#endif

#ifdef SEG7_DISPLAY
    #include "SEG7.h"
#endif

#define MAX_VOL 8
#define AUTO_NEXT_SONG
#define HW_MAX_VOL     127
#define HW_MIN_VOL     47
#define HW_DEFAULT_VOL  120

#define MAX_FILE_NUM    128
#define FILENAME_LEN    32

typedef struct{
    int nFileNum;
    char szFilename[MAX_FILE_NUM][FILENAME_LEN];
}WAVE_PLAY_LIST;


static WAVE_PLAY_LIST gWavePlayList;
#define WAVE_BUF_SIZE  512
typedef struct{
    FAT_FILE_HANDLE hFile;
    alt_u8          szBuf[WAVE_BUF_SIZE];
    alt_u32         uWavePlayIndex;
    alt_u32         uWaveReadPos;
    alt_u32         uWavePlayPos;
    alt_u32         uWaveMaxPlayPos;
    char szFilename[FILENAME_LEN];
    alt_u8          nVolume;
    bool            bRepeatMode;
}PLAYWAVE_CONTEXT;

static PLAYWAVE_CONTEXT gWavePlay;
static FAT_HANDLE hFat;
static int nMute_Volume = 0;



static bool bLastSwitch = FALSE;
static bool bNextSwitch = FALSE;
static bool bMuteSwitch = FALSE;
static bool bPlaySwitch = TRUE;

class Main
{
public:
    Main();
    int run();
    void update_status();
    void wait_sdcard_insert();
    int build_wave_play_list(FAT_HANDLE hFat);
    void handle_key(bool *pNexSongPressed);
    void handle_IrDA(bool * pNexSongPressed,alt_u32 id);
    bool waveplay_execute(bool *bEOF);
    bool Fat_Test(FAT_HANDLE hFat, char *pDumpFile);
    bool waveplay_start(char *pFilename);
    void DisplayTime(alt_u32 TimeElapsed);
    void lcd_display(char *pText);
    void lcd_open();
    bool is_supporrted_sample_rate(int sample_rate);
    void led_display(alt_u32 mask);
    void led_display_count(alt_u8 count);
    void waveplay_stop();
private:
    Audio *audio;
};

Main::Main()
{
    Uart::getInstance()->puts("Main constructor\r\n");
}

void Main::update_status(void)
{
    char szText[64];       
    sprintf(szText, "\r%s\nVol:%d(%d-%d)%C\n", gWavePlay.szFilename, gWavePlay.nVolume, 
        HW_MIN_VOL, HW_MAX_VOL, gWavePlay.bRepeatMode?'R':'S');
    lcd_display((szText));
}

void Main::DisplayTime(alt_u32 TimeElapsed)
{
    alt_u32 msx10;
    msx10 = TimeElapsed*100/alt_ticks_per_second();
    //SEG7_Decimal(msx10, 0x01 << 2);
}

void Main::lcd_open()
{
    LCD_Open();
}

void Main::lcd_display(char *pText)
{
    LCD_Clear();  
    LCD_TextOut(pText);
}

void Main::led_display(alt_u32 mask)
{
    LED_Display(mask);
}

void Main::led_display_count(alt_u8 count)
{
    LED_LightCount(count);
}

void Main::wait_sdcard_insert()
{
    Uart::getInstance()->puts("Wait SDCard Insert\r\n");
    bool bFirstTime2Detect = TRUE;
    alt_u8 led_mask = 0x02;
    LED_AllOff();

    //SDLIB_Init();

    while(!SDLIB_Init())
    {
        if (bFirstTime2Detect)
            bFirstTime2Detect = FALSE;

        usleep(100*1000);
    }


    Uart::getInstance()->puts("SD card inserted 1\r\n");
}

bool Main::is_supporrted_sample_rate(int sample_rate)
{
    bool bSupport = FALSE;
    switch(sample_rate){
        case 96000:
        case 48000:
        case 44100:
        case 32000:
        case 8000:
            bSupport = TRUE;
            break;
    }
    return bSupport;
}

int Main::build_wave_play_list(FAT_HANDLE hFat){
    int count = 0;
    FAT_BROWSE_HANDLE hFileBrowse;
    FILE_CONTEXT FileContext;
    FAT_FILE_HANDLE hFile;
    alt_u8 szHeader[128];
    char szWaveFilename[MAX_FILENAME_LENGTH];
    int sample_rate;
    bool bFlag = FALSE;
    int nPos = 0;
    int length=0;
    gWavePlayList.nFileNum = 0;

    if (!Fat_FileBrowseBegin(hFat,&hFileBrowse))
        return 0;
    
    while (Fat_FileBrowseNext(&hFileBrowse,&FileContext))
    {
        if (FileContext.bLongFilename)
        {
                nPos = 0;
                alt_u16 *pData16;
                char *pData8;
                pData16 = (alt_u16 *)FileContext.szName;
                pData8 = FileContext.szName;
                while(*pData16){
                    if (*pData8 && *pData8 != ' ')
                        szWaveFilename[nPos++] = *pData8;
                    pData8++;
                    if (*pData8 && *pData8 != ' ')
                        szWaveFilename[nPos++] = *pData8;
                    pData8++;                    
                    pData16++;
                }
                szWaveFilename[nPos] = 0;
            }else{
                strcpy(szWaveFilename,FileContext.szName);
            }       
            
            length= strlen(szWaveFilename);   
            if(length >= 4){
               if((szWaveFilename[length-1] =='V' || szWaveFilename[length-1] =='v')
                &&(szWaveFilename[length-2] == 'A' || szWaveFilename[length-2] =='a')
                &&(szWaveFilename[length-3] == 'W' || szWaveFilename[length-3] == 'w')
                &&(szWaveFilename[length-4] == '.')){
                   bFlag = TRUE;
                } 
            }
       
        if (bFlag)
        {
            hFile = Fat_FileOpen(hFat,szWaveFilename);
            if (!hFile)
            {
                  continue;
            }
            
            if (!Fat_FileRead(hFile, szHeader, sizeof(szHeader))){
                  continue;
            }
            Fat_FileClose(hFile);
            sample_rate = Wave_GetSampleRate((char *)szHeader, sizeof(szHeader));

            if (WAVE_IsWaveFile((char *)szHeader, sizeof(szHeader)) &&
                is_supporrted_sample_rate(sample_rate) &&
                Wave_GetChannelNum((char *)szHeader, sizeof(szHeader))==2 &&
                Wave_GetSampleBitNum((char *)szHeader, sizeof(szHeader))==16){
                    strcpy(gWavePlayList.szFilename[count],szWaveFilename);
                    count++;
            }
        }
    }
    gWavePlayList.nFileNum = count;
    return count;
}



bool Main::waveplay_start(char *pFilename)
{
    bool bSuccess;
    int nSize;
    
    strcpy(gWavePlay.szFilename, pFilename);
    gWavePlay.hFile = Fat_FileOpen(hFat, pFilename);

    gWavePlay.hFile;
    
    nSize = Fat_FileSize(gWavePlay.hFile);

    if (gWavePlay.hFile)
        bSuccess = Fat_FileRead(gWavePlay.hFile, gWavePlay.szBuf, WAVE_BUF_SIZE);
                
    if (bSuccess)
    {      
        int sample_rate =  Wave_GetSampleRate((char *)gWavePlay.szBuf, WAVE_BUF_SIZE);

        if (WAVE_IsWaveFile((char *)gWavePlay.szBuf, WAVE_BUF_SIZE) &&
            is_supporrted_sample_rate(sample_rate) &&
            Wave_GetChannelNum((char *)gWavePlay.szBuf, WAVE_BUF_SIZE)==2 &&
            Wave_GetSampleBitNum((char *)gWavePlay.szBuf, WAVE_BUF_SIZE)==16)
        {
                    
            gWavePlay.uWavePlayPos = Wave_GetWaveOffset((char *)gWavePlay.szBuf, WAVE_BUF_SIZE);

            gWavePlay.uWaveMaxPlayPos = gWavePlay.uWavePlayPos +
                    Wave_GetDataByteSize((char *)gWavePlay.szBuf, WAVE_BUF_SIZE);

            gWavePlay.uWaveReadPos = WAVE_BUF_SIZE;
            AUDIO_InterfaceActive(FALSE);

            if (sample_rate == 96000)
                AUDIO_SetSampleRate(RATE_ADC96K_DAC96K);
            else if (sample_rate == 48000)
                AUDIO_SetSampleRate(RATE_ADC48K_DAC48K);
            else if (sample_rate == 44100)                    
                AUDIO_SetSampleRate(RATE_ADC44K1_DAC44K1);
            else if (sample_rate == 32000)                    
                AUDIO_SetSampleRate(RATE_ADC32K_DAC32K);
            else if (sample_rate == 8000)                    
                AUDIO_SetSampleRate(RATE_ADC8K_DAC8K);

            AUDIO_FifoClear();
            AUDIO_InterfaceActive(TRUE);
                  
        }
        else
        {
            bSuccess = FALSE;
        }    
    }            

    if (!bSuccess)
        waveplay_stop();    
    
    return bSuccess;
}

bool Main::waveplay_execute(bool *bEOF)
{
    bool bSuccess = TRUE;
    bool bDataReady = FALSE;
   
    if (gWavePlay.uWavePlayPos >= gWavePlay.uWaveMaxPlayPos)
    {
        *bEOF = TRUE;
        return TRUE;
    }
    
    *bEOF = FALSE;
    while (!bDataReady && bSuccess)
    {
        if (gWavePlay.uWavePlayPos < gWavePlay.uWaveReadPos)
        {
            bDataReady = TRUE;
        }
        else
        {
            int read_size = WAVE_BUF_SIZE;
            if (read_size > (gWavePlay.uWaveMaxPlayPos - gWavePlay.uWavePlayPos))
                read_size = gWavePlay.uWaveMaxPlayPos - gWavePlay.uWavePlayPos;
            bSuccess = Fat_FileRead(gWavePlay.hFile, gWavePlay.szBuf, read_size);
            if (bSuccess)
                gWavePlay.uWaveReadPos += read_size;
        }    
    }
    
    if (bDataReady && bSuccess)
    {
        alt_u32 power_sum = 0, power;
        int play_size; 
        short *pSample = (short *)(gWavePlay.szBuf + gWavePlay.uWavePlayPos%WAVE_BUF_SIZE);
        int i = 0;
        play_size = gWavePlay.uWaveReadPos - gWavePlay.uWavePlayPos;
        play_size = play_size/4*4;

        while (i < play_size)
        {
            if(AUDIO_DacFifoNotFull())
            {
                short ch_right, ch_left;
                ch_left = *pSample++;
                ch_right = *pSample++;
                
                power = abs(ch_left) + abs(ch_right);
                power_sum += power;     
                if ((i & 0x01F) == 0 && i != 0)
                {
                    power = power_sum >> (6+7);
                    led_display_count(power); 
                    power_sum = 0;
                }                        
                AUDIO_DacFifoSetData(ch_left, ch_right);
                i+=4;
            }
        }
        gWavePlay.uWavePlayPos += play_size;
                    
    }
    
    return bSuccess;
}

void Main::waveplay_stop()
{
}

void Main::handle_key(bool *pNexSongPressed)
{
    static bool bFirsTime2SetupVol = TRUE;
    alt_u8 button;
    bool bNextSong, bVolUp, bVolDown;
    int nHwVol;

    bool bRepeat;
    bRepeat = (IORD_ALTERA_AVALON_PIO_DATA(SW_BASE) & 0x01) ? true : false;

    if (bRepeat ^ gWavePlay.bRepeatMode)
    {
        gWavePlay.bRepeatMode = bRepeat;
        Main::update_status();
    } 

    *pNexSongPressed = FALSE;
    button = IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE);
    button = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0); // clear flag 
    bNextSong = (button & 0x08)?TRUE:FALSE;
    bVolUp = (button & 0x04)?TRUE:FALSE;
    bVolDown = (button & 0x02)?TRUE:FALSE;
    
    if ((bVolUp || bVolDown || bFirsTime2SetupVol) && !bMuteSwitch)
    {
        nHwVol = gWavePlay.nVolume;    
        if (bFirsTime2SetupVol){
            bFirsTime2SetupVol = FALSE;
        }else if (bVolUp){
            if (nHwVol < HW_MAX_VOL) 
                nHwVol++;
        }else{
            if (nHwVol > HW_MIN_VOL)
                nHwVol--;
        }                    
        AUDIO_SetLineOutVol(nHwVol, nHwVol); 
        gWavePlay.nVolume = nHwVol;
        Main::update_status();
    }
 
    if (bNextSong)
        *pNexSongPressed = TRUE;
}

bool Main::Fat_Test(FAT_HANDLE hFat, char *pDumpFile)
{
    bool bSuccess;
    int nCount = 0;
    FAT_BROWSE_HANDLE hBrowse;
    FILE_CONTEXT FileContext;
    
    bSuccess = Fat_FileBrowseBegin(hFat, &hBrowse);
    if (bSuccess){
        while(Fat_FileBrowseNext(&hBrowse, &FileContext)){
            if (FileContext.bLongFilename){
                alt_u16 *pData16;
                alt_u8 *pData8;
                pData16 = (alt_u16 *)FileContext.szName;
                pData8 = (alt_u8 *)FileContext.szName;
                printf("[%d]", nCount);
                while(*pData16){
                    if (*pData8)
                        printf("%c", *pData8);
                    pData8++;
                    if (*pData8)
                        printf("%c", *pData8);
                    pData8++;                    
                    //    
                    pData16++;
                }
                printf("\n");
            }
            nCount++;
        }    
    }
    if (bSuccess && pDumpFile && strlen(pDumpFile))
    {
        FAT_FILE_HANDLE hFile;
        hFile =  Fat_FileOpen(hFat, pDumpFile);
        if (hFile)
        {
            char szRead[256];
            int nReadSize, nFileSize, nTotalReadSize=0;
            nFileSize = Fat_FileSize(hFile);

            if (nReadSize > sizeof(szRead))
                nReadSize = sizeof(szRead);

            while(bSuccess && nTotalReadSize < nFileSize)
            {
                nReadSize = sizeof(szRead);

                if (nReadSize > (nFileSize - nTotalReadSize))
                    nReadSize = (nFileSize - nTotalReadSize);

                if (Fat_FileRead(hFile, szRead, nReadSize))
                {
                    for(int i=0;i<nReadSize;i++)
                        printf("%c", szRead[i]);

                    nTotalReadSize += nReadSize;
                }else{
                    bSuccess = FALSE;
                }     
            }

            if (bSuccess)
                printf("\n");
            Fat_FileClose(hFile);
        }else{            
            bSuccess = FALSE;
        }            
    }
    
    return bSuccess;
}


int main()
{
    Uart::getInstance()->init((volatile uint32_t *)UART_BASE);
    Uart::getInstance()->puts("Int Main\r\n");
    Main *main = new Main();
    return main->run();
}

int Main::run()
{
    int nPlayIndex;
    alt_u32 cnt, uSongStartTime, uTimeElapsed;
    alt_8 led_mask = 0x03;
    alt_u8 szWaveFile[FILENAME_LEN];
    Uart::getInstance()->puts("LCD open\r\n");
    //lcd_open();
    audio = new Audio();

    if (!audio->AUDIO_Init())
    {
        Uart::getInstance()->puts("Audio Init fail\r\n");
        lcd_display(("Audio Init fail!\n\n"));
        return 0;
    }
    
    Uart::getInstance()->puts("Audio Init success\r\n");
    ::memset(&gWavePlay, 0, sizeof(gWavePlay));
    gWavePlay.nVolume = HW_DEFAULT_VOL;
    AUDIO_SetLineOutVol(gWavePlay.nVolume, gWavePlay.nVolume);
    Uart::getInstance()->puts("Volume set\r\n");
  
    SDCard sdCard;
    sdCard.init(ALTERA_UP_SD_CARD_AVALON_INTERFACE_0_NAME);
    
    if (sdCard.isPresent())
    {
        if (sdCard.isFAT16())
        {
            char fn[13];
            ::alt_up_sd_card_find_first("/.", fn);
            sdCard.findNext(fn);

            short int fd = ::alt_up_sd_card_fopen(fn, 0);
            
            short int bijt;
            int fs = 0;
            
            for (int i = 0; i < 1000; i++)
                bijt = ::alt_up_sd_card_read(fd);

            Uart::getInstance()->puts("Onzin\r\n");
            ::printf("%u\r\n", fs);

/*
            for (int i = 0; i < 100; i++)
            {
                bijt = ::alt_up_sd_card_read(fd);
                Uart::getInstance()->putc((char)bijt);
            }*/
        }
    }

/*
    while(1)
    {
        wait_sdcard_insert();
        Uart::getInstance()->puts("SD card inserted\r\n");
        hFat = Fat_Mount(FAT_SD_CARD, 0);

        if (!hFat)
        {
            Uart::getInstance()->puts("SD card mount fail\r\n");
            lcd_display(("SD card mount fail.\n\n"));
            return 0;
        }  
        else
        {
            if (build_wave_play_list(hFat) == 0)
            {
                lcd_display(("No Wave Files.\n\n"));
                return 0;
            }
        }
   
        bool bSdacrdReady = TRUE;
        nPlayIndex = 0;
        while(bSdacrdReady)
        {
            bool bPlayDone = FALSE;
            strcpy((char *)szWaveFile, gWavePlayList.szFilename[nPlayIndex]);
            if (!waveplay_start((char *)szWaveFile)){
                lcd_display("Play Error.\n\n");
                bSdacrdReady = FALSE;
            }                
            update_status();
            cnt = 0;
            uSongStartTime = alt_nticks();
            while(!bPlayDone && bSdacrdReady)
            {
                bool bLastSongPressed  = FALSE;
                bool bNextSongPressed  = FALSE;
                bool bEndOfFile = FALSE;

                if ((cnt++ & 0x1F) == 0)
                {
                    led_display(led_mask);
                    led_mask ^= 0x01;
                }

                if (!waveplay_execute(&bEndOfFile)){
                    lcd_display("Play Error.\n\n");
                    bSdacrdReady = FALSE;
                }    
                
                handle_key(&bNextSongPressed);
                if(bLastSwitch)
                {
                    bLastSongPressed = TRUE;
                    bNextSongPressed = FALSE;
                }

                 if(bNextSwitch){
                    bLastSongPressed = FALSE;
                    bNextSongPressed = TRUE;
                }
                if (bSdacrdReady && (bEndOfFile || bLastSongPressed || bNextSongPressed)){
                    bool bNextSong = false;
                    bool bLastSong = false;
                    
                    if(bLastSongPressed) bLastSong = TRUE;
                    if(bNextSongPressed || bEndOfFile) bNextSong = TRUE;

                    if (!bLastSongPressed && !bNextSongPressed && gWavePlay.bRepeatMode)
                    {
                        bNextSong = false;
                        bLastSong = false;
                    }

                    if (bNextSong)
                    {
                        nPlayIndex++;
                        if (nPlayIndex >= gWavePlayList.nFileNum)
                            nPlayIndex = 0;
                    }   
                    if (bLastSong)
                    {
                        nPlayIndex--;
                        if (nPlayIndex < 0)
                            nPlayIndex = gWavePlayList.nFileNum-1;
                    }                           
                    bPlayDone = TRUE;                        
                } 
                bLastSwitch = FALSE;
                bNextSwitch = FALSE;
                
                uTimeElapsed = alt_nticks() - uSongStartTime;    
                DisplayTime(uTimeElapsed);    
            }
            waveplay_stop();    
        }
  }*/

  return 0;
}


