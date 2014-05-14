#define SYSTEM_BUS_WIDTH 32

#include "sd_lib.h"
#include <stdint.h>
#include "terasic_includes.h"
#include "LED.h"
#include "FatFileSystem.h"
#include "FatInternal.h"
#include "I2C.h"
#include "AUDIO.h"
#include "AUDIO_REG.h"
#include "misc.h"

#define LCD_DISPLAY
#define SUPPORT_PLAY_MODE
#define xENABLE_DEBOUNCE

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
    void handle_key(bool *pNexSongPressed);
    void handle_IrDA(bool * pNexSongPressed,alt_u32 id);
    bool waveplay_execute(bool *bEOF);
    bool waveplay_start(char *pFilename);
    void DisplayTime(alt_u32 TimeElapsed);
    bool is_supporrted_sample_rate(int sample_rate);
private:
    Audio *audio;
};

Main::Main()
{
    Uart::getInstance()->puts("Main constructor\r\n");
}

int main()
{
    Uart::getInstance()->init((volatile uint32_t *)UART_BASE);
    Main *main = new Main();
    return main->run();
}

int Main::run()
{
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


