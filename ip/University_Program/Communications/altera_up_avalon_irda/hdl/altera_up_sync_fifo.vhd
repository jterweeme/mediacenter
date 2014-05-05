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
-- * This module is a FIFO with same clock for both reads and writes.           *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_sync_fifo IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	DW				:INTEGER									:= 31;		-- Data width
	DATA_DEPTH	:INTEGER									:= 128;
	AW				:INTEGER									:= 6			-- Address width
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (

	-- Inputs
	clk				:IN		STD_LOGIC;
	reset				:IN		STD_LOGIC;

	write_en			:IN		STD_LOGIC;
	write_data		:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	

	read_en			:IN		STD_LOGIC;

	-- Bidirectionals

	-- Outputs
	fifo_is_empty	:BUFFER	STD_LOGIC;
	fifo_is_full	:BUFFER	STD_LOGIC;
	words_used		:BUFFER	STD_LOGIC_VECTOR(AW DOWNTO  0);	

	read_data		:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0)	

);

END altera_up_sync_fifo;

ARCHITECTURE Behaviour OF altera_up_sync_fifo IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************

-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT scfifo
	GENERIC (
		add_ram_output_register	:STRING;
		intended_device_family	:STRING;
		lpm_numwords				:INTEGER;
		lpm_showahead				:STRING;
		lpm_type						:STRING;
		lpm_width					:INTEGER;
		lpm_widthu					:INTEGER;
		overflow_checking			:STRING;
		underflow_checking		:STRING;
		use_eab						:STRING
	);
	PORT (
		-- Inputs
		clock				:IN		STD_LOGIC;
		sclr				:IN		STD_LOGIC;

		data				:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);
		wrreq				:IN		STD_LOGIC;

		rdreq				:IN		STD_LOGIC;

		-- Bidirectionals

		-- Outputs
		empty				:BUFFER	STD_LOGIC;
		full				:BUFFER	STD_LOGIC;
		usedw				:BUFFER	STD_LOGIC_VECTOR(AW DOWNTO  0);
	
		q					:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0)

		-- synopsys translate_off
	
		-- synopsys translate_on
	);
	END COMPONENT;

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


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************


	Sync_FIFO : scfifo 
	GENERIC MAP (
		add_ram_output_register	=> "OFF",
		intended_device_family	=> "Cyclone II",
		lpm_numwords				=> DATA_DEPTH,
		lpm_showahead				=> "ON",
		lpm_type						=> "scfifo",
		lpm_width					=> DW + 1,
		lpm_widthu					=> AW + 1,
		overflow_checking			=> "OFF",
		underflow_checking		=> "OFF",
		use_eab						=> "ON"
	)
	PORT MAP (
		-- Inputs
		clock				=> clk,
		sclr				=> reset,
	
		data				=> write_data,
		wrreq				=> write_en,
	
		rdreq				=> read_en,
	
		-- Bidirectionals
	
		-- Outputs
		empty				=> fifo_is_empty,
		full				=> fifo_is_full,
		usedw				=> words_used,
		
		q					=> read_data
	
		-- synopsys translate_off
		
		-- synopsys translate_on
	);


END Behaviour;
