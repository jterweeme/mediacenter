#include <alt_types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/alt_alarm.h>
#include <system.h>
#include <altera_avalon_pio_regs.h>
#include "sd_hal.h"
#include <stdbool.h>

#define xFAT_READONLY
#define SUPPORT_SD_CARD
#define xSUPPORT_USB_DISK

typedef void *FAT_HANDLE;
typedef void *FAT_FILE_HANDLE;
typedef void *DEVICE_HANDLE;
#define MAX_FILENAME_LENGTH 256

#define fat_packed __attribute__ ((packed,aligned(1)))


typedef enum{
    FAT_SD_CARD=0,
    FAT_USB_DISK,
}FAT_DEVICE;

typedef enum{
    FILE_SEEK_BEGIN,
    FILE_SEEK_CURRENT,
    FILE_SEEK_END
}FAT_SEEK_POS;

typedef struct{
    char szName[MAX_FILENAME_LENGTH];
    bool bLongFilename;
    bool bFile;
    bool bDirectory;
    bool bVolume;
    alt_u32 DirEntryIndex;
    char Attribute;
    unsigned short CreateTime;
    unsigned short CreateDate;
    unsigned short LastAccessDate;
    unsigned short FirstLogicalClusterHi;
    unsigned short LastWriteTime;
    unsigned short LastWriteDate;
    unsigned short FirstLogicalCluster;
    unsigned int FileSize;
}FILE_CONTEXT;

typedef struct{
    unsigned int DirectoryIndex;
    FAT_HANDLE   hFat;
}FAT_BROWSE_HANDLE;

typedef struct{
    char szName[256];
    char szExt[8];
    unsigned int nFileSize;
}FILE_INFO;

typedef enum{
    FAT_UNKNOWN = 0,
    FAT_FAT16,
    FAT_FAT32
}FAT_TYPE;

typedef enum{
    FILE_OPEN_CREATE = 0,
    FILE_OPEN_READ,
    FILE_OPEN_READWRITE
}FILE_OPEN_ATTR;


void Fat_Unmount(FAT_HANDLE Fat);
FAT_TYPE Fat_Type(FAT_HANDLE Fat);
bool Fat_Test(FAT_HANDLE hFat, char *pDumpFile);

bool Fat_FileBrowseBegin(FAT_HANDLE hFat, FAT_BROWSE_HANDLE *pFatBrowseHandle);
bool Fat_FileBrowseNext(FAT_BROWSE_HANDLE *pFatBrowseHandle, FILE_CONTEXT *pFileContext);
bool Fat_DumpFilename(char *pFilename, bool bLongFilename);



FAT_FILE_HANDLE Fat_FileOpen(FAT_HANDLE Fat, const char *pFilename);
bool Fat_FileRead(FAT_FILE_HANDLE hFileHandle, void *pBuffer, const int nBufferSize);
bool Fat_FileSeek(FAT_FILE_HANDLE hFileHandle, const FAT_SEEK_POS SeekPos, const int nOffset);
void Fat_FileClose(FAT_FILE_HANDLE hFileHandle);
bool Fat_FileExist(FAT_HANDLE Fat, const char *pFilename);


bool fatSameLongFilename(alt_u16 *p1, alt_u16 *p2);

#define MY_SECTER_SIZE 512

typedef void *DISK_HANDLE;
typedef bool (*READ_BLOCK512_FUNC)(DISK_HANDLE DiskHandle, alt_u32 PysicalSelector, alt_u8 szBuf[512]);
typedef bool (*WRITE_BLOCK512_FUNC)(DISK_HANDLE DiskHandle, alt_u32 PysicalSelector, alt_u8 szBuf[512]);

#define UNUSED_DIR_ENTRY           0xE5
#define REMAINED_UNUSED_DIR_ENTRY  0x00


typedef enum{
    PARTITION_FAT16=6,
    PARTITION_FAT32=11,
}PARTITION_TYPE;

typedef struct{
    DISK_HANDLE     DiskHandle;
    READ_BLOCK512_FUNC ReadBlock512;
    WRITE_BLOCK512_FUNC WriteBlock512;
    alt_u32 Partition_Type;
    bool bMount;
    alt_u32 PartitionStartSecter;
    alt_u32 BPB_BytsPerSec;
    alt_u32 BPB_SecPerCluster;
    alt_u32 BPB_RsvdSecCnt;
    alt_u32 BPB_NumFATs;
    alt_u32 BPB_RootEntCnt;
    alt_u32 BPB_FATSz;
    alt_u32 nBytesPerCluster;
    alt_u32 FatEntrySecter;
    alt_u32 RootDirectoryEntrySecter;
    alt_u32 DataEntrySecter;
    alt_u8  Secter_Data[MY_SECTER_SIZE];
    alt_u32 Secter_Index;
#ifdef FAT_READONLY    
    char *szFatTable;
#endif


}VOLUME_INFO;


typedef struct{
    bool IsOpened;
    unsigned int OpenAttribute;
    unsigned int SeekPos;
    unsigned int Cluster;
    unsigned int ClusterSeq;
    FILE_CONTEXT Directory;
    FAT_HANDLE Fat;
}FAT_FILE_INFO;

typedef enum{
    CLUSTER_UNUSED,
    CLUSTER_RESERVED,
    CLUSTER_BAD,
    CLUSTER_LAST_INFILE,
    CLUSTER_NEXT_INFILE
}CLUSTER_TYPE;

typedef struct{
    char Name[8];
    char Extension[3];
    char Attribute;
    char reserved[2];
    unsigned short CreateTime;
    unsigned short CreateDate;
    unsigned short LastAccessDate;
    unsigned short FirstLogicalClusterHi;
    unsigned short LastWriteTime;
    unsigned short LastWriteDate;
    unsigned short FirstLogicalCluster;
    unsigned int FileSize;
}fat_packed FAT_DIRECTORY;

typedef struct{
    char LDIR_Ord;
    char LDIR_Name1[10];
    char LDIR_Attr;
    char LDIR_Type;
    char LDIR_Chksum;
    char LDIR_Name2[12];
    char LDIR_FstClusLO[2];
    char LDIR_Name3[4];
}fat_packed FAT_LONG_DIRECTORY;


#define ATTR_IBUTE_VOLUME_BIT       (0x01)
#define ATTRIBUTE_DIRECTORY_BIT    (0x01 << 1)
#define ATTRIBUTE_SYSTEM_BIT       (0x01 << 2)
#define ATTRIBUTE_READONLY_BIT     (0x01 << 3)
#define ATTRIBUTE_ARCHIVE_BIT      (0x01 << 4)
#define ATTRIBUTE__BIT             (0x01 << 5)

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LONG_NAME (ATTR_READ_ONLY |ATTR_HIDDEN |ATTR_SYSTEM |ATTR_VOLUME_ID)
#define ATTR_LONG_NAME_MASK (ATTR_READ_ONLY |ATTR_HIDDEN |ATTR_SYSTEM |ATTR_VOLUME_ID | ATTR_DIRECTORY | ATTR_ARCHIVE)

#define LAST_LONG_ENTRY         0x40

CLUSTER_TYPE fat16ClusterType(alt_u32 Fat);
CLUSTER_TYPE fat32ClusterType(alt_u32 Fat);
alt_u32 fatNextCluster(VOLUME_INFO *pVol, alt_u32 ThisCluster);
alt_u32 fatFindUnusedCluster(VOLUME_INFO *pVol);
void fatDumpDate(unsigned short Date);
void fatDumpTime(unsigned short Date);
bool fatIsValidDir(FAT_DIRECTORY *pDir);
bool fatIsLastDir(FAT_DIRECTORY *pDir);
bool fatDelClusterList(VOLUME_INFO *pVol, alt_u32 StartCluster);

bool fatReadSecter(VOLUME_INFO *pVol, alt_u32 nSecter);
alt_u32 fatFindUnusedDirectory(VOLUME_INFO *pVol);
bool fatIsUnusedDir(FAT_DIRECTORY *pDir);

#define SD_4BIT_MODE

#define SD_BIT_APP_CMD              (0x01 << 5)
#define SD_BIT_READY_FOR_DATA       (0x01 << 8)

#define SD_BITS_CURRENT_STATE       (0x0F << 9)

#define SD_CURRENT_STATE_DECODE(x)      ((x >> 9) & 0x0F)
#define SD_CURRENT_STATE_ENCODE(x)      ((x & 0x0F) << 9)


alt_u32 ArrayToU32(alt_u8 szData[]);

#define SD_VHS_2V7_3V6              0x01
#define CMD8_DEFAULT_TEST_PATTERN   0xAA

typedef enum{
    SD_STATE_IDLE = 0,
    SD_STATE_READY = 1,
    SD_STATE_IDENT = 2,
    SD_STATE_STBY = 3,
    SD_STATE_TRAN
}SD_CURRENT_STATE;


#define HW_MIN_VOL     47
#define HW_DEFAULT_VOL  120


