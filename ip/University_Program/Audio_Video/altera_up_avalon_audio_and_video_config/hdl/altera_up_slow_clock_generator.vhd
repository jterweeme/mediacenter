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
-- * This module can create clock signals that have a frequency lower           *
-- *  than those a PLL can generate.                                            *
-- *                                                                            *
-- * Revision: 1.2                                                              *
-- *                                                                            *
-- * Used in IP Cores:                                                          *
-- *   Altera UP Avalon Audio and Video Config                                  *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_slow_clock_generator IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************
	
GENERIC (
	
	CB				:INTEGER									:= 10		--COUNTER BITS
	
);
-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (

	-- Inputs
	clk						:IN		STD_LOGIC;
	reset						:IN		STD_LOGIC;

	enable_clk				:IN		STD_LOGIC;
	
	-- Bidirectionals

	-- Outputs
	new_clk					:BUFFER	STD_LOGIC;

	rising_edge				:BUFFER	STD_LOGIC;
	falling_edge			:BUFFER	STD_LOGIC;

	middle_of_high_level	:BUFFER	STD_LOGIC;
	middle_of_low_level	:BUFFER	STD_LOGIC

);

END altera_up_slow_clock_generator;

ARCHITECTURE Behaviour OF altera_up_slow_clock_generator IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	
	-- Internal Registers
	SIGNAL	clk_counter	:STD_LOGIC_VECTOR(CB DOWNTO 1);	
	
	-- State Machine Registers
	
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

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				clk_counter	<= (OTHERS => '0');
			ELSIF (enable_clk = '1') THEN
				clk_counter	<= clk_counter + 1;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				new_clk	<= '0';
			ELSE
				new_clk	<= clk_counter(CB);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				rising_edge	<= '0';
			ELSE
				rising_edge	<= (clk_counter(CB) XOR new_clk) AND NOT new_clk;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				falling_edge <= '0';
			ELSE
				falling_edge <= (clk_counter(CB) XOR new_clk) AND new_clk;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				middle_of_high_level <= '0';
			ELSE
				middle_of_high_level <= 
					clk_counter(CB) AND 
					 NOT clk_counter((CB - 1)) AND 
					( AND_REDUCE (clk_counter((CB - 2) DOWNTO 1)));
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				middle_of_low_level <= '0';
			ELSE
				middle_of_low_level <= 
					 NOT clk_counter(CB) AND 
					 NOT clk_counter((CB - 1)) AND 
					( AND_REDUCE (clk_counter((CB - 2) DOWNTO 1)));
			END IF;
		END IF;
	END PROCESS;




-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments

	-- Internal Assignments

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************


END Behaviour;
