/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2003-2005 Altera Corporation, San Jose, California, USA.      *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
*                                                                             *
* altera_avalon_lan91c111.c - LWIP ethernet interface for the the Lan91C111   *
* on the Nios boards.                                                         *
*                                                                             *
* Author PRR                                                                  *
*                                                                             *
******************************************************************************/

#ifdef LWIP

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "alt_types.h"
#include "altera_avalon_lan91c111_regs.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "netif/etharp.h"
#include "system.h"
#include "arch/perf.h"
#include "sys/alt_irq.h"
#include "altera_avalon_lan91c111.h"

/* Unique descricption of the network interface. */
#define IFNAME0 'n'
#define IFNAME1 '9'

extern sys_mbox_t  rx_mbox;

/* 
* Prototypes
*/
static err_t low_level_init(alt_avalon_lan91c111_if* dev);
static struct pbuf* low_level_input(alt_avalon_lan91c111_if *dev);
static err_t low_level_output(struct netif *netif, struct pbuf *p);
static err_t alt_avalon_lan91c111_output(struct netif *netif, struct pbuf *p,
      struct ip_addr *ipaddr);

/*-----------------------------------------------------------------------------------* 
 *
 * alt_avalon_lan91c111_input():
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 *
 * 
 *-----------------------------------------------------------------------------------*/
static void alt_avalon_lan91c111_input(alt_avalon_lan91c111_if *dev)
{
  struct eth_hdr  *ethhdr;
  struct pbuf     *p;
  struct netif*   netif = dev->lwip_dev_list.dev.netif;

  /* move received packet into a new pbuf */
  p = low_level_input(dev);

  /* no packet could be read, silently ignore this */
  if (p == NULL) return;

  ethhdr = p->payload;

  switch (htons(ethhdr->type)) {
  /* IP packet? */
  case ETHTYPE_IP:
    /* 
    * update ARP table 
    * 
    * The code which updates the ARP tables does not appear to be thread safe
    * so I've added a MUTEX around all calls to the arp code
    * 
    */
    sys_sem_wait(dev->arp_semaphore);
    etharp_ip_input(netif, p);
    sys_sem_signal(dev->arp_semaphore);
    /* skip Ethernet header */
    pbuf_header(p, -(s16_t)sizeof(struct eth_hdr));
    /* pass to network layer */
    netif->input(p, netif);
    break;
      
    case ETHTYPE_ARP:
      /* 
      * pass p to ARP module  
      * 
      * The code which updates the ARP tables does not appear to be thread safe
      * so I've added a MUTEX around all calls to the arp code
      * 
      */
      sys_sem_wait(dev->arp_semaphore);
      etharp_arp_input(netif, (struct eth_addr *)&netif->hwaddr, p);
      sys_sem_signal(dev->arp_semaphore);
      break;
    default:
      pbuf_free(p);
      p = NULL;
      break;
  }
}

/*-----------------------------------------------------------------------------------*
 *
 * alt_avalon_lan91c111_init():
 *
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * 
 *-----------------------------------------------------------------------------------*/
err_t alt_avalon_lan91c111_init(struct netif *netif)
{
  err_t ret_code;
  alt_avalon_lan91c111_if* dev = (alt_avalon_lan91c111_if*)netif->state;
  dev->lwip_dev_list.dev.netif = netif;

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("alt_avalon_lan91c111_init()\n"));
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  netif->output = alt_avalon_lan91c111_output;
  netif->linkoutput = low_level_output;

  if (netif->state == NULL ) 
  {
    ret_code = ERR_IF;
    goto exit;
  }

  if (dev->bus_width != 32)
  {
    printf("This driver doesn't support the Ethernet add on card at present\n");
    ret_code = ERR_IF;
    goto exit;
  }

  ret_code = low_level_init(dev);
  if (ret_code == ERR_OK)
  {
    etharp_init();
  }
  else
  {
    free(netif->state);
  }

exit:
  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("alt_avalon_lan91c111_init() exit = %d\n",ret_code));

  return ret_code;
}