#define MAX_FILE_NUM    128
#define FILENAME_LEN    32

bool SD_CMD17(alt_u32 Addr, alt_u8 ExpectedStatus);
bool SD_CMD24(alt_u32 Addr, alt_u8 ExpectedStatus);
bool SD_CMD3(alt_u16 *pRCA16);
bool SD_ACMD6(bool b4BitsBus, alt_u8 ExpectedState);
bool SD_IsSupport4Bits(void);

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
void handle_key(bool *pNexSongPressed);
bool waveplay_start(char *pFilename);
bool waveplay_execute(bool *bEOF);
void DisplayTime(alt_u32 TimeElapsed);

typedef enum {FALSE = 0, TRUE} boolean;

static bool bLastSwitch = FALSE;
static bool bNextSwitch = FALSE;
static bool bMuteSwitch = FALSE;
static bool bPlaySwitch = TRUE;

static const alt_u8 crc7_syndrome_table[256] = {
      0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
      0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
      0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
      0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
      0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
      0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
      0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
      0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
      0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
      0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
      0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
      0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
      0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
      0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
      0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
      0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
      0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
      0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
      0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
      0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
      0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
      0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
      0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
      0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
      0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
      0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
      0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
      0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
      0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
      0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
      0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
      0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};

alt_u8 crc7_byte(alt_u8 crc, alt_u8 data)
{
        return crc7_syndrome_table[(crc << 1) ^ data];
}

alt_u8 crc7(alt_u8 crc, const alt_u8 *buffer, int len)
{
      while (len--)
            crc = crc7_byte(crc, *buffer++);
      return crc;
}

alt_u32 ArrayToU32(alt_u8 szData[])
{
    alt_u32 Value32 = 0;
    int i;

    for(i=0;i<4;i++){
        Value32 <<= 8;
        Value32 |= szData[i];
    }
    return Value32;
}

bool SD_ReadData(alt_u8 szBuf[], int nBufLen){
    bool bSuccess;
    bSuccess = SDHAL_ReadData(szBuf, nBufLen);
    return bSuccess;
}

bool SD_WriteData(alt_u8 szDataWrite[], int nDataLen){
    bool bSuccess;
    bSuccess = SDHAL_WriteData(szDataWrite, nDataLen);
    return bSuccess;
}


class SDCard
{
public:
    void waitInsert();
    bool SDLIB_Init();
private:
    void SD_CMD0();
    bool SD_CMD2(alt_u8 szCID[], int nBufLen);
    bool SD_CMD10(alt_u16 RCA16, alt_u8 szCID[], int nBufLen);
    bool SD_CMD8(alt_u8 VolId, alt_u8 TestPattern);
    void SD_Init();
    bool SD_CMD55(alt_u16 RCA16, alt_u8 ExpectedState);
    bool SD_CMD16(alt_u32 BlockSize);
    bool SD_CMD7(alt_u16 RCA16);
};

class Application
{
public:
    int run();
private:
    int build_wave_play_list(FAT_HANDLE hFat);
    FAT_HANDLE fatMountSdcard();
    SDCard *sdCard;
};

int Application::build_wave_play_list(FAT_HANDLE hFat)
{
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
    {
        printf("browse file fail.\r\n");
        return 0;
    } 
    
    while (Fat_FileBrowseNext(&hFileBrowse,&FileContext))
    {
        if (FileContext.bLongFilename)
        {
            nPos = 0;
            alt_u16 *pData16;
            alt_u8 *pData8;
            pData16 = (alt_u16 *)FileContext.szName;
            pData8 = (alt_u8 *)FileContext.szName;

            while(*pData16)
            {
                if (*pData8 && *pData8 != ' ')
                    szWaveFilename[nPos++] = *pData8;

                pData8++;

                if (*pData8 && *pData8 != ' ')
                    szWaveFilename[nPos++] = *pData8;

                pData8++;                    
                pData16++;
            }

            szWaveFilename[nPos] = 0;
            printf("\n--Music Name:%s --\r\n",szWaveFilename);
        }
        else
        {
            strcpy(szWaveFilename,FileContext.szName);
            printf("\n--Music Name:%s --\r\n",FileContext.szName);
        }       
    }
    gWavePlayList.nFileNum = count;
    return count;
}

#ifdef DEBUG_FAT
    #define FAT_DEBUG(x)    {DEBUG(("[FAT]")); DEBUG(x);}
#else
    #define FAT_DEBUG(x)
#endif

void SDHAL_SendCmd(alt_u8 szCommand[6], int nCmdLen){
    int i, k;
    alt_u8 Mask, Data;
    SD_CMD_OUT;
    for(i=0;i<nCmdLen;i++){
        Mask = 0x80;
        Data = szCommand[i];
        for(k=0;k<8;k++){
            SD_CLK_LOW;
            if (Data & Mask)
                SD_CMD_HIGH;
            else
                SD_CMD_LOW;
            SD_CLK_HIGH;
            Mask >>= 1;
        }
    }
}

bool Fat_FileExist(FAT_HANDLE Fat, const char *pFilename)
{
    bool bFind = false;
    FAT_BROWSE_HANDLE hBrowse;
    FILE_CONTEXT FileContext;

    if (Fat_FileBrowseBegin(Fat, &hBrowse))
    {
        while (!bFind && Fat_FileBrowseNext(&hBrowse, &FileContext))
        {
            if (FileContext.bLongFilename)
            {
                bFind = fatSameLongFilename((alt_u16 *)FileContext.szName, (alt_u16 *)pFilename);
            }
            else
            {
                if (strcmpi(FileContext.szName, pFilename) == 0)
                    bFind = true;
            }
        }
        Fat_FileClose(Fat);
    }
    return bFind;
}

alt_u32 fatFindUnusedCluster(VOLUME_INFO *pVol){
    CLUSTER_TYPE ClusterType;
    alt_u32 UnusedCluster=-1, ThisCluster = 2, ClusterEntryValue;
    void *pFatEntry;
    const alt_32 nFatEntrySize = (pVol->Partition_Type == PARTITION_FAT32)?4:2;
    const alt_32 nFatEntryPerSecter = pVol->BPB_BytsPerSec/nFatEntrySize;
    alt_u32 Secter;
    bool bDone = false;

    while(!bDone){
        Secter = pVol->FatEntrySecter + (ThisCluster*nFatEntrySize)/pVol->BPB_BytsPerSec;
        if (fatReadSecter(pVol, Secter)){
            pFatEntry = (pVol->Secter_Data + (ThisCluster%nFatEntryPerSecter)*nFatEntrySize);
            if (pVol->Partition_Type == PARTITION_FAT32){
                ClusterEntryValue = *(alt_u32*)pFatEntry;
                ClusterType = fat32ClusterType(ClusterEntryValue);
            }else{
                ClusterEntryValue = *(alt_u16 *)pFatEntry;
                ClusterType = fat16ClusterType(ClusterEntryValue);
            }
            if (ClusterType == CLUSTER_UNUSED){
                UnusedCluster = ThisCluster;
                bDone = true;
            }
        }else{
            bDone = true;
        }
        ThisCluster++;
    }

    return UnusedCluster;
}

bool SD_ACMD42(bool b4BitsBus, alt_u8 ExpectedState)
{
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 42;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    szCmd[0] |= Cmd;
    szCmd[4] |= b4BitsBus?1:0;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess){
        alt_u32 CardStatus, ExpectedStatus;
        if (bSuccess && szResponse[0] != Cmd)
            bSuccess = FALSE;

        CardStatus = ArrayToU32(&szResponse[1]);
        ExpectedStatus = SD_BIT_APP_CMD | SD_BIT_READY_FOR_DATA | SD_CURRENT_STATE_ENCODE(ExpectedState);
        if (CardStatus != ExpectedStatus)
            bSuccess = FALSE;

        if (bSuccess){
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);
            if (crc != (szResponse[5] >> 1))
                bSuccess = FALSE;
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01))
            bSuccess = FALSE;
    }

    return bSuccess;
}


bool SDLIB_ReadBlock512(alt_u32 block_number, alt_u8 *buff)
{
    bool bSuccess;
    alt_u32  addr;

    if (gSdInfo.bSDHC)
        addr = block_number;
    else
        addr = block_number * 512;

    if (!SD_CMD17(addr, SD_STATE_TRAN))
        return FALSE;

    bSuccess = SD_ReadData(buff, 512);
    return bSuccess;
}

bool SD_CMD9(alt_u16 RCA16, alt_u8 szCSD[], int nBufLen){
    bool bSuccess;
    alt_u8 szResponse[17];
    const alt_u8 Cmd = 9;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    szCmd[0] |= Cmd;
    szCmd[1] |= RCA16 >> 8;
    szCmd[2] |= RCA16 & 0xFF;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess){
        if (bSuccess && szResponse[0] != 0x3F)
            bSuccess = FALSE;

        if (bSuccess){
            alt_u8 crc;
            crc = crc7(0, &szResponse[1], 15);
            if (crc != (szResponse[16] >> 1))
                bSuccess = FALSE;
        }

        if (bSuccess && ((szResponse[16] & 0x01) != 0x01))
            bSuccess = FALSE;
    }

    if (bSuccess){
        int nCopyLen, x;
        if (nCopyLen > 16)
            nCopyLen = 16;
        for(x=0;x<nCopyLen;x++)
            szCSD[x] = szResponse[x+1];
    }

    return bSuccess;
}

