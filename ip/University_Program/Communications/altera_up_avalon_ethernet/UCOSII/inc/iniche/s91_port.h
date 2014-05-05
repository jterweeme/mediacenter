/*
 * FileName: s91_port.h
 *
 * Port definitions for SMSC9C111 series ethernet chips on the 
 * Altera Nios2 Cyclone Dev board.
 *
 * Copyright 2004 by InterNiche Technologies Inc. All rights reserved.
 *
 *
 * This file for:
 *   ALTERA Cyclone Dev board with the ALTERA Nios2 Core.
 *   SMSC91C11 10/100 Ethernet Controller
 *   GNU C Compiler provided by ALTERA Quartus Toolset.
 *   Quartus HAL BSP
 *   uCOS-II RTOS Rel 2.76 as distributed by Altera/NIOS2
 *
 * 06/21/2004
 *
 */

#ifndef _S91PORT_H_
#define _S91PORT_H_ 1

#include "ipport.h"

/* number of SMSC91x devices to support */
#define S91_DEVICES 1

/* SMCS91x hardware parameters for Altera Nios2 port */\

#define SMSC_MEM_MAPPED  1

/* Critical section Macros */
#ifdef INICHE_TASKS
#define ENTER_S91_SECTION(dev) ENTER_CRIT_SECTION(dev)
#define EXIT_S91_SECTION(dev)  EXIT_CRIT_SECTION(dev)
#else
void irq_Mask(void);
void irq_Unmask(void);
#define ENTER_S91_SECTION(dev) irq_Mask();
#define EXIT_S91_SECTION(dev)  irq_Unmask();
#endif

#define BASE_TYPE   u_long

#endif /* _S91PORT_H_ */
