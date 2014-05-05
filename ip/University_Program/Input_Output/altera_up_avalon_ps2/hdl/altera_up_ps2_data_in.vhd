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
-- * This module accepts incoming data from a PS2 core.                         *
-- *                                                                            *
-- ******************************************************************************


ENTITY altera_up_ps2_data_in IS 


-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************

-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
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

END altera_up_ps2_data_in;

ARCHITECTURE Behaviour OF altera_up_ps2_data_in IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	-- states
	TYPE State_Type IS (	PS2_STATE_0_IDLE,
								PS2_STATE_1_WAIT_FOR_DATA,
								PS2_STATE_2_DATA_IN,
								PS2_STATE_3_PARITY_IN,
								PS2_STATE_4_STOP_IN
							);
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	SIGNAL	data_count			:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	SIGNAL	data_shift_reg		:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
	
	-- State Machine Registers
	SIGNAL	ns_ps2_receiver	:State_Type;	
	SIGNAL	s_ps2_receiver		:State_Type;	
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
BEGIN
-- *****************************************************************************
-- *                         Finite State Machine(s)                           *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				s_ps2_receiver <= PS2_STATE_0_IDLE;
			ELSE
				s_ps2_receiver <= ns_ps2_receiver;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (ns_ps2_receiver, s_ps2_receiver, wait_for_incoming_data, 
				received_data_en, start_receiving_data, ps2_data, ps2_clk_posedge, 
				data_count)
	BEGIN
		-- Defaults
		ns_ps2_receiver <= PS2_STATE_0_IDLE;
	
		CASE (s_ps2_receiver) IS
		WHEN PS2_STATE_0_IDLE =>
			IF ((wait_for_incoming_data = '1') AND 
					(received_data_en = '0')) THEN
				ns_ps2_receiver <= PS2_STATE_1_WAIT_FOR_DATA;
			ELSIF ((start_receiving_data = '1') AND 
					(received_data_en = '0')) THEN
				ns_ps2_receiver <= PS2_STATE_2_DATA_IN;
			ELSE
				ns_ps2_receiver <= PS2_STATE_0_IDLE;
			END IF;
		WHEN PS2_STATE_1_WAIT_FOR_DATA =>
			IF ((ps2_data = '0') AND (ps2_clk_posedge = '1')) THEN
				ns_ps2_receiver <= PS2_STATE_2_DATA_IN;
			ELSIF (wait_for_incoming_data = '0') THEN
				ns_ps2_receiver <= PS2_STATE_0_IDLE;
			ELSE
				ns_ps2_receiver <= PS2_STATE_1_WAIT_FOR_DATA;
			END IF;
		WHEN PS2_STATE_2_DATA_IN =>
			IF ((data_count = B"0111") AND (ps2_clk_posedge = '1')) THEN
				ns_ps2_receiver <= PS2_STATE_3_PARITY_IN;
			ELSE
				ns_ps2_receiver <= PS2_STATE_2_DATA_IN;
			END IF;
		WHEN PS2_STATE_3_PARITY_IN =>
			IF (ps2_clk_posedge = '1') THEN
				ns_ps2_receiver <= PS2_STATE_4_STOP_IN;
			ELSE
				ns_ps2_receiver <= PS2_STATE_3_PARITY_IN;
			END IF;
		WHEN PS2_STATE_4_STOP_IN =>
			IF (ps2_clk_posedge = '1') THEN
				ns_ps2_receiver <= PS2_STATE_0_IDLE;
			ELSE
				ns_ps2_receiver <= PS2_STATE_4_STOP_IN;
			END IF;
		WHEN OTHERS =>
			ns_ps2_receiver <= PS2_STATE_0_IDLE;
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN 
				data_count	<= (OTHERS => '0');
			ELSIF ((s_ps2_receiver = PS2_STATE_2_DATA_IN) AND 
					(ps2_clk_posedge = '1')) THEN
				data_count	<= data_count + 1;
			ELSIF (s_ps2_receiver /= PS2_STATE_2_DATA_IN) THEN
				data_count	<= (OTHERS => '0');
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				data_shift_reg			<= B"00000000";
			ELSIF ((s_ps2_receiver = PS2_STATE_2_DATA_IN) AND 
					(ps2_clk_posedge = '1')) THEN
				data_shift_reg	<= (ps2_data & data_shift_reg(7 DOWNTO 1));
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				received_data		<= B"00000000";
			ELSIF (s_ps2_receiver = PS2_STATE_4_STOP_IN) THEN
				received_data	<= data_shift_reg;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				received_data_en		<= '0';
			ELSIF ((s_ps2_receiver = PS2_STATE_4_STOP_IN) AND 
					(ps2_clk_posedge = '1')) THEN
				received_data_en	<= '1';
			ELSE
				received_data_en	<= '0';
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************


-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
