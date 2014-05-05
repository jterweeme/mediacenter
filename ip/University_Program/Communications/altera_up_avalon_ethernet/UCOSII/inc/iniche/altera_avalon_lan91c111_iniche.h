/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
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
* altera_avalon_lan91c111.h - LWIP ethernet interface for the the Lan91C111   *
* on the Nios boards.                                                         *
*                                                                             *
* Author PRR                                                                  *
*                                                                             *
******************************************************************************/

#ifndef __ALTERA_AVALON_LAN91C111_INICHE_H__
#define __ALTERA_AVALON_LAN91C111_INICHE_H__

#include "alt_iniche_dev.h"
#include "system.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct alt_avalon_lan91c111_if_struct
{
    alt_iniche_dev dev;
	// base address of the Ethernet component
	unsigned base;
	unsigned irq_num;
} alt_avalon_lan91c111_if;

error_t alt_avalon_lan91c111_init(alt_avalon_lan91c111_if *lan91c111, alt_iniche_dev *p_dev);

#define ALTERA_UP_AVALON_ETHERNET_INSTANCE(name, dev) \
alt_avalon_lan91c111_if dev =   \
{                               \
    {                           \
        ALT_LLIST_ENTRY,        \
        name##_NAME,            \
        alt_avalon_lan91c111_init, 	\
        NULL,                  	\
        0,                   	\
        NULL                  	\
    },                          \
	name##_BASE,				\
	name##_IRQ					\
}

#define ALTERA_UP_AVALON_ETHERNET_INIT(name, dev_inst)	\
{														\
    alt_iniche_dev_reg(&(dev_inst.dev));				\
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ALTERA_AVALON_LAN91C111_INICHE_H__ */

/*
#define ALTERA_UP_AVALON_ETHERNET_INSTANCE(name, dev) \
alt_avalon_lan91c111_if dev =   \
{                               \
    {                           \
        ALT_LLIST_ENTRY,        \
        name##_NAME,            \
        alt_avalon_lan91c111_init, 	\
        NULL,                  	\
        0,                   	\
        NULL                  	\
    },                          \
	name##_BASE,				\
	name##_IRQ              	\
}

#define ALTERA_UP_AVALON_ETHERNET_INIT(name, dev_inst)	\
{														\
    alt_iniche_dev_reg(&(dev_inst.dev));				\
}
*/
