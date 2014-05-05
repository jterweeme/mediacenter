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
-- * This module is a rom for auto initializing the on board ADV7181 video chip.*
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_av_config_auto_init_ob_adv7181 IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	INPUT_CONTROL	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000001000000";
	CHROMA_GAIN_1	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0010110111110100";
	CHROMA_GAIN_2	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0010111000000000"
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	rom_address	:IN		STD_LOGIC_VECTOR( 5 DOWNTO  0);	

	-- Bidirectionals

	-- Outputs
	rom_data		:BUFFER	STD_LOGIC_VECTOR(26 DOWNTO  0)	

);

END altera_up_av_config_auto_init_ob_adv7181;

ARCHITECTURE Behaviour OF altera_up_av_config_auto_init_ob_adv7181 IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	SIGNAL	data	:STD_LOGIC_VECTOR(23 DOWNTO  0);	
	
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
	rom_data <= data(23 DOWNTO 16) & '0' & 
					data(15 DOWNTO  8) & '0' & 
					data( 7 DOWNTO  0) & '0';

	-- Internal Assignments
	PROCESS (rom_address, data)
	BEGIN
		CASE (rom_address) IS
		--	Video Config Data
		WHEN B"001010" =>	data	<=	B"01000000" & B"0001010100000000";
		WHEN B"001011" =>	data	<=	B"01000000" & B"0001011101000001";
		WHEN B"001100" =>	data	<=	B"01000000" & B"0011101000010110";
		WHEN B"001101" =>	data	<=	B"01000000" & B"0101000000000100";
		WHEN B"001110" =>	data	<=	B"01000000" & B"1100001100000101";
		WHEN B"001111" =>	data	<=	B"01000000" & B"1100010010000000";
		WHEN B"010000" =>	data	<=	B"01000000" & B"0000111010000000";
		WHEN B"010001" =>	data	<=	B"01000000" & B"0101000000000100";
		WHEN B"010010" =>	data	<=	B"01000000" & B"0101001000011000";
		WHEN B"010011" =>	data	<=	B"01000000" & B"0101100011101101";
		WHEN B"010100" =>	data	<=	B"01000000" & B"0111011111000101";
		WHEN B"010101" =>	data	<=	B"01000000" & B"0111110010010011";
		WHEN B"010110" =>	data	<=	B"01000000" & B"0111110100000000";
		WHEN B"010111" =>	data	<=	B"01000000" & B"1101000001001000";
		WHEN B"011000" =>	data	<=	B"01000000" & B"1101010110100000";
		WHEN B"011001" =>	data	<=	B"01000000" & B"1101011111101010";
		WHEN B"011010" =>	data	<=	B"01000000" & B"1110010000111110";
		WHEN B"011011" =>	data	<=	B"01000000" & B"1110101000001111";
		WHEN B"011100" =>	data	<=	B"01000000" & B"0011000100010010";
		WHEN B"011101" =>	data	<=	B"01000000" & B"0011001010000001";
		WHEN B"011110" =>	data	<=	B"01000000" & B"0011001110000100";
		WHEN B"011111" =>	data	<=	B"01000000" & B"0011011110100000";
		WHEN B"100000" =>	data	<=	B"01000000" & B"1110010110000000";
		WHEN B"100001" =>	data	<=	B"01000000" & B"1110011000000011";
		WHEN B"100010" =>	data	<=	B"01000000" & B"1110011110000101";
		WHEN B"100011" =>	data	<=	B"01000000" & B"0101000000000100";
		WHEN B"100100" =>	data	<=	B"01000000" & B"0101000100000000";
		WHEN B"100101" =>	data	<=	B"01000000" & INPUT_CONTROL;
		WHEN B"100110" =>	data	<=	B"01000000" & B"0001000000000000";
		WHEN B"100111" =>	data	<=	B"01000000" & B"0000010000000010";
		WHEN B"101000" =>	data	<=	B"01000000" & B"0000100001100000";
		WHEN B"101001" =>	data	<=	B"01000000" & B"0000101000011000";
		WHEN B"101010" =>	data	<=	B"01000000" & B"0001000100000000";
		WHEN B"101011" =>	data	<=	B"01000000" & B"0010101100000000";
		WHEN B"101100" =>	data	<=	B"01000000" & B"0010110010001100";
		WHEN B"101101" =>	data	<=	B"01000000" & CHROMA_GAIN_1;
		WHEN B"101110" =>	data	<=	B"01000000" & CHROMA_GAIN_2;
		WHEN B"101111" =>	data	<=	B"01000000" & B"0010111111110100";
		WHEN B"110000" =>	data	<=	B"01000000" & B"0011000011010010";
		WHEN B"110001" =>	data	<=	B"01000000" & B"0000111000000101";
		WHEN OTHERS    =>	data	<=	B"00000000" & B"0000000000000000";
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
