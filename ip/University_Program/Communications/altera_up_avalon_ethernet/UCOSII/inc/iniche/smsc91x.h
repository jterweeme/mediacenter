/* FILE: smsc91x.h
 * 
 * SMSC 91C111 header file
 * 
 */

#ifndef _SMSC91X_H_ 
#define _SMSC91X_H_  1

#include "s91_port.h"
#include "altera_avalon_lan91c111_iniche.h"
 
struct smsc_parms
{
   int      intnum;         /* interrupt this device uses */
   u_long   regbase;        /* base address or IO number for register banks */
   int      memory;         /* amount of buffer memory on chip */
   char     mac_addr[6];    /* MAC address */
   NET      netp;           /* !!!!!!!!!! pointer to Interniche NET struct */ 
   unshort  rev;            /* Revision of chip silicon */
   int      req_speed;      /* requested speed and duplex */
   queue    tosend;         /* packets queued for sending */
   int      sending;        /* TRUE while device is transmitting */
   PACKET   snd_pkt;        /* send packet info */
   u_char   snd_pnr;		// XXX
   u_char  *snd_data;
   int      snd_len;
   int      snd_odd; 		/* the number of odd halfword at the end */
   PACKET   rcv_pkt;        /* receive packet info */
   u_char   rcv_pnr;
   u_char  *rcv_data;
   int      rcv_len;
   int      rcv_odd;
   u_char   mask;           /* interrupt mask */
   int      dma;            /* TRUE if DMA in progress */

   /* counters */
   u_long   rx_ints;
   u_long   tx_ints;
   u_long   alloc_ints;
   u_long   total_ints;
   u_long   coll1;         /* single collisions */
   u_long   collx;         /* multiple collisions */
   u_long   rx_overrun;    /* FIFO overrun on receive */
   u_long   tx_errors;     /* all TX errors */
   u_long   rx_empty;      /* got RX int w/empty fifo */
   u_long   rx_errors;     /* error bits in rx status */
   u_long   rx_good;      /* got RX int w/empty fifo */
   u_long   lnk_chg;     /* error bits in rx status */
   u_long   mdint;         /* phy mi register 18 state changes */
   
   /* PHY managment variables */
   unshort  phyreg18;      /* recent value of PHY register 18 */
#ifdef NOT_USED
   u_char   phy_addr;      /* address slot we found PHY at */
#endif
};
typedef   struct smsc_parms * SMSC;
 
extern struct smsc_parms smsc91s[S91_DEVICES];

#ifdef NOT_USED
/* This all gets replaced with the ALTERA AVALON macros for HAL interface */

/* registers in SMSC91X chip family */

#define BSR    0xE      /* bank select - same for all backs */

/* Bank select macro: */
#define SMSC_BANK(smsc, bank)  out16(smsc->regbase +  BSR, bank)

/* Bank 0 */
#define  TCR      0x00
#define  EPHSR    0x02
#define  RCR      0x04
#define  ECR      0x06
#define  MIR      0x08
#define  RPCR     0x0A


/* Bank 1 */
#define  CR       0x00
#define  BAR      0x02
#define  IAR      0x04  /* 3 16-bit regs */
#define  GPR      0x0A
#define  CTR      0x0C


/* Bank 2 */
#define  MMUCR    0x00
#define  PNR      0x02
#define  ARR      0x03
#define  FIFO     0x04
#define  PTR      0x06
#define  DATA     0x08  /* 2 16 bit regs */
#define  IST      0x0C
#define  MSK      0x0D

/* Bank 3 */
#define  MT       0x00  /* Multicast table (4 16-bit regs) */
#define	MGMT		0x08
#define	REV      0x0A
#define	ERCV     0x0C


/* Bit definitions and masks */
#define  RCR_SOFTRESET  0x8000      /* resets the chip */	
#define  RCR_STRIP_CRC  0x0200      /* strips CRC */
#define  RCR_ENABLE     0x0100      /* IFF this is set, we can recieve packets */
#define  RCR_ALMUL      0x0004      /* receive all multicast packets */
#define  RCR_PROMISC    0x0002      /* enable promiscuous mode */