/*
* alt_avalon_lan91c111_irq
*
* The Interrupt handler
*/
static void alt_avalon_lan91c111_irq(void* context, alt_u32 interrupt)
{
  alt_u8  irq_status;
  alt_u8  irq_msk;
  alt_avalon_lan91c111_if *dev =  (alt_avalon_lan91c111_if *)context;

  irq_status = IORD_ALTERA_AVALON_LAN91C111_IST(dev->base_addr);

  /*
   *  Rx Overrun, clear the interrupt and increment the stats counter
   */
#if LINK_STATS
  if(irq_status & ALTERA_AVALON_LAN91C111_INT_RX_OVRN_INT_MSK)
  {
    lwip_stats.link.drop++;
    IOWR_ALTERA_AVALON_LAN91C111_ACK(dev->base_addr, 
                            ALTERA_AVALON_LAN91C111_INT_RX_OVRN_INT_MSK);
  }
#endif /* LINK_STATS */

  /*
   *  Packet to Receive
   * 
   *  signal the thread which will read the data and disable interrupts
   *
   */
  irq_msk = IORD_ALTERA_AVALON_LAN91C111_MSK(dev->base_addr); 

  if((irq_status & ALTERA_AVALON_LAN91C111_INT_RCV_INT_MSK) && 
      (irq_msk & ALTERA_AVALON_LAN91C111_INT_RCV_INT_MSK))
  {
    irq_msk &= ~ALTERA_AVALON_LAN91C111_INT_RCV_INT_MSK;
    IOWR_ALTERA_AVALON_LAN91C111_MSK(dev->base_addr, irq_msk);
    sys_mbox_post(rx_mbox, &dev->lwip_dev_list.dev);
  }
  return;
}

/*
* alt_avalon_lan91c111_rx
* Handle all the receives
*/
void alt_avalon_lan91c111_rx(  alt_lwip_dev* dev )
{
  alt_u8  irq_msk;
  alt_u8  irq_status;
  alt_avalon_lan91c111_if *lan_dev = (alt_avalon_lan91c111_if *)dev->netif->state;
  
  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("lan91c111_rx()"));

  irq_status = IORD_ALTERA_AVALON_LAN91C111_IST(lan_dev->base_addr);
  while(irq_status & ALTERA_AVALON_LAN91C111_INT_RCV_INT_MSK)
  {
    alt_avalon_lan91c111_input(lan_dev);
    irq_status = IORD_ALTERA_AVALON_LAN91C111_IST(lan_dev->base_addr);
  }
  
  /* Re-enable RX interrupts */
  irq_msk = IORD_ALTERA_AVALON_LAN91C111_MSK(lan_dev->base_addr); 
  irq_msk |= ALTERA_AVALON_LAN91C111_INT_RCV_INT_MSK;
  IOWR_ALTERA_AVALON_LAN91C111_MSK(lan_dev->base_addr, irq_msk);
}

/*
* Clock one bit of data to the PHY
*/
static void clock_phy_bit(alt_avalon_lan91c111_if* dev, const unsigned value)
{
  IOWR_ALTERA_AVALON_LAN91C111_MGMT(dev->base_addr, value);
  IOWR_ALTERA_AVALON_LAN91C111_MGMT(dev->base_addr, value |
                                    ALTERA_AVALON_LAN91C111_MGMT_MCLK_MSK);
}

/*
* Read a PHY register, this involves manually clocking lots
* of data.
*/
static alt_u16 read_phy_register(alt_avalon_lan91c111_if* dev, 
                                 const unsigned phy_register)
{
  alt_u16 register_value;
  alt_u16 old_bank;
  alt_u16 mgmt_value;
  int i;

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("read_phy_register (%d)=", phy_register));

  /*
  * Swap to bank 3
  */
  old_bank = IORD_ALTERA_AVALON_LAN91C111_BSR(dev->base_addr);
  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr,3);

  mgmt_value = IORD_ALTERA_AVALON_LAN91C111_MGMT( dev->base_addr);
  mgmt_value &= 0xfff0;

  /* Output an Idle of 32 1's */
  for(i=0;i<32;i++)
  {
    clock_phy_bit(dev, mgmt_value | 
                                  ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                  ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
  }

  /* Ouput 01 which marks the change of cycle */
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK );
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
  
  /* Output the command, 10 = read */
  clock_phy_bit(dev, mgmt_value |
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK );

  /* Output the PHY address, 0 is the internal PHY */
  for(i=0;i<5;i++) 
  {
    clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK );
  }

  /* Write the Register Address, MSB first */
  for(i=4;i>=0;i--) 
  {
    clock_phy_bit(dev, mgmt_value | 
                                  ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                  ((phy_register >> i) & 0x1));
  }

  /* 
  * The datasheet says we output Z0 here, however Application Note 79 mentions a chip bug
  * which configuring the Nat Semi in built PHY we only output Z, if using an off chip PHY
  * uncomment the second line
  */
  clock_phy_bit(dev, mgmt_value);
  /*
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK );
  */

  register_value = 0;

  /* Read in the data */
  for(i=15;i>=0;i--) 
  {
    clock_phy_bit(dev, mgmt_value);
    register_value |= 
      ((IORD_ALTERA_AVALON_LAN91C111_MGMT(dev->base_addr) 
       & ALTERA_AVALON_LAN91C111_MGMT_MDI_MSK) >> 1) << i;
  }

  /* Output the Turnaround bit */
  clock_phy_bit(dev, mgmt_value);

  IOWR_ALTERA_AVALON_LAN91C111_MGMT(dev->base_addr, mgmt_value);

  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr,old_bank);

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, (" %#x\n", register_value));
  return register_value;
}

