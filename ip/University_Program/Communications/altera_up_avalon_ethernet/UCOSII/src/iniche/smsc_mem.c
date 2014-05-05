/*
 * Filename: smsc_mem.c
 *
 * Copyright 2003 by InterNiche Technologies Inc. All rights reserved.
 *
 * This is the Altera LAN91C111-specific portion of the 91C111 driver
 * demo port. Since this is not intended to high highly portable, it 
 * contains scraps of assembler where appropriate.
 */
#ifdef ALT_INICHE

#include "ipport.h"

#include "netbuf.h"
#include "net.h"
#include "q.h"
#include "ether.h"

#include "altera_avalon_lan91c111_regs.h"
#include "altera_avalon_lan91c111_iniche.h"

//#include "altera_up_avalon_ethernet.h"
#include "altera_up_avalon_ethernet_regs.h"
//#include "alt_up_ethernet_high_level_driver.h"
#include "alt_up_ethernet_low_level_driver.h"


#include "stdio.h"
#include "system.h"
#include "sys/alt_irq.h"
#include "sys/alt_dma.h"
#include "smsc91x.h"
#include "alt_iniche_dev.h"

/* #define STATIC_TX   1 */

void s91_isr(int);     /* only support one device for now */
void dma_rx_done(void *, void *);
void dma_tx_done(void *);

alt_dma_rxchan  dma_rx;         /* DMA channel structures */
alt_dma_txchan  dma_tx;


#ifdef DAWANG
/* FUNCTION: mac2ip()
 * 
 * Map MAC addresses into IP addresses
 * 
 * PARAMS: none
 * 
 * RETURN: unsigned long ipaddr       IP address
 */
u_long mac2ip(void)
{
	BASE_TYPE base = (BASE_TYPE)(LAN91C111_BASE + LAN91C111_LAN91C111_REGISTERS_OFFSET);
	u_long ipaddr;
	u_char mac4, mac5;

	/* read the MAC address and assign either 10.103 or 10.101 */

	IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 1);
	mac4 = IORD_ALTERA_AVALON_LAN91C111_IAR4(base);
	mac5 = IORD_ALTERA_AVALON_LAN91C111_IAR5(base);

	if ((mac4 == 0x63) && (mac5 == 0x5f))
		ipaddr = 0x0a000067;      /* 10.0.0.103 */
	else if ((mac4 == 0x63) && (mac5 == 0x55))
		ipaddr = 0x0a000065;      /* 10.0.0.101 */
	else
	{
		dtrap();
		ipaddr = 0x0a000067;      /* 10.0.0.103 */
	}

	return (ipaddr);
}
#endif


/* FUNCTION: s91_prep_port()
 * 
 * Do port-specific device initialization
 * 
 * PARAM1: s91_dev         device index; 0..n-1
 * 
 * RETURN: 0 if successful, otherwise a non-zero error code
 * 
 * Initializes the SMSC device structure with the device's
 * base address, interrupt priority, MAC address.
 * 
 * For now, only support one device.
 */