/* the normal settings for the RCR register : */
#define  RCR_DEFAULT       (RCR_STRIP_CRC | RCR_ENABLE | RCR_ALMUL)

/* PHY control for RPCR reg */
#define PHY_ANEG	0x0800


#define  TCR_PAD_ENABLE    0x0080	/* pads short packets to 64 bytes */
#define  TCR_ENABLE        0x0001	/* if this is 1, we can transmit */ 

/* the normal settings for the TCR register : */
#define  TCR_DEFAULT       (TCR_PAD_ENABLE | TCR_ENABLE)

/* MMUCR command bits */
#define  MMU_ALLOC         0x20  /* Allocate TX buffer on chip */
#define  MMU_RESET         0x40  /* reset the MMU */
#define  MMU_RELEASE       0x80  /* release a received pkt buffer */
#define  MMU_FREEPKT       0xA0  /* Release packet in PNR register */
#define  MMU_ENQUEUE	      0xC0 	/* queue packet for transmit */


#define  CTL_AUTO_RELEASE  0x0800


/* Bank 2 PTR register bits: */
#define  PTR_READ       0x2000
#define  PTR_RCV        0x8000
#define  PTR_AUTOINC    0x4000


/* SMSC interrupt mask register bits */
#define RCV_INT         0x01
#define	TX_INT	        0x02
#define	TX_EMPTY_INT	0x04	
#define	ALLOC_INT   	0x08
#define	RX_OVRN_INT     0x10
#define	EPH_INT	        0x20
#define	ERCV_INT        0x40


/* SMCS PTR register control bits */
#define  RCV         0x8000
#define  AUTOINCR    0x4000
#define  READ        0x2000
#define  ETEN        0x1000
#define  NOTEMPTY    0x0800

/*
 * Receive packets status word bits
 */
#define  RX_ALGNERR  0x8000
#define  RX_BADCRC   0x2000
#define  RX_ODDSIZE  0x1000
#define  RX_LONG     0x0800
#define  RX_SHORT    0x0400
#define  RX_MCAST    0x0001

#define  RX_ERR_MASK	(RX_ALGNERR | RX_BADCRC | RX_LONG | RX_SHORT) 


/* definitions for the MGMT (PHY) bits */
#define MII_MDO    0x0001
#define MII_MDI    0x0002
#define MII_MCLK   0x0004
#define MII_MDOE   0x0008

/* RPCR register bits */
#define RPCR_SPEED   0x2000  /* Set for 100 mb */
#define RPCR_ANEG    0x0800  /* Set for autoneg */


/* Standard IEEE PHY MII register definitions */
#define  PHY_CONTROL    0     /* control reg */
#define  PHY_STATUS     1     /* status reg */
#define  PHY_ID1        2     /* part ID */
#define  PHY_ID2        3
#define  PHY_AN_ADV     4     /* AutoNeg. Advertisment */
#define  PHY_AN_REMOTE  5     /* Remote Advertisment */
#define  PHY_OUTPUT    18     /* Status output (feedback) */
#define  PHY_MASK      19     /* Int mask for reg 18 chages */


/* bits we need in the PHY registers */
#define  CONTROL_RESET     0x8000   /* first bit in stream */
#define  CONTROL_SPEED     0x2000   /* 1 for 100, 0 for 10 */
#define  CONTROL_ANEG_EN   0x1000   /* Enable autoneg. */
#define  CONTROL_ANEG_RST  0x0200   /* Reset autoneg. */

#define  STATUS_ANEG_ACK   0x0020   /* Autoneg done (acked) */
#define  STATUS_REM_FLT    0x0010   /* remote fault */

/* LED control bits */
#define RPCR_LED_MODE_LINK      0x00
#define RPCR_LED_MODE_10MB      0x02
#define RPCR_LED_MODE_FULL      0x03
#define RPCR_LED_MODE_ACTIVITY  0x04
#define RPCR_LED_MODE_100MB     0x05
#define RPCR_LED_MODE_RX        0x06
#define RPCR_LED_MODE_TX        0x07
#define RPCR_LED_A(mode)        ((mode)<<5)
#define RPCR_LED_B(mode)        ((mode)<<2)

