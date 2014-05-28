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
-- * This module controls 16x2 character LCD on the Altera DE2 Board.           *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_avalon_character_lcd IS 


-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	CURSOR_ON	:STD_LOGIC								:= '1';
	BLINKING_ON	:STD_LOGIC								:= '1'
	
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
	read			:IN		STD_LOGIC;
	write			:IN		STD_LOGIC;
	writedata	:IN		STD_LOGIC_VECTOR( 7 DOWNTO  0);	

	-- Bidirectionals
	LCD_DATA		:INOUT	STD_LOGIC_VECTOR( 7 DOWNTO  0);	--	LCD Data bus 8 bits

	-- Outputs
	LCD_ON		:BUFFER	STD_LOGIC;								--	LCD Power ON/OFF
	LCD_BLON		:BUFFER	STD_LOGIC;								--	LCD Back Light ON/OFF

	LCD_EN		:BUFFER	STD_LOGIC;								--	LCD Enable
	LCD_RS		:BUFFER	STD_LOGIC;								--	LCD 0-Command/1-Data Select
	LCD_RW		:BUFFER	STD_LOGIC;								--	LCD 1-Read/0-Write Select

	readdata		:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	waitrequest	:BUFFER	STD_LOGIC

);

END altera_up_avalon_character_lcd;

