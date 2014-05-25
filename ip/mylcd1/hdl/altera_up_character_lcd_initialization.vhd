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
-- * This module runs through the 16x2 Character LCD initialization             *
-- *  commands for the DE2 Board.                                               *
-- *                                                                            *
-- ******************************************************************************


ENTITY altera_up_character_lcd_initialization IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	CURSOR_ON						:STD_LOGIC								:= '1';
	BLINKING_ON						:STD_LOGIC								:= '1';
	
	-- Timing info for waiting for power up 
	--   when using a 50MHz system clock
	CLOCK_CYCLES_FOR_15MS		:INTEGER									:= 750000;
	W15								:INTEGER									:= 20;	-- Counter width required for 15ms
	COUNTER_INCREMENT_FOR_15MS	:STD_LOGIC_VECTOR(19 DOWNTO 0)	:= B"00000000000000000001";
	
	-- Timing info for waiting between commands 
	--   when using a 50MHz system clock
	CLOCK_CYCLES_FOR_5MS			:INTEGER									:= 250000;
	W5									:INTEGER									:= 18;	-- Counter width required for 5ms
	COUNTER_INCREMENT_FOR_5MS	:STD_LOGIC_VECTOR(17 DOWNTO 0)	:= B"000000000000000001"
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk							:IN		STD_LOGIC;
	reset							:IN		STD_LOGIC;

	initialize_LCD_display	:IN		STD_LOGIC;

	command_was_sent			:IN		STD_LOGIC;

	-- Bidirectionals

	-- Outputs
	done_initialization		:BUFFER	STD_LOGIC;

	send_command				:BUFFER	STD_LOGIC;
	the_command					:BUFFER	STD_LOGIC_VECTOR( 8 DOWNTO  0)	

);

END altera_up_character_lcd_initialization;

