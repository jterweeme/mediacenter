/*
 * FILENAME: s91phy.c
 *
 * Copyright  2002 By InterNiche Technologies Inc. All rights reserved
 *
 *  This file contains the code to communicate with the PHY on
 * the SMSC91C111 Ethernet chip.
 * 
 *
 * MODULE: smsc91x
 *
 * ROUTINES: s91_phy_init(), s91_readphy(), s91_writephy(),
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

#include "smsc91x.h"
#include "altera_avalon_lan91c111_regs.h"
#include "unistd.h"

/* FUNCTION: clock_phy_bit
 * 
 * Clock one bit of data to the PHY
 * 
 * PARAM1: dev          device address
 * PARAM2: value        unsigned short value to transmit
 * 
 * RETURN: none
 */
static void
clock_phy_bit(u_long dev, const unsigned value)
{
   IOWR_ALTERA_AVALON_LAN91C111_MGMT(dev, value);
   IOWR_ALTERA_AVALON_LAN91C111_MGMT(dev, value |
                     ALTERA_AVALON_LAN91C111_MGMT_MCLK_MSK);
}
 
 
 
/* FUNCTION: Read a PHY register
 * 
 * PARAM1: dev          SMSC device structure pointer
 * PARAM2: reg          PHY register number
 * 
 * RETURN: value        16-bit PHY register value
 */
unshort
s91_readphy(SMSC smsc, const unsigned reg)
{
   unshort  value = 0;
   unshort  old_bank;
   unshort  mgmt;
   int      i;
   u_long   base = smsc->regbase;
      
   /* TODO: disable interrupts? */
   
   /* switch to Bank 3 */
   old_bank = IORD_ALTERA_AVALON_LAN91C111_BSR(base);
   IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 3);
   
   mgmt = (IORD_ALTERA_AVALON_LAN91C111_MGMT(base)) & 0xfff0;
   
   /* Output an Idle of 32 1's */
   for (i = 0; i < 32; i++)
   {
      clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                               | ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
   }
   
   /* Output 01 = change of cycle */
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK);
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                            | ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
                            
   /* Output 10 = read */
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                            | ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK);
   
   /* Output 00000 = internal Phy address */
   for (i = 0; i < 5; i++)
   {
      clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK);
   }
   
   /* Output 5-bit register address; MSB first */
   for (i = 4; i >= 0; i--)
   {
      clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                               | ((reg >> i) & 0x1));
   }
   
   /* Output Z0 = write to read transition */
   /* AppNote 79: for internal PHY, only output Z */
   clock_phy_bit(base, mgmt);
   /*
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK);
   */
   
   /* Read 16-bit data from Phy register; MSB first */
   for (i = 0; i < 16; i++)
   {
      clock_phy_bit(base, mgmt);
      value = (value << 1) |
              (((IORD_ALTERA_AVALON_LAN91C111_MGMT(base)) &
                ALTERA_AVALON_LAN91C111_MGMT_MDI_MSK) >>
               ALTERA_AVALON_LAN91C111_MGMT_MDI_OFST);
   }
   
   /* Output Z = turnaround bit */
   clock_phy_bit(base, mgmt);
   
   /* Leave output in Z */
   IOWR_ALTERA_AVALON_LAN91C111_MGMT(base, mgmt);
   
   /* Restore previous bank selection */
   IOWR_ALTERA_AVALON_LAN91C111_BSR(base, old_bank);
   
   /* TODO: enable interrupts? */
   
   return (value);
}    
 
 
 
/* FUNCTION: Write a PHY register
 * 
 * PARAM1: dev          SMSC device structure pointer
 * PARAM2: reg          PHY register number
 * PARAM3: value        16-bit register value
 * 
 * RETURN: none
 */
void
s91_writephy(SMSC smsc, const unsigned reg, const unshort value)
{
   unshort  old_bank;
   unshort  mgmt;
   int      i;
   u_long   base = smsc->regbase;
      
   /* TODO: disable interrupts? */
   
   /* switch to Bank 3 */
   old_bank = IORD_ALTERA_AVALON_LAN91C111_BSR(base);
   IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 3);
   
   mgmt = (IORD_ALTERA_AVALON_LAN91C111_MGMT(base)) & 0xfff0;
   
   /* Output an Idle of 32 1's */
   for (i = 0; i < 32; i++)
   {
      clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                               | ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
   }
   
   /* Output 01 = change of cycle */
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK);
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                            | ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
                            
   /* Output 01 = write */
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK);
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                            | ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
   
   /* Output 00000 = internal Phy address */
   for (i = 0; i < 5; i++)
   {
      clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK);
   }
   
   /* Output 5-bit register address; MSB first */
   for (i = 4; i >= 0; i--)
   {
      clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                               | ((reg >> i) & 0x1));
   }
   
   /* Output 10 = turnaround bit */
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                            | ALTERA_AVALON_LAN91C111_MGMT_MDO_MSK );
   clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK);
   
   /* Write 16-bit data value; MSB first */
   for (i = 15; i >= 0; i--)
   {
      clock_phy_bit(base, mgmt | ALTERA_AVALON_LAN91C111_MGMT_MDOE_MSK
                               | ((value >> i) & 0x1));
   }
   
   /* Leave output in Z */
   IOWR_ALTERA_AVALON_LAN91C111_MGMT(base, mgmt);
   
   /* Restore previous bank selection */
   IOWR_ALTERA_AVALON_LAN91C111_BSR(base, old_bank);
   
   /* TODO: enable interrupts? */

}  