ARCHITECTURE Behaviour OF altera_up_avalon_character_lcd IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	-- states
	TYPE State_Type IS (	LCD_STATE_0_IDLE,
								LCD_STATE_1_INITIALIZE,
								LCD_STATE_2_START_CHECK_BUSY,
								LCD_STATE_3_CHECK_BUSY,
								LCD_STATE_4_BEGIN_TRANSFER,
								LCD_STATE_5_TRANSFER,
								LCD_STATE_6_COMPLETE
							);
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	SIGNAL	transfer_complete			:STD_LOGIC;
	SIGNAL	done_initialization		:STD_LOGIC;
	
	SIGNAL	init_send_command			:STD_LOGIC;
	SIGNAL	init_command				:STD_LOGIC_VECTOR( 8 DOWNTO  0);	
	
	SIGNAL	send_data					:STD_LOGIC;
	
	SIGNAL	data_received				:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	
	
	-- Internal Registers
	SIGNAL	initialize_lcd_display	:STD_LOGIC;
	
	SIGNAL	data_to_send				:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	SIGNAL	rs								:STD_LOGIC;
	SIGNAL	rw								:STD_LOGIC;
	
	-- State Machine Registers
	SIGNAL	ns_lcd_controller			:State_Type;	
	SIGNAL	s_lcd_controller			:State_Type;	
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altera_up_character_lcd_communication
	PORT (
		-- Inputs
		clk					:IN		STD_LOGIC;
		reset					:IN		STD_LOGIC;

		data_in				:IN		STD_LOGIC_VECTOR( 7 DOWNTO  0);
		enable				:IN		STD_LOGIC;
		rs						:IN		STD_LOGIC;
		rw						:IN		STD_LOGIC;

		display_on			:IN		STD_LOGIC;
		back_light_on		:IN		STD_LOGIC;

		-- Bidirectionals
		LCD_DATA				:INOUT	STD_LOGIC_VECTOR( 7 DOWNTO  0);

		-- Outputs
		LCD_ON				:BUFFER	STD_LOGIC;
		LCD_BLON				:BUFFER	STD_LOGIC;

		LCD_EN				:BUFFER	STD_LOGIC;
		LCD_RS				:BUFFER	STD_LOGIC;
		LCD_RW				:BUFFER	STD_LOGIC;

		data_out				:BUFFER	STD_LOGIC_VECTOR( 7 DOWNTO  0);
		transfer_complete	:BUFFER	STD_LOGIC
	);
	END COMPONENT;

	COMPONENT altera_up_character_lcd_initialization
	GENERIC (
		CURSOR_ON					:STD_LOGIC;
		BLINKING_ON					:STD_LOGIC
	);
	PORT (
		-- Inputs
		clk							:IN		STD_LOGIC;
		reset							:IN		STD_LOGIC;

		initialize_LCD_display	:IN		STD_LOGIC;

		command_was_sent			:IN		STD_LOGIC;
	
		-- Bidirectionals

		-- Outputs
		done_initialization		:BUFFER	STD_LOGIC;

		send_command				:BUFFER	STD_LOGIC;
		the_command					:BUFFER	STD_LOGIC_VECTOR( 8 DOWNTO  0)
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
				s_lcd_controller <= LCD_STATE_0_IDLE;
			ELSE
				s_lcd_controller <= ns_lcd_controller;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (ns_lcd_controller, s_lcd_controller, initialize_lcd_display, 
				chipselect, done_initialization, transfer_complete, data_received)
	BEGIN
		-- Defaults
		ns_lcd_controller <= LCD_STATE_0_IDLE;
	
	   CASE (s_lcd_controller) IS
		WHEN LCD_STATE_0_IDLE =>
			IF (initialize_lcd_display = '1') THEN
				ns_lcd_controller <= LCD_STATE_1_INITIALIZE;
			ELSIF (chipselect = '1') THEN
				ns_lcd_controller <= LCD_STATE_2_START_CHECK_BUSY; 
			ELSE
				ns_lcd_controller <= LCD_STATE_0_IDLE;
			END IF;
		WHEN LCD_STATE_1_INITIALIZE =>
			IF (done_initialization = '1') THEN
				ns_lcd_controller <= LCD_STATE_6_COMPLETE;
			ELSE
				ns_lcd_controller <= LCD_STATE_1_INITIALIZE;
			END IF;
		WHEN LCD_STATE_2_START_CHECK_BUSY =>
			IF (transfer_complete = '0') THEN
				ns_lcd_controller <= LCD_STATE_3_CHECK_BUSY;
			ELSE
				ns_lcd_controller <= LCD_STATE_2_START_CHECK_BUSY;
			END IF;
		WHEN LCD_STATE_3_CHECK_BUSY =>
			IF ((transfer_complete = '1') AND (data_received(7) = '1')) THEN
				ns_lcd_controller <= LCD_STATE_2_START_CHECK_BUSY;
			ELSIF ((transfer_complete = '1') AND (data_received(7) = '0')) THEN
				ns_lcd_controller <= LCD_STATE_4_BEGIN_TRANSFER;
			ELSE
				ns_lcd_controller <= LCD_STATE_3_CHECK_BUSY;
			END IF;
		WHEN LCD_STATE_4_BEGIN_TRANSFER =>
			IF (transfer_complete = '0') THEN
				ns_lcd_controller <= LCD_STATE_5_TRANSFER;
			ELSE
				ns_lcd_controller <= LCD_STATE_4_BEGIN_TRANSFER;
			END IF;
		WHEN LCD_STATE_5_TRANSFER =>
			IF (transfer_complete = '1') THEN
				ns_lcd_controller <= LCD_STATE_6_COMPLETE;
			ELSE
				ns_lcd_controller <= LCD_STATE_5_TRANSFER;
			END IF;
		WHEN LCD_STATE_6_COMPLETE =>
			ns_lcd_controller <= LCD_STATE_0_IDLE;
		WHEN OTHERS =>
			ns_lcd_controller <= LCD_STATE_0_IDLE;
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				initialize_lcd_display <= '1';
			ELSIF (done_initialization = '1') THEN
				initialize_lcd_display <= '0';
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				data_to_send <= B"00000000";
			ELSIF (s_lcd_controller = LCD_STATE_1_INITIALIZE) THEN
				data_to_send <= init_command(7 DOWNTO 0);
			ELSIF (s_lcd_controller = LCD_STATE_4_BEGIN_TRANSFER) THEN
				data_to_send <= writedata(7 DOWNTO 0);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				rs <= '0';
			ELSIF (s_lcd_controller = LCD_STATE_1_INITIALIZE) THEN
				rs <= init_command(8);
			ELSIF (s_lcd_controller = LCD_STATE_2_START_CHECK_BUSY) THEN
				rs <= '0';
			ELSIF (s_lcd_controller = LCD_STATE_4_BEGIN_TRANSFER) THEN
				rs <= address;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				rw <= '0';
			ELSIF (s_lcd_controller = LCD_STATE_1_INITIALIZE) THEN
				rw <= '0';
			ELSIF (s_lcd_controller = LCD_STATE_2_START_CHECK_BUSY) THEN
				rw <= '1';
			ELSIF (s_lcd_controller = LCD_STATE_4_BEGIN_TRANSFER) THEN
				rw <= NOT write;
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments
	readdata 	<= data_received;

	waitrequest <= 
		'1' WHEN ((chipselect = '1') AND (s_lcd_controller /= LCD_STATE_6_COMPLETE))
		ELSE '0';

	-- Internal Assignments
	send_data 	<= 
		init_send_command WHEN (s_lcd_controller = LCD_STATE_1_INITIALIZE) ELSE 
		'1' WHEN (s_lcd_controller = LCD_STATE_3_CHECK_BUSY) ELSE 
		'1' WHEN (s_lcd_controller = LCD_STATE_5_TRANSFER) ELSE 
		'0';

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************


	Char_LCD_Comm : altera_up_character_lcd_communication 
	PORT MAP (
		-- Inputs
		clk					=> clk,
		reset					=> reset,
	
		data_in				=> data_to_send,
		enable				=> send_data,
		rs						=> rs,
		rw						=> rw,
	
		display_on			=> '1',
		back_light_on		=> '1',
	
		-- Bidirectionals
		LCD_DATA				=> LCD_DATA,
	
		-- Outputs
		LCD_ON				=> LCD_ON,
		LCD_BLON				=> LCD_BLON,
	
		LCD_EN				=> LCD_EN,
		LCD_RS				=> LCD_RS,
		LCD_RW				=> LCD_RW,
	
		data_out				=> data_received,
		transfer_complete	=> transfer_complete
	);

	Char_LCD_Init : altera_up_character_lcd_initialization 
	GENERIC MAP (
		CURSOR_ON					=> CURSOR_ON,
		BLINKING_ON					=> BLINKING_ON
	)
	PORT MAP (
		-- Inputs
		clk							=> clk,
		reset							=> reset,
	
		initialize_LCD_display	=> initialize_lcd_display,
	
		command_was_sent			=> transfer_complete,
		
		-- Bidirectionals
	
		-- Outputs
		done_initialization		=> done_initialization,
	
		send_command				=> init_send_command,
		the_command					=> init_command
	);


END Behaviour;