ARCHITECTURE Behaviour OF altera_up_character_lcd_initialization IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	-- States
	TYPE State_Type IS (	LCD_INIT_STATE_0_WAIT_POWER_UP,
								LCD_INIT_STATE_1_SEND_COMMAND,
								LCD_INIT_STATE_2_CHECK_DONE,
								LCD_INIT_STATE_3_DONE
							);
	
	CONSTANT	AUTO_INIT_LENGTH						:STD_LOGIC_VECTOR( 3 DOWNTO  0)	:= B"1000";
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	SIGNAL	command_rom			:STD_LOGIC_VECTOR( 8 DOWNTO  0);	
	
	-- Internal Registers
	SIGNAL	waiting_power_up	:STD_LOGIC_VECTOR(W15 DOWNTO  1);	
	SIGNAL	waiting_to_send	:STD_LOGIC_VECTOR(W5 DOWNTO  1);	
	
	SIGNAL	command_counter	:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	
	
	-- State Machine Registers
	SIGNAL	ns_lcd_initialize	:State_Type;	
	SIGNAL	s_lcd_initialize	:State_Type;	
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
BEGIN
-- *****************************************************************************
-- *                         Finite State Machine(s)                           *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				s_lcd_initialize <= LCD_INIT_STATE_0_WAIT_POWER_UP;
			ELSE
				s_lcd_initialize <= ns_lcd_initialize;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (ns_lcd_initialize, s_lcd_initialize, waiting_power_up, 
				initialize_LCD_display, command_was_sent, command_counter, waiting_to_send)
	BEGIN
		-- Defaults
		ns_lcd_initialize <= LCD_INIT_STATE_0_WAIT_POWER_UP;
	
	   CASE (s_lcd_initialize) IS
		WHEN LCD_INIT_STATE_0_WAIT_POWER_UP =>
			IF ((waiting_power_up = CLOCK_CYCLES_FOR_15MS) AND 
					(initialize_LCD_display = '1')) THEN
				ns_lcd_initialize <= LCD_INIT_STATE_1_SEND_COMMAND;
			ELSE
				ns_lcd_initialize <= LCD_INIT_STATE_0_WAIT_POWER_UP;
			END IF;
		WHEN LCD_INIT_STATE_1_SEND_COMMAND =>
			IF (command_was_sent = '1') THEN
				ns_lcd_initialize <= LCD_INIT_STATE_2_CHECK_DONE;
			ELSE
				ns_lcd_initialize <= LCD_INIT_STATE_1_SEND_COMMAND;
			END IF;
		WHEN LCD_INIT_STATE_2_CHECK_DONE =>
			IF (command_counter = AUTO_INIT_LENGTH) THEN
				ns_lcd_initialize <= LCD_INIT_STATE_3_DONE;
			ELSIF (waiting_to_send = CLOCK_CYCLES_FOR_5MS) THEN
				ns_lcd_initialize <= LCD_INIT_STATE_1_SEND_COMMAND;
			ELSE
				ns_lcd_initialize <= LCD_INIT_STATE_2_CHECK_DONE;
			END IF;
		WHEN LCD_INIT_STATE_3_DONE =>
			IF (initialize_LCD_display = '1') THEN
				ns_lcd_initialize <= LCD_INIT_STATE_3_DONE;
			ELSE
				ns_lcd_initialize <= LCD_INIT_STATE_0_WAIT_POWER_UP;
			END IF;
		WHEN OTHERS =>
			ns_lcd_initialize <= LCD_INIT_STATE_0_WAIT_POWER_UP;
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************

	-- Output Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				the_command <= B"000000000";
			ELSE
				the_command <= command_rom;
			END IF;
		END IF;
	END PROCESS;


	-- Internal Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				waiting_power_up <= (OTHERS => '0');
			ELSIF ((s_lcd_initialize = LCD_INIT_STATE_0_WAIT_POWER_UP) AND 
						(waiting_power_up /= CLOCK_CYCLES_FOR_15MS)) THEN
				waiting_power_up <= waiting_power_up + COUNTER_INCREMENT_FOR_15MS;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				waiting_to_send <= (OTHERS => '0');
			ELSIF (s_lcd_initialize = LCD_INIT_STATE_2_CHECK_DONE) THEN
				IF (waiting_to_send /= CLOCK_CYCLES_FOR_5MS) THEN
					waiting_to_send <= waiting_to_send + COUNTER_INCREMENT_FOR_5MS;
			END IF;
			ELSE
				waiting_to_send <= (OTHERS => '0');
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				command_counter <= B"0000";
			ELSIF (s_lcd_initialize = LCD_INIT_STATE_1_SEND_COMMAND) THEN
				IF (command_was_sent = '1') THEN
					command_counter <= command_counter + B"0001";
			END IF;
			ELSIF (s_lcd_initialize = LCD_INIT_STATE_3_DONE) THEN
				command_counter <= B"0101";
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments
	send_command <= '1' WHEN (s_lcd_initialize = LCD_INIT_STATE_1_SEND_COMMAND) ELSE
						 '0';
	done_initialization <= '1' WHEN (s_lcd_initialize = LCD_INIT_STATE_3_DONE) ELSE
									'0';

	-- Internal Assignments
	PROCESS (command_counter, command_rom)
	BEGIN
		CASE (command_counter) IS
		WHEN B"0000" =>	command_rom	<=	B"000110000";
		WHEN B"0001" =>	command_rom	<=	B"000110000";
		WHEN B"0010" =>	command_rom	<=	B"000110000";
		WHEN B"0011" =>	command_rom	<=	B"000111100";
		WHEN B"0100" =>	command_rom	<=	B"000001000";
		WHEN B"0101" =>	command_rom	<=	B"000000001";
		WHEN B"0110" =>	command_rom	<=	B"000000110";
		WHEN B"0111" =>	command_rom	<=	B"0000011" & CURSOR_ON & BLINKING_ON;
		WHEN OTHERS  =>	command_rom	<=	B"000000000";
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