bool SDLIB_WriteBlock512(alt_u32 block_number, alt_u8 *szDataWrite)
{
    bool bSuccess;
    alt_u32  addr;

    if (gSdInfo.bSDHC)
        addr = block_number;
    else
        addr = block_number * 512;

    if (!SD_CMD24(addr, SD_STATE_TRAN))
        return FALSE;

    bSuccess = SD_WriteData(szDataWrite, 512);
    return bSuccess;
}

bool SD_ReadBlock512(DISK_HANDLE DiskHandle, alt_u32 PysicalSelector, alt_u8 szBuf[512]){
    return SDLIB_ReadBlock512(PysicalSelector, szBuf);
}

bool SD_WriteBlock512(DISK_HANDLE DiskHandle, alt_u32 PysicalSelector, alt_u8 szBuf[512]){
    return SDLIB_WriteBlock512(PysicalSelector, szBuf);
}

unsigned int fatArray2Value(unsigned char *pValue, unsigned int nNum)
{
    alt_u8 *pMSB = (pValue + nNum - 1);
    alt_u32 nValue;

    for (int i=0;i<nNum;i++)
    {
        nValue <<= 8;
        nValue |= *pMSB--;

    }

    return nValue;
}

bool fatSameLongFilename(alt_u16 *p1, alt_u16 *p2)
{
    bool bSame = true;

    while (bSame && ((*p1 != 0) || (*p2 != 0)))
    {
        if (*p1 != *p2)
            bSame = false;

        p1++;
        p2++;
    }

    return bSame;
}


bool Fat_FileSeek(FAT_FILE_HANDLE hFileHandle, const FAT_SEEK_POS SeekPos, const int nOffset){
    FAT_FILE_INFO *f = (FAT_FILE_INFO *)hFileHandle;
    VOLUME_INFO *pVol;
    bool bSuccess= true;

    if (!f || !f->Fat)
        return false;
    pVol = (VOLUME_INFO *)f->Fat;

    if (!f->IsOpened)
        return false;

    switch(SeekPos){
        case FILE_SEEK_BEGIN:
            f->SeekPos = nOffset;
            break;
        case FILE_SEEK_CURRENT:
            f->SeekPos += nOffset;
            break;
        case FILE_SEEK_END:
            f->SeekPos = f->Directory.FileSize+nOffset;
            break;
        default:
            bSuccess = false;
            break;

    }
    f->Cluster = f->Directory.FirstLogicalCluster;
    f->ClusterSeq = 0;
    return bSuccess;
}


CLUSTER_TYPE fat16ClusterType(alt_u32 Fat)
{
    CLUSTER_TYPE Type;
    Fat = Fat & 0xFFFF;

    if (Fat > 0 && Fat < 0xFFF0)
        Type = CLUSTER_NEXT_INFILE;
    else if (Fat >= 0xFFF8) 
        Type = CLUSTER_LAST_INFILE;
    else if (Fat == (alt_u16)0x00)
        Type = CLUSTER_UNUSED;
    else if (Fat >= 0xFFF0 && Fat <= 0xFFF6)
        Type = CLUSTER_RESERVED;
    else if (Fat == 0xFFF7)
        Type = CLUSTER_BAD;

    return Type;
}

CLUSTER_TYPE fat32ClusterType(alt_u32 Fat)
{
    CLUSTER_TYPE Type;
    Fat = Fat & 0xFFFFFFF;
    
    if (Fat > 0 && Fat < 0xFFFFFF0)
        Type = CLUSTER_NEXT_INFILE;
    else if (Fat >= 0xFFFFFF8)
        Type = CLUSTER_LAST_INFILE;
    else if (Fat == (alt_u32)0x00)
        Type = CLUSTER_UNUSED;
    else if (Fat >= 0xFFFFFF0 && Fat <= 0xFFFFFF6)
        Type = CLUSTER_RESERVED;
    else if (Fat == 0xFFFFFF7)
        Type = CLUSTER_BAD;

    return Type;

}

bool fatDelClusterList(VOLUME_INFO *pVol, alt_u32 StartCluster){
    CLUSTER_TYPE ClusterType;
    bool bSuccess = true;
    bool bDone = false;
    alt_u32 ClusterIndex, Secter, ClusterEntryValue;
    void *pFatEntryValue;
    const int nFatEntrySize = (pVol->Partition_Type == PARTITION_FAT32)?4:2;
    const int nFatEntryPerSecter = pVol->BPB_BytsPerSec/nFatEntrySize;
    ClusterIndex = StartCluster;

    while (bSuccess && !bDone)
    {
        Secter = pVol->FatEntrySecter + (ClusterIndex*nFatEntrySize)/pVol->BPB_BytsPerSec;
        bSuccess = fatReadSecter(pVol, Secter);

        if (bSuccess)
        {
            pFatEntryValue = (pVol->Secter_Data + (ClusterIndex%nFatEntryPerSecter)*nFatEntrySize);
            if (pVol->Partition_Type == PARTITION_FAT32){
                ClusterEntryValue = *(alt_u32 *)pFatEntryValue;
                ClusterType = fat32ClusterType(ClusterEntryValue);
            }else{
                ClusterEntryValue = *(alt_u16 *)pFatEntryValue;
                ClusterType = fat16ClusterType(ClusterEntryValue);
            }

            if (ClusterType != CLUSTER_NEXT_INFILE && ClusterType != CLUSTER_LAST_INFILE)
            {
                bDone = true;
                bSuccess = false;
            }
            else
            {
                if (ClusterType == CLUSTER_LAST_INFILE)
                    bDone = true;

                if (pVol->Partition_Type == PARTITION_FAT32)
                    *(alt_u32 *)pFatEntryValue = CLUSTER_UNUSED;
                else
                    *(alt_u16 *)pFatEntryValue = CLUSTER_UNUSED;

                bSuccess = pVol->WriteBlock512(pVol->DiskHandle, pVol->Secter_Index, pVol->Secter_Data);
            }
        }
    }

    return bSuccess;
}

