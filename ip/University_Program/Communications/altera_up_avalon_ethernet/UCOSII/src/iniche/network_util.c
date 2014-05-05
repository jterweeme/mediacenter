#include "network_util.h"
#include "altera_up_avalon_ethernet_regs.h"
#include <alt_types.h>

#include <DE2_pio_regs.h>

#include <alt_iniche_dev.h>
#include "includes.h"
#include "ipport.h"
#include "tcpport.h"

#define IP4_ADDR(ipaddr, a,b,c,d) (ipaddr) =  htonl(((alt_u32)(a & 0xff) << 24) | ((alt_u32)(b & 0xff) << 16) | \
                                                                 ((alt_u32)(c & 0xff) << 8) | (alt_u32)(d & 0xff))
/**
 * @brief Get the MAC address from the DM9000A chip on the DE2 Board 
 *
 * @param net -- The NET structure for the Ethernet structure
 * @param mac_addr -- the array to store the MAC address  
 *
 * @return 0 for success or corresponding negated errno
 *
 * @note The MAC address is assigned from user in the SOPC Builder or a default one "01:60:6E:11:02:0F"
 *
 **/
int get_mac_addr(NET net, unsigned char mac_addr[6])
{
#ifdef ALT_UP_ETHERNET_USER_MAC
	// use the user specified MAC address
	alt_u8 sys_MAC_addr[6] = ALT_UP_ETHERNET_USER_MAC;
#else
	// use the default MAC
	alt_u8 sys_MAC_addr[6] = { 0x01, 0x60, 0x6E, 0x11, 0x02, 0x0F };
#endif
	int i;
	for (i = 0; i < 6; i++)
		mac_addr[i] = sys_MAC_addr[i];
	return 0;
}

int get_ip_addr(alt_iniche_dev *p_dev,
				ip_addr *ipaddr,
				ip_addr *netmask,
				ip_addr *gw,
				int		*use_dhcp)
{
	int ret_code = 0;
	//if ( !strcmp(p_dev->name ,"/dev/" INICHE_DEFAULT_IF) )
	{
		dprintf("Assigning IP 10.1.1.3\n");
		IP4_ADDR(*ipaddr, 10, 1, 1, 3);
		IP4_ADDR(*gw, 10, 1, 1, 254);
		IP4_ADDR(*netmask, 255, 255, 255, 0);

#ifdef DHCP_CLIENT
		*use_dhcp = 1;
#else
		*use_dhcp = 0;
#endif
		ret_code = 1;
	}
	return ret_code;
}

void jtag_print_hex_number(alt_u32 num, alt_u8 n)
{
	alt_u8 i = 0;
	alt_u8 cur_char;
	while (i < 2*n)
	{
		cur_char = (num >> ((2*n-1)*4)) & 0x000F;
		cur_char = (cur_char < 10) ? 
			(cur_char + '0') : 
			(cur_char + ('A' - 10));
		while ((((IORD(JTAG_UART_BASE,1)) & 0xFF00) >> 8) <= 4) /* Add Time Out */;
		IOWR(JTAG_UART_BASE, 0, cur_char);
		num = num << 4;
		i++;
	}
	return;
}

void write_leds(alt_u8 leds)
{
	alt_u32 addr = 0x489020;
	//#define IORD_DE2_PIO_DATA(base)             IORD(base, 0) 
	//#define IOWR_DE2_PIO_DATA(base, data)       IOWR(base, 0, data)
	IOWR_DE2_PIO_DATA(addr, leds);
}


