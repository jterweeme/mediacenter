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
-- * This module reads and writes data to the Audio chip on Altera's DE2        *
-- *  Development and Education Board. The audio chip must be in master mode    *
-- *  and the digital format must be left justified.                            *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_avalon_mm_audio IS 

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

	address		:IN		STD_LOGIC_VECTOR( 1 DOWNTO  0);	
	chipselect	:IN		STD_LOGIC;
	read			:IN		STD_LOGIC;
	write			:IN		STD_LOGIC;
	writedata	:IN		STD_LOGIC_VECTOR(31 DOWNTO  0);	

`ifdef USE_AUDIO_IN
	AUD_ADCDAT	:IN		STD_LOGIC;
	AUD_ADCLRCK	:IN		STD_LOGIC;
`endif
	AUD_BCLK		:IN		STD_LOGIC;
`ifdef USE_AUDIO_OUT
	AUD_DACLRCK	:IN		STD_LOGIC;
`endif

	-- Bidirectionals

	-- Outputs
	irq			:BUFFER	STD_LOGIC;
	readdata		:BUFFER	STD_LOGIC_VECTOR(31 DOWNTO  0);	

`ifdef USE_AUDIO_OUT
	AUD_DACDAT	:BUFFER	STD_LOGIC
`endif

);

END altera_up_avalon_mm_audio;

ARCHITECTURE Behaviour OF altera_up_avalon_mm_audio IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	
	CONSTANT	DW	:INTEGER									:= 15;
	CONSTANT	BIT_COUNTER_INIT	:STD_LOGIC_VECTOR( 4 DOWNTO  0)	:= B"01111";
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	SIGNAL	bclk_rising_edge					:STD_LOGIC;
	SIGNAL	bclk_falling_edge					:STD_LOGIC;
	
`ifdef USE_AUDIO_IN
	SIGNAL	adc_lrclk_rising_edge			:STD_LOGIC;
	SIGNAL	adc_lrclk_falling_edge			:STD_LOGIC;
	
	SIGNAL	new_left_channel_audio			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	SIGNAL	new_right_channel_audio			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	
	SIGNAL	left_channel_read_available	:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	SIGNAL	right_channel_read_available	:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
`endif
`ifdef USE_AUDIO_OUT
	SIGNAL	dac_lrclk_rising_edge			:STD_LOGIC;
	SIGNAL	dac_lrclk_falling_edge			:STD_LOGIC;
	
	SIGNAL	left_channel_write_space		:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	SIGNAL	right_channel_write_space		:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
`endif
	
	-- Internal Registers
`ifdef USE_AUDIO_IN
	SIGNAL	done_adc_channel_sync			:STD_LOGIC;
	SIGNAL	read_interrupt_en					:STD_LOGIC;
	SIGNAL	clear_read_fifos					:STD_LOGIC;
	SIGNAL	read_interrupt						:STD_LOGIC;
	
`endif
`ifdef USE_AUDIO_OUT
	SIGNAL	done_dac_channel_sync			:STD_LOGIC;
	SIGNAL	write_interrupt_en				:STD_LOGIC;
	SIGNAL	clear_write_fifos					:STD_LOGIC;
	SIGNAL	write_interrupt					:STD_LOGIC;
`endif
	
	-- State Machine Registers
	
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altera_up_clock_edge
	PORT (
		-- Inputs
		clk				:IN		STD_LOGIC;
		reset				:IN		STD_LOGIC;
	
		test_clk			:IN		STD_LOGIC;
	
		-- Bidirectionals

		-- Outputs
		rising_edge		:BUFFER	STD_LOGIC;
		falling_edge	:BUFFER	STD_LOGIC
	);
	END COMPONENT;

`ifdef USE_AUDIO_IN
	COMPONENT altera_up_audio_in_deserializer
	GENERIC (
		DW										:INTEGER;
		BIT_COUNTER_INIT					:STD_LOGIC_VECTOR( 4 DOWNTO  0)
	);
	PORT (
		-- Inputs
		clk									:IN		STD_LOGIC;
		reset									:IN		STD_LOGIC;
	
		bit_clk_rising_edge				:IN		STD_LOGIC;
		bit_clk_falling_edge				:IN		STD_LOGIC;
		left_right_clk_rising_edge		:IN		STD_LOGIC;
		left_right_clk_falling_edge	:IN		STD_LOGIC;

		done_channel_sync					:IN		STD_LOGIC;

		serial_audio_in_data				:IN		STD_LOGIC;

		read_left_audio_data_en			:IN		STD_LOGIC;
		read_right_audio_data_en		:IN		STD_LOGIC;

		-- Bidirectionals

		-- Outputs
		left_audio_fifo_read_space		:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);
		right_audio_fifo_read_space	:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);

		left_channel_data					:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);
		right_channel_data				:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0)
	);
	END COMPONENT;
`endif
	
