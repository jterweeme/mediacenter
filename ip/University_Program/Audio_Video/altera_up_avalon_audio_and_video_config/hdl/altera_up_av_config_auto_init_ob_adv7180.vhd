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
-- * This module is a rom for auto initializing the on board ADV7180 video chip.*
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_av_config_auto_init_ob_adv7180 IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	INPUT_CTRL					:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000000";
	VIDEO_SELECTION			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000111001000";
	OUTPUT_CTRL					:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000001100001100";
	EXT_OUTPUT_CTRL			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000010001000101";
	AUTODETECT					:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000011101111111";
	BRIGHTNESS					:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000101000000000";
	HUE							:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000101100000000";
	DEFAULT_VALUE_Y			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000110000110110";
	DEFAULT_VALUE_C			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000110101111100";
	POWER_MGMT					:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000111100000000";
	ANALOG_CLAMP_CTRL			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0001010000010010";
	DIGITAL_CLAMP_CTRL		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0001010100000000";
	SHAPING_FILTER_CTRL_1	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0001011100000001";
	SHAPING_FILTER_CTRL_2	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0001100010010011";
	COMB_FILTER_CTRL_2		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0001100111110001";
	PIXEL_DELAY_CTRL			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0010011101011000";
	MISC_GAIN_CTRL				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0010101111100001";
	AGC_MODE_CTRL				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0010110010101110";
	CHROMA_GAIN_CTRL_1		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0010110111110100";
	CHROMA_GAIN_CTRL_2		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0010111000000000";
	LUMA_GAIN_CTRL_1			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0010111111110000";
	LUMA_GAIN_CTRL_2			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011000000000000";
	VSYNC_FIELD_CTRL_1		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011000100010010";
	VSYNC_FIELD_CTRL_2		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011001001000001";
	VSYNC_FIELD_CTRL_3		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011001110000100";
	HSYNC_FIELD_CTRL_1		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011010000000000";
	HSYNC_FIELD_CTRL_2		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011010100000010";
	HSYNC_FIELD_CTRL_3		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011011000000000";
	POLARITY						:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011011100000001";
	NTSC_COMB_CTRL				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011100010000000";
	PAL_COMB_CTRL				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011100111000000";
	ADC_CTRL						:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011101000010000";
	MANUAL_WINDOW_CTRL		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0011110110110010";
	RESAMPLE_CONTROL			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0100000100000001";
	CRC							:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1011001000011100";
	ADC_SWITCH_1				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1100001100000000";
	ADC_SWITCH_2				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1100010000000000";
	LETTERBOX_CTRL_1			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1101110010101100";
	LETTERBOX_CTRL_2			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1101110101001100";
	NTSC_V_BIT_BEGIN			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1110010100100101";
	NTSC_V_BIT_END				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1110011000000100";
	NTSC_F_BIT_TOGGLE			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1110011101100011";
	PAL_V_BIT_BEGIN			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1110100001100101";
	PAL_V_BIT_END				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1110100100010100";
	PAL_F_BIT_TOGGLE			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1110101001100011";
	VBLANK_CTRL_1				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1110101101010101";
	VBLANK_CTRL_2				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"1110110001010101"
	
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

END altera_up_av_config_auto_init_ob_adv7180;