/*
* Write to a PHY register, this involves manually clocking out lots
* of data.
*/
static void write_phy_register(alt_avalon_lan91c111_if* dev, 
                        const unsigned phy_register,
                        const alt_u16 value)
{
  alt_u16 old_bank;
  alt_u16 mgmt_value;
  int i;

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("write_phy_register (%d, %#x)\n", phy_register, value));

  /*
  * Swap to bank 3, if this code is ever changed to run with interrups we will
  * need a critical section here
  */
  old_bank = IORD_ALTERA_AVALON_LAN91C111_BSR(dev->base_addr);
  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr,3);

  mgmt_value = IORD_ALTERA_AVALON_LAN91C111_MGMT(dev->base_addr);
  mgmt_value &= 0xfff0;

  /* Output an Idle of 32 1's */
  for(i=0;i<32;i++) 
  {
    clock_phy_bit(dev, mgmt_value | 
                                  ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                  ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
  }

  /* Ouput 01 which marks the change of cycle */
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK );
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
  
  /* Output the command, 01 = write */
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK );
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );


  /* Output the PHY address, 0 is the internal PHY */
  for(i=0;i<5;i++) 
  {
    clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK );
  }

  /* Write the Register Address, MSB first */
  for(i=4;i>=0;i--) 
  {
    clock_phy_bit(dev, mgmt_value | 
                                  ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                  ((phy_register >> i) & 0x1));
  }

  /* Write 10 to the turnaround bit */
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
  clock_phy_bit(dev, mgmt_value | 
                                    ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK );

  /* Now write the data MSB first */
  for(i=15;i>=0;i--) 
  {
    clock_phy_bit(dev, mgmt_value | 
                                  ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK |
                                  ((value >> i) & 0x1));
  }

  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr,old_bank);

  return;  
}

/*-----------------------------------------------------------------------------------*
 *
 * reset_device(alt_avalon_lan91c111_if *dev):
 *
 * Reset the device and get it into a known state
 *
 * 
 *-----------------------------------------------------------------------------------*/
static err_t reset_device(alt_avalon_lan91c111_if *dev)
{
  alt_u16 register_value;
  alt_u16 chip_id;
  err_t   ret_code = ERR_OK;

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("reset_device()\n"));

  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr, 3);
  chip_id = IORD_ALTERA_AVALON_LAN91C111_REV(dev->base_addr);

  if (( chip_id & ALTERA_AVALON_LAN91C111_CHIP_ID) != 
                  ALTERA_AVALON_LAN91C111_CHIP_ID ) 
  {
    LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("Not an SMC91C111 chip\n"));
    ret_code = ERR_IF;
    goto exit;
  }

  /* Bank 0 */
  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr, 0);
  IOWR_ALTERA_AVALON_LAN91C111_RCR( dev->base_addr, 
                                ALTERA_AVALON_LAN91C111_RCR_SOFTRST_OFST);

  /* a very generous reset pulse */
  usleep(1000);

  IOWR_ALTERA_AVALON_LAN91C111_RCR(dev->base_addr, 0);

  /* This should be plenty of time for the device to settle */
  usleep(5000);

  /* Reset the MMU */
  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr, 2);
  IOWR_ALTERA_AVALON_LAN91C111_MMUCR( dev->base_addr,
                                  ALTERA_AVALON_LAN91C111_MMUCR_RESET_MSK);

  do {
    register_value = IORD_ALTERA_AVALON_LAN91C111_MMUCR(dev->base_addr);
  }while(register_value & ALTERA_AVALON_LAN91C111_MMUCR_BUSY_MSK);

exit:
  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("reset_device() exit = %d\n", ret_code));

  return ret_code;

}

/*
* set_mac_address
*
* Configure the MAC address of the SMSC chip
*/
static err_t set_mac_address(alt_avalon_lan91c111_if* dev)
{
  struct netif* netif = dev->lwip_dev_list.dev.netif;
  IOWR_ALTERA_AVALON_LAN91C111_BSR(dev->base_addr, 1);
  IOWR_ALTERA_AVALON_LAN91C111_IAR0(dev->base_addr, 
                                    netif->hwaddr[0]);
  IOWR_ALTERA_AVALON_LAN91C111_IAR1(dev->base_addr, 
                                    netif->hwaddr[1]);
  IOWR_ALTERA_AVALON_LAN91C111_IAR2(dev->base_addr, 
                                    netif->hwaddr[2]);
  IOWR_ALTERA_AVALON_LAN91C111_IAR3(dev->base_addr, 
                                    netif->hwaddr[3]);
  IOWR_ALTERA_AVALON_LAN91C111_IAR4(dev->base_addr, 
                                    netif->hwaddr[4]);
  IOWR_ALTERA_AVALON_LAN91C111_IAR5(dev->base_addr, 
                                    netif->hwaddr[5]);

  return ERR_OK;
}


