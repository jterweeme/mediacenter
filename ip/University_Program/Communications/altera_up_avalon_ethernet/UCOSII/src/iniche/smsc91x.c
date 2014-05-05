/*
 * FILENAME: smsc91x.c
 *
 * Copyright  2002 By InterNiche Technologies Inc. All rights reserved
 *
 *
 *  This file contains the portable portions fo the Interniche SMSC91c111
 * ethernet chip family driver.
 * 
 *
 * MODULE: smsc91x
 *
 * ROUTINES: prep_s91(), s91_init(), s91_close(),
 * ROUTINES: s91_pkt_send(), s91_low_send(), s91_stats(), s91_rcv(), 
 * ROUTINES: s91_isr(), s91_reset(), s91_enable()
 *
 * PORTABLE: no
 */

#ifdef ALT_INICHE

#include "ipport.h"
#include "in_utils.h"
#include "netbuf.h"
#include "net.h"
#include "q.h"
#include "ether.h"
#include "altera_avalon_lan91c111_regs.h"
#include "altera_avalon_lan91c111_iniche.h"
#include "smsc91x.h"
#include "unistd.h"

#include "alt_up_ethernet_low_level_driver.h"
//#include "altera_up_avalon_ethernet.h"
#include "altera_up_avalon_ethernet_regs.h"

/* #define STATIC_TX   1 */

int SMSC_UP = FALSE;

struct smsc_parms smsc91s[S91_DEVICES];


#ifdef ALT_INICHE

#include <errno.h>
#include "alt_iniche_dev.h"

error_t alt_avalon_lan91c111_init(alt_avalon_lan91c111_if *lan91c111, alt_iniche_dev *p_dev)
{
	extern int prep_s91(alt_avalon_lan91c111_if *lan91c111, int index);

	prep_s91(lan91c111, p_dev->if_num);

	return (0);
}

#endif /* ALT_INICHE */


/* FUNCTION: prep_s91()
 * 
 * PARAM1: int index
 *
 * RETURNS: 
 */
/*-- Da:
 * Initialization function
 */

int prep_s91(alt_avalon_lan91c111_if *lan91c111, int index)
{
	int      i;
	SMSC  smsc;
	/////////////////////////////////////////////////////////////////////////////
	// TODO: THIS ifp is the gateway to the OS!!!!!!!!
	/////////////////////////////////////////////////////////////////////////////
	NET ifp;

	dprintf("lan91c111 base = 0x%X.\n", lan91c111->base);
	alt_iniche_dev iniche = lan91c111->dev;
	dprintf("Iniche Device Name = %s.\n", iniche.name);

	for (i = 0; i < S91_DEVICES; i++)
	{
		smsc = &smsc91s[i];  /* get pointer to device structure */

		/* Call the per-port hardware setup. 
		 * Speed settings should be
		 * set in smsc->req_speed by the application prior to starting
		 * the driver, else it will default to autoneg.
		 */
		//XXX: Done for this one
		s91_port_prep(lan91c111, i);    /* set up device parameters (base address, intnum, MAC address) */
		dprintf("Checking device[%d].\n", i);
		dprintf("Device base = 0x%X.\n", smsc->regbase);

		// Read the Vendor ID to make sure it is the Ethernet we support
		alt_u16 vendor_id = 0;
		vendor_id = (alt_up_ethernet_reg_read(smsc->regbase, ALT_UP_ETHERNET_VID) & 0xFF);
		vendor_id |= (((alt_up_ethernet_reg_read(smsc->regbase, ALT_UP_ETHERNET_VID+1)) & 0xFF ) <<8);
		dprintf("checking vendor ID...Read result = 0x%04X.\n", vendor_id);
		if ( vendor_id != 0x0A46) // for DM9000A, the vendor ID is 0x0A46
		{
			dtrap();      /* programing or hardware setup error? */
			continue;
		}

		// this part should go into n_init function
		// set up the NET structure (which is used by the nichestack)
		ifp = nets[index];
		// MIB part, which is required to be done when return from n_init function
		ifp->n_mib->ifAdminStatus = 2;   /* status = down */
		ifp->n_mib->ifOperStatus = 2;    /* will be set up in init()  */
		ifp->n_mib->ifLastChange = cticks * (100/TPS);
		ifp->n_mib->ifPhysAddress = (u_char*)smsc->mac_addr;
		ifp->n_mib->ifDescr = (u_char*)"DM9000A series ethernet";
		ifp->n_lnh = ETHHDR_SIZE;        /* ethernet header size */
		ifp->n_hal = 6;                  /* hardware address length */
		ifp->n_mib->ifType = ETHERNET;   /* device type */
		if (ifp->n_mib->ifType == ETHERNET)
			dprintf("ifType = Ethernet(%d)\n", ifp->n_mib->ifType);
		else
			dprintf("ifType != Ethernet(%d)!!!!!!\n", ifp->n_mib->ifType);
					
		ifp->n_mtu = MTU;                /* max frame size */

		/* install our hardware driver routines */
		ifp->n_init = s91_init;
		ifp->pkt_send = s91_pkt_send;
		ifp->n_close = s91_close;
		ifp->n_stats = s91_stats;

#ifdef IP_V6
		ifp->n_flags |= (NF_NBPROT | NF_IPV6);
#else
		ifp->n_flags |= NF_NBPROT;
#endif

		// XXX: below should be redundant
		// nets[index]->n_mib->ifPhysAddress = (u_char*)smsc->mac_addr;   /* ptr to MAC address */

		/* set cross-pointers between iface and smsc structs */
		smsc->netp = ifp;
		ifp->n_local = (void*)smsc;

		index++;
	}

	return index;
}


