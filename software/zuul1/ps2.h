#ifndef _PS2_H_
#define _PS2_H_

#include <alt_types.h>
#include <io.h>
#include <errno.h>

#define PS2_ACK     (0xFA)

#define ALT_UP_PS2_PORT_DATA_REG                    0
#define IOADDR_ALT_UP_PS2_PORT_DATA_REG(base)       \
        __IO_CALC_ADDRESS_NATIVE(base, ALT_UP_PS2_PORT_DATA_REG)
#define IORD_ALT_UP_PS2_PORT_DATA_REG(base)         \
        IORD(base, ALT_UP_PS2_PORT_DATA_REG) 
#define IOWR_ALT_UP_PS2_PORT_DATA_REG(base, data)   \
        IOWR(base, ALT_UP_PS2_PORT_DATA_REG, data)

#define ALT_UP_PS2_PORT_DATA_REG_DATA_MSK           (0x000000FF)
#define ALT_UP_PS2_PORT_DATA_REG_DATA_OFST          (0)
#define ALT_UP_PS2_PORT_DATA_REG_RVALID_MSK         (0x00008000)
#define ALT_UP_PS2_PORT_DATA_REG_RVALID_OFST        (15)
#define ALT_UP_PS2_PORT_DATA_REG_RAVAIL_MSK         (0xFFFF0000)
#define ALT_UP_PS2_PORT_DATA_REG_RAVAIL_OFST        (16)

#define ALT_UP_PS2_PORT_DATA                        0
#define IOADDR_ALT_UP_PS2_PORT_DATA(base)           \
        __IO_CALC_ADDRESS_DYNAMIC(base, ALT_UP_PS2_PORT_DATA)
#define IORD_ALT_UP_PS2_PORT_DATA(base)             \
        IORD_8DIRECT(base, ALT_UP_PS2_PORT_DATA) 
#define IOWR_ALT_UP_PS2_PORT_DATA(base, data)       \
        IOWR_8DIRECT(base, ALT_UP_PS2_PORT_DATA, data)

#define ALT_UP_PS2_PORT_DATA_MSK                    (0x000000FF)
#define ALT_UP_PS2_PORT_DATA_OFST                   (0)

#define ALT_UP_PS2_PORT_RVALID                      1
#define IOADDR_ALT_UP_PS2_PORT_RVALID(base)         \
        __IO_CALC_ADDRESS_DYNAMIC(base, ALT_UP_PS2_PORT_RVALID)
#define IORD_ALT_UP_PS2_PORT_RVALID(base)           \
        IORD_8DIRECT(base, ALT_UP_PS2_PORT_RVALID) 
#define IOWR_ALT_UP_PS2_PORT_RVALID(base, data)     \
        IOWR_8DIRECT(base, ALT_UP_PS2_PORT_RVALID, data)

#define ALT_UP_PS2_PORT_RVALID_MSK                  (0x00000080)
#define ALT_UP_PS2_PORT_RVALID_OFST                 (7)

#define ALT_UP_PS2_PORT_RAVAIL                      2
#define IOADDR_ALT_UP_PS2_PORT_RAVAIL(base)         \
        __IO_CALC_ADDRESS_DYNAMIC(base, ALT_UP_PS2_PORT_RAVAIL)
#define IORD_ALT_UP_PS2_PORT_RAVAIL(base)           \
        IORD_16DIRECT(base, ALT_UP_PS2_PORT_RAVAIL) 
#define IOWR_ALT_UP_PS2_PORT_RAVAIL(base, data)     \
        IOWR_16DIRECT(base, ALT_UP_PS2_PORT_RAVAIL, data)

#define ALT_UP_PS2_PORT_RAVAIL_MSK                  (0x0000FFFF)
#define ALT_UP_PS2_PORT_RAVAIL_OFST                 (0)

#define ALT_UP_PS2_PORT_CTRL_REG                    1
#define IOADDR_ALT_UP_PS2_PORT_CTRL_REG(base)       \
        __IO_CALC_ADDRESS_NATIVE(base, ALT_UP_PS2_PORT_CTRL_REG)
#define IORD_ALT_UP_PS2_PORT_CTRL_REG(base)         \
        IORD(base, ALT_UP_PS2_PORT_CTRL_REG)
#define IOWR_ALT_UP_PS2_PORT_CTRL_REG(base, data)   \
        IOWR(base, ALT_UP_PS2_PORT_CTRL_REG, data)

