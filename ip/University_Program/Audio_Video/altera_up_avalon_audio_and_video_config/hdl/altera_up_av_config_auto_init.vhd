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
-- * This module loads data into the Audio and Video chips' control             *
-- *  registers after system reset.                                             *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_av_config_auto_init IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	ROM_SIZE	:INTEGER									:= 50;
	
	AW			:INTEGER									:= 5;		-- Auto Initialize ROM's address width 
	DW			:INTEGER									:= 23		-- Auto Initialize ROM's datawidth
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk						:IN		STD_LOGIC;
	reset						:IN		STD_LOGIC;

	clear_error				:IN		STD_LOGIC;

	ack						:IN		STD_LOGIC;
	transfer_complete		:IN		STD_LOGIC;

	rom_data					:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	

	-- Bidirectionals

	-- Outputs
	data_out					:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	
	transfer_data			:BUFFER	STD_LOGIC;

	rom_address				:BUFFER	STD_LOGIC_VECTOR(AW DOWNTO  0);	

	auto_init_complete	:BUFFER	STD_LOGIC;
	auto_init_error		:BUFFER	STD_LOGIC

);

END altera_up_av_config_auto_init;

ARCHITECTURE Behaviour OF altera_up_av_config_auto_init IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	SIGNAL	toggle_next_transfer	:STD_LOGIC;
	
	-- Internal Registers
	
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

	-- Output Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				data_out <= (OTHERS => '0');
			ELSE
				data_out <= rom_data;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				transfer_data <= '0';
			ELSIF ((auto_init_complete = '1') OR (transfer_complete = '1')) THEN
				transfer_data <= '0';
			ELSE
				transfer_data <= '1';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				rom_address <= (OTHERS => '0');
			ELSIF (toggle_next_transfer = '1') THEN
				rom_address <= rom_address + 1;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				auto_init_complete <= '0';
			ELSIF ((toggle_next_transfer = '1') AND (rom_address = (ROM_SIZE - 1))) THEN
				auto_init_complete <= '1';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				auto_init_error <= '0';
			ELSIF ((toggle_next_transfer = '1') AND (ack = '1')) THEN
				auto_init_error <= '1';
			ELSIF (clear_error = '1') THEN
				auto_init_error <= '0';
			END IF;
		END IF;
	END PROCESS;


	-- Internal Registers

-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments

	-- Internal Assignments
	toggle_next_transfer <= transfer_data AND transfer_complete;

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
