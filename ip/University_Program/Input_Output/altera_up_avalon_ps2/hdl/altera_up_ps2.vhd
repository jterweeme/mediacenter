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
-- * This module communicates with the PS2 core.                                *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_ps2 IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	-- Command path parameters
	CLOCK_CYCLES_FOR_101US	:INTEGER									:= 5050;
	DATA_WIDTH_FOR_101US		:INTEGER									:= 13;
	CLOCK_CYCLES_FOR_15MS	:INTEGER									:= 750000;
	DATA_WIDTH_FOR_15MS		:INTEGER									:= 20;
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

	-- Bidirectionals
	PS2_CLK								:INOUT	STD_LOGIC;
	PS2_DAT								:INOUT	STD_LOGIC;

	-- Outputs
	command_was_sent					:BUFFER	STD_LOGIC;
	error_communication_timed_out	:BUFFER	STD_LOGIC;

	received_data						:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	received_data_en					:BUFFER	STD_LOGIC

);

END altera_up_ps2;

ARCHITECTURE Behaviour OF altera_up_ps2 IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	-- states
	TYPE State_Type IS (	PS2_STATE_0_IDLE,
								PS2_STATE_1_DATA_IN,
								PS2_STATE_2_COMMAND_OUT,
								PS2_STATE_3_END_TRANSFER,
								PS2_STATE_4_END_DELAYED
							);
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	SIGNAL	ps2_clk_posedge			:STD_LOGIC;
	SIGNAL	ps2_clk_negedge			:STD_LOGIC;
	
	SIGNAL	start_receiving_data		:STD_LOGIC;
	SIGNAL	wait_for_incoming_data	:STD_LOGIC;
	
	-- Internal Registers
	SIGNAL	idle_counter				:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	
	SIGNAL	ps2_clk_reg					:STD_LOGIC;
	SIGNAL	ps2_data_reg				:STD_LOGIC;
	SIGNAL	last_ps2_clk				:STD_LOGIC;
	
	-- State Machine Registers
	SIGNAL	ns_ps2_transceiver		:State_Type;	
	SIGNAL	s_ps2_transceiver			:State_Type;	
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altera_up_ps2_data_in
	PORT (
		-- Inputs
		clk							:IN		STD_LOGIC;
		reset							:IN		STD_LOGIC;

		wait_for_incoming_data	:IN		STD_LOGIC;
		start_receiving_data		:IN		STD_LOGIC;

		ps2_clk_posedge			:IN		STD_LOGIC;
		ps2_clk_negedge			:IN		STD_LOGIC;
		ps2_data						:IN		STD_LOGIC;

		-- Bidirectionals

		-- Outputs
		received_data				:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);
		received_data_en			:BUFFER	STD_LOGIC
	);
	END COMPONENT;

	COMPONENT altera_up_ps2_command_out
	GENERIC (
		CLOCK_CYCLES_FOR_101US			:INTEGER;
		DATA_WIDTH_FOR_101US				:INTEGER;
		CLOCK_CYCLES_FOR_15MS			:INTEGER;
		DATA_WIDTH_FOR_15MS				:INTEGER;
		CLOCK_CYCLES_FOR_2MS				:INTEGER;
		DATA_WIDTH_FOR_2MS				:INTEGER
	);
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
	END COMPONENT;