`ifdef USE_AUDIO_OUT
	COMPONENT altera_up_audio_out_serializer
	GENERIC (
		DW											:INTEGER
	);
	PORT (
		-- Inputs
		clk										:IN		STD_LOGIC;
		reset										:IN		STD_LOGIC;
	
		bit_clk_rising_edge					:IN		STD_LOGIC;
		bit_clk_falling_edge					:IN		STD_LOGIC;
		left_right_clk_rising_edge			:IN		STD_LOGIC;
		left_right_clk_falling_edge		:IN		STD_LOGIC;
	
		left_channel_data						:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);
		left_channel_data_en					:IN		STD_LOGIC;

		right_channel_data					:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);
		right_channel_data_en				:IN		STD_LOGIC;
	
		-- Bidirectionals

		-- Outputs
		left_channel_fifo_write_space		:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);
		right_channel_fifo_write_space	:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);

		serial_audio_out_data				:BUFFER	STD_LOGIC
	);
	END COMPONENT;
`endif

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
				irq <= 
`ifdef USE_AUDIO_OUT
					write_interrupt OR 
`else
					'0' OR 
`endif
`ifdef USE_AUDIO_IN
					read_interrupt;
`else
					'0';
`endif
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				readdata <= B"00000000000000000000000000000000";
			ELSIF (chipselect = '1') THEN
				IF (address = B"00") THEN
					readdata <= 
						(B"0000000000000000000000" & 
`ifdef USE_AUDIO_OUT
						 write_interrupt & 
`else
						'0' & 
`endif
`ifdef USE_AUDIO_IN
						 read_interrupt & 
`else
						'0' & 
`endif
						 B"0000" & 
`ifdef USE_AUDIO_OUT
						 clear_write_fifos & 
`else
						'0' & 
`endif
`ifdef USE_AUDIO_IN
						 clear_read_fifos & 
`else
						'0' & 
`endif
`ifdef USE_AUDIO_OUT
						 write_interrupt_en & 
`else
						'0' & 
`endif
`ifdef USE_AUDIO_IN
						 read_interrupt_en);
`else
						'0');
`endif
				ELSIF (address = B"01") THEN
`ifdef USE_AUDIO_OUT
					readdata(31 DOWNTO 24) <= left_channel_write_space;
					readdata(23 DOWNTO 16) <= right_channel_write_space;
`else
					readdata(31 DOWNTO 24) <= B"00000000";
					readdata(23 DOWNTO 16) <= B"00000000";
`endif
`ifdef USE_AUDIO_IN
					readdata(15 DOWNTO  8) <= left_channel_read_available;
					readdata( 7 DOWNTO  0) <= right_channel_read_available;
`else
					readdata(15 DOWNTO  8) <= B"00000000";
					readdata( 7 DOWNTO  0) <= B"00000000";
`endif
`ifdef USE_AUDIO_IN
				ELSIF (address = B"10") THEN
					readdata <= B"00000000000000000000000000000000" OR 
						new_left_channel_audio;
				ELSE
					readdata <= B"00000000000000000000000000000000" OR 
						new_right_channel_audio;
`else
				ELSE
					readdata <= B"00000000000000000000000000000000";
`endif
				END IF;
			END IF;
		END IF;
	END PROCESS;



`ifdef USE_AUDIO_IN
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				read_interrupt_en <= '0';
			ELSIF ((chipselect = '1') AND (write = '1') AND (address = B"00")) THEN
				read_interrupt_en <= writedata(0);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				clear_read_fifos <= '0';
			ELSIF ((chipselect = '1') AND (write = '1') AND (address = B"00")) THEN
				clear_read_fifos <= writedata(2);
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
				read_interrupt <=
					(AND_REDUCE (left_channel_read_available(6 DOWNTO 5))  OR left_channel_read_available(7)) OR 
					(AND_REDUCE (right_channel_read_available(6 DOWNTO 5)) OR right_channel_read_available(7));
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				done_adc_channel_sync <= '0';
			ELSIF (adc_lrclk_rising_edge = '1') THEN
				done_adc_channel_sync <= '1';
			END IF;
		END IF;
	END PROCESS;