/* FUNCTION: s91_phy_init()
 * 
 * Configures the SMSC PHY to the desired fixed/autonegotiated speed
 *
 * PARAM1: SMSC smsc
 *
 * RETURNS: 0 if successful, otherwise non-zero error code
 * 
 * The desired speed parameter is smsc->req_speed.
 */

int s91_phy_init(SMSC smsc)
{
   u_long   base  = smsc->regbase;
   int      speed = smsc->req_speed;
   int      i, timeout;
   unshort  status, tmp;
   unshort  rpcr, ledA, ledB;
   unshort  id1, id2;

   if (speed == 0)
      speed = LINKSTAT_AUTO_NEGOTIATE;

   /* Read the PHY ID register to validate presence of the PHY */
   /* The internal PHY is Phy #0 */ 
#ifdef NOT_USED
   phyaddr = 0;
#endif    
   id1  = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_ID1_REG);
   id2  = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_ID2_REG);
   
   if ((id1 != ALTERA_AVALON_LAN91C111_PHY_COMPANY_ID) || 
       (id2 != ALTERA_AVALON_LAN91C111_PHY_MFCT_ID))
   {
      dprintf("s91: No PHY found\n");
      return ENP_HARDWARE;
   }

#ifdef NOT_USED
   /* Save the PHY's address */
   smsc->phy_addr = phyaddr;
#endif

   /* Start the PHY reset process */
   s91_writephy(smsc, ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG,
                      ALTERA_AVALON_LAN91C111_PHY_CONTROL_RST_MSK);

   /* Wait for the reset complete; spec says wait 50 msec */
   usleep(50000);
   
   /* Check to make sure it's reset */
   status = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG);
   if (status & ALTERA_AVALON_LAN91C111_PHY_CONTROL_RST_MSK)
   {
      dprintf("s91: timed out resetting PHY\n");
      return ENP_HARDWARE;
   }

   /* save PHY Status Output */
   smsc->phyreg18 = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_INT_STATUS_REG);

#if 0
   /* Mask all PHY interrupts except MINT and LINKFAIL */
   s91_writephy(smsc, ALTERA_AVALON_LAN91C111_PHY_INT_MASK_REG, 0x3fff);
#endif

   /* The PHY RESET should have disabled auto-negotiation, and put the 
    * device in 10Mbps half duplex, but just in case, we'll write the 
    * CONTROL register again.
    */
   s91_writephy(smsc, ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG, 0);
   
   switch (speed)
   {
      case LINKSTAT_SPEED_10:
         rpcr = 0;                                       /* 10 Mbps half duplex */
         break;
      case LINKSTAT_SPEED_100:
         rpcr = ALTERA_AVALON_LAN91C111_RPCR_SPEED_MSK;  /* 100 Mbps half duplex */
         s91_writephy(smsc, ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG, 
                            ALTERA_AVALON_LAN91C111_PHY_CONTROL_SPEED_MSK);
         break;
      case LINKSTAT_AUTO_NEGOTIATE:
         rpcr = ALTERA_AVALON_LAN91C111_RPCR_ANEG_MSK;   /* Auto-negotiation */
         break;
      default:
         dprintf("s91: unknown speed selection\n");
         return ENP_HARDWARE;
   }

   /* Configure the Receive/Phy Control register (LED status indicators) 
    * ledA = 0x0 = Link detected (10 or 100 Mbps) 
    * ledB = 0x4 = Link activity (transmit or recieve) 
    */
   ledA = 0;
   ledB = ALTERA_AVALON_LAN91C111_RPCR_LS2B_MSK;
   
   IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 0);
   IOWR_ALTERA_AVALON_LAN91C111_RPCR(base, rpcr | ledA | ledB);
