#ifndef __ALTERA_UP_SD_CARD_AVALON_INTERFACE_H__
#define __ALTERA_UP_SD_CARD_AVALON_INTERFACE_H__

#include <stddef.h>
#include <alt_types.h>
#include <sys/alt_dev.h>
#include "misc.h"

#define CHAR_TO_UPPER(ch)   ((char) (((ch >= 'a') && (ch <= 'z')) ? ((ch-'a')+'A'): ch))

#define SD_CARD_BUFFER(base, x)         (base + x)
#define SD_CARD_CID(base, x)            (base + 0x0200 + x)
#define SD_CARD_CSD(base, x)            (base + 0x0210 + x)
#define SD_CARD_OCR(base)               (base + 0x0220)
#define SD_CARD_STATUS(base)            (base + 0x0224)
#define SD_CARD_RCA(base)               (base + 0x0228)
#define SD_CARD_ARGUMENT(base)          (base + 0x022C)
#define SD_CARD_COMMAND(base)           (base + 0x0230)
#define SD_CARD_AUX_STATUS(base)        (base + 0x0234)
#define SD_CARD_R1_RESPONSE(base)       (base + 0x0238)

typedef struct alt_up_sd_card_dev
{
	alt_dev dev;
	unsigned int base;
}
alt_up_sd_card_dev;

typedef struct s_file_record
{
    unsigned char name[8];
    unsigned char extension[3];
    unsigned char attributes;
    unsigned short create_time;
    unsigned short create_date;
    unsigned short last_access_date;
    unsigned short last_modified_time;
    unsigned short last_modified_date;
    unsigned short start_cluster_index;
    unsigned file_size_in_bytes;
    unsigned current_cluster_index;
    unsigned current_sector_in_cluster;
    unsigned current_byte_position;
    unsigned file_record_cluster;
    unsigned file_record_sector_in_cluster;
    short file_record_offset;
    unsigned home_directory_cluster;
    bool modified;
    bool in_use;
}
t_file_record;

typedef struct s_FAT_12_16_boot_sector
{
    unsigned char jump_instruction[3];
    char OEM_name[8];
    unsigned short int sector_size_in_bytes;
    unsigned char sectors_per_cluster;
    unsigned short int reserved_sectors;
    unsigned char number_of_FATs;
    unsigned short int max_number_of_dir_entires;
    unsigned short int number_of_sectors_in_partition;
    unsigned char media_descriptor;
    unsigned short int number_of_sectors_per_table;
    unsigned short int number_of_sectors_per_track;
    unsigned short int number_of_heads;
    unsigned int number_of_hidden_sectors;
    unsigned int total_sector_count_if_above_32MB;
    unsigned char drive_number;
    unsigned char current_head;
    unsigned char boot_signature;
    unsigned char volume_id[4];
    char volume_label[11];
    unsigned char file_system_type[8];
    unsigned char bits_for_cluster_index;
    unsigned int first_fat_sector_offset;
    unsigned int second_fat_sector_offset;
    unsigned int root_directory_sector_offset;
    unsigned int data_sector_offset;
}
t_FAT_12_16_boot_sector;

typedef struct s_find_data
{
    unsigned int directory_root_cluster;
    unsigned int current_cluster_index;
    unsigned int current_sector_in_cluster;
    short int file_index_in_sector;
    bool valid;
}
t_find_data;




#define ALTERA_UP_SD_CARD_AVALON_INTERFACE_INSTANCE(name, device)	\
  static alt_up_sd_card_dev device =		\
  {                                                 	\
    {                                               	\
      ALT_LLIST_ENTRY,                              	\
      name##_NAME,                                  	\
      NULL , /* open */		\
      NULL , /* close */		\
      NULL, /* read */		\
      NULL, /* write */		\
      NULL , /* lseek */		\
      NULL , /* fstat */		\
      NULL , /* ioctl */		\
    },                                              	\
	name##_BASE,                                	\
  }

#define ALTERA_UP_SD_CARD_AVALON_INTERFACE_INIT(name, device) \
{	\
    alt_dev_reg(&device.dev);                          	\
}

class ISDCardEx
{
public:
    virtual short readFile(int fd) = 0;
    virtual size_t getSize(int i) = 0;
    virtual ~ISDCardEx() { }
};

class MyFileRecord
{
    t_file_record fr;
public:
    MyFileRecord(t_file_record &fr) : fr(fr) { }
    const char *toString();
};

class MyFile
{
    ISDCardEx *sd;
public:
    int fd;
    MyFile(int fd, ISDCardEx *sd) : sd(sd), fd(fd) { }
    short int read() { return sd->readFile(fd); }
    size_t fread(void *ptr, size_t size, size_t nmemb);
    unsigned getSize() { return sd->getSize(this->fd); }
};

