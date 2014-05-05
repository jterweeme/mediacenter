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
-- * This module writes data to the Audio DAC on the Altera DE2 board.          *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_audio_out_serializer IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	DW	:INTEGER									:= 15
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
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

END altera_up_audio_out_serializer;

ARCHITECTURE Behaviour OF altera_up_audio_out_serializer IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	SIGNAL	read_left_channel					:STD_LOGIC;
	SIGNAL	read_right_channel				:STD_LOGIC;
	
	SIGNAL	left_channel_fifo_is_empty		:STD_LOGIC;
	SIGNAL	right_channel_fifo_is_empty	:STD_LOGIC;
	
	SIGNAL	left_channel_fifo_is_full		:STD_LOGIC;
	SIGNAL	right_channel_fifo_is_full		:STD_LOGIC;
	
	SIGNAL	left_channel_fifo_used			:STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	SIGNAL	right_channel_fifo_used			:STD_LOGIC_VECTOR( 6 DOWNTO  0);	
	
	SIGNAL	left_channel_from_fifo			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	SIGNAL	right_channel_from_fifo			:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	
	-- Internal Registers
	SIGNAL	left_channel_was_read			:STD_LOGIC;
	SIGNAL	data_out_shift_reg				:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	
	-- State Machine Registers
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altera_up_sync_fifo
	GENERIC ( 
		DW					:INTEGER;
		DATA_DEPTH		:INTEGER;
		AW					:INTEGER
	);
	PORT (
		-- Inputs
		clk				:IN		STD_LOGIC;
		reset				:IN		STD_LOGIC;

		write_en			:IN		STD_LOGIC;
		write_data		:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);

		read_en			:IN		STD_LOGIC;
	
		-- Bidirectionals

		-- Outputs
		fifo_is_empty	:BUFFER	STD_LOGIC;
		fifo_is_full	:BUFFER	STD_LOGIC;
		words_used		:BUFFER	STD_LOGIC_VECTOR( 6 DOWNTO  0);

		read_data		:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0)
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
				left_channel_fifo_write_space <= B"00000000";
			ELSE
				left_channel_fifo_write_space <= B"10000000" - 
						(left_channel_fifo_is_full & left_channel_fifo_used);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				right_channel_fifo_write_space <= B"00000000";
			ELSE
				right_channel_fifo_write_space <= B"10000000" - 
						(right_channel_fifo_is_full & right_channel_fifo_used);
			END IF;
		END IF;
	END PROCESS;



	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				serial_audio_out_data <= '0';
			ELSE
				serial_audio_out_data <= data_out_shift_reg(DW);
			END IF;
		END IF;
	END PROCESS;



	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				left_channel_was_read <= '0';
			ELSIF (read_left_channel = '1') THEN
				left_channel_was_read <= '1';
			ELSIF (read_right_channel = '1') THEN
				left_channel_was_read <= '0';
			END IF;
		END IF;
	END PROCESS;



	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				data_out_shift_reg	<= (OTHERS => '0');
			ELSIF (read_left_channel = '1') THEN
				data_out_shift_reg	<= left_channel_from_fifo;
			ELSIF (read_right_channel = '1') THEN
				data_out_shift_reg	<= right_channel_from_fifo;
			ELSIF ((left_right_clk_rising_edge = '1') OR 
						(left_right_clk_falling_edge = '1')) THEN
				data_out_shift_reg	<= (OTHERS => '0');
			ELSIF (bit_clk_falling_edge = '1') THEN
				data_out_shift_reg	<= 
					(data_out_shift_reg((DW - 1) DOWNTO 0) & '0');
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	read_left_channel <= left_right_clk_rising_edge AND 
											 NOT left_channel_fifo_is_empty AND 
											 NOT right_channel_fifo_is_empty;
	read_right_channel <= left_right_clk_falling_edge AND 
											left_channel_was_read;

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************

	Audio_Out_Left_Channel_FIFO : altera_up_sync_fifo
	GENERIC MAP ( 
		DW					=> DW,
		DATA_DEPTH		=> 128,
		AW					=> 6
	)
	PORT MAP (
		-- Inputs
		clk				=> clk,
		reset				=> reset,
	
		write_en			=> left_channel_data_en AND NOT left_channel_fifo_is_full,
		write_data		=> left_channel_data,
	
		read_en			=> read_left_channel,
		
		-- Bidirectionals
	
		-- Outputs
		fifo_is_empty	=> left_channel_fifo_is_empty,
		fifo_is_full	=> left_channel_fifo_is_full,
		words_used		=> left_channel_fifo_used,
	
		read_data		=> left_channel_from_fifo
	);

	Audio_Out_Right_Channel_FIFO : altera_up_sync_fifo
	GENERIC MAP ( 
		DW					=> DW,
		DATA_DEPTH		=> 128,
		AW					=> 6
	)
	PORT MAP (
		-- Inputs
		clk				=> clk,
		reset				=> reset,
	
		write_en			=> right_channel_data_en AND NOT right_channel_fifo_is_full,
		write_data		=> right_channel_data,
	
		read_en			=> read_right_channel,
		
		-- Bidirectionals
	
		-- Outputs
		fifo_is_empty	=> right_channel_fifo_is_empty,
		fifo_is_full	=> right_channel_fifo_is_full,
		words_used		=> right_channel_fifo_used,
	
		read_data		=> right_channel_from_fifo
	);


END Behaviour;