`endif

`ifdef USE_AUDIO_OUT
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				write_interrupt_en <= '0';
			ELSIF ((chipselect = '1') AND (write = '1') AND (address = B"00")) THEN
				write_interrupt_en <= writedata(1);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				clear_write_fifos <= '0';
			ELSIF ((chipselect = '1') AND (write = '1') AND (address = B"00")) THEN
				clear_write_fifos <= writedata(3);
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
				write_interrupt <= 
					(AND_REDUCE (left_channel_write_space(6 DOWNTO 5))  OR left_channel_write_space(7)) OR 
					(AND_REDUCE (right_channel_write_space(6 DOWNTO 5)) OR right_channel_write_space(7));
			END IF;
		END IF;
	END PROCESS;



	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				done_dac_channel_sync <= '0';
			ELSIF (dac_lrclk_falling_edge = '1') THEN
				done_dac_channel_sync <= '1';
			END IF;
		END IF;
	END PROCESS;

`endif

-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************


	Bit_Clock_Edges : altera_up_clock_edge 
	PORT MAP (
		-- Inputs
		clk				=> clk,
		reset				=> reset,
		
		test_clk			=> AUD_BCLK,
		
		-- Bidirectionals
	
		-- Outputs
		rising_edge		=> bclk_rising_edge,
		falling_edge	=> bclk_falling_edge
	);

`ifdef USE_AUDIO_IN
	ADC_Left_Right_Clock_Edges : altera_up_clock_edge 
	PORT MAP (
		-- Inputs
		clk				=> clk,
		reset				=> reset,
		
		test_clk			=> AUD_ADCLRCK,
		
		-- Bidirectionals
	
		-- Outputs
		rising_edge		=> adc_lrclk_rising_edge,
		falling_edge	=> adc_lrclk_falling_edge
	);

`endif

`ifdef USE_AUDIO_OUT
	DAC_Left_Right_Clock_Edges : altera_up_clock_edge 
	PORT MAP (
		-- Inputs
		clk				=> clk,
		reset				=> reset,
		
		test_clk			=> AUD_DACLRCK,
		
		-- Bidirectionals
	
		-- Outputs
		rising_edge		=> dac_lrclk_rising_edge,
		falling_edge	=> dac_lrclk_falling_edge
	);

`endif

`ifdef USE_AUDIO_IN
	Audio_In_Deserializer : altera_up_audio_in_deserializer 
	GENERIC MAP (
		DW										=> DW,
		BIT_COUNTER_INIT					=> BIT_COUNTER_INIT
	)
	PORT MAP (
		-- Inputs
		clk									=> clk,
		reset									=> reset OR clear_read_fifos,
		
		bit_clk_rising_edge				=> bclk_rising_edge,
		bit_clk_falling_edge				=> bclk_falling_edge,
		left_right_clk_rising_edge		=> adc_lrclk_rising_edge,
		left_right_clk_falling_edge	=> adc_lrclk_falling_edge,
	
		done_channel_sync					=> done_adc_channel_sync,
	
		serial_audio_in_data				=> AUD_ADCDAT,
	
		read_left_audio_data_en			=> address(1) AND (NOT address(0)) AND chipselect AND read,
		read_right_audio_data_en		=> address(1) AND address(0) AND chipselect AND read,
	
		-- Bidirectionals
	
		-- Outputs
		left_audio_fifo_read_space		=> left_channel_read_available,
		right_audio_fifo_read_space	=> right_channel_read_available,
	
		left_channel_data					=> new_left_channel_audio,
		right_channel_data				=> new_right_channel_audio
	);
`endif

`ifdef USE_AUDIO_OUT
	Audio_Out_Serializer : altera_up_audio_out_serializer 
	GENERIC MAP (
		DW											=> DW
	)
	PORT MAP (
		-- Inputs
		clk										=> clk,
		reset										=> reset OR clear_write_fifos,
		
		bit_clk_rising_edge					=> bclk_rising_edge,
		bit_clk_falling_edge					=> bclk_falling_edge,
		left_right_clk_rising_edge			=> done_dac_channel_sync AND dac_lrclk_rising_edge,
		left_right_clk_falling_edge		=> done_dac_channel_sync AND dac_lrclk_falling_edge,
		
		left_channel_data						=> writedata(DW DOWNTO 0),
		left_channel_data_en					=> address(1) AND (NOT address(0)) AND chipselect AND write,
	
		right_channel_data					=> writedata(DW DOWNTO 0),
		right_channel_data_en				=> address(1) AND address(0) AND chipselect AND write,
		
		-- Bidirectionals
	
		-- Outputs
		left_channel_fifo_write_space		=> left_channel_write_space,
		right_channel_fifo_write_space	=> right_channel_write_space,
	
		serial_audio_out_data				=> AUD_DACDAT
	);
`endif


END Behaviour;
