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
#include "AUDIO_REG.h"
#include "misc.h"

#define LCD_DISPLAY
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
    int build_wave_play_list(FAT_HANDLE hFat);
    void handle_key(bool *pNexSongPressed);
    void handle_IrDA(bool * pNexSongPressed,alt_u32 id);
    bool waveplay_execute(bool *bEOF);
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

void Main::DisplayTime(alt_u32 TimeElapsed)
{
    alt_u32 msx10;
    msx10 = TimeElapsed*100/alt_ticks_per_second();
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

bool Main::waveplay_start(char *pFilename)
{
    bool bSuccess;
    int nSize;
    strcpy(gWavePlay.szFilename, pFilename);
    gWavePlay.hFile;

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
    bool bSuccess = true;
    bool bDataReady = false;
   
    if (gWavePlay.uWavePlayPos >= gWavePlay.uWaveMaxPlayPos)
    {
        *bEOF = true;
        return true;
    }
    
    *bEOF = false;
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
    } 

    *pNexSongPressed = FALSE;
    button = IORD_ALTERA_AVALON_PIO_DATA(KEY_BASE);
    button = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
    IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0);
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
    }
 
    if (bNextSong)
        *pNexSongPressed = TRUE;
}

int main()
{
    Uart::getInstance()->init((volatile uint32_t *)UART_BASE);
    Main *main = new Main();
    return main->run();
}

int Main::run()
{
    int nPlayIndex;
    alt_u32 cnt, uSongStartTime, uTimeElapsed;
    alt_8 led_mask = 0x03;
    alt_u8 szWaveFile[FILENAME_LEN];
    audio = new Audio();

    if (!audio->AUDIO_Init())
    {
        Uart::getInstance()->puts("Audio Init fail\r\n");
        return 0;
    }
    
    Uart::getInstance()->puts("Audio Init success\r\n");
    ::memset(&gWavePlay, 0, sizeof(gWavePlay));
    gWavePlay.nVolume = HW_DEFAULT_VOL;
    AUDIO_SetLineOutVol(gWavePlay.nVolume, gWavePlay.nVolume);
    Uart::getInstance()->puts("Volume set\r\n");

    for (int i = 0; i < 99999; i++)
    {
        AUDIO_DAC_WRITE_L(i);
        AUDIO_DAC_WRITE_R(i);
        usleep(10);
    }

    Uart::getInstance()->puts("Le fin\r\n");

    return 0;
}