bool fatMount(VOLUME_INFO *pVol)
{
    bool bSuccess = TRUE;
    alt_32 FirstPartitionEntry,PartitionType,FirstSectionInVolume1;
    alt_32 nFatTableSize,nFatTableSecterNum;
    alt_u8 szBlock[512];

    if (!pVol->ReadBlock512(pVol->DiskHandle, 0, szBlock)){
        FAT_DEBUG(("Read section 0 error.\n"));
        return FALSE;
    }

    FirstPartitionEntry = 0x1BE;
    PartitionType = szBlock[0x01C2];
    if (PartitionType == PARTITION_FAT16){
        FAT_DEBUG(("FAT16\n"));
    }else if (PartitionType == PARTITION_FAT32){
        FAT_DEBUG(("FAT32\n"));
    }else{
        FAT_DEBUG(("the partition type(%d) is not supported.\n", PartitionType));
        return FALSE;
    }
    pVol->Partition_Type = PartitionType;
    FirstSectionInVolume1 = fatArray2Value(&szBlock[FirstPartitionEntry + 8],4);

    if (!pVol->ReadBlock512(pVol->DiskHandle, FirstSectionInVolume1, szBlock)){
        FAT_DEBUG(("Read first sector in volume one fail.\n"));
        return FALSE;
    }
    pVol->PartitionStartSecter = FirstSectionInVolume1;
    pVol->BPB_BytsPerSec = szBlock[0x0B+1]*256 + szBlock[0x0B];
    pVol->BPB_SecPerCluster = szBlock[0x0D];
    pVol->BPB_RsvdSecCnt = szBlock[0x0E + 1]*256 + szBlock[0x0E];
    pVol->BPB_NumFATs = szBlock[0x10];
    pVol->BPB_RootEntCnt = szBlock[0x11+1]*256 + szBlock[0x11];
    pVol->BPB_FATSz = szBlock[0x16+1]*256 + szBlock[0x16];
    pVol->Secter_Index = -1;
    memset(pVol->Secter_Data, 0, MY_SECTER_SIZE);

    if (pVol->Partition_Type == PARTITION_FAT32)
        pVol->BPB_FATSz = fatArray2Value(&szBlock[0x24], 4);

    if (pVol->BPB_BytsPerSec != MY_SECTER_SIZE)
    {
        FAT_DEBUG(("This program only supports FAT BPB_BytsPerSec == %d\n", MY_SECTER_SIZE));
        return FALSE;
    }
#ifdef DUMP_DEBUG    
    FAT_DEBUG(("First section in partition 1: %04Xh(%d)\n", gVolumeInfo.PartitionStartSecter, gVolumeInfo.PartitionStartSecter));
    FAT_DEBUG(("Byte Per Sector: %04Xh(%d)\n", gVolumeInfo.BPB_BytsPerSec, gVolumeInfo.BPB_BytsPerSec));
    FAT_DEBUG(("Sector Per Clusoter: %02Xh(%d)\n", gVolumeInfo.BPB_SecPerCluster, gVolumeInfo.BPB_SecPerCluster));
    FAT_DEBUG(("Reserved Sectors: %04Xh(%d)\n", gVolumeInfo.BPB_RsvdSecCnt, gVolumeInfo.BPB_RsvdSecCnt));
    FAT_DEBUG(("Number of Copyies of FAT: %02Xh(%d)\n", gVolumeInfo.BPB_NumFATs, gVolumeInfo.BPB_NumFATs));
    FAT_DEBUG(("Maxmun Root Directory Entries: %04Xh(%d)\n", gVolumeInfo.BPB_RootEntCnt, gVolumeInfo.BPB_RootEntCnt));
    FAT_DEBUG(("Sectors Per FAT: %04Xh(%d)\n", gVolumeInfo.BPB_FATSz, gVolumeInfo.BPB_FATSz));
#endif
    pVol->FatEntrySecter = pVol->PartitionStartSecter + pVol->BPB_RsvdSecCnt;
    pVol->RootDirectoryEntrySecter = pVol->FatEntrySecter + pVol->BPB_NumFATs * pVol->BPB_FATSz;
    pVol->DataEntrySecter = pVol->RootDirectoryEntrySecter + ((pVol->BPB_RootEntCnt*32)+(pVol->BPB_BytsPerSec-1))/pVol->BPB_BytsPerSec;

    pVol->nBytesPerCluster = pVol->BPB_BytsPerSec * pVol->BPB_SecPerCluster;
    nFatTableSecterNum = pVol->BPB_NumFATs * pVol->BPB_FATSz;
    nFatTableSize = nFatTableSecterNum * pVol->BPB_BytsPerSec;
#ifdef FAT_READONLY    
    pVol->szFatTable = malloc(nFatTableSize);
    if (!pVol->szFatTable){
        FAT_DEBUG(("fat malloc(%d) fail!\r\n", nFatTableSize));
        return FALSE;
    }
    for(i=0;i<nFatTableSecterNum && bSuccess; i++ ){
        if (!pVol->ReadBlock512(pVol->DiskHandle, pVol->FatEntrySecter+i, pVol->szFatTable + i*pVol->BPB_BytsPerSec)){
            FAT_DEBUG(("Read first sector in volume one fail.\r\n"));
            bSuccess = FALSE;
        }
    }    if (!bSuccess && pVol->szFatTable){
        free(pVol->szFatTable);
        pVol->szFatTable = 0;
    }
#endif


    if (bSuccess){
        FAT_DEBUG(("Fat_Mount success\r\n"));
    }else{
        FAT_DEBUG(("Fat_Mount fail\r\n"));
    }
    pVol->bMount = bSuccess;
    return bSuccess;
}

alt_u32 fatNextCluster(VOLUME_INFO *pVol, alt_u32 ThisCluster){
    CLUSTER_TYPE ClusterType;
    alt_u32 NextCluster;
#ifdef FAT_READONLY    

    NextCluster =  *(unsigned short *)(pVol->szFatTable + (ThisCluster << 1));

    ClusterType = fatClusterType(pVol, NextCluster);

    if (ClusterType != CLUSTER_NEXT_INFILE && ClusterType != CLUSTER_LAST_INFILE)
        NextCluster = 0;
#else
    alt_32 nFatEntryPerSecter;
    const alt_32 nFatEntrySize = (pVol->Partition_Type == PARTITION_FAT32)?4:2;
    alt_u32 Secter;
    nFatEntryPerSecter = pVol->BPB_BytsPerSec/nFatEntrySize;
    Secter = pVol->FatEntrySecter + (ThisCluster*nFatEntrySize)/pVol->BPB_BytsPerSec;
    if (fatReadSecter(pVol, Secter)){
        if (pVol->Partition_Type == PARTITION_FAT32){
            NextCluster = *(alt_u32*)(pVol->Secter_Data + (ThisCluster%nFatEntryPerSecter)*nFatEntrySize);
            ClusterType = fat32ClusterType(NextCluster);
        }else{
            NextCluster = *(alt_u16 *)(pVol->Secter_Data + (ThisCluster%nFatEntryPerSecter)*nFatEntrySize);
            ClusterType = fat16ClusterType(NextCluster);
        }
        if (ClusterType != CLUSTER_NEXT_INFILE && ClusterType != CLUSTER_LAST_INFILE)
            NextCluster = 0;
    }

    return NextCluster;

#endif

    return NextCluster;
}


FAT_HANDLE Application::fatMountSdcard()
{
    FAT_HANDLE hFat = 0;
    VOLUME_INFO *pVol;
    const int nMaxTry=10;
    bool bFind = FALSE;
    int nTry=0;
    bool bSuccess = TRUE;

    while(!bFind && nTry++ < nMaxTry){
        bFind = sdCard->SDLIB_Init();
        if (!bFind)
            usleep(100*1000);
    }
    if (!bFind){
        FAT_DEBUG(("Cannot find SD card.\r\n"));
        return hFat;
    }

    hFat = malloc(sizeof(VOLUME_INFO));
    pVol = (VOLUME_INFO *)hFat;
    pVol->ReadBlock512 = SD_ReadBlock512;
    pVol->WriteBlock512 = SD_WriteBlock512;
    bSuccess = fatMount(pVol);

    if (bSuccess){
        FAT_DEBUG(("Fat_Mount success\r\n"));
        pVol->bMount = TRUE;
    }else{
        FAT_DEBUG(("Fat_Mount fail\r\n"));
        free((void *)hFat);
        hFat = 0;
    }

    return hFat;
}

bool fatIsLastDir(FAT_DIRECTORY *pDir)
{
    if (pDir->Name[0] == REMAINED_UNUSED_DIR_ENTRY)
        return TRUE;

    return FALSE;
}


bool fatIsValidDir(FAT_DIRECTORY *pDir){
    char szTest[] = {0x00, 0xE5, 0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, 0x7C};
    int i;

    for(i=0;i<sizeof(szTest)/sizeof(szTest[0]);i++){
        if (pDir->Name[0] == szTest[i]){
            return FALSE;
        }
    }
    return TRUE;
}

void fatDumpDate(unsigned short Date){
    int Year, Month, Day;
    Year = ((Date >> 9) & 0x1F) + 1980;
    Month = ((Date >> 5) & 0xF);
    Day = ((Date >> 0) & 0x1F);
    FAT_DEBUG(("%d,%d,%d", Year, Month, Day));
}

void fatDumpTime(unsigned short Date){
    int H,M,S;
    H = ((Date >> 9) & 0x1F);
    M = ((Date >> 5) & 0x3F);
    S = ((Date >> 0) & 0x1F)*2;
    FAT_DEBUG(("%d:%d:%d", H, M, S));
}

bool fatIsUnusedDir(FAT_DIRECTORY *pDir){
    if (pDir->Name[0] == (alt_8)UNUSED_DIR_ENTRY)
        return TRUE;
    return FALSE;
}

#ifdef DEBUG_SDCARD
    #define SDCARD_TRACE(x)    {DEBUG(("[SD_LIB]")); DEBUG(x);}  
    #define SDCARD_DEBUG(x)    {DEBUG(("[SD_LIB]")); DEBUG(x);}  
#else
    #define SDCARD_DEBUG(x)
    #define SDCARD_TRACE(x)
#endif

#define DEBUG_SDCARD_HEX

bool SD_ACMD41(alt_u32 HostOCR32, alt_u32 *pOCR32)
{
    alt_u32 OCR;
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 41;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    szCmd[0] |= Cmd;
    szCmd[1] |= (HostOCR32>> 24) & 0x40;
    szCmd[2] = (HostOCR32 >> 16) & 0xFF;
    szCmd[3] = (HostOCR32 >> 8) & 0xFF;
    szCmd[4] = HostOCR32 & 0xFF;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;

    SDHAL_SendCmd(szCmd, sizeof(szCmd));

    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess){
        int i;
        if (bSuccess && szResponse[0] != 0x3F){
            bSuccess = FALSE;
        }

        OCR = 0;
        for(i=0;i<4;i++){
            OCR <<= 8;
            OCR |= szResponse[i+1];
        }

        if ((OCR & 0x80000000) != 0x80000000)
            bSuccess = false;

        if (bSuccess && (szResponse[5] != 0xFF))
            bSuccess = false;

    }

   if (bSuccess)
      *pOCR32 = OCR;

    return bSuccess;
}