#define ALT_UP_PS2_PORT_CTRL_REG_RE_MSK             (0x00000001)
#define ALT_UP_PS2_PORT_CTRL_REG_RE_OFST            (0)
#define ALT_UP_PS2_PORT_CTRL_REG_RI_MSK             (0x00000100)
#define ALT_UP_PS2_PORT_CTRL_REG_RI_OFST            (8)
#define ALT_UP_PS2_PORT_CTRL_REG_CE_MSK             (0x00000400)
#define ALT_UP_PS2_PORT_CTRL_REG_CE_OFST            (10)

typedef enum {
    /// @brief Indicate that the device is a PS/2 Mouse.
    PS2_MOUSE = 0,
    /// @brief Indicate that the device is a PS/2 Keyboard.
    PS2_KEYBOARD = 1,
    /// @brief The program cannot determine what type the device is.
    PS2_UNKNOWN = 2
} PS2_DEVICE;

typedef struct alt_up_ps2_dev {
    /// @brief character mode device structure. 
    /// @sa Developing Device Drivers for the HAL in Nios II Software Developer's Handbook.
    alt_dev dev;
    /// @brief the base address of the device.
    unsigned int base;
    /// @brief the interrupt id of the device.
    unsigned int irq_id;
    /// @brief the default timeout value.
    unsigned int timeout;
    /// @brief the device type (PS/2 Mouse or PS/2 Keyboard).
    PS2_DEVICE device_type;
} alt_up_ps2_dev;

void alt_up_ps2_init(alt_up_ps2_dev *ps2);

/**
 * @brief Enable read interrupts for the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 *
 * @return nothing
 **/
void alt_up_ps2_enable_read_interrupt(alt_up_ps2_dev *ps2);

/**
 * @brief Diaable read interrupts for the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 *
 * @return nothing
 **/
void alt_up_ps2_disable_read_interrupt(alt_up_ps2_dev *ps2);

/**
 * @brief Write a byte to the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 * @param byte -- the byte to be written to the PS/2 port.
 *
 * @return 0 on success, or \c -EIO on failure.
 **/
int alt_up_ps2_write_data_byte(alt_up_ps2_dev *ps2, unsigned char byte);

int alt_up_ps2_write_data_byte_with_ack(alt_up_ps2_dev *ps2, unsigned char byte);

/**
 * @brief Read a byte from the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 * @param byte -- pointer to the memory location to store the byte.
 *
 * @return 0 on success, else -1
 *
 **/
int alt_up_ps2_read_data_byte(alt_up_ps2_dev *ps2, unsigned char *byte);

int alt_up_ps2_read_data_byte_timeout(alt_up_ps2_dev *ps2, unsigned char *byte);

/**
 * @brief Clear the FIFO for the PS/2 port.
 *
 * @param ps2 -- the PS/2 device structure.
 *
 **/
void alt_up_ps2_clear_fifo(alt_up_ps2_dev *ps2);

//////////////////////////////////////////////////////////////////////////
// file-like operation functions
/**
 * @brief Read \em len bytes from the PS/2 device.
 *
 * @param fd -- the file descriptor for the PS/2 device.
 * @param ptr -- memory location to store the bytes read.
 * @param len -- number of bytes to be read.
 *
 * @return the number of bytes actually read.
 *
 **/
int alt_up_ps2_read_fd (alt_fd* fd, char* ptr, int len);

int alt_up_ps2_write_fd (alt_fd* fd, const char* ptr, int len);


//////////////////////////////////////////////////////////////////////////
// direct operation functions

/**
 * @brief Open a PS/2 device structure with \em name in SOPC Builder
 *
 * @param name the specified name of the device in SOPC Builder 
 *
 * @return the PS/2 device structure
 **/
alt_up_ps2_dev* alt_up_ps2_open_dev(const char* name);

#define ALTERA_UP_AVALON_PS2_INSTANCE(name, device) \
  static alt_up_ps2_dev device =        \
  {                                                 \
    {                                               \
      ALT_LLIST_ENTRY,                              \
      name##_NAME,                                  \
      NULL , /* open */                             \
      NULL , /* close */                            \
      alt_up_ps2_read_fd , /* read */               \
      alt_up_ps2_write_fd , /* write */             \
      NULL , /* lseek */                            \
      NULL , /* fstat */                            \
      NULL , /* ioctl */                            \
    },                                              \
    name##_BASE,                                    \
    name##_IRQ,                                     \
    0x0001ffff,                                     \
    PS2_UNKNOWN                                     \
  }

#define ALTERA_UP_AVALON_PS2_INIT(name, device)  \
  {                                                         \
    alt_up_ps2_init(&device);             \
    /* make the device available to the system */           \
    alt_dev_reg(&device.dev);                               \
  }


#endif


