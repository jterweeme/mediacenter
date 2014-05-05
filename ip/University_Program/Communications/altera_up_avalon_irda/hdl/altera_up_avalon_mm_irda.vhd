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
-- * This module reads and writes data to the IrDA connector on Altera's        *
-- *  DE-series Development and Education Boards.                               *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_avalon_mm_irda IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	CW								:INTEGER		:= 9;			-- Baud counter width
	BAUD_TICK_COUNT			:INTEGER		:= 433;
	BAUD_3_16_TICK_COUNT		:INTEGER		:= 81;
	CAPTURE_IN_TICK_COUNT	:INTEGER		:= 60;
	HALF_BAUD_TICK_COUNT		:INTEGER		:= 216;
	
	TDW							:INTEGER		:= 11;		-- Total data width
	DW								:INTEGER		:= 8;			-- Data width
	ODD_PARITY					:STD_LOGIC	:= '1'
	
);
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

	IRDA_RXD		:IN		STD_LOGIC;

	-- Bidirectionals

	-- Outputs
	irq			:BUFFER	STD_LOGIC;
	readdata		:BUFFER	STD_LOGIC_VECTOR(31 DOWNTO  0);	

	IRDA_TXD		:BUFFER	STD_LOGIC

);

END altera_up_avalon_mm_irda;

ARCHITECTURE Behaviour OF altera_up_avalon_mm_irda IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	SIGNAL	read_fifo_read_en		:STD_LOGIC;
	SIGNAL	read_available			:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	SIGNAL	read_data_valid		:STD_LOGIC;
`ifdef USE_PARITY
	SIGNAL	read_data				:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`else
	SIGNAL	read_data				:STD_LOGIC_VECTOR((DW-1) DOWNTO  0);	
`endif
	
	SIGNAL	parity_error			:STD_LOGIC;
	
	SIGNAL	write_data_parity		:STD_LOGIC;
	SIGNAL	write_space				:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	
	-- Internal Registers
	SIGNAL	read_interrupt_en		:STD_LOGIC;
	SIGNAL	write_interrupt_en	:STD_LOGIC;
	
	SIGNAL	read_interrupt			:STD_LOGIC;
	SIGNAL	write_interrupt		:STD_LOGIC;
	
	SIGNAL	write_fifo_write_en	:STD_LOGIC;
	SIGNAL	data_to_uart			:STD_LOGIC_VECTOR((DW-1) DOWNTO  0);	
	
	-- State Machine Registers
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altera_up_irda_in_deserializer
	GENERIC ( 
		CW								:INTEGER;
		BAUD_TICK_COUNT			:INTEGER;
		BAUD_3_16_TICK_COUNT		:INTEGER;
		CAPTURE_IN_TICK_COUNT	:INTEGER;
		HALF_BAUD_TICK_COUNT		:INTEGER;
		TDW							:INTEGER;
		DW								:INTEGER
	);
	PORT (
		-- Inputs
		clk						:IN		STD_LOGIC;
		reset						:IN		STD_LOGIC;
	
		serial_data_in			:IN		STD_LOGIC;

		receive_data_en		:IN		STD_LOGIC;

		-- Bidirectionals

		-- Outputs
		fifo_read_available	:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);

		received_data_valid	:BUFFER	STD_LOGIC;
`ifdef USE_PARITY
		received_data			:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0)
