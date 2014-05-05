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

ENTITY altera_up_avalon_st_audio IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************

-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk									:IN		STD_LOGIC;
	reset									:IN		STD_LOGIC;

`ifdef USE_AUDIO_IN
	from_adc_left_channel_ready	:IN		STD_LOGIC;
	from_adc_right_channel_ready	:IN		STD_LOGIC;
`endif

`ifdef USE_AUDIO_OUT
	to_dac_left_channel_data		:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	
	to_dac_left_channel_valid		:IN		STD_LOGIC;
	to_dac_right_channel_data		:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);	
	to_dac_right_channel_valid		:IN		STD_LOGIC;
`endif

`ifdef USE_AUDIO_IN
	AUD_ADCDAT							:IN		STD_LOGIC;
`endif
`ifdef USE_AUDIO_IN
	AUD_ADCLRCK							:IN		STD_LOGIC;
`endif
	AUD_BCLK								:IN		STD_LOGIC;
`ifdef USE_AUDIO_OUT
	AUD_DACLRCK							:IN		STD_LOGIC;
`endif

	-- Bidirectionals

	-- Outputs
`ifdef USE_AUDIO_IN
	from_adc_left_channel_data		:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	
	from_adc_left_channel_valid	:BUFFER	STD_LOGIC;
	from_adc_right_channel_data	:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);	
	from_adc_right_channel_valid	:BUFFER	STD_LOGIC;
`endif

`ifdef USE_AUDIO_OUT
	to_dac_left_channel_ready		:BUFFER	STD_LOGIC;
	to_dac_right_channel_ready		:BUFFER	STD_LOGIC;
`endif

`ifdef USE_AUDIO_OUT
	AUD_DACDAT							:BUFFER	STD_LOGIC
`endif

);

END altera_up_avalon_st_audio;

ARCHITECTURE Behaviour OF altera_up_avalon_st_audio IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	
	CONSTANT	DW						:INTEGER									:= 15;
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
`endif
`ifdef USE_AUDIO_OUT
	SIGNAL	done_dac_channel_sync			:STD_LOGIC;
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

	-- Output Registers

	-- Internal Registers
`ifdef USE_AUDIO_IN
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

	-- Output Assignments
	from_adc_left_channel_valid <= ( OR (left_channel_read_available));
	from_adc_right_channel_valid <= ( OR (right_channel_read_available));

	to_dac_left_channel_ready <= ( OR (left_channel_write_space));
	to_dac_right_channel_ready <= ( OR (right_channel_write_space));

	-- Internal Assignments

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
		reset									=> reset,
		
		bit_clk_rising_edge				=> bclk_rising_edge,
		bit_clk_falling_edge				=> bclk_falling_edge,
		left_right_clk_rising_edge		=> adc_lrclk_rising_edge,
		left_right_clk_falling_edge	=> adc_lrclk_falling_edge,
	
		done_channel_sync					=> done_adc_channel_sync,
	
		serial_audio_in_data				=> AUD_ADCDAT,
	
		read_left_audio_data_en			=> from_adc_left_channel_valid AND from_adc_left_channel_ready,
		read_right_audio_data_en		=> from_adc_right_channel_valid AND from_adc_right_channel_ready,
	
		-- Bidirectionals
	
		-- Outputs
		left_audio_fifo_read_space		=> left_channel_read_available,
		right_audio_fifo_read_space	=> right_channel_read_available,
	
		left_channel_data					=> from_adc_left_channel_data,
		right_channel_data				=> from_adc_right_channel_data
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
		reset										=> reset,
		
		bit_clk_rising_edge					=> bclk_rising_edge,
		bit_clk_falling_edge					=> bclk_falling_edge,
		left_right_clk_rising_edge			=> done_dac_channel_sync AND dac_lrclk_rising_edge,
		left_right_clk_falling_edge		=> done_dac_channel_sync AND dac_lrclk_falling_edge,
		
		left_channel_data						=> to_dac_left_channel_data,
		left_channel_data_en					=> to_dac_left_channel_valid AND to_dac_left_channel_ready,
	
		right_channel_data					=> to_dac_right_channel_data,
		right_channel_data_en				=> to_dac_right_channel_valid AND to_dac_right_channel_ready,
		
		-- Bidirectionals
	
		-- Outputs
		left_channel_fifo_write_space		=> left_channel_write_space,
		right_channel_fifo_write_space	=> right_channel_write_space,
	
		serial_audio_out_data				=> AUD_DACDAT
	);
`endif

END Behaviour;
