#define SYSTEM_BUS_WIDTH 32

#include "misc.h"
#include "terasic_includes.h"
#include "FatFileSystem.h"
#include "FatInternal.h"
#include "FatConfig.h"


#ifdef SUPPORT_SD_CARD
    #include "sd_lib.h"
#endif

#ifdef SUPPORT_USB_DISK
    #include "..\terasic_usb_isp1761\class\usb_disk\usb_disk.h"
    #include "..\terasic_usb_isp1761\usb_host\usb_hub.h"
#endif

#define FAT_DEBUG(x) {Uart::getInstance()->puts(x);}

CLUSTER_TYPE fat16ClusterType(alt_u32 Fat)
{
    CLUSTER_TYPE Type;
    Fat = Fat & 0xFFFF;
    
    if (Fat > 0 && Fat < 0xFFF0)
        Type = CLUSTER_NEXT_INFILE;
    else if (Fat >= 0xFFF8) // && Fat <= (unsigned short)0xFFFF)
        Type = CLUSTER_LAST_INFILE;
    else if (Fat == (alt_u16)0x00)
        Type = CLUSTER_UNUSED;
    else if (Fat >= 0xFFF0 && Fat <= 0xFFF6)
        Type = CLUSTER_RESERVED;
    else if (Fat == 0xFFF7)
        Type = CLUSTER_BAD;
        
    return Type;        
         
}

CLUSTER_TYPE fat32ClusterType(alt_u32 Fat){
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

alt_u32 fatNextCluster(VOLUME_INFO *pVol, alt_u32 ThisCluster){
    CLUSTER_TYPE ClusterType;
    alt_u32 NextCluster;
#ifdef FAT_READONLY    
    NextCluster =  *(unsigned short *)(pVol->szFatTable + (ThisCluster << 1));
    ClusterType = fatClusterType(pVol, NextCluster);

    if (ClusterType != CLUSTER_NEXT_INFILE && ClusterType != CLUSTER_LAST_INFILE){
        NextCluster = 0;  // invalid cluster
    }        
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
            NextCluster = 0;  // invalid cluster
    }                
    
    
    return NextCluster;

#endif    
    
    return NextCluster;
}

alt_u32 fatFindUnusedCluster(VOLUME_INFO *pVol){
    CLUSTER_TYPE ClusterType;
    alt_u32 UnusedCluster=-1, ThisCluster = 2, ClusterEntryValue;
    void *pFatEntry;

    const alt_32 nFatEntrySize = (pVol->Partition_Type == PARTITION_FAT32)?4:2;
    const alt_32 nFatEntryPerSecter = pVol->BPB_BytsPerSec/nFatEntrySize;
    alt_u32 Secter;
    bool bDone = FALSE;

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
                UnusedCluster = ThisCluster;  // invalid cluster
                bDone = TRUE;
            }    
        }else{
            bDone = TRUE;
        }     
        ThisCluster++;           
    }                
    
    return UnusedCluster;

}


bool fatDelClusterList(VOLUME_INFO *pVol, alt_u32 StartCluster){
    CLUSTER_TYPE ClusterType;
    bool bSuccess = TRUE;
    bool bDone = FALSE;
    alt_u32 ClusterIndex, Secter, ClusterEntryValue;
    void *pFatEntryValue;
    
    
    const int nFatEntrySize = (pVol->Partition_Type == PARTITION_FAT32)?4:2;
    const int nFatEntryPerSecter = pVol->BPB_BytsPerSec/nFatEntrySize;
     
    ClusterIndex = StartCluster;
    while(bSuccess && !bDone){
        // read FAT Table
        Secter = pVol->FatEntrySecter + (ClusterIndex*nFatEntrySize)/pVol->BPB_BytsPerSec;
        bSuccess = fatReadSecter(pVol, Secter);
        if (bSuccess){
            // get the value of FAT entry
            pFatEntryValue = (pVol->Secter_Data + (ClusterIndex%nFatEntryPerSecter)*nFatEntrySize);
            if (pVol->Partition_Type == PARTITION_FAT32){
                ClusterEntryValue = *(alt_u32 *)pFatEntryValue; 
                ClusterType = fat32ClusterType(ClusterEntryValue);
            }else{
                ClusterEntryValue = *(alt_u16 *)pFatEntryValue; //(pVol->Secter_Data + (ThisCluster%nFatEntryPerSecter)*nFatEntrySize); 
                ClusterType = fat16ClusterType(ClusterEntryValue);
            }
                
            if (ClusterType != CLUSTER_NEXT_INFILE && ClusterType != CLUSTER_LAST_INFILE){
                bDone = TRUE;
                bSuccess = FALSE;
            }else{
                if (ClusterType == CLUSTER_LAST_INFILE)
                    bDone = TRUE; // the last cluster
                // update
                if (pVol->Partition_Type == PARTITION_FAT32)
                    *(alt_u32 *)pFatEntryValue = CLUSTER_UNUSED;
                else 
                    *(alt_u16 *)pFatEntryValue = CLUSTER_UNUSED;
                //write back FAT Table
                bSuccess = pVol->WriteBlock512(pVol->DiskHandle, pVol->Secter_Index, pVol->Secter_Data);
            }    
        }                
    }  // while                
    
    
    return bSuccess;

}