/*
* configure_phy 
*
* reset the PHY and auto negotiate for the link capabilities
*
*/
static err_t configure_phy(alt_avalon_lan91c111_if *dev)
{
  int timeout, i;
  alt_u16 value, old_value;
  err_t ret_code = ERR_OK;

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("configure_phy()\n"));

  /* Read the ID register let's just check we've detected the correct PHY */
  value = read_phy_register(dev, 
                            ALTERA_AVALON_LAN91C111_PHY_ID1_REG);
  if (value != ALTERA_AVALON_LAN91C111_PHY_COMPANY_ID) 
  {
    ret_code = ERR_IF;
    goto exit;
  }

  value = read_phy_register(dev, 
                            ALTERA_AVALON_LAN91C111_PHY_ID2_REG);
  if ((value & 0xfff0) != ALTERA_AVALON_LAN91C111_PHY_MFCT_ID ) 
  {
    ret_code = ERR_IF;
    goto exit;
  }

  /* Reset the PHY */
  write_phy_register( dev, 
                      ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG, 
                      ALTERA_AVALON_LAN91C111_PHY_CONTROL_RST_MSK);

  /* Spec says wait 50 mS */
  usleep(50000);

  /* Let's just check  it's out of reset */
  value = read_phy_register(dev, 
                            ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG);

  if (value & ALTERA_AVALON_LAN91C111_PHY_CONTROL_RST_MSK) 
  {
    ret_code = ERR_IF;
    goto exit;
  }

  /* Turn off Auto negotiation til we're ready for it */
  write_phy_register( dev, 
                      ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG , 0);

  /* 
   * We'll auto negotiate the capabilities thanks orange LED for packet Rx, 
   * green LED for Tx 
   */
  IOWR_ALTERA_AVALON_LAN91C111_BSR(dev->base_addr, 0); 
  IOWR_ALTERA_AVALON_LAN91C111_RPCR(dev->base_addr,
                                    ALTERA_AVALON_LAN91C111_RPCR_ANEG_MSK |
                                    ALTERA_AVALON_LAN91C111_RPCR_LS2A_MSK | 
                                    ALTERA_AVALON_LAN91C111_RPCR_LS1A_MSK |                                     
                                    ALTERA_AVALON_LAN91C111_RPCR_LS2B_MSK | 
                                    ALTERA_AVALON_LAN91C111_RPCR_LS1B_MSK | 
                                    ALTERA_AVALON_LAN91C111_RPCR_LS0B_MSK ); 

  /* Start the Auto Negotiation process */
  /* Try to negotiate, we have a go at this five times if it fails */
  i = 5;
  do 
  {
    write_phy_register( dev, 
                        ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG, 
                        ALTERA_AVALON_LAN91C111_PHY_CONTROL_ANEG_EN_MSK | 
                        ALTERA_AVALON_LAN91C111_PHY_CONTROL_ANEG_RST_MSK);

    /* 
    * Spec says wait 1500 mS after turning on auto negotiation before 
    * reading the ANEG_ACK bit see section 7.7.12
    */
    usleep(1500000);

    /* This may take a few seconds */
    timeout = 20;

    do 
    {
      value = read_phy_register(dev, ALTERA_AVALON_LAN91C111_PHY_STATUS_REG);
      timeout --;
      usleep(500000);
    }while (!(value & ALTERA_AVALON_LAN91C111_PHY_STATUS_ANEG_ACK_MSK)  
              && (timeout > 0));

    i --;
  }
  while( (i != 0) && (timeout == 0));

  if (timeout == 0)
  {
    ret_code = ERR_IF;
    goto exit;
  }

  if (value & ALTERA_AVALON_LAN91C111_PHY_STATUS_REM_FLT_MSK) 
  {
    /* 
     * We may wish to come up with a sensible default here, 
     *set to 10MBits, CSMA?
     */
    ret_code = ERR_IF;
    goto exit;
  }

  /* 
   * Find out what we've negotiated so that we can configure the MAC 
   * takes a few reads to stabilise
   */

  old_value = read_phy_register(dev, 
                            ALTERA_AVALON_LAN91C111_PHY_INT_STATUS_REG);
  value = read_phy_register(dev, 
                            ALTERA_AVALON_LAN91C111_PHY_INT_STATUS_REG);

  while(value != old_value)
  {
    old_value = value;
    value = read_phy_register(dev, 
                            ALTERA_AVALON_LAN91C111_PHY_INT_STATUS_REG);
  }

  if (value & ALTERA_AVALON_LAN91C111_PHY_INT_SPDDET_MSK) 
  {
    LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("100MBits/s\n"));
  }
  else 
  {
    LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("10MBits/s\n"));
  }

  if (value & ALTERA_AVALON_LAN91C111_PHY_INT_DPLXDET_MSK) 
  {
    LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("Full Duplex\n"));

    /* Set the MAC for Full Duplex */
    value = IORD_ALTERA_AVALON_LAN91C111_TCR(dev->base_addr);
    value |= ALTERA_AVALON_LAN91C111_TCR_SWFDUP_MSK;
    IOWR_ALTERA_AVALON_LAN91C111_TCR(dev->base_addr,value);
  }
  else 
  {
    LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("Half Duplex\n"));
  }

