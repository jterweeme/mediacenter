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
-- * This module reads and writes data to the IrDA connectpr on Altera's        *
-- *  DE1 and DE2 Development and Education Boards.                             *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_irda_counters IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	CW								:INTEGER		:= 9;		-- BAUD COUNTER WIDTH
	BAUD_TICK_COUNT			:INTEGER		:= 433;
	BAUD_3_16_TICK_COUNT		:INTEGER		:= 81;
	CAPTURE_IN_TICK_COUNT	:INTEGER		:= 60;
	HALF_BAUD_TICK_COUNT		:INTEGER		:= 216;
	
	TDW							:INTEGER		:= 11		-- TOTAL DATA WIDTH
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk							:IN		STD_LOGIC;
	reset							:IN		STD_LOGIC;

	reset_counters				:IN		STD_LOGIC;

	-- Bidirectionals

	-- Outputs
	baud_clock_rising_edge	:BUFFER	STD_LOGIC;
	baud_clock_falling_edge	:BUFFER	STD_LOGIC;
	capture_in_bit				:BUFFER	STD_LOGIC;
	transmitting_bit			:BUFFER	STD_LOGIC;
	all_bits_transmitted		:BUFFER	STD_LOGIC

);

END altera_up_irda_counters;

ARCHITECTURE Behaviour OF altera_up_irda_counters IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	
	-- Internal Registers
	SIGNAL	baud_counter	:STD_LOGIC_VECTOR((CW-1) DOWNTO  0);	
	SIGNAL	bit_counter		:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	
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
				baud_counter <= (OTHERS => '0');
			ELSIF (reset_counters = '1') THEN
				baud_counter <= (OTHERS => '0');
			ELSIF (baud_counter = BAUD_TICK_COUNT) THEN
				baud_counter <= (OTHERS => '0');
			ELSE
				baud_counter <= baud_counter + 1;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				baud_clock_rising_edge <= '0';
			ELSIF (baud_counter = BAUD_TICK_COUNT) THEN
				baud_clock_rising_edge <= '1';
			ELSE
				baud_clock_rising_edge <= '0';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				baud_clock_falling_edge <= '0';
			ELSIF (baud_counter = HALF_BAUD_TICK_COUNT) THEN
				baud_clock_falling_edge <= '1';
			ELSE
				baud_clock_falling_edge <= '0';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				capture_in_bit <= '0';
			ELSIF (baud_counter = CAPTURE_IN_TICK_COUNT) THEN
				capture_in_bit <= '1';
			ELSE
				capture_in_bit <= '0';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				transmitting_bit <= '0';
			ELSIF (reset_counters = '1') THEN
				transmitting_bit <= '1';
			ELSIF (bit_counter = TDW) THEN
				transmitting_bit <= '0';
			ELSIF (baud_counter = BAUD_3_16_TICK_COUNT) THEN
				transmitting_bit <= '0';
			ELSIF (baud_clock_rising_edge = '1') THEN
				transmitting_bit <= '1';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				bit_counter <= B"0000";
			ELSIF (reset_counters = '1') THEN
				bit_counter <= B"0000";
			ELSIF (bit_counter = TDW) THEN
				bit_counter <= B"0000";
			ELSIF (baud_counter = BAUD_TICK_COUNT) THEN
				bit_counter <= bit_counter + B"0001";
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				all_bits_transmitted <= '0';
			ELSIF (bit_counter = TDW) THEN
				all_bits_transmitted <= '1';
			ELSE
				all_bits_transmitted <= '0';
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