BEGIN
-- *****************************************************************************
-- *                         Finite State Machine(s)                           *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				s_ps2_transceiver <= PS2_STATE_0_IDLE;
			ELSE
				s_ps2_transceiver <= ns_ps2_transceiver;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (ns_ps2_transceiver, s_ps2_transceiver, idle_counter, send_command, 
				ps2_data_reg, ps2_clk_posedge, received_data_en, command_was_sent, 
				error_communication_timed_out)
	BEGIN
		-- Defaults
		ns_ps2_transceiver <= PS2_STATE_0_IDLE;
	
	   CASE (s_ps2_transceiver) IS
		WHEN PS2_STATE_0_IDLE =>
			IF ((idle_counter = B"11111111") AND 
					(send_command = '1')) THEN
				ns_ps2_transceiver <= PS2_STATE_2_COMMAND_OUT;
			ELSIF ((ps2_data_reg = '0') AND (ps2_clk_posedge = '1')) THEN
				ns_ps2_transceiver <= PS2_STATE_1_DATA_IN;
			ELSE
				ns_ps2_transceiver <= PS2_STATE_0_IDLE;
			END IF;
		WHEN PS2_STATE_1_DATA_IN =>
			IF (received_data_en = '1') THEN
				ns_ps2_transceiver <= PS2_STATE_0_IDLE;
			ELSE
				ns_ps2_transceiver <= PS2_STATE_1_DATA_IN;
			END IF;
		WHEN PS2_STATE_2_COMMAND_OUT =>
			IF ((command_was_sent = '1') OR 
				(error_communication_timed_out = '1')) THEN
				ns_ps2_transceiver <= PS2_STATE_3_END_TRANSFER;
			ELSE
				ns_ps2_transceiver <= PS2_STATE_2_COMMAND_OUT;
			END IF;
		WHEN PS2_STATE_3_END_TRANSFER =>
			IF (send_command = '0') THEN
				ns_ps2_transceiver <= PS2_STATE_0_IDLE;
			ELSIF ((ps2_data_reg = '0') AND (ps2_clk_posedge = '1')) THEN
				ns_ps2_transceiver <= PS2_STATE_4_END_DELAYED;
			ELSE
				ns_ps2_transceiver <= PS2_STATE_3_END_TRANSFER;
			END IF;
		WHEN PS2_STATE_4_END_DELAYED =>	
			IF (received_data_en = '1') THEN
				IF (send_command = '0') THEN
					ns_ps2_transceiver <= PS2_STATE_0_IDLE;
				ELSE
					ns_ps2_transceiver <= PS2_STATE_3_END_TRANSFER;
				END IF;
			ELSE
				ns_ps2_transceiver <= PS2_STATE_4_END_DELAYED;
			END IF;
		WHEN OTHERS =>
			ns_ps2_transceiver <= PS2_STATE_0_IDLE;
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				last_ps2_clk	<= '1';
				ps2_clk_reg		<= '1';
		
				ps2_data_reg	<= '1';
			ELSE
				last_ps2_clk	<= ps2_clk_reg;
				ps2_clk_reg		<= PS2_CLK;
		
				ps2_data_reg	<= PS2_DAT;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				idle_counter <= (OTHERS => '0');
			ELSIF ((s_ps2_transceiver = PS2_STATE_0_IDLE) AND 
					(idle_counter /= B"11111111")) THEN
				idle_counter <= idle_counter + 1;
			ELSIF (s_ps2_transceiver /= PS2_STATE_0_IDLE) THEN
				idle_counter <= (OTHERS => '0');
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	ps2_clk_posedge <= 
				'1' WHEN ((ps2_clk_reg = '1') AND (last_ps2_clk = '0')) ELSE '0';
	ps2_clk_negedge <= 
				'1' WHEN ((ps2_clk_reg = '0') AND (last_ps2_clk = '1')) ELSE '0';

	start_receiving_data <= 
				'1' WHEN (s_ps2_transceiver = PS2_STATE_1_DATA_IN) ELSE '0';
	wait_for_incoming_data 	<= 
				'1' WHEN (s_ps2_transceiver = PS2_STATE_3_END_TRANSFER) ELSE '0';

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************


	PS2_Data_In : altera_up_ps2_data_in 
	PORT MAP (
		-- Inputs
		clk							=> clk,
		reset							=> reset,
	
		wait_for_incoming_data	=> wait_for_incoming_data,
		start_receiving_data		=> start_receiving_data,
	
		ps2_clk_posedge			=> ps2_clk_posedge,
		ps2_clk_negedge			=> ps2_clk_negedge,
		ps2_data						=> ps2_data_reg,
	
		-- Bidirectionals
	
		-- Outputs
		received_data				=> received_data,
		received_data_en			=> received_data_en
	);

	PS2_Command_Out : altera_up_ps2_command_out 
	GENERIC MAP (
		CLOCK_CYCLES_FOR_101US			=> 5050,
		DATA_WIDTH_FOR_101US				=> 13,
		CLOCK_CYCLES_FOR_15MS			=> 750000,
		DATA_WIDTH_FOR_15MS				=> 20,
		CLOCK_CYCLES_FOR_2MS				=> 100000,
		DATA_WIDTH_FOR_2MS				=> 17
	)
	PORT MAP (
		-- Inputs
		clk									=> clk,
		reset									=> reset,
	
		the_command							=> the_command,
		send_command						=> send_command,
	
		ps2_clk_posedge					=> ps2_clk_posedge,
		ps2_clk_negedge					=> ps2_clk_negedge,
	
		-- Bidirectionals
		PS2_CLK								=> PS2_CLK,
	 	PS2_DAT								=> PS2_DAT,
	
		-- Outputs
		command_was_sent					=> command_was_sent,
		error_communication_timed_out	=> error_communication_timed_out
	);



END Behaviour;
