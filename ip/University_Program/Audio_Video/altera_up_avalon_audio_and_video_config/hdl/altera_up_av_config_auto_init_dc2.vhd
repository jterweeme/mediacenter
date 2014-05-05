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
-- * This module is a rom for auto initializing the TRDB DC2 digital camera.    *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_av_config_auto_init_dc2 IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	DC_ROW_START		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000001100";
	DC_COLUMN_START	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000011110";
	DC_ROW_WIDTH		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000010000000000";
	DC_COLUMN_WIDTH	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000010100000000";
	DC_H_BLANK_B		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000010001000";
	DC_V_BLANK_B		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000011001";
	DC_H_BLANK_A		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000011000110";
	DC_V_BLANK_A		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000011001";
	DC_SHUTTER_WIDTH	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000010000110010";
	DC_ROW_SPEED		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000010001";
	DC_EXTRA_DELAY		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000000";
	DC_SHUTTER_DELAY	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000000";
	DC_RESET				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000001000";
	DC_FRAME_VALID		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000000";
	DC_READ_MODE_B		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000001";
	DC_READ_MODE_A		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000010000001100";
	DC_DARK_COL_ROW	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000100101001";
	DC_FLASH				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000011000001000";
	DC_GREEN_GAIN_1	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000010110000";
	DC_BLUE_GAIN		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000011001111";
	DC_RED_GAIN			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000011001111";
	DC_GREEN_GAIN_2	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000010110000";
	--parameter DC_GLOBAL_GAIN		= 16'h0120;
	DC_CONTEXT_CTRL	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000001011"
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	rom_address	:IN		STD_LOGIC_VECTOR( 4 DOWNTO  0);	

	-- Bidirectionals

	-- Outputs
	rom_data		:BUFFER	STD_LOGIC_VECTOR(35 DOWNTO  0)	

);

END altera_up_av_config_auto_init_dc2;

ARCHITECTURE Behaviour OF altera_up_av_config_auto_init_dc2 IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	SIGNAL	data	:STD_LOGIC_VECTOR(31 DOWNTO  0);	
	
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

	-- Internal Registers

-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments
	rom_data <= data(31 DOWNTO 24) & '0' & 
					data(23 DOWNTO 16) & '0' & 
					data(15 DOWNTO  8) & '0' & 
					data( 7 DOWNTO  0) & '0';

	-- Internal Assignments
	PROCESS (rom_address, data)
	BEGIN
		CASE (rom_address) IS
		WHEN B"00000" =>	data	<=	B"10111010" & B"00000001" & DC_ROW_START;
		WHEN B"00001" =>	data	<=	B"10111010" & B"00000010" & DC_COLUMN_START;
		WHEN B"00010" =>	data	<=	B"10111010" & B"00000011" & DC_ROW_WIDTH;
		WHEN B"00011" =>	data	<=	B"10111010" & B"00000100" & DC_COLUMN_WIDTH;
		WHEN B"00100" =>	data	<=	B"10111010" & B"00000101" & DC_H_BLANK_B;
		WHEN B"00101" =>	data	<=	B"10111010" & B"00000110" & DC_V_BLANK_B;
		WHEN B"00110" =>	data	<=	B"10111010" & B"00000111" & DC_H_BLANK_A;
		WHEN B"00111" =>	data	<=	B"10111010" & B"00001000" & DC_V_BLANK_A;
		WHEN B"01000" =>	data	<=	B"10111010" & B"00001001" & DC_SHUTTER_WIDTH;
		WHEN B"01001" =>	data	<=	B"10111010" & B"00001010" & DC_ROW_SPEED;
		WHEN B"01010" =>	data	<=	B"10111010" & B"00001011" & DC_EXTRA_DELAY;
		WHEN B"01011" =>	data	<=	B"10111010" & B"00001100" & DC_SHUTTER_DELAY;
		WHEN B"01100" =>	data	<=	B"10111010" & B"00001101" & DC_RESET;
		WHEN B"01101" =>	data	<=	B"10111010" & B"00011111" & DC_FRAME_VALID;
		WHEN B"01110" =>	data	<=	B"10111010" & B"00100000" & DC_READ_MODE_B;
		WHEN B"01111" =>	data	<=	B"10111010" & B"00100001" & DC_READ_MODE_A;
		WHEN B"10000" =>	data	<=	B"10111010" & B"00100010" & DC_DARK_COL_ROW;
		WHEN B"10001" =>	data	<=	B"10111010" & B"00100011" & DC_FLASH;
		WHEN B"10010" =>	data	<=	B"10111010" & B"00101011" & DC_GREEN_GAIN_1;
		WHEN B"10011" =>	data	<=	B"10111010" & B"00101100" & DC_BLUE_GAIN;
		WHEN B"10100" =>	data	<=	B"10111010" & B"00101101" & DC_RED_GAIN;
		WHEN B"10101" =>	data	<=	B"10111010" & B"00101110" & DC_GREEN_GAIN_2;
		WHEN B"10110" =>	data	<=	B"10111010" & B"11001000" & DC_CONTEXT_CTRL;
		WHEN OTHERS   =>	data	<=	B"00000000000000000000000000000000";
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