void fatDumpDate(unsigned short Date){
    int Year, Month, Day;
    Year = ((Date >> 9) & 0x1F) + 1980;
    Month = ((Date >> 5) & 0xF);
    Day = ((Date >> 0) & 0x1F);
    //FAT_DEBUG(("%d,%d,%d", Year, Month, Day)); 
}

void fatDumpTime(unsigned short Date){
    int H,M,S;
    H = ((Date >> 9) & 0x1F);
    M = ((Date >> 5) & 0x3F);
    S = ((Date >> 0) & 0x1F)*2;
    //FAT_DEBUG(("%d:%d:%d", H, M, S));     
}

bool fatIsLastDir(FAT_DIRECTORY *pDir){
    if (pDir->Name[0] == REMAINED_UNUSED_DIR_ENTRY)  // 0x00
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

bool fatIsUnusedDir(FAT_DIRECTORY *pDir){
    if (pDir->Name[0] == (alt_8)UNUSED_DIR_ENTRY)
        return TRUE;
    return FALSE; 
}

// debug
void fatDump(FAT_DIRECTORY *pDir){
    char szInvalidName[] = {0x22, 0x2A, 0x2B, 0x2C, 0x2E, 0x2F, 0x3A, 0x3B, 0x3C, 0x3E, 0x3F, 0x5B, 0x5C, 0x5D, 0x7C};
    int i;
    if (pDir->Name[0] == (char)0xE5){
        Uart::getInstance()->puts("the directory entry is free.\n");
        return;
    }
    if (pDir->Name[0] == 0x00)
    {
        Uart::getInstance()->puts("the directory entry is free, and there are no allocated directory entries after tis one.\n");
        return;
    }
    
    if (pDir->Name[0] <= 0x20 && pDir->Name[0] != 0x05){
        FAT_DEBUG(("Invalid file name.\n"));
        return;
    }
    
    for(i=0;i<sizeof(szInvalidName)/sizeof(szInvalidName[0]);i++){
        if (pDir->Name[0] == szInvalidName[i]){
            FAT_DEBUG(("Invalid file name.\n"));
            return;
        }
    }    
    
    //printf("sizeof(FAT_TABLE):%d\n", (int)sizeof(FAT_TABLE));
    if (pDir->Name[0] == 0x05){
        //FAT_DEBUG(("Name:%c%c%c%c%c%c%c%c\n", 0xE5,pDir->Name[1],pDir->Name[2],pDir->Name[3],pDir->Name[4],pDir->Name[5],pDir->Name[6],pDir->Name[6]));
    }else{
        //FAT_DEBUG(("Name:%c%c%c%c%c%c%c%c\n", pDir->Name[0],pDir->Name[1],pDir->Name[2],pDir->Name[3],pDir->Name[4],pDir->Name[5],pDir->Name[6],pDir->Name[6]));
    }        
    //FAT_DEBUG(("Extention:%c%c%c\n", pDir->Extension[0],pDir->Extension[1],pDir->Extension[2]));
    //FAT_DEBUG(("Attribute:%02Xh\n", pDir->Attribute));
    if (pDir->Attribute & ATTR_READ_ONLY)
        FAT_DEBUG(("  Read-Only\n"));
    if (pDir->Attribute & ATTR_HIDDEN)
        FAT_DEBUG(("  Hidden\n"));
    if (pDir->Attribute & ATTR_SYSTEM)
        FAT_DEBUG(("  System\n"));
    if (pDir->Attribute & ATTR_VOLUME_ID)
        FAT_DEBUG(("  Volume\n"));
    if (pDir->Attribute & ATTR_DIRECTORY)
        FAT_DEBUG(("  Directory\n"));
    if (pDir->Attribute & ATTR_ARCHIVE)
        FAT_DEBUG(("  Archive\n"));
    if (pDir->Attribute & ATTR_LONG_NAME)
        FAT_DEBUG(("  Long Name\n"));
    FAT_DEBUG(("CreateTime:")); fatDumpTime(pDir->CreateTime);FAT_DEBUG(("\n"));
    FAT_DEBUG(("CreateDate:")); fatDumpDate(pDir->LastAccessDate);FAT_DEBUG(("\n"));
    //FAT_DEBUG(("ClusterHi:%04Xh\n", pDir->FirstLogicalClusterHi));
    FAT_DEBUG(("LastWriteTime:")); fatDumpTime(pDir->LastWriteTime);FAT_DEBUG(("\n"));
    FAT_DEBUG(("LastWriteDate:")); fatDumpDate(pDir->LastWriteDate);FAT_DEBUG(("\n"));
    //FAT_DEBUG(("Cluster:%04Xh(%d)\n", pDir->FirstLogicalCluster,pDir->FirstLogicalCluster));
    //FAT_DEBUG(("File Size:%08Xh(%ld)\n", pDir->FileSize, (long)pDir->FileSize));
}


unsigned int fatArray2Value(unsigned char *pValue, unsigned int nNum){
    alt_u8 *pMSB = (pValue + nNum - 1);
    alt_u32 nValue;
    int i;
    for(i=0;i<nNum;i++){
        nValue <<= 8;
        nValue |= *pMSB--;
        
    }
    
    return nValue;
}

//=================================================================================
//// add in v2.0
bool fatReadSecter(VOLUME_INFO *pVol, alt_u32 nSecter){
    bool bSuccess = TRUE;
    if (nSecter != pVol->Secter_Index){
        if (!pVol->ReadBlock512(pVol->DiskHandle, nSecter, pVol->Secter_Data)){
            pVol->Secter_Index = -1;
            bSuccess = FALSE;
        }else{
            pVol->Secter_Index = nSecter;
        }
    }
    return bSuccess;          
}     

FAT_DIRECTORY* fatFindDirectory(VOLUME_INFO *pVol, alt_u32 nDirectoryIndex){
    FAT_DIRECTORY *pDir=NULL;
    alt_u32 nSecterOffset, nSecter, nByteOffset;
             
    nSecterOffset = (sizeof(FAT_DIRECTORY)*nDirectoryIndex)/pVol->BPB_BytsPerSec;
    nSecter = pVol->RootDirectoryEntrySecter + nSecterOffset;
        // read sector
    if (fatReadSecter(pVol, nSecter)){
        nByteOffset = (sizeof(FAT_DIRECTORY)*nDirectoryIndex)%pVol->BPB_BytsPerSec; 
        pDir = (FAT_DIRECTORY *)(pVol->Secter_Data + nByteOffset);
    }        
        
    return pDir;        
}

alt_u32 fatFindUnusedDirectory(VOLUME_INFO *pVol){
    alt_u32 Dir = -1;
    FAT_DIRECTORY *pDir=NULL;
    alt_u32 nSecterOffset, nSecter, nByteOffset;
    alt_u32 nDirectoryIndex = 2; //??? start from ?
    bool bDone = FALSE;
             
    while(!bDone){             
        nSecterOffset = (sizeof(FAT_DIRECTORY)*nDirectoryIndex)/pVol->BPB_BytsPerSec;
        nSecter = pVol->RootDirectoryEntrySecter + nSecterOffset;
            // read sector
        if (fatReadSecter(pVol, nSecter)){
            nByteOffset = (sizeof(FAT_DIRECTORY)*nDirectoryIndex)%pVol->BPB_BytsPerSec; 
            pDir = (FAT_DIRECTORY *)(pVol->Secter_Data + nByteOffset);
            if (fatIsLastDir(pDir) || fatIsUnusedDir(pDir)){
                bDone = TRUE;
                if (fatIsUnusedDir(pDir))
                    Dir = nDirectoryIndex;
            }
            nDirectoryIndex++;        
        }else{
                bDone = FALSE;
        }
    }             
    
    return Dir;      
}

bool fatMount(VOLUME_INFO *pVol)
{
    Uart::getInstance()->puts("Fat Mount\r\n");
    bool bSuccess = TRUE;
    alt_32 FirstPartitionEntry,PartitionType,FirstSectionInVolume1;
    alt_32 nFatTableSize,nFatTableSecterNum;//, i;
    alt_u8 szBlock[512];
    
    if (!pVol->ReadBlock512(pVol->DiskHandle, 0, szBlock))
    {
        Uart::getInstance()->puts("Read section 0 error.\n");
        return FALSE;
    }    
        
    FirstPartitionEntry = 0x1BE;
    PartitionType = szBlock[0x01C2];

    if (PartitionType == PARTITION_FAT16)
    {
        Uart::getInstance()->puts("FAT16\r\n");
    }
    else if (PartitionType == PARTITION_FAT32)
    {
        FAT_DEBUG(("FAT32\n"));
    }
    else
    {        
        return false;
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
    memset(pVol->Secter_Data, 0, MY_SECTER_SIZE); // 512 bytes

   
    if (pVol->Partition_Type == PARTITION_FAT32){
        pVol->BPB_FATSz = fatArray2Value(&szBlock[0x24], 4);
    }
    
    if (pVol->BPB_BytsPerSec != MY_SECTER_SIZE)
    {
        return FALSE; // only support FAT16 in this example
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


    //
    pVol->FatEntrySecter = pVol->PartitionStartSecter + pVol->BPB_RsvdSecCnt;



    pVol->RootDirectoryEntrySecter = pVol->FatEntrySecter + pVol->BPB_NumFATs * pVol->BPB_FATSz;
       Uart::getInstance()->puts("Komt ie hier X?\r\n");
    pVol->DataEntrySecter = pVol->RootDirectoryEntrySecter + ((pVol->BPB_RootEntCnt*32)+(pVol->BPB_BytsPerSec-1))/pVol->BPB_BytsPerSec;

  
    // read FAT table into memory
    pVol->nBytesPerCluster = pVol->BPB_BytsPerSec * pVol->BPB_SecPerCluster;
    nFatTableSecterNum = pVol->BPB_NumFATs * pVol->BPB_FATSz;
    nFatTableSize = nFatTableSecterNum * pVol->BPB_BytsPerSec;
#ifdef FAT_READONLY    
    pVol->szFatTable = malloc(nFatTableSize);
    if (!pVol->szFatTable){
        FAT_DEBUG(("fat malloc(%d) fail!", nFatTableSize));
        return FALSE;
    }
    for(i=0;i<nFatTableSecterNum && bSuccess; i++ ){
        if (!pVol->ReadBlock512(pVol->DiskHandle, pVol->FatEntrySecter+i, pVol->szFatTable + i*pVol->BPB_BytsPerSec)){
            FAT_DEBUG(("Read first sector in volume one fail.\n"));
            bSuccess = FALSE;
        }          
    }
    
    
    if (!bSuccess && pVol->szFatTable){
        free(pVol->szFatTable);
        pVol->szFatTable = 0;
    }
#endif    
    
  
    if (bSuccess){
        FAT_DEBUG(("Fat_Mount success\n"));
    }else{        
        FAT_DEBUG(("Fat_Mount fail\n"));
    }        
    pVol->bMount = bSuccess;
    return bSuccess;   
    

}

//===================== SUPPORT_SD_CARD =================================================
#ifdef SUPPORT_SD_CARD

bool SD_ReadBlock512(DISK_HANDLE DiskHandle, alt_u32 PysicalSelector, alt_u8 szBuf[512]){
    return SDLIB_ReadBlock512(PysicalSelector, szBuf);
}

bool SD_WriteBlock512(DISK_HANDLE DiskHandle, alt_u32 PysicalSelector, alt_u8 szBuf[512]){
    return SDLIB_WriteBlock512(PysicalSelector, szBuf);
}

FAT_HANDLE fatMountSdcard(void){
    FAT_HANDLE hFat = 0;
    VOLUME_INFO *pVol;
    const int nMaxTry=10;
    bool bFind = FALSE;
    int nTry=0;
    bool bSuccess = TRUE;
    
        
    //1. chek whether SD Card existed. Init SD card if it is present.
    while(!bFind && nTry++ < nMaxTry){
        bFind = SDLIB_Init();
        if (!bFind)
            usleep(100*1000);
    }
    if (!bFind){
        FAT_DEBUG(("Cannot find SD card.\n"));
        return hFat;
    }    
    
    hFat = malloc(sizeof(VOLUME_INFO));
    pVol = (VOLUME_INFO *)hFat;
    pVol->ReadBlock512 = SD_ReadBlock512;
    pVol->WriteBlock512 = SD_WriteBlock512;
    bSuccess = fatMount(pVol);
        
    
  
    if (bSuccess){
        FAT_DEBUG(("Fat_Mount success\n"));
        pVol->bMount = TRUE;
    }else{        
        FAT_DEBUG(("Fat_Mount fail\n"));
        free((void *)hFat);
        hFat = 0;
    }        

    return hFat;   
}

#endif

#ifdef SUPPORT_USB_DISK


FAT_HANDLE fatMountUsbDisk(DEVICE_HANDLE hUsbDisk){
    bool bSuccess = FALSE;    
    FAT_HANDLE hFat;
    VOLUME_INFO *pVol;
    
    hFat = malloc(sizeof(VOLUME_INFO));
    if (!hFat)
        return 0;
        
    pVol = (VOLUME_INFO *)hFat;
    pVol->DiskHandle = hUsbDisk;
    pVol->ReadBlock512 = USBDISK_ReadBlock512; 
    if (fatMount(pVol)){
        bSuccess = TRUE;
    }else{
        free((void *)hFat);
        hFat = 0;
    }
    
    
    return hFat;
}

#endif ////===================== SUPPORT_USB_DISK =================================================
