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
-- * This module read and write data to the 16x2 Character LCD on the DE2       *
-- *  Board.                                                                    *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_character_lcd_communication IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	-- Timing info for minimum wait between consecutive communications 
	--   if using a 50MHz Clk
	CLOCK_CYCLES_FOR_IDLE_STATE		:STD_LOGIC_VECTOR(6 DOWNTO 0)		:= B"1111111";	-- Minimum 2500 ns
	IC											:INTEGER									:= 7;			-- Number of bits for idle counter
	IDLE_COUNTER_INCREMENT				:STD_LOGIC_VECTOR(6 DOWNTO 0)		:= B"0000001";
	
	CLOCK_CYCLES_FOR_OPERATION_STATE	:INTEGER									:= 3;			-- Minimum  40 ns
	CLOCK_CYCLES_FOR_ENABLE_STATE		:INTEGER									:= 15;		-- Minimum 230 ns
	CLOCK_CYCLES_FOR_HOLD_STATE		:INTEGER									:= 1;			-- Minimum  10 ns
	SC											:INTEGER									:= 4;			-- Number of bits for states counter
	COUNTER_INCREMENT						:STD_LOGIC_VECTOR(3 DOWNTO 0)		:= B"0001"
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk					:IN		STD_LOGIC;
	reset					:IN		STD_LOGIC;

	data_in				:IN		STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	rs						:IN		STD_LOGIC;
	rw						:IN		STD_LOGIC;
	enable				:IN		STD_LOGIC;

	display_on			:IN		STD_LOGIC;
	back_light_on		:IN		STD_LOGIC;

	-- Bidirectionals
	LCD_DATA				:INOUT	STD_LOGIC_VECTOR( 7 DOWNTO  0);	

	-- Outputs
	LCD_ON				:BUFFER	STD_LOGIC;
	LCD_BLON				:BUFFER	STD_LOGIC;

	LCD_EN				:BUFFER	STD_LOGIC;
	LCD_RS				:BUFFER	STD_LOGIC;
	LCD_RW				:BUFFER	STD_LOGIC;

	data_out				:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);		-- Stores data read from the LCD
	transfer_complete	:BUFFER	STD_LOGIC									-- Indicates the end of the transfer

);

END altera_up_character_lcd_communication;

ARCHITECTURE Behaviour OF altera_up_character_lcd_communication IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	-- states
	TYPE State_Type IS (	LCD_STATE_4_IDLE,
								LCD_STATE_0_OPERATION,
								LCD_STATE_1_ENABLE,
								LCD_STATE_2_HOLD,
								LCD_STATE_3_END
							);
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	
	-- Internal Registers
	SIGNAL	data_to_lcd			:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	
	SIGNAL	idle_counter		:STD_LOGIC_VECTOR(IC DOWNTO  1);	
	SIGNAL	state_0_counter	:STD_LOGIC_VECTOR(SC DOWNTO  1);	
	SIGNAL	state_1_counter	:STD_LOGIC_VECTOR(SC DOWNTO  1);	
	SIGNAL	state_2_counter	:STD_LOGIC_VECTOR(SC DOWNTO  1);	
	
	-- State Machine Registers
	SIGNAL	ns_lcd				:State_Type;	
	SIGNAL	s_lcd					:State_Type;	
	
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
				s_lcd <= LCD_STATE_4_IDLE;
			ELSE
				s_lcd <= ns_lcd;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (ns_lcd, s_lcd, idle_counter, enable, state_0_counter, state_1_counter, 
				state_2_counter)
	BEGIN
		ns_lcd <= LCD_STATE_4_IDLE;
	
	   CASE (s_lcd) IS
		WHEN LCD_STATE_4_IDLE =>
			IF ((idle_counter = CLOCK_CYCLES_FOR_IDLE_STATE) AND (enable = '1')) THEN
				ns_lcd <= LCD_STATE_0_OPERATION;
			ELSE
				ns_lcd <= LCD_STATE_4_IDLE;
			END IF;
		WHEN LCD_STATE_0_OPERATION =>
			IF (state_0_counter = CLOCK_CYCLES_FOR_OPERATION_STATE) THEN
				ns_lcd <= LCD_STATE_1_ENABLE;
			ELSE
				ns_lcd <= LCD_STATE_0_OPERATION;
			END IF;
		WHEN LCD_STATE_1_ENABLE =>
			IF (state_1_counter = CLOCK_CYCLES_FOR_ENABLE_STATE) THEN
				ns_lcd <= LCD_STATE_2_HOLD;
			ELSE
				ns_lcd <= LCD_STATE_1_ENABLE;
			END IF;
		WHEN LCD_STATE_2_HOLD =>
			IF (state_2_counter = CLOCK_CYCLES_FOR_HOLD_STATE) THEN
				ns_lcd <= LCD_STATE_3_END;
			ELSE
				ns_lcd <= LCD_STATE_2_HOLD;
			END IF;
		WHEN LCD_STATE_3_END =>
			IF (enable = '0') THEN
				ns_lcd <= LCD_STATE_4_IDLE;
			ELSE
				ns_lcd <= LCD_STATE_3_END;
			END IF;
		WHEN OTHERS =>
			ns_lcd <= LCD_STATE_4_IDLE;
		END CASE;
	END PROCESS;



