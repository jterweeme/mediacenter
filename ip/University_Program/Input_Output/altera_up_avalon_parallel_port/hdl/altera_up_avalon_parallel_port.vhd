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
-- * This module can read and/or write data to a parallel I/O port based        *
-- *  on various user selected parameters. It has some predefined               *
-- *  configurations for some devices on the Altera DE boards.                  *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_avalon_parallel_port IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	-- DW represents the Data Width minus 1
	DW	:INTEGER									:= 31
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk			:IN		STD_LOGIC;
	reset			:IN		STD_LOGIC;

	address		:IN		STD_LOGIC_VECTOR( 1 DOWNTO  0);	
	byteenable	:IN		STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	chipselect	:IN		STD_LOGIC;
	read			:IN		STD_LOGIC;
	write			:IN		STD_LOGIC;
	writedata	:IN		STD_LOGIC_VECTOR(31 DOWNTO  0);	

`ifdef USE_INPUT_PORT
	in_port		:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	
`elsif USE_SLIDER_SWITCHES
	SW				:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	
`elsif USE_DIP_SWITCHES
	DIP			:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	
`elsif USE_PUSHBUTTONS
	KEY			:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	
`endif

	-- Bidirectionals
`ifdef USE_BIDIR_PORT
	bidir_port	:INOUT	STD_LOGIC_VECTOR(DW DOWNTO  0);	
`elsif USE_GPIO_S
	GPIO			:INOUT	STD_LOGIC_VECTOR(31 DOWNTO  0);	
`elsif USE_GPIO_0
	GPIO_0		:INOUT	STD_LOGIC_VECTOR(31 DOWNTO  0);	
`elsif USE_GPIO_1
	GPIO_1		:INOUT	STD_LOGIC_VECTOR(31 DOWNTO  0);	
`elsif USE_GPIO_2
	GPIO_2		:INOUT	STD_LOGIC_VECTOR(12 DOWNTO  0);	
`elsif USE_GPIO_2_IN
	GPIO_2_IN	:IN		STD_LOGIC_VECTOR( 2 DOWNTO  0);	
`endif

	-- Outputs
`ifdef USE_OUTPUT_PORT
	out_port		:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	
`elsif USE_TRI_COLOUR_LEDS
	LED			:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	
`elsif USE_GREEN_LEDS
	LEDG			:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	
`elsif USE_RED_LEDS
	LEDR			:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	
`elsif USE_TWO_SEVEN_SEGMENT
	HEX0			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	HEX1			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
`elsif USE_LOW_SEVEN_SEGMENT
`ifdef USE_DE2_70
	HEX0			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX1			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX2			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX3			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
`elsif USE_DE0
	HEX0			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX1			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX2			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX3			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
`else
	HEX0			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	HEX1			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	HEX2			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	HEX3			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
`endif
`elsif USE_HIGH_SEVEN_SEGMENT
`ifdef USE_DE2_70
	HEX4			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX5			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX6			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	HEX7			:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
`else
	HEX4			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	HEX5			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	HEX6			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	HEX7			:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);	
`endif
`endif

`ifdef USE_INTERRUPTS
	irq			:BUFFER	STD_LOGIC;
`endif
	readdata		:BUFFER	STD_LOGIC_VECTOR(31 DOWNTO  0)	

);

END altera_up_avalon_parallel_port;

ARCHITECTURE Behaviour OF altera_up_avalon_parallel_port IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
`ifdef USE_CAPTURE
	SIGNAL	new_capture		:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`endif
	
	-- Internal Registers
	SIGNAL	data				:STD_LOGIC_VECTOR(31 DOWNTO  0);	
`ifdef USE_INOUT
	SIGNAL	direction		:STD_LOGIC_VECTOR(31 DOWNTO  0);	
`endif
`ifdef USE_INTERRUPTS
	SIGNAL	interrupt		:STD_LOGIC_VECTOR(31 DOWNTO  0);	
`endif
`ifdef USE_CAPTURE
	SIGNAL	capture			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`endif
	
	SIGNAL	data_in			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`ifdef USE_DATA_OUT
	SIGNAL	data_out			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`endif
	
`ifdef USE_CAPTURE
	SIGNAL	last_data_in	:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`endif
	
	-- State Machine Registers
	
	-- Internal Variables
	--VARIABLE	i					:INTEGER;
	
	
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

	-- Input Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
`ifdef USE_INPUT_PORT
			data_in <= in_port;
`elsif USE_SLIDER_SWITCHES
			data_in <= SW;
`elsif USE_DIP_SWITCHES
			data_in	<= DIP;
