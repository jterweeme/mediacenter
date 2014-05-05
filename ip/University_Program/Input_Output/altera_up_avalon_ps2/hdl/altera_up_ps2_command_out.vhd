LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.std_logic_unsigned.all;
USE ieee.std_logic_misc.all;
USE ieee.numeric_std.all;

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
-- * This module sends commands out to the PS2 core.                            *
-- *                                                                            *
-- ******************************************************************************


ENTITY altera_up_ps2_command_out IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	-- Timing info for initiating Host-to-Device communication 
	--   when using a 50MHz system clock
	CLOCK_CYCLES_FOR_101US	:INTEGER									:= 5050;
	DATA_WIDTH_FOR_101US		:INTEGER									:= 13;
	
	-- Timing info for start of transmission error 
	--   when using a 50MHz system clock
	CLOCK_CYCLES_FOR_15MS	:INTEGER									:= 750000;
	DATA_WIDTH_FOR_15MS		:INTEGER									:= 20;
	
	-- Timing info for sending data error 
	--   when using a 50MHz system clock
	CLOCK_CYCLES_FOR_2MS		:INTEGER									:= 100000;
	DATA_WIDTH_FOR_2MS		:INTEGER									:= 17
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk									:IN		STD_LOGIC;
	reset									:IN		STD_LOGIC;

	the_command							:IN		STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	send_command						:IN		STD_LOGIC;

	ps2_clk_posedge					:IN		STD_LOGIC;
	ps2_clk_negedge					:IN		STD_LOGIC;

	-- Bidirectionals
	PS2_CLK								:INOUT	STD_LOGIC;
	PS2_DAT								:INOUT	STD_LOGIC;

	-- Outputs
	command_was_sent					:BUFFER	STD_LOGIC;
	error_communication_timed_out	:BUFFER	STD_LOGIC

);

END altera_up_ps2_command_out;

ARCHITECTURE Behaviour OF altera_up_ps2_command_out IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	-- states
	TYPE State_Type IS (	PS2_STATE_0_IDLE,
								PS2_STATE_1_INITIATE_COMMUNICATION,
								PS2_STATE_2_WAIT_FOR_CLOCK,
								PS2_STATE_3_TRANSMIT_DATA,
								PS2_STATE_4_TRANSMIT_STOP_BIT,
								PS2_STATE_5_RECEIVE_ACK_BIT,
								PS2_STATE_6_COMMAND_WAS_SENT,
								PS2_STATE_7_TRANSMISSION_ERROR
							);
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	
	-- Internal Registers
	SIGNAL	cur_bit							:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	SIGNAL	ps2_command						:STD_LOGIC_VECTOR( 8 DOWNTO  0);	
	
	SIGNAL	command_initiate_counter	:STD_LOGIC_VECTOR(DATA_WIDTH_FOR_101US DOWNTO  1);	
	
	SIGNAL	waiting_counter				:STD_LOGIC_VECTOR(DATA_WIDTH_FOR_15MS DOWNTO  1);	
	SIGNAL	transfer_counter				:STD_LOGIC_VECTOR(DATA_WIDTH_FOR_2MS DOWNTO  1);	
	
	-- State Machine Registers
	SIGNAL	ns_ps2_transmitter			:State_Type;	
	SIGNAL	s_ps2_transmitter				:State_Type;	
	
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
				s_ps2_transmitter <= PS2_STATE_0_IDLE;
			ELSE
				s_ps2_transmitter <= ns_ps2_transmitter;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (ns_ps2_transmitter, s_ps2_transmitter, send_command, 
				command_initiate_counter, ps2_clk_negedge, waiting_counter, cur_bit, 
				transfer_counter, ps2_clk_posedge)
	BEGIN
		-- Defaults
		ns_ps2_transmitter <= PS2_STATE_0_IDLE;
	
	   CASE (s_ps2_transmitter) IS
		WHEN PS2_STATE_0_IDLE =>
			IF (send_command = '1') THEN
				ns_ps2_transmitter <= PS2_STATE_1_INITIATE_COMMUNICATION;
			ELSE
				ns_ps2_transmitter <= PS2_STATE_0_IDLE;
			END IF;
		WHEN PS2_STATE_1_INITIATE_COMMUNICATION =>
			IF (command_initiate_counter = CLOCK_CYCLES_FOR_101US) THEN
				ns_ps2_transmitter <= PS2_STATE_2_WAIT_FOR_CLOCK;
			ELSE
				ns_ps2_transmitter <= PS2_STATE_1_INITIATE_COMMUNICATION;
			END IF;
		WHEN PS2_STATE_2_WAIT_FOR_CLOCK =>
			IF (ps2_clk_negedge = '1') THEN
				ns_ps2_transmitter <= PS2_STATE_3_TRANSMIT_DATA;
			ELSIF (waiting_counter = CLOCK_CYCLES_FOR_15MS) THEN
				ns_ps2_transmitter <= PS2_STATE_7_TRANSMISSION_ERROR;
			ELSE
				ns_ps2_transmitter <= PS2_STATE_2_WAIT_FOR_CLOCK;
			END IF;
		WHEN PS2_STATE_3_TRANSMIT_DATA =>
			IF ((cur_bit = B"1000") AND (ps2_clk_negedge = '1')) THEN
				ns_ps2_transmitter <= PS2_STATE_4_TRANSMIT_STOP_BIT;
			ELSIF (transfer_counter = CLOCK_CYCLES_FOR_2MS) THEN
				ns_ps2_transmitter <= PS2_STATE_7_TRANSMISSION_ERROR;
			ELSE
				ns_ps2_transmitter <= PS2_STATE_3_TRANSMIT_DATA;
			END IF;
		WHEN PS2_STATE_4_TRANSMIT_STOP_BIT =>
			IF (ps2_clk_negedge = '1') THEN
				ns_ps2_transmitter <= PS2_STATE_5_RECEIVE_ACK_BIT;
			ELSIF (transfer_counter = CLOCK_CYCLES_FOR_2MS) THEN
				ns_ps2_transmitter <= PS2_STATE_7_TRANSMISSION_ERROR;
			ELSE
				ns_ps2_transmitter <= PS2_STATE_4_TRANSMIT_STOP_BIT;
			END IF;
		WHEN PS2_STATE_5_RECEIVE_ACK_BIT =>
			IF (ps2_clk_posedge = '1') THEN
				ns_ps2_transmitter <= PS2_STATE_6_COMMAND_WAS_SENT;
			ELSIF (transfer_counter = CLOCK_CYCLES_FOR_2MS) THEN
				ns_ps2_transmitter <= PS2_STATE_7_TRANSMISSION_ERROR;
			ELSE
				ns_ps2_transmitter <= PS2_STATE_5_RECEIVE_ACK_BIT;
			END IF;
		WHEN PS2_STATE_6_COMMAND_WAS_SENT =>
			IF (send_command = '0') THEN
				ns_ps2_transmitter <= PS2_STATE_0_IDLE;
			ELSE
				ns_ps2_transmitter <= PS2_STATE_6_COMMAND_WAS_SENT;
			END IF;
		WHEN PS2_STATE_7_TRANSMISSION_ERROR =>
			IF (send_command = '0') THEN
				ns_ps2_transmitter <= PS2_STATE_0_IDLE;
			ELSE
				ns_ps2_transmitter <= PS2_STATE_7_TRANSMISSION_ERROR;
			END IF;
		WHEN OTHERS =>
			ns_ps2_transmitter <= PS2_STATE_0_IDLE;
		END CASE;
	END PROCESS;

