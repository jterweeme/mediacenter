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
-- * This module connects the PS2 core to Avalon.                               *
-- *                                                                            *
-- ******************************************************************************

--
-- *
-- * Data Register Bits
-- * Read Available 31-16, Read Valid 15, Incoming Data or Outgoing Command 7-0
-- *
-- * Control Register Bits
-- * CE 10, RI 8, RE 0
-- *
-- *

ENTITY altera_up_avalon_mm_ps2 IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************

-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk			:IN		STD_LOGIC;
	reset			:IN		STD_LOGIC;

	address		:IN		STD_LOGIC;
	chipselect	:IN		STD_LOGIC;
	byteenable	:IN		STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	read			:IN		STD_LOGIC;
	write			:IN		STD_LOGIC;
	writedata	:IN		STD_LOGIC_VECTOR(31 DOWNTO  0);	

	-- Bidirectionals
	PS2_CLK		:INOUT	STD_LOGIC;
	PS2_DAT		:INOUT	STD_LOGIC;

	-- Outputs
	irq			:BUFFER	STD_LOGIC;
	readdata		:BUFFER	STD_LOGIC_VECTOR(31 DOWNTO  0);	
	waitrequest	:BUFFER	STD_LOGIC

);

END altera_up_avalon_mm_ps2;

ARCHITECTURE Behaviour OF altera_up_avalon_mm_ps2 IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	
	-- Command path parameters
	CONSTANT	CLOCK_CYCLES_FOR_101US	:INTEGER									:= 5050;
	CONSTANT	DATA_WIDTH_FOR_101US		:INTEGER									:= 13;
	CONSTANT	CLOCK_CYCLES_FOR_15MS	:INTEGER									:= 750000;
	CONSTANT	DATA_WIDTH_FOR_15MS		:INTEGER									:= 20;
	CONSTANT	CLOCK_CYCLES_FOR_2MS		:INTEGER									:= 100000;
	CONSTANT	DATA_WIDTH_FOR_2MS		:INTEGER									:= 17;
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	SIGNAL	data_from_the_PS2_port		:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	SIGNAL	data_from_the_PS2_port_en	:STD_LOGIC;
	
	SIGNAL	get_data_from_PS2_port		:STD_LOGIC;
	SIGNAL	send_command_to_PS2_port	:STD_LOGIC;
	SIGNAL	clear_command_error			:STD_LOGIC;
	SIGNAL	set_interrupt_enable			:STD_LOGIC;
	
	SIGNAL	command_was_sent				:STD_LOGIC;
	SIGNAL	error_sending_command		:STD_LOGIC;
	
	SIGNAL	data_fifo_is_empty			:STD_LOGIC;
	SIGNAL	data_fifo_is_full				:STD_LOGIC;
	
	SIGNAL	data_in_fifo					:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	SIGNAL	data_valid						:STD_LOGIC;
	SIGNAL	data_available					:STD_LOGIC_VECTOR( 8 DOWNTO  0);	
	
	-- Internal Registers
	SIGNAL	control_register				:STD_LOGIC_VECTOR(31 DOWNTO  0);	
	
	-- State Machine Registers
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altera_up_ps2
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

		-- Bidirectionals
		PS2_CLK								:INOUT	STD_LOGIC;
		PS2_DAT								:INOUT	STD_LOGIC;

		-- Outputs
		command_was_sent					:BUFFER	STD_LOGIC;
		error_communication_timed_out	:BUFFER	STD_LOGIC;

		received_data						:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);
		received_data_en					:BUFFER	STD_LOGIC
	);
	END COMPONENT;

	COMPONENT scfifo
	GENERIC (
		add_ram_output_register	:STRING;
		intended_device_family	:STRING;
		lpm_numwords				:INTEGER;
		lpm_showahead				:STRING;
		lpm_type						:STRING;
		lpm_width					:INTEGER;
		lpm_widthu					:INTEGER;
		overflow_checking			:STRING;
		underflow_checking		:STRING;
		use_eab						:STRING
	);
	PORT (
		-- Inputs
		clock				:IN		STD_LOGIC;
		sclr				:IN		STD_LOGIC;

		rdreq				:IN		STD_LOGIC;
		wrreq				:IN		STD_LOGIC;
		data				:IN		STD_LOGIC_VECTOR( 7 DOWNTO  0);

		-- Bidirectionals

		-- Outputs
		q					:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);

		usedw				:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);
		empty				:BUFFER	STD_LOGIC;
		full				:BUFFER	STD_LOGIC
	);
	END COMPONENT;

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
				readdata <= B"00000000000000000000000000000000";
			ELSIF (chipselect = '1') THEN
				IF (address = '0') THEN
					readdata <= (B"0000000" & data_available & data_valid & B"0000000" & data_in_fifo);
				ELSE
					readdata <= control_register;
				END IF;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				control_register <= B"00000000000000000000000000000000";
			ELSE
				IF (error_sending_command = '1') THEN
					control_register(10) <= '1';
				ELSIF (clear_command_error = '1') THEN
					control_register(10) <= '0';
				
			END IF;
				control_register(8) <= NOT data_fifo_is_empty AND control_register(0);
		
				IF ((chipselect = '1') AND (set_interrupt_enable = '1')) THEN
					control_register(0)  <= writedata(0);
				END IF;
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	irq 			<= control_register(8);
	waitrequest <= send_command_to_PS2_port AND 
							 NOT (command_was_sent OR error_sending_command);

	get_data_from_PS2_port 		<= chipselect AND byteenable(0) AND NOT address AND read;
	send_command_to_PS2_port 	<= chipselect AND byteenable(0) AND NOT address AND write;
	clear_command_error 			<= chipselect AND byteenable(1) AND address AND write;
	set_interrupt_enable 		<= chipselect AND byteenable(0) AND address AND write;

	data_available(8) 			<= data_fifo_is_full;
	data_valid 						<= NOT data_fifo_is_empty;

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************

	PS2_Serial_Port : altera_up_ps2 
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
	
		the_command							=> writedata(7 DOWNTO 0),
		send_command						=> send_command_to_PS2_port,
	
		-- Bidirectionals
		PS2_CLK								=> PS2_CLK,
	 	PS2_DAT								=> PS2_DAT,
	
		-- Outputs
		command_was_sent					=> command_was_sent,
		error_communication_timed_out	=> error_sending_command,
	
		received_data						=> data_from_the_PS2_port,
		received_data_en					=> data_from_the_PS2_port_en
	);

	Incoming_Data_FIFO : scfifo 
	GENERIC MAP (
		add_ram_output_register	=> "ON",
		intended_device_family	=> "Cyclone II",
		lpm_numwords				=> 256,
		lpm_showahead				=> "ON",
		lpm_type						=> "scfifo",
		lpm_width					=> 8,
		lpm_widthu					=> 8,
		overflow_checking			=> "OFF",
		underflow_checking		=> "OFF",
		use_eab						=> "ON"
	)
	PORT MAP (
		-- Inputs
		clock				=> clk,
		sclr				=> reset,
	
		rdreq				=> get_data_from_PS2_port AND NOT data_fifo_is_empty,
		wrreq				=> data_from_the_PS2_port_en AND NOT data_fifo_is_full,
		data				=> data_from_the_PS2_port,
	
		-- Bidirectionals
	
		-- Outputs
		q					=> data_in_fifo,
	
		usedw				=> data_available(7 DOWNTO 0),
		empty				=> data_fifo_is_empty,
		full				=> data_fifo_is_full
	);

END Behaviour;