`elsif USE_PUSHBUTTONS
			data_in <= NOT KEY;
`elsif USE_BIDIR_PORT
			data_in <= bidir_port;
`elsif USE_GPIO_S
			data_in <= GPIO;
`elsif USE_GPIO_0
			data_in <= GPIO_0;
`elsif USE_GPIO_1
			data_in <= GPIO_1;
`elsif USE_GPIO_2
			data_in <= GPIO_2;
`elsif USE_GPIO_2_IN
			data_in <= GPIO_2_IN;
`else
			data_in <= data(DW DOWNTO  0);
`endif
		END IF;
	END PROCESS;


	-- Output Registers
`ifdef USE_INTERRUPTS
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				irq <= '0';
			ELSE
`ifdef USE_LEVEL_INTERRUPTS
				irq <= (OR_REDUCE(interrupt(DW DOWNTO 0) AND data_in));
`else
				irq <= (OR_REDUCE(interrupt(DW DOWNTO 0) AND capture));
`endif
			END IF;
		END IF;
	END PROCESS;

`endif

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				readdata <= B"00000000000000000000000000000000";
			ELSIF (chipselect = '1') THEN
				IF (address = B"00") THEN
					readdata <= ((31 DOWNTO (DW + 1) => '0') & data_in);
`ifdef USE_INOUT
				ELSIF (address = B"01") THEN
					readdata <= ((31 DOWNTO (DW + 1) => '0') & direction(DW DOWNTO 0));
`endif
`ifdef USE_INTERRUPTS
				ELSIF (address = B"10") THEN
					readdata <= ((31 DOWNTO (DW + 1) => '0') & interrupt(DW DOWNTO 0));
`endif
`ifdef USE_CAPTURE
				ELSIF (address = B"11") THEN
					readdata <= ((31 DOWNTO (DW + 1) => '0') & capture);
`endif
				ELSE
					readdata <= B"00000000000000000000000000000000";
				END IF;
			END IF;
		END IF;
	END PROCESS;


	-- Internal Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				data <= (OTHERS => '0');
			ELSIF ((chipselect = '1') AND 
					(write = '1') AND 
					(address = B"00")) THEN
				IF (byteenable(0) = '1') THEN
					data( 7 DOWNTO  0) <= writedata( 7 DOWNTO  0);
				END IF;	
			
				IF (byteenable(1) = '1') THEN
					data(15 DOWNTO  8) <= writedata(15 DOWNTO  8);
				END IF;
			
				IF (byteenable(2) = '1') THEN
					data(23 DOWNTO 16) <= writedata(23 DOWNTO 16);
				END IF;		
			
				IF (byteenable(3) = '1') THEN
					data(31 DOWNTO 24) <= writedata(31 DOWNTO 24);
				END IF;
			END IF;
		END IF;
	END PROCESS;


`ifdef USE_INOUT
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				direction <= (OTHERS => '0');
			ELSIF ((chipselect = '1') AND 
					(write = '1') AND 
					(address = B"01")) THEN
				IF (byteenable(0) = '1') THEN
					direction( 7 DOWNTO  0) <= writedata( 7 DOWNTO  0);
				END IF;	
			
				IF (byteenable(1) = '1') THEN
					direction(15 DOWNTO  8) <= writedata(15 DOWNTO  8);
				END IF;
			
				IF (byteenable(2) = '1') THEN
					direction(23 DOWNTO 16) <= writedata(23 DOWNTO 16);
				END IF;		
			
				IF (byteenable(3) = '1') THEN
					direction(31 DOWNTO 24) <= writedata(31 DOWNTO 24);
				END IF;
			END IF;
		END IF;
	END PROCESS;

`endif

`ifdef USE_INTERRUPTS
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				interrupt <= (OTHERS => '0');
			ELSIF ((chipselect = '1') AND 
					(write = '1') AND 
					(address = B"10")) THEN
				IF (byteenable(0) = '1') THEN
					interrupt( 7 DOWNTO  0) <= writedata( 7 DOWNTO  0);
				END IF;	
			
				IF (byteenable(1) = '1') THEN
					interrupt(15 DOWNTO  8) <= writedata(15 DOWNTO  8);
				END IF;
			
				IF (byteenable(2) = '1') THEN
					interrupt(23 DOWNTO 16) <= writedata(23 DOWNTO 16);
				END IF;	
			
				IF (byteenable(3) = '1') THEN
					interrupt(31 DOWNTO 24) <= writedata(31 DOWNTO 24);
				END IF;
			END IF;
		END IF;
	END PROCESS;

`endif