int s91_port_prep(alt_avalon_lan91c111_if *lan91c111, int s91_dev)
{
	/* 
	 * This function does:
	 * - set the regbase and interrupt number for smsc, according to SOPC system configuration
	 * - read the MAC and set it to the Ethernet chip
	 */
	SMSC smsc;

	if(s91_dev >= S91_DEVICES)
		return -1;

	smsc = &smsc91s[s91_dev];  /* get pointer to device structure */
	NET net = smsc->netp;

	//
	//XXX CHANGE DA: here we use HAL to make it adapt to any SOPC component name
	//
	dprintf("assign regbase to 0x%X\n", lan91c111->base);
	smsc->regbase = (u_long)(lan91c111->base);
	dprintf("assign IRQ number to %d\n", lan91c111->irq_num);
	smsc->intnum = lan91c111->irq_num;

	////////////////////////////////////////////////////
	// initialize the device
	unsigned int base = smsc->regbase;
	int i;

	// Power on the internal PHY
	alt_up_ethernet_reg_write(base, 0x1F, 0);

	// Software Reset of the Ethernet chip
	alt_up_ethernet_reg_write(base, 0x00, 0);
	usleep(10);
	alt_up_ethernet_reg_write(base, 0x00, 1);
	usleep(1);

#ifdef ALT_INICHE
	// Initialize the MAC Address
	// get the MAC address (from SOPC Builder configuration or use the default one)
	get_mac_addr(net, smsc->mac_addr);
	// set MAC address to the Ethernet chip
	for (i = 0; i < 6; ++i)
		alt_up_ethernet_reg_write(smsc->regbase, ALT_UP_ETHERNET_PAR, smsc->mac_addr[i]);
#else /* not ALT_INICHE */
#if 1
//TODO: DA: currently we ignore this case
	IOWR_ALTERA_AVALON_LAN91C111_IAR0(smsc->regbase, 0x00);
	IOWR_ALTERA_AVALON_LAN91C111_IAR1(smsc->regbase, 0x07);
	IOWR_ALTERA_AVALON_LAN91C111_IAR2(smsc->regbase, 0xed);
	IOWR_ALTERA_AVALON_LAN91C111_IAR3(smsc->regbase, 0x0f);
	IOWR_ALTERA_AVALON_LAN91C111_IAR4(smsc->regbase, 0x63);
	IOWR_ALTERA_AVALON_LAN91C111_IAR5(smsc->regbase, 0x5f);
#endif
	smsc->mac_addr[0] = IORD_ALTERA_AVALON_LAN91C111_IAR0(smsc->regbase);
	smsc->mac_addr[1] = IORD_ALTERA_AVALON_LAN91C111_IAR1(smsc->regbase);
	smsc->mac_addr[2] = IORD_ALTERA_AVALON_LAN91C111_IAR2(smsc->regbase);
	smsc->mac_addr[3] = IORD_ALTERA_AVALON_LAN91C111_IAR3(smsc->regbase);
	smsc->mac_addr[4] = IORD_ALTERA_AVALON_LAN91C111_IAR4(smsc->regbase);
	smsc->mac_addr[5] = IORD_ALTERA_AVALON_LAN91C111_IAR5(smsc->regbase);

#endif /* not ALT_INICHE */

	return 0;
}


#ifdef ALTERA_NIOS2

/* FUNCTION: s91_isr_wrap
 * 
 * Wrapper Function for Altera Nios-II HAL BSP's use.
 * 
 * PARAM1: context         device-specific context value
 * PARAM2: intnum          interrupt number
 * 
 * RETURN: none
 * 
 * The "context" parameter is the LAN91C111 device index: 0..n-1.
 * Interrupts are disabled on entry, so don't need to do anything
 * unless we want to reenable higher pirority interrupts for performance
 * reasons.
 */
void s91_isr_wrap(void *context, u_long intnum)
{
	//jtag_print_hex_number(0xDEEDBEEF, 4);
	
	//ENTER_S91_SECTION(0); 
	s91_isr((int)context);

    
	//EXIT_S91_SECTION(0); 
}
#endif  /* ALTERA_NIOS2 */


/* FUNCTION: s91_port_init
 * 
 * Initializes the Interrupt Service Routine for the device
 * 
 * PARAM1: smsc            SMSC structure pointer
 * 
 * RETURN: 0 if successful, otherwise a non-zero error code
 */

/**
 * @brief Initializes the Interrupt Service Routine for the device
 *
 * @param smsc  
 *
 * @return 
 **/
