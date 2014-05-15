#ifndef __ALTERA_UP_SD_CARD_AVALON_INTERFACE_H__
#define __ALTERA_UP_SD_CARD_AVALON_INTERFACE_H__

#include <stddef.h>
#include <alt_types.h>
#include <sys/alt_dev.h>

#ifdef __cplusplus
extern "C"
{
#endif

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

alt_up_sd_card_dev* alt_up_sd_card_open_dev(const char *name);
bool alt_up_sd_card_is_Present(void);
bool alt_up_sd_card_is_FAT16(void);
short int alt_up_sd_card_fopen(char *name, bool create);
short int alt_up_sd_card_find_first(char *directory_to_search_through, char *file_name);
short int alt_up_sd_card_find_next(char *file_name);
/* This function searches for the next file in a given directory, as specified by the find_first function.
 * Inputs:
 *		file_name - an array to store a name of the file found. Must be 13 bytes long (12 bytes for file name and 1 byte of NULL termination).
 * Outputs:
 *		-1 - end of directory.
 *		0 - success
 *		2 - No card or incorrect card format.
 *		4 - find_first has not been called successfully.
 */

void alt_up_sd_card_set_attributes(short int file_handle, short int attributes);
short int alt_up_sd_card_get_attributes(short int file_handle);
/* Return file attributes, or -1 if the file_handle is invalid.
 */


short int alt_up_sd_card_read(short int file_handle);
/* Read a single character from the given file. Return -1 if at the end of a file. Any other negative number
 * means that the file could not be read. A number between 0 and 255 is an ASCII character read from the SD Card. */


bool alt_up_sd_card_write(short int file_handle, char byte_of_data);
/* Write a single character to a given file. Return true if successful, and false otherwise. */



// This function closes an opened file and saves data to SD Card if necessary.


//////////////////////////////////////////////////////////////////////////
// file-like operation functions

//////////////////////////////////////////////////////////////////////////
// direct operation functions


/*
 * Macros used by alt_sys_init 
 */
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



#ifdef __cplusplus
}
#endif /* __cplusplus */

class MyFile
{
private:
    int fd;
public:
    MyFile(int fd) { this->fd = fd; }
};


// moet nog Singleton worden
class SDCard
{
private:
    alt_up_sd_card_dev *sd_card_dev;
    bool alt_up_sd_card_fclose(short int file_handle);
public:
    void init(const char *name) { sd_card_dev = ::alt_up_sd_card_open_dev(name); }
    bool isPresent() { ::alt_up_sd_card_is_Present(); }
    bool isFAT16() { ::alt_up_sd_card_is_FAT16(); }
    int fopen(char *fn) { ::alt_up_sd_card_fopen(fn, true); }
    MyFile *openFile(char *fn) { return new MyFile(fopen(fn)); }
    bool write(int, char);
    bool fclose(int);
    short int findNext(char *fn) { ::alt_up_sd_card_find_next(fn); }
};

#endif /* __ALTERA_UP_SD_CARD_AVALON_INTERFACE_H__ */