/*   dprintf("RPCR = %x\n", rpcr | ledA | ledB); */
 
   /* If Auto-negotiation selected, begin the process */
   if (speed == LINKSTAT_AUTO_NEGOTIATE)
   {
      /* try 5 times before giving up */
      for (i = 5; i > 0; i--)
      {
         /* Set our Autoneg "Advertisement" */
         s91_writephy(smsc, ALTERA_AVALON_LAN91C111_PHY_ADVERT_REG,
                          ( ALTERA_AVALON_LAN91C111_PHY_NEGOTIATE_TX_FDX_MSK |
                            ALTERA_AVALON_LAN91C111_PHY_NEGOTIATE_TX_HDX_MSK |
                            ALTERA_AVALON_LAN91C111_PHY_NEGOTIATE_10_FDX_MSK |
                            ALTERA_AVALON_LAN91C111_PHY_NEGOTIATE_10_HDX_MSK |
                            ALTERA_AVALON_LAN91C111_PHY_NEGOTIATE_CSMA_MSK ) );

         /* Start auto-negotiation process */
         s91_writephy(smsc, ALTERA_AVALON_LAN91C111_PHY_CONTROL_REG,
                          ( ALTERA_AVALON_LAN91C111_PHY_CONTROL_ANEG_EN_MSK |
                            ALTERA_AVALON_LAN91C111_PHY_CONTROL_ANEG_RST_MSK ) );

         /* Wait for the autoneg complete; the Spec says wait 1500 msec
          * before reading the ANEG_ACK bit (see section 7.7.12)
          */
         usleep(1500000);
          
         for (timeout = 20; timeout > 0; timeout--)
         {
            status = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_STATUS_REG);
            usleep(500000);
            if (status & ALTERA_AVALON_LAN91C111_PHY_STATUS_ANEG_ACK_MSK)
               break;
         }
         if (timeout > 0)  break;
      }

       /* dprintf results */
       dprintf("smsc91c111 Auto-negotiation: ");
       if (timeout <= 0)
       {
           dprintf("ERROR; timed out\n");
           return ENP_HARDWARE;
       }
       
#if 1
       if (status & ALTERA_AVALON_LAN91C111_PHY_STATUS_REM_FLT_MSK)
       {
           dprintf("ERROR; remote fault\n");
           /* return ENP_HARDWARE; */
       }
#else
       /* update the PHY status register info */
       IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 2);
       do
       {
          status = IORD_ALTERA_AVALON_LAN91C111_IST(base);
       } while ((status & ALTERA_AVALON_LAN91C111_INT_MDINT_MSK) == 0);
      IOWR_ALTERA_AVALON_LAN91C111_ACK(base, 
                           ALTERA_AVALON_LAN91C111_INT_MDINT_MSK);
#endif
   }

   /* A phy interrupt should have updated the register 18 value, but
    * re-read it anyway. May need to do multiple reads until the
    * status register value stablizes.
    */
   i = 0;
   status = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_INT_STATUS_REG);
   do
   {
      i++;
      tmp = status;
      status = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_INT_STATUS_REG);
   }
   while (tmp != status);
   smsc->phyreg18 = status;

/*   dprintf("(%d) ", i); */
   if (smsc->phyreg18 & ALTERA_AVALON_LAN91C111_PHY_INT_SPDDET_MSK)
      dprintf("100 Mbps, ");
   else
      dprintf("10 Mbps, ");

   if (smsc->phyreg18 & ALTERA_AVALON_LAN91C111_PHY_INT_DPLXDET_MSK )
   {
      dprintf("Full Duplex\n");
      IOWR_ALTERA_AVALON_LAN91C111_BSR(base, 0);
      tmp = IORD_ALTERA_AVALON_LAN91C111_TCR(base);
      tmp |= ALTERA_AVALON_LAN91C111_TCR_SWFDUP_MSK;
      IOWR_ALTERA_AVALON_LAN91C111_TCR(base, tmp);
   }
   else
   {
      dprintf("Half Duplex\n");
   }

   return 0;   /* OK return */
}



/* FUNCTION: s91_setSpeed
 * 
 * Set the MAC transmission speed
 * 
 * PARAM1: smsc            SMSC structure pointer
 * PARAM2: speed           integer speed selection value
 * 
 * RETURN: 0 if speed set successfully, otherwise non-zero error code
 */
int s91_setSpeed (SMSC smsc, int speed)
{
   smsc->req_speed = speed;
   return (s91_phy_init(smsc));
}



/* FUNCTION: s91_getSpeed
 * 
 * Get the current MAC speed setting
 * 
 * PARAM1: smsc            SMSC structure pointer
 * 
 * RETURN: int             bit mask indicating current speed setting
 *                         10/100/auto Mbps, half/full-duplex
 */
int s91_getSpeed (SMSC smsc)
{
    int rval=0;
    unshort  data;

    /* get the current RPCR value */
   IOWR_ALTERA_AVALON_LAN91C111_BSR(smsc->regbase, 0);
   data = IORD_ALTERA_AVALON_LAN91C111_RPCR(smsc->regbase);

   /* see if autonegotiation was active */
   if (data & ALTERA_AVALON_LAN91C111_RPCR_ANEG_MSK) {
      rval |= LINKSTAT_AUTO_NEGOTIATE;
   }

   /* check the link status and speed */
   data = s91_readphy(smsc, ALTERA_AVALON_LAN91C111_PHY_INT_STATUS_REG);
   if (data & ALTERA_AVALON_LAN91C111_PHY_INT_SPDDET_MSK)
      rval |= LINKSTAT_SPEED_100;
   else
      rval |= LINKSTAT_SPEED_10;

   if (data & ALTERA_AVALON_LAN91C111_PHY_INT_DPLXDET_MSK)
      rval |= LINKSTAT_DUPLEX_FULL;
   else
      rval |= LINKSTAT_DUPLEX_HALF;

   if (data & ALTERA_AVALON_LAN91C111_PHY_INT_LNKFAIL_MSK) {
      rval |= LINKSTAT_LINKDOWN;
   }

   return (rval);
}

#endif /* ALT_INICHE */