/* Autoneg advertisments we support */
#define  CONTROL_DUPLEX    0x0100   /* 1 for full, 0 for half */
#define  AN_ADV_100_FDX    0x0100   /* 100Mb, full duplex */
#define  AN_ADV_100_HDX    0x0080   /* 100Mb, half duplex */
#define  AN_ADV_10_FDX     0x0040   /* 10Mb, full duplex */
#define  AN_ADV_10_HDX     0x0020   /* 10Mb, half duplex */

#define  AN_ADV_CSMA       0x0001   /* CSMA capable */
#define  AN_ADV_CAPS       (AN_ADV_100_FDX | \
                            AN_ADV_100_HDX | \
                            AN_ADV_10_FDX  | \
                            AN_ADV_10_HDX  | \
                            AN_ADV_CSMA)


/* register 18 bits status */
#define  OUTPUT_DPLXDET    0x0080   /* 1=full duplex */
#define  OUTPUT_SPEDDET    0x0040   /* 1=100 Mbs */
#define  OUTPUT_LNKFAIL    0x4000   /* 1=link not up */

#endif


/* Set these bits in smsc->req_speed prior to starting driver */
#define LINKSTAT_SPEED_10           0x1
#define LINKSTAT_SPEED_100          0x2
#define LINKSTAT_DUPLEX_HALF        0x4
#define LINKSTAT_DUPLEX_FULL        0x8

#define LINKSTAT_LINKDOWN           0x8000
#define LINKSTAT_AUTO_NEGOTIATE     0x4000

/* get/set speed at phy */
int s91_setSpeed (SMSC smsc, int speed);
int s91_getSpeed (SMSC smsc);

/**************************************
 * external routines in per-port file
 **************************************/

/**
 * @brief Do port-specific device initialization
 *
 * @param lan91c111 -- the Ethernet interface structure
 * @param s91_dev -- the device index
 *
 * @return 0 for success, or -1 for error
 **/
extern int s91_port_prep(alt_avalon_lan91c111_if *lan91c111, int s91_dev);

extern   int   s91_port_init(struct smsc_parms * smsc);
extern   int   s91_port_close(struct smsc_parms * smsc);
extern   void  s91_senddata(SMSC smsc, u_char * data, int len);
extern   void  s91_rcvdata(SMSC smsc, u_char * data, int len);


/* Interniche MAC interface routines in smsc91x.c */
extern int prep_s91(alt_avalon_lan91c111_if *lan91c111, int index);

/**
 * @brief This routine is responsible for preparing the device to send and
 * receive packets. It is called during system startup time after prep_ifaces()
 * has been called, but before any of the other network interface's routines
 * are invoked. 
 *
 * @post
 *   - NET hardware ready to send and receive packets.
 *   - All required fields of the net structure are filled in.
 *   - Interface's MIB-II structure filled in as show below.
 *   - IP addressing information should be set before this returns unless DHCP or BOOTP is to be used. 
 *
 * @param iface -- the interface index
 *
 * @return 0 if successful
 **/
extern   int   s91_init(int iface);
extern   int   s91_pkt_send(PACKET);
extern   int   s91_close(int iface);
extern   void  s91_stats(void * pio, int iface);


/* C level interrupt handler */
void  s91_isr(int devnum);

/* portable utility functions */
void  s91_reset(struct smsc_parms * smsc);
void  s91_enable(struct smsc_parms * smsc);
void  s91_initsleep(void);
void  s91_usleep(u_long msecs);

/* PHY control */
unshort  s91_readphy(SMSC smsc, const unsigned phyreg);
void     s91_writephy(SMSC smsc, const unsigned phyreg, const unshort phydata);
int      s91_findphy(SMSC smsc);
int      s91_phy_init(SMSC smsc);

/* REV A silicon doesn't handle received packets with odd # of bytes
 * correctly. Find out of you have it and set the #define here!
 */

/*
#define REV_A_SILICON
*/

#endif /* _SMSC91X_H_ */
