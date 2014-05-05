LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;
USE ieee.numeric_std.all;

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
-- * This module connects the an External Bus to Avalon Switch Fabric           *
-- *  It acts as both a master for the avalon swtich fabric                     *
-- *  and a slave to the master peripheral                                      *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_external_bus_to_avalon_bridge IS 


-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	AW	:INTEGER									:= 17;	-- Address width
	DW	:INTEGER									:= 16;	-- Data width
	
	BW	:INTEGER									:= 1	-- Byte enable width
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk						:IN		STD_LOGIC;
	reset						:IN		STD_LOGIC;

	avalon_waitrequest	:IN		STD_LOGIC;
	avalon_readdata		:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	

	address					:IN		STD_LOGIC_VECTOR(AW DOWNTO  0);	
	byte_enable				:IN		STD_LOGIC_VECTOR(BW DOWNTO  0);	
	write						:IN		STD_LOGIC;
	read						:IN		STD_LOGIC;
	write_data				:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	

	-- Bidirectionals

	-- Outputs
	avalon_address			:BUFFER	STD_LOGIC_VECTOR(AW DOWNTO  0);	
	avalon_byteenable		:BUFFER	STD_LOGIC_VECTOR(BW DOWNTO  0);	
	avalon_read				:BUFFER	STD_LOGIC;
	avalon_write			:BUFFER	STD_LOGIC;
	avalon_writedata		:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	

	acknowledge				:BUFFER	STD_LOGIC;
	read_data				:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0)	

);

END altera_up_external_bus_to_avalon_bridge;

ARCHITECTURE Behaviour OF altera_up_external_bus_to_avalon_bridge IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************

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

-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************
	read_data 			<= avalon_readdata;
	avalon_address 	<= address;
	avalon_byteenable <= byte_enable;
	avalon_write 		<= write;
	avalon_read 		<= read;
	avalon_writedata 	<= write_data;
	acknowledge 		<= NOT avalon_waitrequest AND (avalon_read OR avalon_write);

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************


END Behaviour;