/* FUNCTION: s91_init()
 * 
 * Initialize a LAN91C111 device
 * 
 * PARAM1: int iface
 *
 * RETURNS: 0 if successful
 */

/**
 * @brief This routine is responsible for preparing the device to send and receive packets. It is called during system startup time after prep_ifaces() has been called, but before any of the other network interface's routines are invoked. 
 *
 * @post
    - Net hardware ready to send and receive packets.
    - All required fields of the net structure are filled in.
    - Interface's MIB-II structure filled in as show below.
    - IP addressing information should be set before this returns unless DHCP or BOOTP is to be used. 
 *
 * @param iface -- the interface index
 *
 * @return 0 if successful
 * @todo deal with the IP address
 **/
int s91_init(int iface)
{
	SMSC     smsc;

	/* get pointer to device structure */
	smsc = (SMSC)(nets[iface]->n_local);
	//BASE_TYPE base = (BASE_TYPE)smsc->regbase;

	s91_reset(smsc);

	// Read chip's Product ID 
	alt_u16 product_id = 0;
	product_id = (alt_up_ethernet_reg_read(smsc->regbase, ALT_UP_ETHERNET_PID) & 0xFF);
	product_id |= (((alt_up_ethernet_reg_read(smsc->regbase, ALT_UP_ETHERNET_PID+1)) & 0xFF ) <<8);
	
	// currently we only get the last byte of the product_id to comply with the old interface
	// TODO: change rev type to make it contain 2 bytes
	dprintf("DM9000A Ethernet PRODUCT_ID: 0x%04X\n", product_id);
	smsc->rev = product_id & 0xFF;

	// Enable necessary interrupts 
	s91_enable(smsc);       // prepare for use 

	// register ISR in HAL and prepare for DMA in HAL
	// TODO: add DMA support for DM9000A
	s91_port_init(smsc);    

	// XXX: seems for DM9000A, no PHY init is needed...
	// s91_phy_init(smsc);

	dprintf("DM9000A Ethernet MAC:");
	int i;
	for (i = 0; i < 6; i++) 
		dprintf(" %02X", smsc->mac_addr[i]);
	dprintf("\n");

	nets[iface]->n_mib->ifAdminStatus = 1;    /* status = UP */
	nets[iface]->n_mib->ifOperStatus = 1;

	// Now the NIC is activated and should be ready for RX/TX
	SMSC_UP = TRUE;

	return (0);
}




/* FUNCTION: s91_close()
 * 
 * PARAM1: int iface
 *
 * RETURNS: 
 */

