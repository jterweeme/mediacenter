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
-- * This module is a rom for auto initializing the TRDB LCM lcd screen.        *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_av_config_auto_init_lcm IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	LCM_INPUT_FORMAT_UB				:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00000000";
	LCM_INPUT_FORMAT_LB				:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00000001";
	LCM_POWER							:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00111111";
	LCM_DIRECTION_AND_PHASE			:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00010111";
	LCM_HORIZONTAL_START_POSITION	:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00011000";
	LCM_VERTICAL_START_POSITION	:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00001000";
	LCM_ENB_NEGATIVE_POSITION		:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00000000";
	LCM_GAIN_OF_CONTRAST				:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00100000";
	LCM_R_GAIN_OF_SUB_CONTRAST		:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00100000";
	LCM_B_GAIN_OF_SUB_CONTRAST		:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00100000";
	LCM_OFFSET_OF_BRIGHTNESS		:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00010000";
	LCM_VCOM_HIGH_LEVEL				:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00111111";
	LCM_VCOM_LOW_LEVEL				:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00111111";
	LCM_PCD_HIGH_LEVEL				:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00101111";
	LCM_PCD_LOW_LEVEL					:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00101111";
	LCM_GAMMA_CORRECTION_0			:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"10011000";
	LCM_GAMMA_CORRECTION_1			:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"10011010";
	LCM_GAMMA_CORRECTION_2			:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"10101001";
	LCM_GAMMA_CORRECTION_3			:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"10011001";
	LCM_GAMMA_CORRECTION_4			:STD_LOGIC_VECTOR( 7 DOWNTO  0)	:= B"00001000"
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	rom_address	:IN		STD_LOGIC_VECTOR( 4 DOWNTO  0);	

	-- Bidirectionals

	-- Outputs
	rom_data		:BUFFER	STD_LOGIC_VECTOR(15 DOWNTO  0)	

);

END altera_up_av_config_auto_init_lcm;

ARCHITECTURE Behaviour OF altera_up_av_config_auto_init_lcm IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	SIGNAL	data	:STD_LOGIC_VECTOR(13 DOWNTO  0);	
	
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
	rom_data <= data(13 DOWNTO  8) & B"00" & 
					data( 7 DOWNTO  0);

	-- Internal Assignments
	PROCESS (rom_address, data)
	BEGIN
		CASE (rom_address) IS
		WHEN B"00000" =>	data	<=	B"000010" & LCM_INPUT_FORMAT_UB;
		WHEN B"00001" =>	data	<=	B"000011" & LCM_INPUT_FORMAT_LB;
		WHEN B"00010" =>	data	<=	B"000100" & LCM_POWER;
		WHEN B"00011" =>	data	<=	B"000101" & LCM_DIRECTION_AND_PHASE;
		WHEN B"00100" =>	data	<=	B"000110" & LCM_HORIZONTAL_START_POSITION;
		WHEN B"00101" =>	data	<=	B"000111" & LCM_VERTICAL_START_POSITION;
		WHEN B"00110" =>	data	<=	B"001000" & LCM_ENB_NEGATIVE_POSITION;
		WHEN B"00111" =>	data	<=	B"001001" & LCM_GAIN_OF_CONTRAST;
		WHEN B"01000" =>	data	<=	B"001010" & LCM_R_GAIN_OF_SUB_CONTRAST;
		WHEN B"01001" =>	data	<=	B"001011" & LCM_B_GAIN_OF_SUB_CONTRAST;
		WHEN B"01010" =>	data	<=	B"001100" & LCM_OFFSET_OF_BRIGHTNESS;
		WHEN B"01011" =>	data	<=	B"010000" & LCM_VCOM_HIGH_LEVEL;
		WHEN B"01100" =>	data	<=	B"010001" & LCM_VCOM_LOW_LEVEL;
		WHEN B"01101" =>	data	<=	B"010010" & LCM_PCD_HIGH_LEVEL;
		WHEN B"01110" =>	data	<=	B"010011" & LCM_PCD_LOW_LEVEL;
		WHEN B"01111" =>	data	<=	B"010100" & LCM_GAMMA_CORRECTION_0;
		WHEN B"10000" =>	data	<=	B"010101" & LCM_GAMMA_CORRECTION_1;
		WHEN B"10001" =>	data	<=	B"010110" & LCM_GAMMA_CORRECTION_2;
		WHEN B"10010" =>	data	<=	B"010111" & LCM_GAMMA_CORRECTION_3;
		WHEN B"10011" =>	data	<=	B"011000" & LCM_GAMMA_CORRECTION_4;
		WHEN OTHERS   =>	data	<=	B"00000000000000";
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
