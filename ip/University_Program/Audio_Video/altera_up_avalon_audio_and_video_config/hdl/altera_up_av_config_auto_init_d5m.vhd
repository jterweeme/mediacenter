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
-- * This module is a rom for auto initializing the TRDB D5M digital camera.    *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_av_config_auto_init_d5m IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************

GENERIC (
	
	D5M_COLUMN_SIZE	:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000101000011111";
	D5M_ROW_SIZE		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000011110010111";
	D5M_COLUMN_BIN		:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000000";
	D5M_ROW_BIN			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000000"
	
);
parameter D5M_COLUMN_SIZE	= 16'd2591;
parameter D5M_ROW_SIZE		= 16'd1943;
parameter D5M_COLUMN_BIN	= 16'h0000;
parameter D5M_ROW_BIN		= 16'h0000;

-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	rom_address	:IN		STD_LOGIC_VECTOR( 4 DOWNTO  0);	

	exposure		:IN		STD_LOGIC_VECTOR(15 DOWNTO  0);	

	-- Bidirectionals

	-- Outputs
	rom_data		:BUFFER	STD_LOGIC_VECTOR(35 DOWNTO  0)	

);

END altera_up_av_config_auto_init_d5m;

ARCHITECTURE Behaviour OF altera_up_av_config_auto_init_d5m IS
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
		WHEN B"00000" =>	data	<= B"10111010" & B"00000000" & B"0000000000000000";
		WHEN B"00001" =>	data	<= B"10111010" & B"00100000" & B"1100000000000000"; -- Mirror Row and Columns
		WHEN B"00010" =>	data	<= B"10111010" & B"00001001" & exposure;				 -- Exposure
		WHEN B"00011" =>	data	<= B"10111010" & B"00000101" & B"0000000000000000"; -- H_Blanking
		WHEN B"00100" =>	data	<= B"10111010" & B"00000110" & B"0000000000011001"; -- V_Blanking	
		WHEN B"00101" =>	data	<= B"10111010" & B"00001010" & B"1000000000000000"; -- change latch
		WHEN B"00110" =>	data	<= B"10111010" & B"00101011" & B"0000000000001011"; -- Green 1 Gain
		WHEN B"00111" =>	data	<= B"10111010" & B"00101100" & B"0000000000001111"; -- Blue Gain
		WHEN B"01000" =>	data	<= B"10111010" & B"00101101" & B"0000000000001111"; -- Red Gain
		WHEN B"01001" =>	data	<= B"10111010" & B"00101110" & B"0000000000001011"; -- Green 2 Gain
		WHEN B"01010" =>	data	<= B"10111010" & B"00010000" & B"0000000001010001"; -- set up PLL power on
		WHEN B"01011" =>	data	<= B"10111010" & B"00010001" & B"0001100000000111"; -- PLL_m_Factor<<8+PLL_n_Divider
		WHEN B"01100" =>	data	<= B"10111010" & B"00010010" & B"0000000000000010"; -- PLL_p1_Divider
		WHEN B"01101" =>	data	<= B"10111010" & B"00010000" & B"0000000001010011"; -- set USE PLL	 
		WHEN B"01110" =>	data	<= B"10111010" & B"10011000" & B"0000000000000000"; -- disble calibration 	
		WHEN B"01111" =>	data	<= B"10111010" & B"10100000" & B"0000000000000000"; -- Test pattern control 
		WHEN B"10000" =>	data	<= B"10111010" & B"10100001" & B"0000000000000000"; -- Test green pattern value
		WHEN B"10001" =>	data	<= B"10111010" & B"10100010" & B"0000111111111111"; -- Test red pattern value
		WHEN B"10010" =>	data	<= B"10111010" & B"00000001" & B"0000000000110110"; -- set start row	
		WHEN B"10011" =>	data	<= B"10111010" & B"00000010" & B"0000000000010000"; -- set start column 	
		WHEN B"10100" =>	data	<= B"10111010" & B"00000011" & D5M_ROW_SIZE;			 -- set row size	
		WHEN B"10101" =>	data	<= B"10111010" & B"00000100" & D5M_COLUMN_SIZE;		 -- set column size
		WHEN B"10110" =>	data	<= B"10111010" & B"00100010" & D5M_ROW_BIN;			 -- set row mode in bin mode
		WHEN B"10111" =>	data	<= B"10111010" & B"00100011" & D5M_COLUMN_BIN;		 -- set column mode in bin mode
		WHEN B"11000" =>	data	<= B"10111010" & B"01001001" & B"0000000110101000"; -- row black target		
		WHEN OTHERS   =>	data	<= B"00000000" & B"00000000" & B"0000000000000000";
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