int s91_port_init(SMSC smsc)
{
	/* Flow:
	 * 	- If the Ethernet is not polled, register the device in HAL
	 * 	- If DMA is chosen, do some stuff (TODO: modify for DM9000A)
	 */
#ifndef SMSC_POLLED
#if 0
	BASE_TYPE base = (BASE_TYPE)smsc->regbase;
#endif
	int err = 0;

// XXX: Da: we use polling in the testing phase, so won't register the ISR)
//#if 0	
	/* register the ISR with the ALTERA HAL interface */
	dprintf("Registering ISR!\n");
	fflush(stdout);
	err = alt_irq_register(smsc->intnum, (void *)0, s91_isr_wrap);
	if (err)
		return (err);
//#endif

#endif  /* SMSC_POLLED */

#ifdef ALTERA_DMA
	// TODO - DA - ignore DMA part for now, add later
	/* create the DMA channels for reading/writing the 91C111 FIFO */

	if (((dma_tx = alt_dma_txchan_open(DMA_NAME)) == NULL) ||
			((dma_rx = alt_dma_rxchan_open(DMA_NAME)) == NULL))
		return (-ENODEV);

	/* setup channels to be 16-bits wide, memory <-> peripheral */
	err = alt_dma_rxchan_ioctl(dma_rx, ALT_DMA_SET_MODE_32, 0);
	if (err)
		return (err);
#if 0
	err = alt_dma_rxchan_ioctl(dma_rx, ALT_DMA_RX_ONLY_ON,
			(void *)(IOADDR_ALTERA_AVALON_LAN91C111_DATA(base)));
#else
	err = alt_dma_rxchan_ioctl(dma_rx, ALT_DMA_RX_ONLY_ON,
			(void *)(smsc->regbase + 8));
#endif

	if (err)
		return (err); 

	err = alt_dma_txchan_ioctl(dma_tx, ALT_DMA_SET_MODE_32, 0);
	if (err)
		return (err);
#if 0
	err = alt_dma_txchan_ioctl(dma_tx, ALT_DMA_TX_ONLY_ON,
			(void *)(IOADDR_ALTERA_AVALON_LAN91C111_DATA(base)));
#else
	err = alt_dma_txchan_ioctl(dma_tx, ALT_DMA_TX_ONLY_ON,
			(void *)(smsc->regbase + 8));
#endif
	if (err)
		return (err); 
#endif  /* ALTERA_DMA */     

	return (0);
}


/* FUNCTION: s91_port_close
 * 
 * Close the LAN91C111 device
 * 
 * PARAM1: smsc            SMSC structure pointer
 * 
 * RETURN: 0
 */
/**
 * @brief Close the DM9000A device
 *
 * @param smsc -- Ethernet device structure
 *
 * @return 0 for success, or corresponding negated error code
 **/
int s91_port_close(SMSC smsc)
{
	return (0);
}


/* FUNCTION: s91_senddata
 * 
 * Transfer data from the packet to the LAN91C111 internal memory
 * 
 * PARAM1: smsc         SMSC structure pointer
 * PARAM2: data         pointer to packet data
 * PARAM3: len          packet data length
 * 
 * RETURN: none
 * 
 * The actual length required by the LAN91C111 includes the status
 * word, length word, and control byte. There is space reserved before
 * and after the data for building the header and trailer information
 * used by the MAC.
 */

/**
 * @brief Send packet data to the internal memory of the Ethernet chip
 *
 * @param smsc -- the Ethernet device structure 
 * @param data -- the pointer to the packet data 
 * @param len -- the packet data length 
 *
 **/
void s91_senddata(SMSC smsc, unsigned char *data, int len)
{
	u_long   base = smsc->regbase;       /* device base address */
	void s91_dma_tx_done(void *);

	/* sanity check */
	if ((len < 60) || (((int)data) & 0x1))
	{
		dtrap();
	}

#ifndef ALTERA_DMA_TX
	/* wirte transmit data to chip SRAM */
	IOWR_ALTERA_UP_AVALON_ETHERNET_INDEX(base, ALT_UP_ETHERNET_MWCMD);
	int i;
	alt_u16 halfword;
	for (i = 0; i < len; i += 2)
	{
		halfword = (data[i+1]<<8) | data[i];
		IOWR_ALTERA_UP_AVALON_ETHERNET_DATA(base, halfword);
		//dprintf("0x%02X, 0x%02X, ", data[i], data[i+1]);
	}
	//dprintf("\nDONE.\n");

	smsc->snd_odd = 0;
	smsc->snd_data = data;
	// XXX: DA: We don't use the dma_tx_done but rather free the packet only
	PACKET pkt = smsc->snd_pkt;

	//////////////////////////////////////////
	// Ask the Ethernet to send the data out
	//////////////////////////////////////////
	/* issue TX packet's length into TXPLH REG. FDH & TXPLL REG. FCH */
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_TXPLH, (len>> 8) & 0xFF);
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_TXPLL,  len & 0xFF);

	/* issue TX polling command activated */
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_TCR, ALT_UP_ETHERNET_TX_REQUEST);

	// check whether the packet is transmitted (or timeout)
	int time_out = 0xff;
	while ( ((alt_up_ethernet_reg_read(base, ALT_UP_ETHERNET_NSR)&(4|8))!=1)
			&& (time_out--))
		;

	pk_free(pkt);
	// s91_dma_tx_done((void *)smsc);