unsigned int Fat_FileSize(FAT_FILE_HANDLE hFileHandle){
    FAT_FILE_INFO *f = (FAT_FILE_INFO *)hFileHandle;
    if (f->IsOpened)
        return f->Directory.FileSize;
    return 0;
}

bool SD_CMD24(alt_u32 Addr, alt_u8 ExpectedStatus){
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 24;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    szCmd[0] |= Cmd;
    szCmd[1] |= (Addr >> 24) & 0xFF;
    szCmd[2] |= (Addr >> 16) & 0xFF;
    szCmd[3] |= (Addr >> 8) & 0xFF;
    szCmd[4] |= Addr & 0xFF;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess){
        alt_u32 CardStatus, ExpectedStatus;
        if (bSuccess && szResponse[0] != Cmd){
            SDCARD_DEBUG(("CMD%d Response: Invalid strat+tx+reserved bit\r\n", Cmd));
            bSuccess = FALSE;
        }

        CardStatus = ArrayToU32(&szResponse[1]);
        ExpectedStatus = SD_BIT_READY_FOR_DATA | SD_CURRENT_STATE_ENCODE(SD_STATE_TRAN);
        if (CardStatus != ExpectedStatus){
            SDCARD_DEBUG(("CMD%d Response: unexpected stauts\r\n", Cmd));
            bSuccess = FALSE;
        }

        if (bSuccess){
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);
            if (crc != (szResponse[5] >> 1)){
                SDCARD_DEBUG(("CMD%d Response: Invalid crc7\r\n", Cmd));
                bSuccess = FALSE;
            }
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01)){
            SDCARD_DEBUG(("CMD%d Response: Invalid end-bits\r\n", Cmd));
            bSuccess = FALSE;
        }
    }
    return bSuccess;
}




bool SDCard::SD_CMD8(alt_u8 VolId, alt_u8 TestPattern)
{
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 8;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;

    SDCARD_TRACE(("CMD8\r\n"));
    szCmd[0] |= Cmd;
    szCmd[3] |= (VolId & 0x0F);
    szCmd[4] = TestPattern;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;

    SDHAL_SendCmd(szCmd, sizeof(szCmd));

    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess){
        if (bSuccess && szResponse[0] != Cmd){
            bSuccess = FALSE;
        }

        if (bSuccess && szResponse[3] != VolId){
            bSuccess = FALSE;
        }

        if (bSuccess && szResponse[4] != TestPattern){
            bSuccess = FALSE;
        }
        if (bSuccess)
        {
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);

            if (crc != (szResponse[5] >> 1))
                bSuccess = FALSE;
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01))
            bSuccess = FALSE;
    }

    return bSuccess;

}

FAT_DIRECTORY *fatFindDirectory(VOLUME_INFO *pVol, alt_u32 nDirectoryIndex)
{
    FAT_DIRECTORY *pDir = NULL;
    alt_u32 nSecterOffset, nSecter, nByteOffset;
    nSecterOffset = (sizeof(FAT_DIRECTORY)*nDirectoryIndex)/pVol->BPB_BytsPerSec;
    nSecter = pVol->RootDirectoryEntrySecter + nSecterOffset;

    if (fatReadSecter(pVol, nSecter))
    {
        nByteOffset = (sizeof(FAT_DIRECTORY)*nDirectoryIndex)%pVol->BPB_BytsPerSec;
        pDir = (FAT_DIRECTORY *)(pVol->Secter_Data + nByteOffset);
    }

    return pDir;
}


void SDCard::SD_CMD0()
{
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    SDCARD_TRACE(("CMD0\r\n"));
    szCmd[0] |= 0X00;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    SDHAL_DummyClock(8);
}

FAT_FILE_HANDLE Fat_FileOpen(FAT_HANDLE Fat, const char *pFilename)
{
    bool bFind = false;
    FAT_BROWSE_HANDLE hBrowse;
    FILE_CONTEXT FileContext;
    FAT_FILE_INFO *pFile = 0;
    int nPos = 0;
    char szWaveFilename[MAX_FILENAME_LENGTH];

    if (Fat_FileBrowseBegin(Fat, &hBrowse)){
        while (!bFind && Fat_FileBrowseNext(&hBrowse, &FileContext)){
            if (FileContext.bLongFilename){
                nPos = 0;
                alt_u16 *pData16;
                alt_u8 *pData8;
                pData16 = (alt_u16 *)FileContext.szName;
                pData8 = (alt_u8 *)FileContext.szName;
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

                if(strcmp(pFilename,szWaveFilename) == 0)
                    bFind = true;
            }else{
                if (strcmpi(FileContext.szName, pFilename) == 0)
                    bFind = true;
            }
        }

        if (bFind)
        {
            pFile = (FAT_FILE_INFO *)malloc(sizeof(FAT_FILE_INFO));

            if (pFile)
            {
                pFile->OpenAttribute = FILE_OPEN_READ;
                pFile->SeekPos = 0;
                pFile->Directory = FileContext;
                pFile->IsOpened = true;
                pFile->Cluster = FileContext.FirstLogicalCluster;
                pFile->ClusterSeq = 0;
                pFile->Fat = Fat;
            }
        }
    }

    return (FAT_FILE_HANDLE)pFile;
}


bool SDCard::SDLIB_Init()
{
    bool bSuccess = FALSE, bTimeout = FALSE;
    alt_u8  szThisCID[16];
    const int nTimeout = alt_ticks_per_second();
    alt_u32 TimeStart;
    memset(&gSdInfo, 0, sizeof(gSdInfo));
    gSdInfo.bSupport4Bits = SD_IsSupport4Bits();
    printf("--- Power On, Card Identification Mode, Idle State\r\n");
    printf("sd %d-bit mode\r\n", gSdInfo.bSupport4Bits?4:1);
    usleep(74*10);
    SD_Init();
    SD_CMD0();
    usleep(100*1000);
    if (SD_CMD8(SD_VHS_2V7_3V6, CMD8_DEFAULT_TEST_PATTERN)){
        gSdInfo.HostOCR32 = 0x40FF8000;
        gSdInfo.bVer2 = TRUE;
        gSdInfo.bSDHC = TRUE;
        SDCARD_DEBUG((" 2.0 \r\n"));
    }else{
        gSdInfo.HostOCR32 = 0x00040000;
        SD_CMD0();
        usleep(100*1000);
    }
    TimeStart = alt_nticks();
    while(!bSuccess && !bTimeout){
        if (!SD_CMD55(gSdInfo.RCA16, SD_STATE_IDLE))
        {
            return FALSE;
        }else{
            bSuccess = SD_ACMD41(gSdInfo.HostOCR32, &gSdInfo.OCR32);
            if (bSuccess && gSdInfo.bSDHC && ((gSdInfo.OCR32 & 0x40000000) == 0x00)){
                gSdInfo.bSDHC = FALSE;
            }
            if (!bSuccess){
                if ((alt_nticks() - TimeStart) > nTimeout){
                    bTimeout = TRUE;
                }else{
                    usleep(10*1000);
                }
            }
        }
    }
    if (!bSuccess){
        return FALSE;
    }
    if (!SD_CMD2(gSdInfo.szCID, sizeof(gSdInfo.szCID))){
        return FALSE;
    }

    if (!SD_CMD3(&gSdInfo.RCA16)){
        return FALSE;
    }

    if (!SD_CMD9(gSdInfo.RCA16, gSdInfo.szCSD, sizeof(gSdInfo.szCSD))){
        SDCARD_DEBUG(("CMD9 fail\r\n"));
        return FALSE;
    }

    if (!SD_CMD10(gSdInfo.RCA16, szThisCID, sizeof(szThisCID))){
        SDCARD_DEBUG(("CMD10 fail\r\n"));
        return FALSE;
    }

    if (!SD_CMD7(gSdInfo.RCA16)){
        SDCARD_DEBUG(("CMD7 fail\r\n"));
        return FALSE;
    }

    if (!SD_CMD16(512))
    {
        return FALSE;
    }
    if (gSdInfo.bSupport4Bits){

        if (!SD_CMD55(gSdInfo.RCA16, SD_STATE_TRAN)){
            return FALSE;
        }

        if (!SD_ACMD6(TRUE, SD_STATE_TRAN)){
            return FALSE;
        }

        if (!SD_CMD55(gSdInfo.RCA16, SD_STATE_TRAN)){
            return FALSE;
        }


        if (!SD_ACMD42(TRUE, SD_STATE_TRAN)){
            return FALSE;
        }
    }

    return TRUE;
}

