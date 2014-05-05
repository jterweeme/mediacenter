LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;

-- ******************************************************************************
-- * License Agreement                                                          *
-- *                                                                            *
-- * Copyright (c) 1991-2013 Altera Corporation, San Jose, California, USA.     *
-- * All rights reserved.                                                       *
-- *                                                                            *
-- * Any megafunction design, and related net list (encrypted or decrypted),    *
-- *  support information, device programming or simulation file, and any other *
-- *  associated documentation or information provided by Altera or a partner   *
-- *  under Altera's Megafunction Partnership Program may be used only to       *
-- *  program PLD devices (but not masked PLD devices) from Altera.  Any other  *
-- *  use of such megafunction design, net list, support information, device    *
-- *  programming or simulation file, or any other related documentation or     *
-- *  information is prohibited for any other purpose, including, but not       *
-- *  limited to modification, reverse engineering, de-compiling, or use with   *
-- *  any other silicon devices, unless such use is explicitly licensed under   *
-- *  a separate agreement with Altera or a megafunction partner.  Title to     *
-- *  the intellectual property, including patents, copyrights, trademarks,     *
-- *  trade secrets, or maskworks, embodied in any such megafunction design,    *
-- *  net list, support information, device programming or simulation file, or  *
-- *  any other related documentation or information provided by Altera or a    *
-- *  megafunction partner, remains with Altera, the megafunction partner, or   *
-- *  their respective licensors.  No other licenses, including any licenses    *
-- *  needed under any third party's intellectual property, are provided herein.*
-- *  Copying or modifying any file, or portion thereof, to which this notice   *
-- *  is attached violates this copyright.                                      *
-- *                                                                            *
-- * THIS FILE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
-- * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
-- * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
-- * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
-- * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    *
-- * FROM, OUT OF OR IN CONNECTION WITH THIS FILE OR THE USE OR OTHER DEALINGS  *
-- * IN THIS FILE.                                                              *
-- *                                                                            *
-- * This agreement shall be governed in all respects by the laws of the State  *
-- *  of California and by the laws of the United States of America.            *
-- *                                                                            *
-- ******************************************************************************

-- ******************************************************************************
-- *                                                                            *
-- * This module reads and writes data to Ethernet chip on the DE2 Board.       *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_avalon_ethernet IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************

-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk			:IN		STD_LOGIC;
	reset			:IN		STD_LOGIC;

	address		:IN		STD_LOGIC;
	chipselect	:IN		STD_LOGIC;
	read			:IN		STD_LOGIC;
	write			:IN		STD_LOGIC;
	writedata	:IN		STD_LOGIC_VECTOR(15 DOWNTO 0);	

	ENET_INT		:IN		STD_LOGIC;

	-- Bidirectionals
	ENET_DATA	:INOUT	STD_LOGIC_VECTOR(15 DOWNTO 0);	

	-- Outputs
	irq			:BUFFER	STD_LOGIC;
	readdata		:BUFFER	STD_LOGIC_VECTOR(15 DOWNTO 0);	

	ENET_RST_N	:BUFFER	STD_LOGIC;
	ENET_CS_N	:BUFFER	STD_LOGIC;
	ENET_CMD		:BUFFER	STD_LOGIC;
	ENET_RD_N	:BUFFER	STD_LOGIC;
	ENET_WR_N	:BUFFER	STD_LOGIC

);

END altera_up_avalon_ethernet;

ARCHITECTURE Behaviour OF altera_up_avalon_ethernet IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	
	-- Internal Registers
	SIGNAL	data_to_ethernet_chip	:STD_LOGIC_VECTOR(15 DOWNTO 0);	
	
	-- State Machine Registers
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
BEGIN
-- *****************************************************************************
-- *                         Finite State Machine(s)                           *
-- *****************************************************************************


-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				irq							<= '0';
				readdata						<= B"0000000000000000";
		
				data_to_ethernet_chip	<= B"0000000000000000";
		
				ENET_RST_N					<= '0';
				ENET_CS_N					<= '1';
				ENET_CMD						<= '0';
				ENET_RD_N					<= '1';
				ENET_WR_N					<= '1';
			ELSE
				irq							<= ENET_INT;
				readdata						<= ENET_DATA; -- {16'h0000,ENET_DATA};
		
				data_to_ethernet_chip	<= writedata(15 DOWNTO 0);
		
				ENET_RST_N					<= '1';
				ENET_CS_N					<= NOT (chipselect AND NOT (read AND NOT address));
				ENET_CMD						<= address;
				ENET_RD_N					<= NOT read;
				ENET_WR_N					<= NOT write;
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- assign ENET_CLK		= ethernet_clk_25MHz;

	ENET_DATA <= (OTHERS => 'Z') WHEN (ENET_WR_N = '1') ELSE data_to_ethernet_chip;

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