#else
	// XXX: DA: we ignore this case as we don't support DMA for now
	/* disable 91C111 interrupts until DMA completes */
	IOWR_ALTERA_AVALON_LAN91C111_MSK(base, 0);

	/* do the odd half-word at the beginning by PIO */
	if (((u_long)data) & 0x02)
	{
		IOWR_ALTERA_AVALON_LAN91C111_DATA_HW(base, *(unshort *)data);
		data += 2;
		len -= 2;
	}

	alt_dma_txchan_send(dma_tx, (void *)(((u_long)data) & ~0x80000000), len & ~0x3, 
			s91_dma_tx_done, (void *)smsc);
	smsc->snd_odd = len & 0x3;
	smsc->snd_data = data + (len & ~0x3);
#endif  /* ALTERA_DMA_TX */

}


/* FUNCTION: s91_dma_tx_done
 * 
 * Callback routine when TX DMA is done
 * 
 * PARAM1: void *handle     callback parameter
 * 
 * RETURN: void
 */
void s91_dma_tx_done(void *handle)
{
	SMSC smsc = (SMSC)handle;
	BASE_TYPE base = (BASE_TYPE)smsc->regbase;
	PACKET pkt = smsc->snd_pkt;
	unshort stats;

	ENTER_S91_SECTION(smsc);

	/* get the packet number and queue the packet for transmission */
	IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 2);

	/* process the odd halfword at the end */
	switch (smsc->snd_odd)
	{
		case 3:
			IOWR_ALTERA_AVALON_LAN91C111_DATA_HW(base, *(unshort *)(smsc->snd_data));
			smsc->snd_data += 2;
			/* fall through for the last byte and control byte */

		case 1:
			IOWR_ALTERA_AVALON_LAN91C111_DATA_BYTE(base, *smsc->snd_data);
			IOWR_ALTERA_AVALON_LAN91C111_DATA_BYTE(base, 
					ALTERA_AVALON_LAN91C111_CONTROL_ODD_MSK);
			break;

		case 2:
			IOWR_ALTERA_AVALON_LAN91C111_DATA_HW(base, *(unshort *)(smsc->snd_data));
			/* fall through to write the control byte */

		case 0:
			IOWR_ALTERA_AVALON_LAN91C111_DATA_HW(base, 0);
			break;
	}

	IOWR_ALTERA_AVALON_LAN91C111_MMUCR(base, ALTERA_AVALON_LAN91C111_MMUCR_ENQUEUE_MSK);

	/* update stats from error counter reg */
	IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 0);
	stats = IORD_ALTERA_AVALON_LAN91C111_ECR(base);
	IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 2);

	/* single collisions */
	smsc->coll1 += (u_long)(stats & 0xf);
	/* multiple collisions */
	smsc->collx += (u_long)((stats >> 4) & 0xf);

	/* free the transmitted packet */
	pk_free(pkt);

	/* if there is more to transmit, allocate the next TX buffer */
	smsc->sending = FALSE;
	if(smsc->tosend.q_len > 0)  /* more packets queued to send? */
	{
		/* turn on the TX_EMPTY interrupt */
		smsc->mask |= ALTERA_AVALON_LAN91C111_INT_TX_EMPTY_INT_MSK;
		smsc->sending = TRUE;   /* start of send operation */
	}

	/* re-enable 91C111 interrupts */
	//IOWR_ALTERA_AVALON_LAN91C111_MSK(base, smsc->mask);

	EXIT_S91_SECTION(smsc);
}