/**
 * @brief Close the selected network interface
 *
 * @param iface -- the selected network interface number
 *
 * @return 0 for success
 **/
int s91_close(int iface)
{
	SMSC  smsc;

	nets[iface]->n_mib->ifAdminStatus = 2;    /* status = down */

	/* get pointer to device structure */
	smsc = (SMSC)nets[iface]->n_local;

	s91_port_close(smsc);   /* release the ISR */
	s91_reset(smsc);        /* reset the chip */

	nets[iface]->n_mib->ifOperStatus = 2;     /* status = down */
	return 0;
}


/* FUNCTION: s91_stats()
 * 
 * Display Ethernet controller statistics
 * 
 * PARAM1: void * pio
 * PARAM2: int iface
 *
 * RETURNS: void
 */

/**
 * @brief Display Ethernet controller statistics
 *
 * @param pio -- pointer to a Generic I/O Structure
 * @param iface -- interface number to dump statistics for 
 *
 * @return 
 **/
void s91_stats(void * pio, int iface)
{
	SMSC  smsc;

	smsc = (SMSC)(nets[iface]->n_local);

	ns_printf(pio, "Interrupts: rx:%lu, tx:%lu alloc:%lu, total:%lu\n",
			smsc->rx_ints, smsc->tx_ints, smsc->alloc_ints, smsc->total_ints);
	ns_printf(pio, "coll1:%lu collx:%lu overrun:%lu mdint:%lu\n",
			smsc->coll1, smsc->collx, smsc->rx_overrun, smsc->mdint);
	ns_printf(pio, "Sendq max:%d, current %d. IObase: 0x%lx ISR %d\n", 
			smsc->tosend.q_max, smsc->tosend.q_len, smsc->regbase, smsc->intnum);

	return;
}


/* FUNCTION: s91_pkt_send()
 * 
 * The Interniche MAC packet send routine
 * This function puts the packet on the device's 'tosend' queue.
 * If the device is not currently sending, it starts the sending
 * process by requesting FIFO space.
 * 
 * PARAM1: PACKET pkt
 *
 * RETURNS: 0 if successful, otherwise an error code
 */

/**
 * @brief This routine is responsible for sending the data described by the
 * passed pkt parameter and queuing the pkt parameter for later release by the
 * device driver. If the MAC hardware is idle the actual transmission of the
 * packet should be started by this routine, else it should be scheduled to be
 * sent later (usually by an "end of transmit" interrupt (EOT) from the
 * hardware).
 *
 * @param pkt  
 *
 * @return 
 **/
int s91_pkt_send(PACKET pkt)
{
	// FIXME: DA: we probably use a quite different approach here for DM9000A
	// as the hardware structure is quite different
	extern void s91_low_send(SMSC smsc);

	SMSC smsc = (SMSC)pkt->net->n_local;

	///*
   	dprintf("DEBUG: Total # interrupt =  %d, RX ones = %d, TX ones = %d\n", smsc->total_ints, smsc->rx_ints, smsc->tx_ints);
	dprintf("DEBUG: Total # rx overrun: %d\n", smsc->rx_overrun);
	dprintf("DEBUG: # TX erros = %d\n", smsc->tx_errors);
	dprintf("DEBUG: # Linnk change = %d\n", smsc->lnk_chg);
	dprintf("DEBUG: RX EMPTY = %d, RX GOOD = %d. RX ERROR= %d\n", smsc->rx_empty, smsc->rx_good, smsc->rx_errors);
	//*/

	//ENTER_S91_SECTION(smsc);
	
	putq(&smsc->tosend, (qp)pkt);    /* new packet goes in the back */
	if (smsc->tosend.q_len >= 1)
		s91_low_send(smsc);
	else
		smsc->sending = FALSE;

	//EXIT_S91_SECTION(smsc);
	// XXX: the method below uses interrupt, we will try it later
#if 0
	ENTER_S91_SECTION(smsc);

	/* Add this packet to the queue, issue the TX memory alloc command, and
	 * return. The memory alloc interrupt will take care of the send.
	 */

	putq(&smsc->tosend, (qp)pkt);    /* new packet goes in the back */

	/* If this is the only packet, start the transmit sequence */
	if (smsc->tosend.q_len == 1)
	{
		/* enable the TX_EMPTY interrupt to start the send 
		 * unless we are in the middle of a DMA */
		smsc->mask |= ALTERA_AVALON_LAN91C111_INT_TX_EMPTY_INT_MSK;
		IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 2);
		mask = IORD_ALTERA_AVALON_LAN91C111_MSK(base);
		if (mask != 0)
		{
			IOWR_ALTERA_AVALON_LAN91C111_MSK(base, smsc->mask);
		}
		smsc->sending = TRUE;   /* start of send operation */
	}

	EXIT_S91_SECTION(smsc);