bool SD_CMD17(alt_u32 Addr, alt_u8 ExpectedStatus){
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 17;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    szCmd[0] |= Cmd;
    szCmd[1] |= (Addr >> 24) & 0xFF;
    szCmd[2] |= (Addr >> 16) & 0xFF;
    szCmd[3] |= (Addr >> 8) & 0xFF;
    szCmd[4] |= Addr & 0xFF;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess){
        alt_u32 CardStatus, ExpectedStatus;
        if (bSuccess && szResponse[0] != Cmd){
            SDCARD_DEBUG(("CMD%d Response: Invalid strat+tx+reserved bit\r\n", Cmd));
            bSuccess = FALSE;
        }

        CardStatus = ArrayToU32(&szResponse[1]);
        ExpectedStatus = SD_BIT_READY_FOR_DATA | SD_CURRENT_STATE_ENCODE(SD_STATE_TRAN);
        if (CardStatus != ExpectedStatus){
            SDCARD_DEBUG(("CMD%d Response: unexpected stauts\r\n", Cmd));
            bSuccess = FALSE;
        }

        if (bSuccess){
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);
            if (crc != (szResponse[5] >> 1)){
                SDCARD_DEBUG(("CMD%d Response: Invalid crc7\r\n", Cmd));
                bSuccess = FALSE;
            }
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01))
            bSuccess = FALSE;
    }
    return bSuccess;
}


void SDCard::waitInsert()
{
    printf("Komt ie hier?\r\n");

    bool bFirstTime2Detect = TRUE;

    while (!SDLIB_Init())
    {
        if (bFirstTime2Detect)
        {
            printf("Please insert SD card.\r\n");
            bFirstTime2Detect = true;
        }

        usleep(100*1000);
    }
}

alt_u32 fatFindUnusedDirectory(VOLUME_INFO *pVol)
{
    alt_u32 Dir = -1;
    FAT_DIRECTORY *pDir=NULL;
    alt_u32 nSecterOffset, nSecter, nByteOffset;
    alt_u32 nDirectoryIndex = 2;
    bool bDone = false;

    while(!bDone)
    {
        nSecterOffset = (sizeof(FAT_DIRECTORY)*nDirectoryIndex)/pVol->BPB_BytsPerSec;
        nSecter = pVol->RootDirectoryEntrySecter + nSecterOffset;
        if (fatReadSecter(pVol, nSecter))
        {
            nByteOffset = (sizeof(FAT_DIRECTORY)*nDirectoryIndex)%pVol->BPB_BytsPerSec;
            pDir = (FAT_DIRECTORY *)(pVol->Secter_Data + nByteOffset);

            if (fatIsLastDir(pDir) || fatIsUnusedDir(pDir))
            {
                bDone = true;

                if (fatIsUnusedDir(pDir))
                    Dir = nDirectoryIndex;
            }
            nDirectoryIndex++;
        }
        else
        {
            bDone = false;
        }
    }

    return Dir;
}



bool SDCard::SD_CMD2(alt_u8 szCID[], int nBufLen)
{
    bool bSuccess;
    alt_u8 szResponse[17];  // 136/8 = 17
    const alt_u8 Cmd = 2;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;

    SDCARD_TRACE(("CMD2\r\n"));
    szCmd[0] |= Cmd;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;

    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess)
    {
        if (bSuccess && szResponse[0] != 0x3F){
            SDCARD_DEBUG(("CMD%d Response: Invalid strat+tx+reserved bit\r\n", Cmd));
            bSuccess = FALSE;
        }


        if (bSuccess){
            alt_u8 crc;
            crc = crc7(0, &szResponse[1], 15);
            if (crc != (szResponse[16] >> 1)){
                SDCARD_DEBUG(("CMD%d Response: Invalid crc7\r\n", Cmd));
                bSuccess = FALSE;
            }
        }

        if (bSuccess && ((szResponse[16] & 0x01) != 0x01)){
            SDCARD_DEBUG(("CMD%d Response: Invalid end-bits\r\n", Cmd));
            bSuccess = FALSE;
        }

    }

    if (bSuccess)
    {
        int nCopyLen, x;
        nCopyLen = nBufLen;

        if (nCopyLen > 16)
            nCopyLen = 16;

        for(x=0;x<nCopyLen;x++)
            szCID[x] = szResponse[x+1];
    }

    return bSuccess;

}

void SDCard::SD_Init(void){
    SDHAL_Init();
}

bool SD_IsSupport4Bits(void){
    return SDHAL_IsSupport4Bits();
}

bool SD_CMD3(alt_u16 *pRCA16){
    bool bSuccess;
    alt_u16 RCA16;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 3;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    SDCARD_TRACE(("CMD3\r\n"));
    szCmd[0] |= Cmd;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess){
        alt_u16 Status16;
        alt_u8  State8;
        if (bSuccess && szResponse[0] != Cmd){
            SDCARD_DEBUG(("CMD%d Response: Invalid strat+tx+reserved bit\r\n", Cmd));
            bSuccess = FALSE;
        }

        RCA16 = (szResponse[1] << 8) | szResponse[2];

        Status16 = (szResponse[3] << 8) | szResponse[4];
        State8 = SD_CURRENT_STATE_DECODE(Status16);
        if (State8 != SD_STATE_IDENT){
           SDCARD_DEBUG(("CMD%d Response: Unexpected CURRENT_STATE\r\n", Cmd));
           bSuccess = FALSE;
        }

        if (bSuccess)
        {
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);

            if (crc != (szResponse[5] >> 1))
                bSuccess = FALSE;
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01))
            bSuccess = FALSE;
    }

    if (bSuccess)
        *pRCA16 = RCA16;

    return bSuccess;
}


bool SD_ACMD6(bool b4BitsBus, alt_u8 ExpectedState)
{
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 6;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    SDCARD_TRACE(("ACMD6\r\n"));
    szCmd[0] |= Cmd;
    szCmd[4] |= b4BitsBus?2:0;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess)
    {
        alt_u32 CardStatus, ExpectedStatus;
        if (bSuccess && szResponse[0] != Cmd){
            SDCARD_DEBUG(("CMD%d Response: Invalid reserved bits\r\n", Cmd));
            bSuccess = FALSE;
        }

        CardStatus = ArrayToU32(&szResponse[1]);
        ExpectedStatus = SD_BIT_APP_CMD | SD_BIT_READY_FOR_DATA | SD_CURRENT_STATE_ENCODE(ExpectedState);
        if (CardStatus != ExpectedStatus){
            SDCARD_DEBUG(("CMD%d Response: unexpected stauts\r\n", Cmd));
            bSuccess = FALSE;
        }

        if (bSuccess){
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);
            if (crc != (szResponse[5] >> 1)){
                SDCARD_DEBUG(("CMD%d Response: Invalid crc7\r\n", Cmd));
                bSuccess = FALSE;
            }
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01))
        {
            SDCARD_DEBUG(("CMD%d Response: Invalid end-bits\r\n", Cmd));
            bSuccess = FALSE;
        }
    }

    return bSuccess;
}

void Fat_Unmount(FAT_HANDLE Fat){
    VOLUME_INFO *pVol = (VOLUME_INFO *)Fat;
    if (!pVol)
        return;
#ifdef FAT_READONLY      
    if (pVol->szFatTable)
    {
        free(pVol->szFatTable);
        pVol->szFatTable = 0;
    }
#endif
    pVol->bMount = false;
    free(pVol);
}

FAT_TYPE Fat_Type(FAT_HANDLE Fat){
    VOLUME_INFO *pVol = (VOLUME_INFO *)Fat;
    FAT_TYPE Type = FAT_UNKNOWN;
    if (pVol){
        if (pVol->Partition_Type == PARTITION_FAT16)
            Type = FAT_FAT16;
        else if (pVol->Partition_Type == PARTITION_FAT32)
            Type = FAT_FAT32;
    }
    return Type;
}

bool Fat_FileBrowseBegin(FAT_HANDLE hFat, FAT_BROWSE_HANDLE *pFatBrowseHandle){
    VOLUME_INFO *pVol = (VOLUME_INFO *)hFat;
    if (!pVol)
        return false;
    if (!pVol->bMount)
        return false;
    pFatBrowseHandle->DirectoryIndex = -1;
    pFatBrowseHandle->hFat = hFat;
    return true;
}

bool SDCard::SD_CMD16(alt_u32 BlockSize)
{
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 16;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    szCmd[0] |= Cmd;
    szCmd[1] |= BlockSize >> 24;
    szCmd[2] |= BlockSize >> 16;
    szCmd[3] |= BlockSize >> 8;
    szCmd[4] |= BlockSize & 0xFF;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess)
    {
        alt_u32 CardStatus, ExpectedStatus;
        if (bSuccess && szResponse[0] != Cmd)
        { 
            SDCARD_DEBUG(("CMD%d Response: Invalid strat+tx+reserved bit\r\n", Cmd));
            bSuccess = FALSE;
        }

        CardStatus = ArrayToU32(&szResponse[1]);
        ExpectedStatus = SD_BIT_READY_FOR_DATA | SD_CURRENT_STATE_ENCODE(SD_STATE_TRAN);
        if (CardStatus != ExpectedStatus){
            SDCARD_DEBUG(("CMD%d Response: unexpected stauts\r\n", Cmd));
            bSuccess = FALSE;
        } 

        if (bSuccess)
        {
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);
            if (crc != (szResponse[5] >> 1)){
                SDCARD_DEBUG(("CMD%d Response: Invalid crc7\r\n", Cmd));
                bSuccess = FALSE;
            }
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01))
        {
            SDCARD_DEBUG(("CMD%d Response: Invalid end-bits\r\n", Cmd));
            bSuccess = FALSE;
        }

    }

    return bSuccess;
}