/* FUNCTION: s91_rcvdata
 * 
 * Move packet data from the card into local memory
 * 
 * PARAM1: smsc            SMSC structure pointer
 * PARAM2: data            destination data pointer (even address)
 * PARAM3: len             data length (bytes)
 * 
 * RETURN: none
 * 
 * The data pointer is assumed to be 16-bit aligned. An even
 * number of bytes will be transfered to the destination memory.
 */

void s91_rcvdata(SMSC smsc, unsigned char *data, int len)
{
	u_long  base = smsc->regbase;
	void s91_dma_rx_done(void *, void *);
	volatile unshort data_in;

	smsc->rcv_len = len;
	len = (len + 1) & ~0x1;        /* round up */

	/* TODO: replace with more efficient 32-bit data move */
#ifndef ALTERA_DMA_RX
	int i;
	for (i = 0; i < len; i++)
	{
		data_in = IORD_ALTERA_UP_AVALON_ETHERNET_DATA (base);
		data[2 * i] = data_in & 0xFF;
		data[(2 * i) + 1] = (data_in >> 8) & 0xFF;
	}

	smsc->rcv_odd = 0;
	smsc->rcv_data = data;
	// below we do some post processing and put the data packet into reception queue
	s91_dma_rx_done((void *)smsc, (void *)data);
#else
	/* disable 91C111 interrupts while DMA is in progress */
	//IOWR_ALTERA_AVALON_LAN91C111_MSK(base, 0);

	/* do the odd half-word at the beginning by PIO */
	if (((u_long)data) & 0x02)
	{
		*(unshort *)data = IORD_ALTERA_AVALON_LAN91C111_DATA_HW(base);
		data += 2;
		len -= 2;
	}

	alt_dma_rxchan_prepare(dma_rx, (void *)(((u_long)data) & ~0x80000000), len & ~0x3, 
			dma_rx_done, (void *)smsc);
	smsc->rcv_odd = len & 0x3;
	smsc->rcv_data = data + (len & ~0x3);
#endif  /* ALTERA_DMA_RX */
}


/* FUNCTION: s91_dma_rx_done
 * 
 * Callback routine when RX DMA is done
 * 
 * PARAM1: void *handle     callback parameter
 * PARAM2: void *data       receive buffer pointer
 * 
 * RETURN: void
 */
	void
s91_dma_rx_done(void *handle, void *data)
{
	SMSC smsc = (SMSC)handle;
	BASE_TYPE base = (BASE_TYPE)smsc->regbase;
	PACKET pkt;
	struct ethhdr * eth;

	//ENTER_S91_SECTION(smsc);

	pkt = smsc->rcv_pkt;
	pkt->nb_prot = pkt->nb_buff + ETHHDR_SIZE;
	pkt->nb_plen = smsc->rcv_len - 14;
	pkt->nb_tstamp = cticks;
	pkt->net = smsc->netp;

	/* set packet type for demux routine */
	eth = (struct ethhdr *)(pkt->nb_buff + ETHHDR_BIAS);
	pkt->type = eth->e_type;

	/* queue the received packet */
	ENTER_CRIT_SECTION(rcvdq); 
	putq(&rcvdq, pkt);
	EXIT_CRIT_SECTION(rcvdq); 
	
	SignalPktDemux();

	/* flush the packet so the card doesn't int us again (or get stuck) */
	/*
	IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 2);
	MMUCR_WAIT(base);
	IOWR_ALTERA_AVALON_LAN91C111_MMUCR(base, 
			ALTERA_AVALON_LAN91C111_MMUCR_REMOVE_RELEASE_MSK);
	*/

	/* restore the 91C111 interrupts */
	//IOWR_ALTERA_AVALON_LAN91C111_MSK(base, smsc->mask);

	//EXIT_S91_SECTION(smsc);   
	//dprintf(" DONE RECEIVING!!!\n");
}
#endif /*ALT_INICHE*/