#endif

	return (0);      /* alloc done interrupt will start xmit */
}


/* FUNCTION: s91_low_send()
 * 
 * This is called from the ISR when a TX buffer ALLOC command has 
 * completed or the TX buffer is EMPTY. It moves the next packet of
 * data to be sent into the SMSC device and does related transmit setup.
 * 
 * PARAM1: SMSC smsc
 *
 * RETURNS: 
 */

/**
 * @brief Low level send data function
 *
 * @param smsc -- the Ethernet device structure 
 *
 **/
void s91_low_send(SMSC smsc)
{
	PACKET    pkt;     /* Interniche pkt structure */
	unsigned  sendlen; /* length to send - may be padded */

  	if ((pkt = (PACKET)getq(&smsc->tosend)) == NULL)
	{
		dtrap();    /* no packet to send? */
		smsc->sending = FALSE;
		return;
	}
	smsc->snd_pkt = pkt;

	/* update packet statistics */
	smsc->netp->n_mib->ifOutOctets += (u_long)pkt->nb_plen;

	if(*pkt->nb_prot & 0x80)
		smsc->netp->n_mib->ifOutNUcastPkts++;
	else
		smsc->netp->n_mib->ifOutUcastPkts++;

	sendlen = pkt->nb_plen - ETHHDR_BIAS;

	if (sendlen < 60) /* pad to minimum length */
		sendlen = 60;

	s91_senddata(smsc, (u_char *)(pkt->nb_prot + ETHHDR_BIAS), sendlen);
}



/* FUNCTION: s91_rcv()
 * 
 * Process received packet from within ISR
 * 
 * PARAM1: smsc            SMSC structure pointer
 *
 * RETURNS: none
 * 
 * Note: Bank 2 is selected by the ISR prior to calling s91_rcv(). 
 */

/**
 * @brief Receive packet from Ethernet and put it into the queue
 *
 * @param smsc -- Ethernet device structure 
 *
 * @return 
 **/