bool SDCard::SD_CMD7(alt_u16 RCA16)
{
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 7;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    szCmd[0] |= Cmd;
    szCmd[1] |= RCA16 >> 8;
    szCmd[2] |= RCA16 & 0xFF;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;
    SDHAL_SendCmd(szCmd, sizeof(szCmd));
    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess)
    {
        alt_u32 CardStatus, ExpectedStatus;

        if (bSuccess && szResponse[0] != Cmd)
            bSuccess = FALSE;

        CardStatus = ArrayToU32(&szResponse[1]);
        ExpectedStatus = SD_BIT_READY_FOR_DATA | SD_CURRENT_STATE_ENCODE(SD_STATE_STBY);
        if (CardStatus != ExpectedStatus){
            SDCARD_DEBUG(("CMD%d Response: unexpected stauts\r\n", Cmd));
            bSuccess = FALSE;
        }

        if (bSuccess){
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);
            if (crc != (szResponse[5] >> 1)){
                SDCARD_DEBUG(("CMD%d Response: Invalid crc7\r\n", Cmd));
                bSuccess = FALSE;
            }
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01))
            bSuccess = FALSE;

    }

    return bSuccess;
}

bool SDCard::SD_CMD55(alt_u16 RCA16, alt_u8 ExpectedState)
{
    bool bSuccess;
    alt_u8 szResponse[6];
    const alt_u8 Cmd = 55;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;
    szCmd[0] |= Cmd;
    szCmd[1] = RCA16 >> 8;
    szCmd[2] = RCA16 & 0xFF;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;

    SDHAL_SendCmd(szCmd, sizeof(szCmd));

    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess){
        alt_u32 CardStatus, ExpectedStatus;
        if (bSuccess && szResponse[0] != Cmd){
            SDCARD_DEBUG(("CMD%d Response: Invalid command index\r\n", Cmd));
            bSuccess = FALSE;
        }

        CardStatus = ArrayToU32(&szResponse[1]);
        ExpectedStatus = SD_BIT_APP_CMD | SD_BIT_READY_FOR_DATA | SD_CURRENT_STATE_ENCODE(ExpectedState);
        if (CardStatus != ExpectedStatus){
            SDCARD_DEBUG(("CMD%d Response: unexpected stauts\r\n", Cmd));
            bSuccess = FALSE;
        }

        if (bSuccess){
            alt_u8 crc;
            crc = crc7(0, szResponse, 5);
            if (crc != (szResponse[5] >> 1)){
                bSuccess = FALSE;
            }
        }

        if (bSuccess && ((szResponse[5] & 0x01) != 0x01))
        {
            SDCARD_DEBUG(("CMD%d Response: Invalid end-bits\r\n", Cmd));
            bSuccess = FALSE;
        }

    }

    return bSuccess;
}

bool Fat_FileRead(FAT_FILE_HANDLE hFileHandle, void *pBuffer, const int nBufferSize){
    FAT_FILE_INFO *f = (FAT_FILE_INFO *)hFileHandle;
    VOLUME_INFO *pVol;
    alt_u32 Pos, PhysicalSecter, NextCluster, Cluster;
    alt_u32 BytesPerCluster, nReadCount=0, nClusterSeq;
    int s;
    bool bSuccess= true;

    if (!f || !f->Fat)
        return false;
    pVol = (VOLUME_INFO *)f->Fat;

    if (!f->IsOpened)
        return bSuccess;

    BytesPerCluster = pVol->nBytesPerCluster;
    Pos = f->SeekPos;
    if (BytesPerCluster == 32768){
        nClusterSeq = Pos >> 15;
        Pos -= (f->ClusterSeq << 15);
    }else if (BytesPerCluster == 16384){
        nClusterSeq = Pos >> 14;
        Pos -= (f->ClusterSeq << 14);
    }else if (BytesPerCluster == 2048){
        nClusterSeq = Pos >> 11;
        Pos -= (f->ClusterSeq << 11);
    }else{
        nClusterSeq = Pos/BytesPerCluster;
        Pos -= f->ClusterSeq*BytesPerCluster;
    }

    Cluster = f->Cluster;
    if (nClusterSeq != f->ClusterSeq){
        Cluster = f->Cluster;
        while (Pos >= BytesPerCluster && bSuccess){
            NextCluster = fatNextCluster(pVol, Cluster);
            if (NextCluster == 0){
                bSuccess = false;
            }else{
                Cluster = NextCluster;
            }
            Pos -= BytesPerCluster;
            f->Cluster = Cluster;
            f->ClusterSeq++;
        }
    }
    while(nReadCount < nBufferSize && bSuccess){
        if (pVol->BPB_SecPerCluster == 32)
            PhysicalSecter = ((Cluster-2) << 5) + pVol->DataEntrySecter;
        else if (pVol->BPB_SecPerCluster == 64)
            PhysicalSecter = ((Cluster-2) << 6) + pVol->DataEntrySecter;
        else
            PhysicalSecter = (Cluster-2)*pVol->BPB_SecPerCluster + pVol->DataEntrySecter;
        for(s=0;s<pVol->BPB_SecPerCluster && nReadCount < nBufferSize && bSuccess;s++)
        {
            if (Pos >= pVol->BPB_BytsPerSec)
            {
                Pos -= pVol->BPB_BytsPerSec;
            }
            else
            {
                bSuccess = fatReadSecter(pVol, PhysicalSecter);

                if (bSuccess)
                {
                    int nCopyCount;
                    nCopyCount = pVol->BPB_BytsPerSec;
                    if (Pos)
                        nCopyCount -= Pos;
                    if (nCopyCount > (nBufferSize-nReadCount))
                        nCopyCount = nBufferSize-nReadCount;
                    if (nCopyCount == 512){
                        memcpy((char *)pBuffer+nReadCount, pVol->Secter_Data, 512);
                        nReadCount += nCopyCount;
                        if (Pos > 0)
                            Pos = 0;
                    }else{
                        memcpy((void *)((char *)pBuffer+nReadCount), pVol->Secter_Data+Pos,nCopyCount);
                        nReadCount += nCopyCount;
                        if (Pos > 0)
                            Pos = 0;
                    }
                }

            }
            PhysicalSecter++;
        }
        if (nReadCount < nBufferSize){
            NextCluster = fatNextCluster(pVol, Cluster);
            if (NextCluster == 0){
                bSuccess = false;
            }else{
                Cluster = NextCluster;
            }
            f->ClusterSeq++;
            f->Cluster = Cluster;
        }
    }

    if (bSuccess){
        f->SeekPos += nBufferSize;
    }

    return bSuccess;
}

void Fat_FileClose(FAT_FILE_HANDLE hFileHandle){
    FAT_FILE_INFO *f = (FAT_FILE_INFO *)hFileHandle;
    if (!f)
        return;

    free(f);
}


bool SDCard::SD_CMD10(alt_u16 RCA16, alt_u8 szCID[], int nBufLen)
{
    bool bSuccess;
    alt_u8 szResponse[17];
    const alt_u8 Cmd = 10;
    alt_u8 szCmd[6]  = {0x40,0x00,0x00,0x00,0x00, 0x00}, crc;

    szCmd[0] |= Cmd;
    szCmd[1] |= RCA16 >> 8;
    szCmd[2] |= RCA16 & 0xFF;
    crc = crc7(0, szCmd, 5);
    szCmd[5] = (crc << 1) | 0x01;

    SDHAL_SendCmd(szCmd, sizeof(szCmd));

    bSuccess = SDHAL_GetResponse(szResponse, sizeof(szResponse));

    if (bSuccess)
    {
        if (bSuccess && szResponse[0] != 0x3F)
        {
            SDCARD_DEBUG(("CMD%d Response: Invalid strat+tx+reserved bit\r\n", Cmd));
            bSuccess = FALSE;
        }

        if (bSuccess)
        {
            alt_u8 crc;
            crc = crc7(0, &szResponse[1], 15);

            if (crc != (szResponse[16] >> 1))
                bSuccess = FALSE;
        }

        if (bSuccess && ((szResponse[16] & 0x01) != 0x01))
            bSuccess = FALSE;
    }

    if (bSuccess)
    {
        int nCopyLen, x;

        if (nCopyLen > 16)
            nCopyLen = 16;

        for(x=0;x<nCopyLen;x++)
            szCID[x] = szResponse[x+1];
    }

    return bSuccess;
}

void SDHAL_Init(void){
    SD_CMD_OUT;
    SD_DAT_IN;
    SD_CLK_HIGH;
    SD_CMD_HIGH;
    SD_DAT_LOW;
}