`else
		received_data			:BUFFER	STD_LOGIC_VECTOR((DW-1) DOWNTO  0)
`endif
	);
	END COMPONENT;

	COMPONENT altera_up_irda_out_serializer
	GENERIC ( 
		CW								:INTEGER;
		BAUD_TICK_COUNT			:INTEGER;
		BAUD_3_16_TICK_COUNT		:INTEGER;
		CAPTURE_IN_TICK_COUNT	:INTEGER;
		HALF_BAUD_TICK_COUNT		:INTEGER;
		TDW							:INTEGER;
		DW								:INTEGER
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

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				irq <= '0';
			ELSE
				irq <= write_interrupt OR read_interrupt;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				readdata <= B"00000000000000000000000000000000";
			ELSIF (chipselect = '1') THEN
				IF (address = '0') THEN
					readdata <= 
						(B"00000000" & 
						 read_available & 
						 read_data_valid & 
						 B"00000" & 
						 parity_error & 
`ifdef USE_DATA_WIDTH_7
						 B"00" & 
`elsif USE_DATA_WIDTH_8
						 '0' & 
`endif
						 read_data((DW - 1) DOWNTO  0));
				ELSE
					readdata <= 
						(B"00000000" & 
						 write_space & 
						 B"000000" & 
						 write_interrupt & 
						 read_interrupt & 
						 B"000000" & 
						 write_interrupt_en & 
						 read_interrupt_en);
				END IF;
			END IF;
		END IF;
	END PROCESS;



	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				read_interrupt_en <= '0';
			ELSIF ((chipselect = '1') AND (write = '1') AND (address = '1') AND 
						(byteenable(0) = '1')) THEN
				read_interrupt_en <= writedata(0);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				write_interrupt_en <= '0';
			ELSIF ((chipselect = '1') AND (write = '1') AND (address = '1') AND 
						(byteenable(0) = '1')) THEN
				write_interrupt_en <= writedata(1);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				read_interrupt <= '0';
			ELSIF (read_interrupt_en = '0') THEN
				read_interrupt <= '0';
			ELSE
				read_interrupt <= (AND_REDUCE(read_available(6 DOWNTO 5)) OR read_available(7));
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				write_interrupt <= '0';
			ELSIF (write_interrupt_en = '0') THEN
				write_interrupt <= '0';
			ELSE
				write_interrupt <= (AND_REDUCE(write_space(6 DOWNTO 5)) OR write_space(7));
			END IF;
		END IF;
	END PROCESS;



	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				write_fifo_write_en <= '0';
			ELSE
				write_fifo_write_en <= 
					chipselect AND write AND NOT address AND byteenable(0);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				data_to_uart <= (OTHERS => '0');
			ELSE
				data_to_uart <= writedata((DW - 1) DOWNTO 0);
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

`ifdef USE_PARITY
	parity_error <= ((XOR_REDUCE(read_data(DW DOWNTO 0))) XOR ODD_PARITY) WHEN (read_available /= B"00000000") ELSE '0';
`else
	parity_error <= '0';
`endif

	read_fifo_read_en <= chipselect AND read AND NOT address AND byteenable(0);

	write_data_parity <= (XOR_REDUCE(data_to_uart)) XOR ODD_PARITY;

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************

	IrDA_In_Deserializer : altera_up_irda_in_deserializer 
	GENERIC MAP ( 
		CW								=> CW,
		BAUD_TICK_COUNT			=> BAUD_TICK_COUNT,
		BAUD_3_16_TICK_COUNT		=> BAUD_3_16_TICK_COUNT,
		CAPTURE_IN_TICK_COUNT	=> CAPTURE_IN_TICK_COUNT,
		HALF_BAUD_TICK_COUNT		=> HALF_BAUD_TICK_COUNT,
		TDW							=> TDW,
`ifdef USE_PARITY
		DW								=> DW
`else
		DW								=> (DW - 1)
`endif
	)
	PORT MAP (
		-- Inputs
		clk						=> clk,
		reset						=> reset,
		
		serial_data_in			=> IRDA_RXD,
	
		receive_data_en		=> read_fifo_read_en,
	
		-- Bidirectionals
	
		-- Outputs
		fifo_read_available	=> read_available,
	
		received_data_valid	=> read_data_valid,
		received_data			=> read_data
	);


	IrDA_Out_Serializer : altera_up_irda_out_serializer 
	GENERIC MAP ( 
		CW								=> CW,
		BAUD_TICK_COUNT			=> BAUD_TICK_COUNT,
		BAUD_3_16_TICK_COUNT		=> BAUD_3_16_TICK_COUNT,
		CAPTURE_IN_TICK_COUNT	=> CAPTURE_IN_TICK_COUNT,
		HALF_BAUD_TICK_COUNT		=> HALF_BAUD_TICK_COUNT,
		TDW							=> TDW,
`ifdef USE_PARITY
		DW								=> DW
`else
		DW								=> (DW - 1)
`endif
	)
	PORT MAP (
		-- Inputs
		clk					=> clk,
		reset					=> reset,
		
`ifdef USE_PARITY
		transmit_data		=> write_data_parity & data_to_uart,
`else
		transmit_data		=> data_to_uart,
`endif
		transmit_data_en	=> write_fifo_write_en,
	
		-- Bidirectionals
	
		-- Outputs
		fifo_write_space	=> write_space,
	
		serial_data_out	=> IRDA_TXD
	);

END Behaviour;