void s91_rcv(SMSC smsc)
{
	// unshort   pkt_num;    /* chip's handle for received pkt */
	BASE_TYPE base = (BASE_TYPE)smsc->regbase;
	volatile unshort   pkstatus;   /* status word for received pkt */
	volatile unshort   pklen;      /* length of received pkt */
	PACKET    pkt;
	//dprintf("RECEIVING!!!!!!!!!!!!!!\n");

	volatile unshort saved_idx = IORD_ALTERA_UP_AVALON_ETHERNET_INDEX(base);
	volatile unshort data_in;
	volatile unshort RX_ready = alt_up_ethernet_reg_read(base, ALT_UP_ETHERNET_MRCMDX);
	RX_ready = IORD_ALTERA_UP_AVALON_ETHERNET_DATA(base);

	usleep(10);

	if ( (RX_ready) &  0x01)
	{
		/* Initial read with address auto-increment */
		IOWR_ALTERA_UP_AVALON_ETHERNET_INDEX (base, ALT_UP_ETHERNET_MRCMD);

		data_in = IORD_ALTERA_UP_AVALON_ETHERNET_DATA(base);
		// ignore CRC for now
		pklen = IORD_ALTERA_UP_AVALON_ETHERNET_DATA(base)-4;
		pkstatus = (data_in >> 8) & 0xFF;

		/* TODO: flush the error packet, if necessary */

		if ((pkt = pk_alloc(pklen + ETHHDR_BIAS)) == NULL)   /* couldn't get a free buffer for rx */
		{
			smsc->netp->n_mib->ifInDiscards++;
			//dprintf("Cannot get free buffer!!!!!!!!\n");
			//XXX: TODO: discard the packet from the hardware

			/* flush the packet so the card doesn't int us again (or get stuck) */

			IOWR_ALTERA_UP_AVALON_ETHERNET_INDEX(base, saved_idx);
			return;
		}
		smsc->rcv_pkt = pkt;

		/* move the received packet into the buffer. We align it so that the
		 * data is front aligned if ETHHDR_SIZE is 14, and start 2 bytes
		 * into the buffer (nb_buff) if it's 16.
		 */
		s91_rcvdata(smsc, (u_char *)(pkt->nb_buff + (ETHHDR_BIAS)), pklen);

		//read out the 4 byte CRC
		data_in = IORD_ALTERA_UP_AVALON_ETHERNET_DATA(base);
		data_in = IORD_ALTERA_UP_AVALON_ETHERNET_DATA(base);

		smsc->netp->n_mib->ifInOctets += (u_long)pklen;
		smsc->rx_good++;

		IOWR_ALTERA_UP_AVALON_ETHERNET_INDEX(base, saved_idx);
		return;
		//return 0;
	}
	else if (RX_ready & 0x02)
	{
		// Reset Ethernet Chip to recover from unstable state
		smsc->rx_errors++;
	}
	else
	{
		//return -EIO;
		smsc->rx_empty++;
	}
	IOWR_ALTERA_UP_AVALON_ETHERNET_INDEX(base, saved_idx);
	return;

}



/* FUNCTION: s91_isr()
 * 
 * LAN 91C111 Interrupt Service Routine
 * 
 * PARAM1: int devnum
 *
 * RETURNS: none
 */
void s91_isr(int devnum)
{
	SMSC      smsc;
	BASE_TYPE base;		/* base address for IO */
	volatile u_char    status, isr, nsr;  /* current interrupt status word at device */

	smsc = &smsc91s[devnum];
	base = (BASE_TYPE) smsc->regbase;

	volatile unshort saved_idx = IORD_ALTERA_UP_AVALON_ETHERNET_INDEX(base);
	
	smsc->total_ints++;

	/* get the current interrupt status */
	status = alt_up_ethernet_reg_read(base, ALT_UP_ETHERNET_ISR);

	write_leds(status); 

	/* receive has priority over transmit */
	if (status & ALT_UP_ETHERNET_ISR_PR_MSK)
	{
		// write 1 to clear the latched interrupt
		alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_ISR, ALT_UP_ETHERNET_ISR_PR_MSK);
		isr = alt_up_ethernet_reg_read(base, ALT_UP_ETHERNET_ISR);
		if (isr & ALT_UP_ETHERNET_ISR_PR_MSK)
		{
			//dprintf("CRAP!\n");
			dtrap();
		}
		smsc->rx_ints++;
		s91_rcv(smsc);
	}
	else if (status & ALT_UP_ETHERNET_ISR_PT_MSK)
	{
		smsc->tx_ints++;
		//write 1 to clear the latched "Transmitted" interrupt
		alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_ISR, ALT_UP_ETHERNET_ISR_PT_MSK);
		/*
		if (smsc->tosend.q_len >= 1)
			s91_low_send(smsc);
		*/
	}
	/* count receive overflows */
	if (status & ALT_UP_ETHERNET_ISR_ROS_MSK)
	{
		smsc->rx_overrun++;

		alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_ISR, ALT_UP_ETHERNET_ISR_ROS_MSK);
	}
	if (status & ALT_UP_ETHERNET_ISR_ROO_MSK)
	{
		smsc->rx_overrun++;

		alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_ISR, ALT_UP_ETHERNET_ISR_ROO_MSK);
	}
	if (status & ALT_UP_ETHERNET_ISR_LNKCHG_MSK)
	{
		smsc->lnk_chg++;
		nsr = alt_up_ethernet_reg_read(base, ALT_UP_ETHERNET_NSR);
		/*
		if (nsr & 0x40)
			smsc->netp->n_mib->ifOperStatus = NI_UP;
		else
			smsc->netp->n_mib->ifOperStatus = NI_DOWN;
		*/

		//IOWR_ALTERA_AVALON_LAN91C111_ACK(base, ALTERA_AVALON_LAN91C111_INT_RX_OVRN_INT_MSK);
		alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_ISR, ALT_UP_ETHERNET_ISR_LNKCHG_MSK);
	}

	/* count MD status changes */
	/* TODO: what does this do?
	if (status & ALTERA_AVALON_LAN91C111_INT_MDINT_MSK)
	{
		smsc->mdint++;

		// update the PHY status register info 
		smsc->phyreg18 = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_INT_STATUS_REG);      

		alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_ISR, ALT_UP_ETHERNET_ISR_ROS_MSK);
		IOWR_ALTERA_AVALON_LAN91C111_ACK(base, 
				ALTERA_AVALON_LAN91C111_INT_MDINT_MSK);
	}
	*/
	IOWR_ALTERA_UP_AVALON_ETHERNET_INDEX(base, saved_idx);

}