class SDCard
{
protected:
    bool alt_up_sd_card_fclose(short int file_handle);
    short alt_up_sd_card_find_next(char *file_name);
    short alt_up_sd_card_find_next(char *file_name, t_file_record *fr);
    alt_up_sd_card_dev *alt_up_sd_card_open_dev(const char *name, volatile void *base);
    bool alt_up_sd_card_is_Present();
    bool alt_up_sd_card_is_FAT16();
    bool Write_Sector_Data(int sector_index, int partition_offset);
    bool Save_Modified_Sector();
    bool Read_Sector_Data(int sector_index, int partition_offset);
    bool get_cluster_flag(unsigned cluster_index, unsigned short int *flag);
    bool mark_cluster(unsigned cluster_index, short int flag, bool first_fat);
    void alt_up_sd_card_set_attributes(short int file_handle, short int attributes);
    short alt_up_sd_card_get_attributes(short int file_handle);
    short alt_up_sd_card_read(short int file_handle);
    bool alt_up_sd_card_write(short int file_handle, char byte_of_data);
    bool Check_for_Master_Boot_Record(void);
    bool Write_File_Record_At_Offset(int offset, t_file_record *record);
    bool Look_for_FAT16();
    bool Check_for_DOS_FAT(int FAT_partition_start_sector);
    void filename_to_upper_case(char *file_name);
    bool check_file_name_for_FAT16_compliance(char *file_name);
    int get_dir_divider_location(char *name);
    bool match_file_record_to_name_ext(t_file_record *file_record, char *name, char *extension);
    bool find_first_empty_cluster(unsigned *cluster_number);
    int find_first_empty_record_in_a_subdirectory(int start_cluster_index);
    int find_first_empty_record_in_root_directory();
    void convert_filename_to_name_extension(char *filename, char *name, char *extension);
    bool create_file(char *name, t_file_record *file_record, t_file_record *home_dir);
    void copy_file_record_name_to_string(t_file_record *file_record, char *file_name);
    bool find_file_in_directory(int directory_start_cluster, char *file_name, t_file_record *fr);
    bool get_home_directory_cluster_for_file(char *file_name,int *hdc, t_file_record *file_record);
    bool Read_File_Record_At_Offset(int ofs,t_file_record *record,unsigned clus,unsigned sct);
private:
    volatile uint16_t * const aux_status_register;
    volatile uint32_t * const status_register;
    volatile uint16_t * const CSD_register_w0;
    volatile uint16_t * const command_register;
    volatile uint32_t * const command_argument_register;
    volatile uint8_t * const buffer_memory;
    bool is_sd_card_formated_as_FAT16;
    int fat_partition_offset_in_512_byte_sectors;
    int fat_partition_size_in_512_byte_sectors;
    t_FAT_12_16_boot_sector boot_sector_data;
    alt_up_sd_card_dev  *device_pointer;
    bool current_sector_modified;
    unsigned current_sector_index;
    volatile void * const base;
    volatile uint8_t * const base8;
public:
    SDCard(const char *name, volatile void * const base);
    short int alt_up_sd_card_find_first(char *directory_to_search_through, char *file_name);
    static const uint8_t CMD_READ_BLOCK = 17;
    static const uint8_t CMD_WRITE_BLOCK = 24;
    static const uint8_t MAX_FILES_OPENED = 20;
    bool initialized;
    t_file_record active_files[MAX_FILES_OPENED];
    t_find_data search_data;
};

class SDCardEx : public SDCard, ISDCardEx
{
public:
    uint16_t fopen(char *fn, bool create = false);
    SDCardEx(const char *name, volatile void * const base);
    MyFile *openFile(char *fn) { return new MyFile(fopen(fn), this); }
    bool isPresent() { return this->alt_up_sd_card_is_Present(); }
    bool isFAT16() { return this->alt_up_sd_card_is_FAT16(); }
    bool write(int sd_fileh, char c) { return this->alt_up_sd_card_write(sd_fileh, c); }
    bool fclose(int sd_fileh) { return alt_up_sd_card_fclose(sd_fileh); }
    short readFile(int fd) { return this->alt_up_sd_card_read(fd); }
    short findNext(char *fn) { return this->alt_up_sd_card_find_next(fn); }
    short findNext(char *fn, t_file_record *fr) { return this->alt_up_sd_card_find_next(fn, fr); }
    size_t getSize(int i) { return active_files[i].file_size_in_bytes; }
    ~SDCardEx() { }
};

class SDCard2
{
public:
    volatile void *base;
    volatile uint16_t *aux_status;
    volatile uint16_t *command_reg;
    volatile uint32_t *argument_reg;
    volatile uint8_t *data;
public:
    SDCard2(volatile void *base)
      :
        base(base),
        aux_status((volatile uint16_t *)((uint8_t *)base + 0x234)),
        command_reg((volatile uint16_t *)((uint8_t *)base + 0x230)),
        argument_reg((volatile uint32_t *)((uint8_t *)base + 0x22c)),
        data((volatile uint8_t *)((uint8_t *)base))
    { }

    inline void waitForInsert() { while (!(*aux_status & 0x02)) { } }
    void command(uint16_t cmd, uint32_t arg1);
    static const uint8_t READ_BLOCK = 17;
    void read(uint32_t offset) { command(READ_BLOCK, offset); }
};

class Obese
{
public:
};

class MPlayer
{
    SoundCard *const soundCard;
    mstd::vector<MyFile *> queue;
public:
    unsigned delay;
    MPlayer(SoundCard *sndCard) : soundCard(sndCard), queue(10), delay(18) { }
    void enqueue(MyFile *myFile) { queue.push_back(myFile); }
    void play();
};

#endif