void fatDump(FAT_DIRECTORY *pDir){
    char szInvalidName[] = {0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B,
                                    0x3C, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, 0x7C};
    int i;
    if (pDir->Name[0] == (char)0xE5)
        return;

    if (pDir->Name[0] == 0x00)
        return;

    if (pDir->Name[0] <= 0x20 && pDir->Name[0] != 0x05)
        return;

    for(i=0;i<sizeof(szInvalidName)/sizeof(szInvalidName[0]);i++){
        if (pDir->Name[0] == szInvalidName[i]){
            FAT_DEBUG(("Invalid file name.\r\n"));
            return;
        }
    }

    if (pDir->Name[0] == 0x05){
        FAT_DEBUG(("Name:%c%c%c%c%c%c%c%c\n", 0xE5,pDir->Name[1],pDir->Name[2],pDir->Name[3],pDir->Name[4],pDir->Name[5],pDir->Name[6],pDir->Name[6]));
    }else{
        FAT_DEBUG(("Name:%c%c%c%c%c%c%c%c\n", pDir->Name[0],pDir->Name[1],pDir->Name[2],pDir->Name[3],pDir->Name[4],pDir->Name[5],pDir->Name[6],pDir->Name[6]));
    }
    FAT_DEBUG(("Extention:%c%c%c\r\n", pDir->Extension[0],pDir->Extension[1],pDir->Extension[2]));
    FAT_DEBUG(("Attribute:%02Xh\r\n", pDir->Attribute));
    if (pDir->Attribute & ATTR_READ_ONLY)
        FAT_DEBUG(("  Read-Only\r\n"));
    if (pDir->Attribute & ATTR_HIDDEN)
        FAT_DEBUG(("  Hidden\r\n"));
    if (pDir->Attribute & ATTR_SYSTEM)
        FAT_DEBUG(("  System\r\n"));
    if (pDir->Attribute & ATTR_VOLUME_ID)
        FAT_DEBUG(("  Volume\r\n"));
    if (pDir->Attribute & ATTR_DIRECTORY)
        FAT_DEBUG(("  Directory\r\n"));
    if (pDir->Attribute & ATTR_ARCHIVE)
        FAT_DEBUG(("  Archive\r\n"));
    if (pDir->Attribute & ATTR_LONG_NAME)
        FAT_DEBUG(("  Long Name\r\n"));
    FAT_DEBUG(("CreateTime:")); fatDumpTime(pDir->CreateTime);FAT_DEBUG(("\r\n"));
    FAT_DEBUG(("CreateDate:")); fatDumpDate(pDir->LastAccessDate);FAT_DEBUG(("\r\n"));
    FAT_DEBUG(("ClusterHi:%04Xh\r\n", pDir->FirstLogicalClusterHi));
    FAT_DEBUG(("LastWriteTime:")); fatDumpTime(pDir->LastWriteTime);FAT_DEBUG(("\r\n"));
    FAT_DEBUG(("LastWriteDate:")); fatDumpDate(pDir->LastWriteDate);FAT_DEBUG(("\r\n"));
    FAT_DEBUG(("Cluster:%04Xh(%d)\n", pDir->FirstLogicalCluster,pDir->FirstLogicalCluster));
    FAT_DEBUG(("File Size:%08Xh(%ld)\n", pDir->FileSize, (long)pDir->FileSize));
}

bool fatReadSecter(VOLUME_INFO *pVol, alt_u32 nSecter)
{
    bool bSuccess = true;
    if (nSecter != pVol->Secter_Index){
        if (!pVol->ReadBlock512(pVol->DiskHandle, nSecter, pVol->Secter_Data)){
            pVol->Secter_Index = -1;
            bSuccess = false;
        }else{
            pVol->Secter_Index = nSecter;
        }
    }
    return bSuccess;
}
bool SDHAL_IsSupport4Bits(void){
    bool bYes = FALSE;
#ifdef SD_4BIT_MODE
    bYes = TRUE;
#endif
    return bYes;
}

void fatComposeShortFilename(FAT_DIRECTORY *pDir, char *szFilename){
    int i,nPos=0;

    i=0;
    while(i < 8 && pDir->Name[i] != 0 && pDir->Name[i] != ' ')
        szFilename[nPos++] = pDir->Name[i++];

    if (pDir->Attribute & (ATTR_ARCHIVE | ATTR_DIRECTORY)){
        if (pDir->Attribute & (ATTR_ARCHIVE | ATTR_DIRECTORY))
            szFilename[nPos++] = '.';
        i=0;
        while(i < 3 && pDir->Extension[i] != 0 && pDir->Extension[i] != ' ')
            szFilename[nPos++] = pDir->Extension[i++];
    }
    szFilename[nPos++] = 0;
}


bool Fat_FileBrowseNext(FAT_BROWSE_HANDLE *pFatBrowseHandle, FILE_CONTEXT *pFileContext)
{
    bool bFind = FALSE, bError=FALSE, bLongFilename = FALSE;
    int OrderValue = 0;
    FAT_DIRECTORY *pDir;
    VOLUME_INFO *pVol = (VOLUME_INFO *)pFatBrowseHandle->hFat;

    if (!pVol)
        return false;

    if (!pVol->bMount)
        return false;

    do
    {
        pFatBrowseHandle->DirectoryIndex++;
        pDir = fatFindDirectory(pVol,pFatBrowseHandle->DirectoryIndex);
        if (!pDir)
            bError = true;

        if (!bError){
            if (fatIsValidDir(pDir) && !fatIsUnusedDir(pDir)){
                if ((pDir->Attribute & ATTR_LONG_NAME) == ATTR_LONG_NAME){
                    FAT_LONG_DIRECTORY *pLDIR = (FAT_LONG_DIRECTORY *)pDir;
                    if ((pLDIR->LDIR_Attr & ATTR_LONG_NAME) != ATTR_LONG_NAME){
                        bError = TRUE;
                    }else{
                        if (OrderValue == 0){
                            if (bLongFilename || ((OrderValue & LAST_LONG_ENTRY) != OrderValue))
                                bError = TRUE;
                            else
                                OrderValue = pLDIR->LDIR_Ord & 0x3F;
                            memset(pFileContext->szName, 0, sizeof(pFileContext->szName));
                        }else{
                            if ((pLDIR->LDIR_Ord & 0x3F) != OrderValue)
                                bError = TRUE;
                        }
                    }                    if (!bError){
                        int BaseOffset;
                        bLongFilename = TRUE;
                        OrderValue--;
                        BaseOffset = OrderValue * 26;
                        memcpy(pFileContext->szName+BaseOffset, pLDIR->LDIR_Name1, 10);
                        memcpy(pFileContext->szName+BaseOffset+10, pLDIR->LDIR_Name2, 12);
                        memcpy(pFileContext->szName+BaseOffset+22, pLDIR->LDIR_Name3, 4);
                    }
                }else{
                    if (bLongFilename){
                        pFileContext->Attribute = ATTR_LONG_NAME;
                        if ((pDir->Attribute & (ATTR_ARCHIVE | ATTR_DIRECTORY)) == 0)
                            bError = TRUE;
                        else
                            bFind = TRUE;
                    }else{
                        fatComposeShortFilename(pDir, pFileContext->szName);
                        bFind = TRUE;
                    }

                    if (bFind)
                    {
                        pFileContext->bLongFilename = bLongFilename;
                        pFileContext->bFile = (pDir->Attribute & ATTR_ARCHIVE)?TRUE:FALSE;
                        pFileContext->bDirectory = (pDir->Attribute & ATTR_DIRECTORY)?TRUE:FALSE;
                        pFileContext->bVolume = (pDir->Attribute & ATTR_VOLUME_ID)?TRUE:FALSE;
                        pFileContext->DirEntryIndex = pFatBrowseHandle->DirectoryIndex;
                        pFileContext->Attribute = pDir->Attribute;
                        pFileContext->CreateTime = pDir->CreateTime;
                        pFileContext->LastAccessDate = pDir->LastAccessDate;
                        pFileContext->FirstLogicalClusterHi = pDir->FirstLogicalClusterHi;
                        pFileContext->LastWriteTime = pDir->LastWriteTime;
                        pFileContext->LastWriteDate = pDir->LastWriteDate;
                        pFileContext->FirstLogicalCluster = pDir->FirstLogicalCluster;
                        pFileContext->FileSize = pDir->FileSize;
                    }
                }
            }
        }
    }while (!bFind && !fatIsLastDir(pDir) && !bError);

    return bFind;
}

int Application::run()
{
    printf("Startup programma\r\n");
    alt_u8 szWaveFile[FILENAME_LEN];
    memset(&gWavePlay, 0, sizeof(gWavePlay));
    sdCard = new SDCard();
    sdCard->waitInsert();
    hFat = fatMountSdcard();
    build_wave_play_list(hFat);
    return 0;
}

int main()
{
    Application app;
    return app.run();
}