exit:
  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("configure_phy() exit = %d\n", ret_code));

  return ret_code;
}

/*
* allocate_tx_packet
*
* Allocate (within the device) the one packet we will use for all transmissions
*/
static err_t allocate_tx_packet( alt_avalon_lan91c111_if *dev )
{
  alt_u16 value;
  alt_u8 byte_value;
  int timeout;
  err_t ret_code = ERR_OK;

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("allocate_tx_packet()\n"));

  IOWR_ALTERA_AVALON_LAN91C111_BSR(dev->base_addr, 2);
  value = IORD_ALTERA_AVALON_LAN91C111_MMUCR(dev->base_addr);
  value |= ALTERA_AVALON_LAN91C111_MMUCR_ALLOC_MSK;
  IOWR_ALTERA_AVALON_LAN91C111_MMUCR(dev->base_addr, value);

  /* Wait for the command to complete */
  timeout = 10;
  do 
  {
    byte_value = IORD_ALTERA_AVALON_LAN91C111_IST(dev->base_addr);
    usleep(100000);
    timeout --;
  }
  while((!(byte_value & ALTERA_AVALON_LAN91C111_INT_ALLOC_INT_MSK)) && 
        (timeout > 0));

  if (timeout == 0) 
  {
    ret_code = ERR_IF;
    goto exit;
  }

  /* Save the allocated packet number for future use */
  byte_value = IORD_ALTERA_AVALON_LAN91C111_ARR(dev->base_addr);

  if (byte_value & ALTERA_AVALON_LAN91C111_ARR_FAILED_MSK) 
  {
    ret_code = ERR_IF;
    goto exit;
  }

  dev->tx_packet_no = byte_value & ~ALTERA_AVALON_LAN91C111_ARR_FAILED_MSK;


exit:
  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("allocate_tx_packet() exit = %d\n", ret_code));
  return ERR_OK;

}