ARCHITECTURE Behaviour OF altera_up_av_config_auto_init_ob_adv7180 IS
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
		WHEN B"001010" =>	data	<=	B"01000000" & INPUT_CTRL;
		WHEN B"001011" =>	data	<=	B"01000000" & VIDEO_SELECTION;
		WHEN B"001100" =>	data	<=	B"01000000" & OUTPUT_CTRL;
		WHEN B"001101" =>	data	<=	B"01000000" & EXT_OUTPUT_CTRL;
		WHEN B"001110" =>	data	<=	B"01000000" & AUTODETECT;
		WHEN B"001111" =>	data	<=	B"01000000" & BRIGHTNESS;
		WHEN B"010000" =>	data	<=	B"01000000" & HUE;
		WHEN B"010001" =>	data	<=	B"01000000" & DEFAULT_VALUE_Y;
		WHEN B"010010" =>	data	<=	B"01000000" & DEFAULT_VALUE_C;
		WHEN B"010011" =>	data	<=	B"01000000" & POWER_MGMT;
		WHEN B"010100" =>	data	<=	B"01000000" & ANALOG_CLAMP_CTRL;
		WHEN B"010101" =>	data	<=	B"01000000" & DIGITAL_CLAMP_CTRL;
		WHEN B"010110" =>	data	<=	B"01000000" & SHAPING_FILTER_CTRL_1;
		WHEN B"010111" =>	data	<=	B"01000000" & SHAPING_FILTER_CTRL_2;
		WHEN B"011000" =>	data	<=	B"01000000" & COMB_FILTER_CTRL_2;
		WHEN B"011001" =>	data	<=	B"01000000" & PIXEL_DELAY_CTRL;
		WHEN B"011010" =>	data	<=	B"01000000" & MISC_GAIN_CTRL;
		WHEN B"011011" =>	data	<=	B"01000000" & AGC_MODE_CTRL;
		WHEN B"011100" =>	data	<=	B"01000000" & CHROMA_GAIN_CTRL_1;
		WHEN B"011101" =>	data	<=	B"01000000" & CHROMA_GAIN_CTRL_2;
		WHEN B"011110" =>	data	<=	B"01000000" & LUMA_GAIN_CTRL_1;
		WHEN B"011111" =>	data	<=	B"01000000" & LUMA_GAIN_CTRL_2;
		WHEN B"100000" =>	data	<=	B"01000000" & VSYNC_FIELD_CTRL_1;
		WHEN B"100001" =>	data	<=	B"01000000" & VSYNC_FIELD_CTRL_2;
		WHEN B"100010" =>	data	<=	B"01000000" & VSYNC_FIELD_CTRL_3;
		WHEN B"100011" =>	data	<=	B"01000000" & HSYNC_FIELD_CTRL_1;
		WHEN B"100100" =>	data	<=	B"01000000" & HSYNC_FIELD_CTRL_2;
		WHEN B"100101" =>	data	<=	B"01000000" & HSYNC_FIELD_CTRL_3;
		WHEN B"100110" =>	data	<=	B"01000000" & POLARITY;
		WHEN B"100111" =>	data	<=	B"01000000" & NTSC_COMB_CTRL;
		WHEN B"101000" =>	data	<=	B"01000000" & PAL_COMB_CTRL;
		WHEN B"101001" =>	data	<=	B"01000000" & ADC_CTRL;
		WHEN B"101010" =>	data	<=	B"01000000" & MANUAL_WINDOW_CTRL;
		WHEN B"101011" =>	data	<=	B"01000000" & RESAMPLE_CONTROL;
		WHEN B"101100" =>	data	<=	B"01000000" & CRC;
		WHEN B"101101" =>	data	<=	B"01000000" & ADC_SWITCH_1;
		WHEN B"101110" =>	data	<=	B"01000000" & ADC_SWITCH_2;
		WHEN B"101111" =>	data	<=	B"01000000" & LETTERBOX_CTRL_1;
		WHEN B"110000" =>	data	<=	B"01000000" & LETTERBOX_CTRL_2;
		WHEN B"110001" =>	data	<=	B"01000000" & NTSC_V_BIT_BEGIN;
		WHEN B"110010" =>	data	<=	B"01000000" & NTSC_V_BIT_END;
		WHEN B"110011" =>	data	<=	B"01000000" & NTSC_F_BIT_TOGGLE;
		WHEN B"110100" =>	data	<=	B"01000000" & PAL_V_BIT_BEGIN;
		WHEN B"110101" =>	data	<=	B"01000000" & PAL_V_BIT_END;
		WHEN B"110110" =>	data	<=	B"01000000" & PAL_F_BIT_TOGGLE;
		WHEN B"110111" =>	data	<=	B"01000000" & VBLANK_CTRL_1;
		WHEN B"111000" =>	data	<=	B"01000000" & VBLANK_CTRL_2;
		WHEN OTHERS =>	data	<=	B"00000000" & B"0000000000000000";
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