/* FUNCTION: s91_reset()
 * 
 * s91_reset() - resets the chip and leaves it in an idle state
 *
 * PARAM1: smsc            SMSC structure pointer
 *
 * RETURNS: none
 */

/**
 * @brief Reset the DM9000A chip and leaves it in an idle state
 *
 * @param smsc -- the Ethernet chip structure 
 *
 **/
void s91_reset(SMSC smsc) 
{
	/* This function:
	 * XXX: DONE
	 * - Power up the PHY
	 * - reset the Ethernet chip
	 * - clear Network Status and Interrupt Status registers (NSR and ISR)
	 * - disable all interrupts by setting Interrupt Mask Register (IMR)
	 */
	BASE_TYPE base;
	PACKET    pkt;     /* Interniche pkt structure */

	base = (BASE_TYPE)smsc->regbase;

	// Power-up the PHY by writing 0 to bit 0 of the GPR (General Purpose Register)
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_GPR, 0x00);

	// Software reset
	// (writing 0x01 will disable wakeup function and set loopback to normal mode, which are desired)
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_NCR, 0x01);
	// need 10us for auto clear
	usleep(10); 
	// clear RESET bit
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_NCR, 0x00);
	// set the normal mode
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_NCR, 0x00);

	// Clear the NSR (Network Status Register) and ISR (Interrupt Status Register)
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_NSR, 0xff); // read or write 1 to clear bits in the registers
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_ISR, 0xff); // read or write 1 to clear bits in the registers

	// wait awhile for things to settle 
	usleep(5000);

	// disable all interrupts by writing to IMR (Interrupt Mask Register)
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_IMR, 0x80);
	smsc->mask = 0;

	// free all possible packets in queue
	while ((pkt = (PACKET)getq(&smsc->tosend)))
	{
		pk_free(pkt);
	}
}


/**
 * @brief Enable the interrupt and receiving function of the Ethernet device
 *
 * @param smsc -- the Ethernet device structure
 *
 **/
void s91_enable(SMSC smsc) 
{
	/*
	 * Flow
	 * 	- enable Point Auto Return function
	 * 	- enable packet RX and TX interrupt
	 * 	- enable RX and some other options
	 */
	BASE_TYPE base = (BASE_TYPE) smsc->regbase;

	/* enable interrupts to activate DM9000A */

	// clear all interrupts
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_ISR, 0xFF);   

	// Enable Point Auto Return function by setting bit 7 of IMR to 1
	// Enable Packet RX and TX interrupt by setting bit 0 and 1 of IMR to 1
	// TODO: use MSK in _regs file to do made it more readable
	//alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_IMR, 0x8F);   
	alt_up_ethernet_reg_write(base, ALT_UP_ETHERNET_IMR, 0x83);   

	//alt_u8 status = alt_up_ethernet_reg_read(base, ALT_UP_ETHERNET_ISR);

	// enable RX and some other options (TODO: use better MSK)
	alt_up_ethernet_reg_write(base, 
		ALT_UP_ETHERNET_RCR, 
		0x30 | 0x01);  

}

#endif /* ALT_INICHE */
