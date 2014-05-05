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
-- * This module reads and writes data to the RS232 connector on Altera's       *
-- *  DE-series Development and Education Boards.                               *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_avalon_st_rs232 IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	CW							:INTEGER		:= 9;		-- Baud counter width
	BAUD_TICK_COUNT		:INTEGER		:= 433;
	HALF_BAUD_TICK_COUNT	:INTEGER		:= 216;
	
	TDW						:INTEGER		:= 11;	-- Total data width
	DW							:INTEGER		:= 8;		-- Data width
	ODD_PARITY				:STD_LOGIC	:= '1'
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk					:IN		STD_LOGIC;
	reset					:IN		STD_LOGIC;

	from_uart_ready	:IN		STD_LOGIC;

	to_uart_data		:IN		STD_LOGIC_VECTOR((DW-1) DOWNTO  0);	
	to_uart_error		:IN		STD_LOGIC;
	to_uart_valid		:IN		STD_LOGIC;

	UART_RXD				:IN		STD_LOGIC;

	-- Bidirectionals

	-- Outputs
	from_uart_data		:BUFFER	STD_LOGIC_VECTOR((DW-1) DOWNTO  0);	
	from_uart_error	:BUFFER	STD_LOGIC;
	from_uart_valid	:BUFFER	STD_LOGIC;

	to_uart_ready		:BUFFER	STD_LOGIC;

	UART_TXD				:BUFFER	STD_LOGIC

);

END altera_up_avalon_st_rs232;

ARCHITECTURE Behaviour OF altera_up_avalon_st_rs232 IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
`ifdef USE_PARITY
	SIGNAL	read_data			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`else
	SIGNAL	read_data			:STD_LOGIC_VECTOR((DW-1) DOWNTO  0);	
`endif
	
	SIGNAL	write_data_parity	:STD_LOGIC;
	SIGNAL	write_space			:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	
	-- Internal Registers
	
	-- State Machine Registers
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altera_up_rs232_in_deserializer
	GENERIC ( 
		CW							:INTEGER;
		BAUD_TICK_COUNT		:INTEGER;
		HALF_BAUD_TICK_COUNT	:INTEGER;
		TDW						:INTEGER;
		DW							:INTEGER
	);
	PORT (
		-- Inputs
		clk						:IN		STD_LOGIC;
		reset						:IN		STD_LOGIC;
	
		serial_data_in			:IN		STD_LOGIC;

		receive_data_en		:IN		STD_LOGIC;

		-- Bidirectionals

		-- Outputs

		received_data_valid	:BUFFER	STD_LOGIC;
`ifdef USE_PARITY
		received_data			:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0)
`else
		received_data			:BUFFER	STD_LOGIC_VECTOR((DW-1) DOWNTO  0)
`endif
	);
	END COMPONENT;

	COMPONENT altera_up_rs232_out_serializer
	GENERIC ( 
		CW							:INTEGER;
		BAUD_TICK_COUNT		:INTEGER;
		HALF_BAUD_TICK_COUNT	:INTEGER;
		TDW						:INTEGER;
		DW							:INTEGER
	);
	PORT (
		-- Inputs
		clk					:IN		STD_LOGIC;
		reset					:IN		STD_LOGIC;
	
`ifdef USE_PARITY
		transmit_data		:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);
`else
		transmit_data		:IN		STD_LOGIC_VECTOR((DW-1) DOWNTO  0);
`endif
		transmit_data_en	:IN		STD_LOGIC;

		-- Bidirectionals

		-- Outputs
		fifo_write_space	:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);

		serial_data_out	:BUFFER	STD_LOGIC
	);
	END COMPONENT;

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
`ifdef USE_PARITY
	from_uart_data <= read_data((DW - 1) DOWNTO  0);
	from_uart_error <= ((XOR_REDUCE(read_data(DW DOWNTO  0))) XOR ODD_PARITY);
`else
	from_uart_data <= read_data;
	from_uart_error <= '0';
`endif

	to_uart_ready <= (OR_REDUCE(write_space));

	-- Internal Assignments
	write_data_parity <= (XOR_REDUCE(to_uart_data)) XOR ODD_PARITY;

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************

	RS232_In_Deserializer : altera_up_rs232_in_deserializer 
	GENERIC MAP ( 
		CW							=> CW,
		BAUD_TICK_COUNT		=> BAUD_TICK_COUNT,
		HALF_BAUD_TICK_COUNT	=> HALF_BAUD_TICK_COUNT,
		TDW						=> TDW,
`ifdef USE_PARITY
		DW							=> DW
`else
		DW							=> (DW - 1)
`endif
	)
	PORT MAP (
		-- Inputs
		clk						=> clk,
		reset						=> reset,
		
		serial_data_in			=> UART_RXD,
	
		receive_data_en		=> from_uart_ready,
	
		-- Bidirectionals
	
		-- Outputs
		received_data_valid	=> from_uart_valid,
		received_data			=> read_data
	);


	RS232_Out_Serializer : altera_up_rs232_out_serializer 
	GENERIC MAP ( 
		CW							=> CW,
		BAUD_TICK_COUNT		=> BAUD_TICK_COUNT,
		HALF_BAUD_TICK_COUNT	=> HALF_BAUD_TICK_COUNT,
		TDW						=> TDW,
`ifdef USE_PARITY
		DW							=> DW
`else
		DW							=> (DW - 1)
`endif
	)
	PORT MAP (
		-- Inputs
		clk					=> clk,
		reset					=> reset,
		
`ifdef USE_PARITY
		transmit_data		=> write_data_parity & to_uart_data,
`else
		transmit_data		=> to_uart_data,
`endif
		transmit_data_en	=> to_uart_valid AND to_uart_ready,
	
		-- Bidirectionals
	
		-- Outputs
		fifo_write_space	=> write_space,
	
		serial_data_out	=> UART_TXD
	);

END Behaviour;
