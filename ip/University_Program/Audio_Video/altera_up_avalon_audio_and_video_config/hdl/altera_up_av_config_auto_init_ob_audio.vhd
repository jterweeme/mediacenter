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
-- * This module is a rom for auto initializing the on board audio chip.        *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_av_config_auto_init_ob_audio IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	AUD_LINE_IN_LC		:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"000011010";
	AUD_LINE_IN_RC		:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"000011010";
	AUD_LINE_OUT_LC	:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"001111011";
	AUD_LINE_OUT_RC	:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"001111011";
	AUD_ADC_PATH		:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"011111000";
	AUD_DAC_PATH		:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"000000110";
	AUD_POWER			:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"000000000";
	AUD_DATA_FORMAT	:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"000000001";
	AUD_SAMPLE_CTRL	:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"000000010";
	AUD_SET_ACTIVE		:STD_LOGIC_VECTOR( 8 DOWNTO  0)	:= B"000000001"
	
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

END altera_up_av_config_auto_init_ob_audio;

ARCHITECTURE Behaviour OF altera_up_av_config_auto_init_ob_audio IS
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
		--	Audio Config Data
		WHEN B"000000" =>	data	<=	B"00110100" & B"0000000" & AUD_LINE_IN_LC;
		WHEN B"000001" =>	data	<=	B"00110100" & B"0000001" & AUD_LINE_IN_RC;
		WHEN B"000010" =>	data	<=	B"00110100" & B"0000010" & AUD_LINE_OUT_LC;
		WHEN B"000011" =>	data	<=	B"00110100" & B"0000011" & AUD_LINE_OUT_RC;
		WHEN B"000100" =>	data	<=	B"00110100" & B"0000100" & AUD_ADC_PATH;
		WHEN B"000101" =>	data	<=	B"00110100" & B"0000101" & AUD_DAC_PATH;
		WHEN B"000110" =>	data	<=	B"00110100" & B"0000110" & AUD_POWER;
		WHEN B"000111" =>	data	<=	B"00110100" & B"0000111" & AUD_DATA_FORMAT;
		WHEN B"001000" =>	data	<=	B"00110100" & B"0001000" & AUD_SAMPLE_CTRL;
		WHEN B"001001" =>	data	<=	B"00110100" & B"0001001" & AUD_SET_ACTIVE;
		WHEN OTHERS    =>	data	<=	B"00000000" & B"0000000" & B"000000000";
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