-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				ps2_command <= B"000000000";
			ELSIF (s_ps2_transmitter = PS2_STATE_0_IDLE) THEN
				ps2_command <= (((XOR_REDUCE(the_command)) XOR '1') & the_command);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				command_initiate_counter <= (OTHERS => '0');
			ELSIF ((s_ps2_transmitter = PS2_STATE_1_INITIATE_COMMUNICATION) AND 
					(command_initiate_counter /= CLOCK_CYCLES_FOR_101US)) THEN
				command_initiate_counter <= command_initiate_counter + 1;
			ELSIF (s_ps2_transmitter /= PS2_STATE_1_INITIATE_COMMUNICATION) THEN
				command_initiate_counter <= (OTHERS => '0');
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				waiting_counter <= (OTHERS => '0');
			ELSIF ((s_ps2_transmitter = PS2_STATE_2_WAIT_FOR_CLOCK) AND 
					(waiting_counter /= CLOCK_CYCLES_FOR_15MS)) THEN
				waiting_counter <= waiting_counter + 1;
			ELSIF (s_ps2_transmitter /= PS2_STATE_2_WAIT_FOR_CLOCK) THEN
				waiting_counter <= (OTHERS => '0');
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				transfer_counter <= (OTHERS => '0');
			ELSE
				IF ((s_ps2_transmitter = PS2_STATE_3_TRANSMIT_DATA) OR 
					(s_ps2_transmitter = PS2_STATE_4_TRANSMIT_STOP_BIT) OR 
					(s_ps2_transmitter = PS2_STATE_5_RECEIVE_ACK_BIT)) THEN
					IF (transfer_counter /= CLOCK_CYCLES_FOR_2MS) THEN
						transfer_counter <= transfer_counter + 1;
					END IF;
				ELSE
					transfer_counter <= (OTHERS => '0');
				END IF;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				cur_bit <= B"0000";
			ELSIF ((s_ps2_transmitter = PS2_STATE_3_TRANSMIT_DATA) AND 
					(ps2_clk_negedge = '1')) THEN
				cur_bit <= cur_bit + B"0001";
			ELSIF (s_ps2_transmitter /= PS2_STATE_3_TRANSMIT_DATA) THEN
				cur_bit <= B"0000";
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				command_was_sent <= '0';
			ELSIF (s_ps2_transmitter = PS2_STATE_6_COMMAND_WAS_SENT) THEN
				command_was_sent <= '1';
			ELSIF (send_command = '0') THEN
					command_was_sent <= '0';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				error_communication_timed_out <= '0';
			ELSIF (s_ps2_transmitter = PS2_STATE_7_TRANSMISSION_ERROR) THEN
				error_communication_timed_out <= '1';
			ELSIF (send_command = '0') THEN
				error_communication_timed_out <= '0';
			END IF;
		END IF;
	END PROCESS;

-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	PS2_CLK <= 
		'0' WHEN (s_ps2_transmitter = PS2_STATE_1_INITIATE_COMMUNICATION) ELSE 'Z';

	PS2_DAT <= 
		ps2_command(TO_INTEGER(UNSIGNED(cur_bit))) WHEN 
					(s_ps2_transmitter = PS2_STATE_3_TRANSMIT_DATA) ELSE 
		'0' WHEN (s_ps2_transmitter = PS2_STATE_2_WAIT_FOR_CLOCK) ELSE 
		'0' WHEN ((s_ps2_transmitter = PS2_STATE_1_INITIATE_COMMUNICATION) AND 
						(command_initiate_counter(DATA_WIDTH_FOR_101US) = '1')) ELSE 
		'Z';

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************


END Behaviour;