/*-----------------------------------------------------------------------------------*/
static err_t low_level_init(alt_avalon_lan91c111_if *dev)
{
  /* 
  * Need to have a pointer to a generice lwip_dev
  * This is so that the get_mac_addr function interface
  * is device agnostic
  */
  alt_lwip_dev* lwip_dev = &dev->lwip_dev_list.dev;
  err_t ret_code = ERR_OK;
  alt_u16 value;

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("low_level_init()\n"));

  /*
  * Read the MAC address out of flash
  */
  ret_code = get_mac_addr(lwip_dev);
  if (ret_code != ERR_OK) 
  {
    goto exit;
  }

  /* Set MAC hardware address length */
  lwip_dev->netif->hwaddr_len = 6;

  ret_code = reset_device(dev);
  if (ret_code != ERR_OK) 
  {
    goto exit;
  }

  ret_code = set_mac_address(dev);
  if (ret_code != ERR_OK) 
  {
    goto exit;
  }

  ret_code = configure_phy(dev);
  if (ret_code != ERR_OK) 
  {
    goto exit;
  }

  ret_code = allocate_tx_packet(dev);
  if (ret_code != ERR_OK) 
  {
    goto exit;
  }

  /* maximum transfer unit */
  dev->lwip_dev_list.dev.netif->mtu = 1500;
  
  /* broadcast capability */
  dev->lwip_dev_list.dev.netif->flags = NETIF_FLAG_BROADCAST;
  
  dev->semaphore = sys_sem_new(1);
  if (dev->semaphore == NULL)
  {
    ret_code = ERR_MEM;
    goto exit;
  }

  dev->arp_semaphore = sys_sem_new(1);
  if (dev->arp_semaphore == NULL)
  {
    ret_code = ERR_MEM;
    goto exit;
  }

  /* Turn on Rx & Tx */
  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr, 0);
  IOWR_ALTERA_AVALON_LAN91C111_RCR( dev->base_addr,
                                    ALTERA_AVALON_LAN91C111_RCR_STRIP_CRC_MSK | 
                                    ALTERA_AVALON_LAN91C111_RCR_RXEN_MSK);

  value = IORD_ALTERA_AVALON_LAN91C111_TCR(dev->base_addr);
  value |= ALTERA_AVALON_LAN91C111_TCR_TXENA_MSK |
                                    ALTERA_AVALON_LAN91C111_TCR_PAD_EN_MSK;
  IOWR_ALTERA_AVALON_LAN91C111_TCR( dev->base_addr,
                                    value);

  /* 
  * From this point on we don't change banks again, bank 2 is all that's needed 
  * for Rx, TX and interrupt handling
  */
  IOWR_ALTERA_AVALON_LAN91C111_BSR( dev->base_addr, 2);

  /*
  * Let's make sure no interupts are enabled
  */
  IOWR_ALTERA_AVALON_LAN91C111_MSK(dev->base_addr, 0);

  /*
   *  Install ISR and turn on interrupts
   *  
   *  If you're counting stats then we also turn on the RCV_OVRN 
   *  interrupt and increment the error counter for each interrupt
   *  
   */
  if (alt_irq_register ( dev->irq, dev, 
                          alt_avalon_lan91c111_irq))
  {
    ret_code = ERR_IF;
    goto exit;
  }
  else
  {
#if LINK_STATS
      IOWR_ALTERA_AVALON_LAN91C111_MSK(dev->base_addr, 
                            ALTERA_AVALON_LAN91C111_INT_RCV_INT_MSK |
                            ALTERA_AVALON_LAN91C111_INT_RX_OVRN_INT_MSK);
#else
      IOWR_ALTERA_AVALON_LAN91C111_MSK(dev->base_addr, 
                            ALTERA_AVALON_LAN91C111_INT_RCV_INT_MSK);

#endif /* LINK_STATS */
  }

exit:
  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("low_level_init() exit = %d\n", ret_code));

  return ret_code;
 
}