-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				LCD_ON 	<= '0';
				LCD_BLON <= '0';
			ELSE
				LCD_ON 	<= display_on;
				LCD_BLON <= back_light_on;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				LCD_EN					<= '0';
				LCD_RS					<= '0';
				LCD_RW					<= '0';
				data_out					<= B"00000000";
				transfer_complete		<= '0';
			ELSE
				IF (s_lcd = LCD_STATE_1_ENABLE) THEN
					LCD_EN				<= '1';
				ELSE
					LCD_EN				<= '0';
		
			END IF;
				IF (s_lcd = LCD_STATE_4_IDLE) THEN
					LCD_RS				<= rs;
					LCD_RW				<= rw;
					
					data_to_lcd 		<= data_in;
		
			END IF;
				IF (s_lcd = LCD_STATE_1_ENABLE) THEN
					data_out				<= LCD_DATA;
		
			END IF;
				IF (s_lcd = LCD_STATE_3_END) THEN
					transfer_complete	<= '1';
				ELSE
					transfer_complete	<= '0';
				END IF;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				idle_counter <= (OTHERS => '0');
			ELSIF (s_lcd = LCD_STATE_4_IDLE) THEN
				idle_counter <= idle_counter + IDLE_COUNTER_INCREMENT;
			ELSE
				idle_counter <= (OTHERS => '0');
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				state_0_counter 		<= (OTHERS => '0');
				state_1_counter 		<= (OTHERS => '0');
				state_2_counter 		<= (OTHERS => '0');
			ELSE
				IF (s_lcd = LCD_STATE_0_OPERATION) THEN
					state_0_counter 	<= state_0_counter + COUNTER_INCREMENT;
				ELSE
					state_0_counter 	<= (OTHERS => '0');
		
			END IF;
				IF (s_lcd = LCD_STATE_1_ENABLE) THEN
					state_1_counter 	<= state_1_counter + COUNTER_INCREMENT;
				ELSE
					state_1_counter 	<= (OTHERS => '0');
		
			END IF;
				IF (s_lcd = LCD_STATE_2_HOLD) THEN
					state_2_counter 	<= state_2_counter + COUNTER_INCREMENT;
				ELSE
					state_2_counter 	<= (OTHERS => '0');
				END IF;
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	LCD_DATA <= data_to_lcd WHEN
			(((s_lcd = LCD_STATE_1_ENABLE) OR 
			  (s_lcd = LCD_STATE_2_HOLD)) AND 
			   (LCD_RW = '0')) ELSE (OTHERS => 'Z');

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
