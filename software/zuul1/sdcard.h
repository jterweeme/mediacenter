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

typedef struct s_file_record {
    unsigned char name[8];
    unsigned char extension[3];
    unsigned char attributes;
    unsigned short int create_time;
    unsigned short int create_date;
    unsigned short int last_access_date;
    unsigned short int last_modified_time;
    unsigned short int last_modified_date;
    unsigned short int start_cluster_index;
    unsigned int file_size_in_bytes;
    unsigned int current_cluster_index;
    unsigned int current_sector_in_cluster;
    unsigned int current_byte_position;
    unsigned int file_record_cluster;
    unsigned int file_record_sector_in_cluster;
    short int    file_record_offset;
    unsigned int home_directory_cluster;
    bool         modified;
    bool         in_use;
} t_file_record;

typedef struct s_FAT_12_16_boot_sector {
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
} t_FAT_12_16_boot_sector;

typedef struct s_find_data {
    unsigned int directory_root_cluster;
    unsigned int current_cluster_index;
    unsigned int current_sector_in_cluster;
    short int file_index_in_sector;
    bool valid;
} t_find_data;




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



class SDCardEx;

class MyFileRecord
{
private:
    t_file_record *fr;
public:
    MyFileRecord(t_file_record *fr);
    const char *toString();
};

class MyFile
{
private:
    int fd;
    SDCardEx *sd;
public:
    MyFile(int fd, SDCardEx *sd) { this->fd = fd; this->sd = sd; }
    short int read();
};


// moet nog Singleton worden
class SDCard
{
protected:
    alt_up_sd_card_dev *sd_card_dev;
    bool alt_up_sd_card_fclose(short int file_handle);
    short int alt_up_sd_card_fopen(char *name, bool create);
    short int alt_up_sd_card_find_next(char *file_name);
    short int alt_up_sd_card_find_next(char *file_name, t_file_record *fr);
    alt_up_sd_card_dev *alt_up_sd_card_open_dev(const char *name);
    bool alt_up_sd_card_is_Present();
    bool alt_up_sd_card_is_FAT16();
    bool Write_Sector_Data(int sector_index, int partition_offset);
    bool Save_Modified_Sector();
    bool Read_Sector_Data(int sector_index, int partition_offset);
    bool get_cluster_flag(unsigned int cluster_index, unsigned short int *flag);
    bool mark_cluster(unsigned int cluster_index, short int flag, bool first_fat);
    void alt_up_sd_card_set_attributes(short int file_handle, short int attributes);
    short int alt_up_sd_card_get_attributes(short int file_handle);
    short int alt_up_sd_card_read(short int file_handle);
    bool alt_up_sd_card_write(short int file_handle, char byte_of_data);
    bool Check_for_Master_Boot_Record(void);
    bool Write_File_Record_At_Offset(int offset, t_file_record *record);
    bool Look_for_FAT16();
    bool Check_for_DOS_FAT(int FAT_partition_start_sector);
    void filename_to_upper_case(char *file_name);
    bool check_file_name_for_FAT16_compliance(char *file_name);
    int get_dir_divider_location(char *name);
    bool match_file_record_to_name_ext(t_file_record *file_record, char *name, char *extension);
    bool find_first_empty_cluster(unsigned int *cluster_number);
    int find_first_empty_record_in_a_subdirectory(int start_cluster_index);
    int find_first_empty_record_in_root_directory();
    void convert_filename_to_name_extension(char *filename, char *name, char *extension);
    bool create_file(char *name, t_file_record *file_record, t_file_record *home_dir);
    void copy_file_record_name_to_string(t_file_record *file_record, char *file_name);

    bool find_file_in_directory(int directory_start_cluster, char *file_name,
                t_file_record *file_record);

    bool get_home_directory_cluster_for_file(char *file_name,
            int *home_directory_cluster, t_file_record *file_record);

    bool Read_File_Record_At_Offset(int offset, t_file_record *record,
        unsigned int cluster_index, unsigned int sector_in_cluster);
public:
    void init(const char *name) { sd_card_dev = this->alt_up_sd_card_open_dev(name); }
    bool isPresent() { this->alt_up_sd_card_is_Present(); }
    bool isFAT16() { this->alt_up_sd_card_is_FAT16(); }
    bool write(int, char);
    bool fclose(int);
    short int readFile(int fd) { this->alt_up_sd_card_read(fd); }
    short int findNext(char *fn) { this->alt_up_sd_card_find_next(fn); }
    short int findNext(char *fn, t_file_record *fr) { this->alt_up_sd_card_find_next(fn, fr); }
    short int alt_up_sd_card_find_first(char *directory_to_search_through, char *file_name);
};

class SDCardEx : public SDCard
{
public:
    int fopen(char *fn) { this->alt_up_sd_card_fopen(fn, false); }
    MyFile *openFile(char *fn) { return new MyFile(fopen(fn), this); }
};

#endif