/*-----------------------------------------------------------------------------------* 
 *
 * low_level_output():
 *
 * Should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * 
 * This funciton should be changed to use all the available transmit
 * packets in the Lan91C111
 *-----------------------------------------------------------------------------------*/

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int total_length_sent = 0;
  int i, unaligned, end, base;
  alt_u8 irq_value, *datap;
  alt_u16 mmu_status, ptr_status;
  alt_avalon_lan91c111_if *dev = netif->state;

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("low_level_output ( %#x)\n",p));

  /* Wait for the last Tx to complete */
  do 
  {
    irq_value = IORD_ALTERA_AVALON_LAN91C111_IST(dev->base_addr);
  }while(!(irq_value & ALTERA_AVALON_LAN91C111_INT_TX_EMPTY_INT_MSK));

  /* Clear the interrupt */
  IOWR_ALTERA_AVALON_LAN91C111_ACK( dev->base_addr, 
                                    ALTERA_AVALON_LAN91C111_INT_TX_EMPTY_INT_MSK);


  /* Always re-use the same packet */
  IOWR_ALTERA_AVALON_LAN91C111_PNR( dev->base_addr, 
                                    dev->tx_packet_no);

  /*
  * The Pointer Register and the MMUCR register are used in both TX and RX and are context sensitive, so 
  * protect them with a semaphore
  */
  sys_sem_wait(dev->semaphore);

  do 
  {
    ptr_status = IORD_ALTERA_AVALON_LAN91C111_PTR( dev->base_addr);
  }while (ptr_status & ALTERA_AVALON_LAN91C111_PTR_NOT_EMPTY_MSK);

  
  IOWR_ALTERA_AVALON_LAN91C111_PTR( dev->base_addr,
                                    ALTERA_AVALON_LAN91C111_PTR_AUTO_INCR_MSK);

  /* The status word */
  IOWR_ALTERA_AVALON_LAN91C111_DATA_HW( dev->base_addr, 0);

  /* 
  * The byte count including the 6 control bytes 
  *
  * Bit odd this, but the length is always written as even, if the frame is an odd length
  * then the byte is written as one of the control words and an appropriate bit is set
  *
  */
  IOWR_ALTERA_AVALON_LAN91C111_DATA_HW( dev->base_addr, ((p->tot_len & ~1) + 6));

  for(q = p; q != NULL; q = q->next) 
  {
    /* 
    * Write each buffer of data to the device
    *
    * Nios requres accesses to be aligned on the correct boundary
    * 
    * You could just do this with 
    * for(i=0;i<q->len;i++)
    *   write_register8( dev, DATA_OFFSET, *(alt_u8*)(q->payload+i));
    *
    * But this is more efficient
    */

    unaligned = ((int)q->payload) % 4;
    unaligned = (4 - unaligned) % 4;

    switch (unaligned)
    {
      case 3:
        IOWR_ALTERA_AVALON_LAN91C111_DATA_BYTE( dev->base_addr,
                                      *(alt_u8*)q->payload);
        IOWR_ALTERA_AVALON_LAN91C111_DATA_HW( dev->base_addr,
                                      *((alt_u16*)((alt_u8*)q->payload + 1)));
        break;
      case 2:
        IOWR_ALTERA_AVALON_LAN91C111_DATA_HW( dev->base_addr,
                                      *(alt_u16*)q->payload);
        break;
      case 1:
        IOWR_ALTERA_AVALON_LAN91C111_DATA_BYTE( dev->base_addr,
                                      *(alt_u8*)q->payload);
        break;
    }

   datap = q->payload;
   end = q->len-3 ;
   base = dev->base_addr;
   for (i=unaligned;i<end;i+=4) {
       IOWR_ALTERA_AVALON_LAN91C111_DATA_WORD( base,
                                         *((u32_t *)(datap+i)));     
    }

    switch (q->len - i){
      case 3:
        IOWR_ALTERA_AVALON_LAN91C111_DATA_HW( dev->base_addr,
                                      *((alt_u16*)((alt_u8*)q->payload + q->len - 3)));
        IOWR_ALTERA_AVALON_LAN91C111_DATA_BYTE( dev->base_addr,
                                      *((alt_u8*)((alt_u8*)q->payload + q->len -1)));
        break;
      case 2: 
        IOWR_ALTERA_AVALON_LAN91C111_DATA_HW( dev->base_addr,
                                      *((alt_u16*)((alt_u8*)q->payload + q->len - 2)));
        break;
      case 1:
        IOWR_ALTERA_AVALON_LAN91C111_DATA_BYTE( dev->base_addr,
                                      *((alt_u8*)((alt_u8*)q->payload + q->len -1)));
        break;
    }

    total_length_sent += q->len;

    /*
    * Last buffer in the packet?
    */
    if (total_length_sent == p->tot_len) 
    {
      if (p->tot_len & 1) 
      {
        IOWR_ALTERA_AVALON_LAN91C111_DATA_BYTE( dev->base_addr,
                                    ALTERA_AVALON_LAN91C111_CONTROL_ODD_MSK);
      }
      else 
      {
        IOWR_ALTERA_AVALON_LAN91C111_DATA_HW( dev->base_addr, 0);
      }
    }
  }

  /* Wait for any pending commands to complete */
  do 
  {
    mmu_status = IORD_ALTERA_AVALON_LAN91C111_MMUCR(dev->base_addr);
  }while (mmu_status & ALTERA_AVALON_LAN91C111_MMUCR_BUSY_MSK);

  /* Queue the packet */
  IOWR_ALTERA_AVALON_LAN91C111_MMUCR( dev->base_addr,
                                  ALTERA_AVALON_LAN91C111_MMUCR_ENQUEUE_MSK);
  sys_sem_signal(dev->semaphore);

#if LINK_STATS
  lwip_stats.link.xmit++;
#endif /* LINK_STATS */      
  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("low_level_ouptput () return OK\n"));


  return ERR_OK;
}

/*-----------------------------------------------------------------------------------* 
 *
 * low_level_input():
 *
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 *
 *-----------------------------------------------------------------------------------*/
