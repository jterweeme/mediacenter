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
-- * This module sends and receives data to/from the DE2's audio and TV         *
-- *  peripherals' control registers.                                           *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_av_config_serial_bus_controller IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	DW		:INTEGER									:= 26;	-- Datawidth
	CW		:INTEGER									:= 4;	-- Counter's datawidth
	
	SCCW	:INTEGER									:= 11	-- Slow clock's counter's datawidth
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (

	-- Inputs
	clk							:IN		STD_LOGIC;
	reset							:IN		STD_LOGIC;

	start_transfer				:IN		STD_LOGIC;

	data_in						:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	
	transfer_mask				:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	

	restart_counter			:IN		STD_LOGIC_VECTOR(CW DOWNTO  0);	
	restart_data_in			:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	
	restart_transfer_mask	:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	

	-- Bidirectionals
	serial_data					:INOUT	STD_LOGIC;								--	I2C Data

	-- Outputs
	serial_clk					:BUFFER	STD_LOGIC;								--	I2C Clock
	serial_en					:BUFFER	STD_LOGIC;

	data_out						:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	
	transfer_complete			:BUFFER	STD_LOGIC

);

END altera_up_av_config_serial_bus_controller;

ARCHITECTURE Behaviour OF altera_up_av_config_serial_bus_controller IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	
	-- States for finite state machines
	TYPE State_Type IS (	STATE_0_IDLE,
								STATE_1_INITIALIZE,
								STATE_2_RESTART_BIT,
								STATE_3_START_BIT,
								STATE_4_TRANSFER,
								STATE_5_STOP_BIT
							);
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	SIGNAL	slow_clk					:STD_LOGIC;
	
	SIGNAL	toggle_data_in			:STD_LOGIC;
	SIGNAL	toggle_data_out		:STD_LOGIC;
	
	-- Internal Registers
	SIGNAL	counter					:STD_LOGIC_VECTOR(CW DOWNTO  0);	
	
	SIGNAL	shiftreg_data			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	SIGNAL	shiftreg_mask			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	
	SIGNAL	new_data					:STD_LOGIC;
	
	-- State Machine Registers
	SIGNAL	ns_serial_protocol	:State_Type;	
	SIGNAL	s_serial_protocol		:State_Type;	
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altera_up_slow_clock_generator
	GENERIC (
		CB							:INTEGER
	);
	PORT (
		-- Inputs
		clk						:IN		STD_LOGIC;
		reset						:IN		STD_LOGIC;

		enable_clk				:IN		STD_LOGIC;
	
		-- Bidirectionals

		-- Outputs
		new_clk					:BUFFER	STD_LOGIC;


		middle_of_high_level	:BUFFER	STD_LOGIC;
		middle_of_low_level	:BUFFER	STD_LOGIC
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
				s_serial_protocol <= STATE_0_IDLE;
			ELSE
				s_serial_protocol <= ns_serial_protocol;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (ns_serial_protocol, s_serial_protocol, start_transfer, transfer_complete, 
				toggle_data_in, toggle_data_out, counter, shiftreg_mask, shiftreg_data)
	BEGIN
		-- Defaults
		ns_serial_protocol <= STATE_0_IDLE;
	
	   CASE (s_serial_protocol) IS
		WHEN STATE_0_IDLE =>
			IF ((start_transfer = '1') AND (transfer_complete = '0') AND (toggle_data_in = '1')) THEN
				ns_serial_protocol <= STATE_1_INITIALIZE;
			ELSE
				ns_serial_protocol <= STATE_0_IDLE;
			END IF;
		WHEN STATE_1_INITIALIZE =>
			ns_serial_protocol <= STATE_3_START_BIT;
		WHEN STATE_2_RESTART_BIT =>
			IF (toggle_data_in = '1') THEN
				ns_serial_protocol <= STATE_3_START_BIT;
			ELSE
				ns_serial_protocol <= STATE_2_RESTART_BIT;
			END IF;
		WHEN STATE_3_START_BIT =>
			IF (toggle_data_out = '1') THEN
				ns_serial_protocol <= STATE_4_TRANSFER;
			ELSE
				ns_serial_protocol <= STATE_3_START_BIT;
			END IF;
		WHEN STATE_4_TRANSFER =>
			IF ((toggle_data_out = '1') AND (counter = DW)) THEN
				ns_serial_protocol <= STATE_5_STOP_BIT;
			ELSIF ((toggle_data_out = '1') AND (shiftreg_mask(DW) = '1') AND (shiftreg_data(DW) = '1')) THEN
				ns_serial_protocol <= STATE_2_RESTART_BIT;
			ELSE
				ns_serial_protocol <= STATE_4_TRANSFER;
			END IF;
		WHEN STATE_5_STOP_BIT =>
			IF (toggle_data_in = '1') THEN
				ns_serial_protocol <= STATE_0_IDLE;
			ELSE
				ns_serial_protocol <= STATE_5_STOP_BIT;
			END IF;
		WHEN OTHERS =>
			ns_serial_protocol <= STATE_0_IDLE;
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
				serial_en			<= '1';
			ELSIF ((toggle_data_out = '1') AND (s_serial_protocol = STATE_3_START_BIT)) THEN
				serial_en			<= '0';
			ELSIF (s_serial_protocol = STATE_5_STOP_BIT) THEN
				serial_en			<= '1';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				transfer_complete	<= '0';
			ELSIF (s_serial_protocol = STATE_5_STOP_BIT) THEN
				transfer_complete	<= '1';
			ELSIF (start_transfer = '0') THEN
				transfer_complete	<= '0';
			END IF;
		END IF;
	END PROCESS;


	-- Input Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				counter				<= (OTHERS => '0');
				shiftreg_data		<= (OTHERS => '0');
				shiftreg_mask		<= (OTHERS => '0');
		
			ELSIF (s_serial_protocol = STATE_1_INITIALIZE) THEN
				counter				<= (OTHERS => '0');
				shiftreg_data		<= data_in;
				shiftreg_mask		<= transfer_mask;
		
			ELSIF ((toggle_data_in = '1') AND (s_serial_protocol = STATE_2_RESTART_BIT)) THEN
				counter				<= restart_counter;
				shiftreg_data		<= restart_data_in;
				shiftreg_mask		<= restart_transfer_mask;
		
			ELSIF ((toggle_data_out = '1') AND (s_serial_protocol = STATE_4_TRANSFER)) THEN
				counter				<= counter + 1;
				shiftreg_data		<= shiftreg_data( (DW - 1) DOWNTO  0) & new_data;
				shiftreg_mask		<= shiftreg_mask( (DW - 1) DOWNTO  0) & '0';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				new_data				<= '0';
			ELSIF ((toggle_data_in = '1') AND (s_serial_protocol = STATE_4_TRANSFER)) THEN
				new_data				<= serial_data;
			END IF;
		END IF;
	END PROCESS;



-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments
	serial_clk <= '1' WHEN (s_serial_protocol = STATE_0_IDLE) ELSE slow_clk;
	serial_data <= '1' WHEN (s_serial_protocol = STATE_0_IDLE) ELSE 
						'1' WHEN (s_serial_protocol = STATE_2_RESTART_BIT) ELSE 
						'Z' WHEN ((s_serial_protocol = STATE_4_TRANSFER) AND (shiftreg_mask(DW) = '1')) ELSE 
						shiftreg_data(DW) WHEN (s_serial_protocol = STATE_4_TRANSFER) ELSE 
						'0';

	data_out <= shiftreg_data;

	-- Input Assignments

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************

	Serial_Config_Clock_Generator : altera_up_slow_clock_generator 
	GENERIC MAP (
		CB							=> SCCW
	)
	PORT MAP (
		-- Inputs
		clk						=> clk,
		reset						=> reset,
	
		enable_clk				=> '1',
		
		-- Bidirectionals
	
		-- Outputs
		new_clk					=> slow_clk,
	
	
		middle_of_high_level	=> toggle_data_in,
		middle_of_low_level	=> toggle_data_out
	);



END Behaviour;