`ifdef USE_CAPTURE
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				capture <= (OTHERS => '0');
			ELSIF ((chipselect = '1') AND 
					(write = '1') AND 
					(address = B"11")) THEN
				capture <= (OTHERS => '0');
			ELSE
				capture <= capture OR new_capture;
			END IF;
		END IF;
	END PROCESS;

`endif

`ifdef USE_DATA_OUT
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
				data_out <= data(DW DOWNTO  0);
		END IF;
	END PROCESS;
`endif

`ifdef USE_CAPTURE
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				last_data_in <= (OTHERS => '0');
			ELSE
				last_data_in <= data_in;
			END IF;
		END IF;
	END PROCESS;

`endif

-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments
`ifdef USE_BIDIR_PORT
	assign_data_out : FOR i IN 0 TO DW GENERATE
		bidir_port(i) <= data_out(i) WHEN (direction(i) = '1') ELSE 'Z';
	END GENERATE assign_data_out;
`elsif USE_GPIO_S
	assign_data_out : FOR i IN 0 TO DW GENERATE
		GPIO(i) <= data_out(i) WHEN (direction(i) = '1') ELSE 'Z';
	END GENERATE assign_data_out;
`elsif USE_GPIO_0
	assign_data_out : FOR i IN 0 TO DW GENERATE
		GPIO_0(i) <= data_out(i) WHEN (direction(i) = '1') ELSE 'Z';
	END GENERATE assign_data_out;
`elsif USE_GPIO_1
	assign_data_out : FOR i IN 0 TO DW GENERATE
		GPIO_1(i) <= data_out(i) WHEN (direction(i) = '1') ELSE 'Z';
	END GENERATE assign_data_out;
`elsif USE_GPIO_2
	assign_data_out : FOR i IN 0 TO DW GENERATE
		GPIO_2(i) <= data_out(i) WHEN (direction(i) = '1') ELSE 'Z';
	END GENERATE assign_data_out;
`endif

`ifdef USE_OUTPUT_PORT
	out_port <= data_out;
`elsif USE_TRI_COLOUR_LEDS
	LED <= data_out;
`elsif USE_GREEN_LEDS
	LEDG <= data_out;
`elsif USE_RED_LEDS
	LEDR <= data_out;
`elsif USE_TWO_SEVEN_SEGMENT
	HEX0 <= NOT data_out( 6 DOWNTO  0);
	HEX1 <= NOT data_out(14 DOWNTO  8);
`elsif USE_LOW_SEVEN_SEGMENT
`ifdef USE_DE2_70
	HEX0 <= NOT data_out( 7 DOWNTO  0);
	HEX1 <= NOT data_out(15 DOWNTO  8);
	HEX2 <= NOT data_out(23 DOWNTO 16);
	HEX3 <= NOT data_out(31 DOWNTO 24);
`elsif USE_DE0
	HEX0 <= NOT data_out( 7 DOWNTO  0);
	HEX1 <= NOT data_out(15 DOWNTO  8);
	HEX2 <= NOT data_out(23 DOWNTO 16);
	HEX3 <= NOT data_out(31 DOWNTO 24);
`else
	HEX0 <= NOT data_out( 6 DOWNTO  0);
	HEX1 <= NOT data_out(14 DOWNTO  8);
	HEX2 <= NOT data_out(22 DOWNTO 16);
	HEX3 <= NOT data_out(30 DOWNTO 24);
`endif
`elsif USE_HIGH_SEVEN_SEGMENT
`ifdef USE_DE2_70
	HEX4 <= NOT data_out( 7 DOWNTO  0);
	HEX5 <= NOT data_out(15 DOWNTO  8);
	HEX6 <= NOT data_out(23 DOWNTO 16);
	HEX7 <= NOT data_out(31 DOWNTO 24);
`else
	HEX4 <= NOT data_out( 6 DOWNTO  0);
	HEX5 <= NOT data_out(14 DOWNTO  8);
	HEX6 <= NOT data_out(22 DOWNTO 16);
	HEX7 <= NOT data_out(30 DOWNTO 24);
`endif
`endif

	-- Internal Assignments
`ifdef USE_EDGE_HIGH_CAPTURE
	new_capture <= data_in AND NOT last_data_in;
`elsif USE_EDGE_LOW_CAPTURE
	new_capture <= NOT data_in AND last_data_in;
`elsif USE_EDGE_BOTH_CAPTURE
	new_capture <= data_in XOR last_data_in;
`endif

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