static struct pbuf*
low_level_input(alt_avalon_lan91c111_if *dev)
{
  struct pbuf *p=NULL, *q;
  alt_u32 status;
  alt_u16 packet_number, mmu_status, ptr_status;
  int length, i, unaligned, end, base;
  alt_u8 *data_ptr; 

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("low_level_input()\n"));

  packet_number = IORD_ALTERA_AVALON_LAN91C111_RX_FIFO(
                                              dev->base_addr);
  if ( packet_number & ALTERA_AVALON_LAN91C111_RX_FIFO_REMPTY_MSK ) 
  {
    LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("low_level_input (exits p =0)\n"));
    return 0;
  }

  /*
  * The Pointer Register and the MMUCR register are used in both TX and RX and are context sensitive, so 
  * protect them with a semaphore
  */
  sys_sem_wait(dev->semaphore);

  do 
  {
    ptr_status = IORD_ALTERA_AVALON_LAN91C111_PTR( dev->base_addr);
  }while (ptr_status & ALTERA_AVALON_LAN91C111_PTR_NOT_EMPTY_MSK);

  
  IOWR_ALTERA_AVALON_LAN91C111_PTR( dev->base_addr,
                                    ALTERA_AVALON_LAN91C111_PTR_READ_MSK |
                                    ALTERA_AVALON_LAN91C111_PTR_AUTO_INCR_MSK |
                                    ALTERA_AVALON_LAN91C111_PTR_RCV_MSK);

  /* The first two half-words are status and byte count */
  status = IORD_ALTERA_AVALON_LAN91C111_DATA_WORD(dev->base_addr);
  length = (status & 0xffff0000ul) >> 16;
  status &= 0xffff;

  if (status & (ALTERA_AVALON_LAN91C111_RS_TOO_SHORT_MSK | 
                ALTERA_AVALON_LAN91C111_RS_TOO_LONG_MSK | 
                ALTERA_AVALON_LAN91C111_RS_BAD_CRC_MSK | 
                ALTERA_AVALON_LAN91C111_RS_ALGN_ERR_MSK))  
  {
#if LINK_STATS
    lwip_stats.link.drop++;
#endif /* LINK_STATS */
  }
  else 
  {
    /* We've already read two of the words */
    length -= 4;

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, length, PBUF_POOL);
    if (p != NULL) 
    {
      /* We iterate over the pbuf chain until we have read the entire
         packet into the pbuf. */
      for(q = p; q != NULL; q = q->next) 
      {

        /* 
        * Fill each buffer in the chain
        *
        * On Nios all the memory accesses must be aligned 
        */

        unaligned = (int)q->payload % 4;
        unaligned = (4 - unaligned) % 4;

        switch (unaligned)
        {
        case 3:
          *((alt_u8*)q->payload) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_BYTE(dev->base_addr);
          *((alt_u16*)((alt_u8*)q->payload+1)) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_HW(dev->base_addr);
          break;
        case 2:
          *((alt_u16*)q->payload) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_HW(dev->base_addr);
          break;
        case 1:
          *((alt_u8*)q->payload) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_BYTE(dev->base_addr);
          break;
        }

        end = q->len-3;
        data_ptr = q->payload;
        base = dev->base_addr;
        for (i = unaligned; i < end; i+= 4)
        {
          *((u32_t*)((alt_u8*)data_ptr + i)) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_WORD(base);
        }

        switch (q->len - i)
        {
        case 3:
          *((alt_u16*)((alt_u8*)q->payload + i)) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_HW(dev->base_addr);
          *((alt_u8*)((alt_u8*)q->payload + i + 2)) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_BYTE(dev->base_addr);
          break;
        case 2:
          *((alt_u16*)((alt_u8*)q->payload + i)) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_HW(dev->base_addr);
          break;
        case 1:
          *((alt_u8*)((alt_u8*)q->payload + i)) = 
            IORD_ALTERA_AVALON_LAN91C111_DATA_BYTE(dev->base_addr);
          break;
        }
      }

#if LINK_STATS
      lwip_stats.link.recv++;
#endif /* LINK_STATS */      
    } 
    else 
    {
#if LINK_STATS
      lwip_stats.link.memerr++;
      lwip_stats.link.drop++;
#endif /* LINK_STATS */      
    }
  }

  /* Wait for any pending commands to complete */
  do 
  {
    mmu_status = IORD_ALTERA_AVALON_LAN91C111_MMUCR(dev->base_addr);
  }while (mmu_status & ALTERA_AVALON_LAN91C111_MMUCR_BUSY_MSK);

  /* Just get rid of the packet */
  IOWR_ALTERA_AVALON_LAN91C111_MMUCR(dev->base_addr,
            ALTERA_AVALON_LAN91C111_MMUCR_REMOVE_RELEASE_MSK);
  sys_sem_signal(dev->semaphore);

  LWIP_DEBUGF(NETIF_DEBUG | DBG_TRACE, ("low_level_input (exits p =%#x)\n", p));
  return p;  
}


/*-----------------------------------------------------------------------------------* 
 *
 * alt_avalon_lan91c111_output():
 *
 * This function is called by the TCP/IP stack when an IP packet
 * should be sent. It calls the function called low_level_output() to
 * do the actuall transmission of the packet.
 *
 * 
 *-----------------------------------------------------------------------------------*/
static err_t
alt_avalon_lan91c111_output(struct netif *netif, struct pbuf *p,
      struct ip_addr *ipaddr)
{
  err_t err;
  alt_avalon_lan91c111_if* dev = (alt_avalon_lan91c111_if*)netif->state;

  sys_sem_wait(dev->arp_semaphore);

  /* 
  * resolve hardware address, then send (or queue) packet 
  * The code which updates the ARP tables does not appear to be thread safe
  * so I've added a MUTEX around all calls to the arp code
  * 
  */
  
  err = etharp_output(netif, ipaddr, p);
  sys_sem_signal(dev->arp_semaphore);

  return err;
}

#endif /* LWIP */
