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
-- * This module reads and writes to the ssram chip on the DE2-70 board,        *
-- *  with 4-cycle read latency and one cycle write latency.                    *
-- *                                                                            *
-- ******************************************************************************


ENTITY altera_up_avalon_ssram IS 


-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************

-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	clk				:IN		STD_LOGIC;
	reset				:IN		STD_LOGIC;

	address			:IN		STD_LOGIC_VECTOR(18 DOWNTO  0);	
	byteenable		:IN		STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	read				:IN		STD_LOGIC;
	write				:IN		STD_LOGIC;
	writedata		:IN		STD_LOGIC_VECTOR(31 DOWNTO  0);	

	-- Bi-Directional
	SRAM_DQ			:INOUT	STD_LOGIC_VECTOR(31 DOWNTO  0);	--	SRAM Data Bus 32 Bits
	SRAM_DPA			:INOUT	STD_LOGIC_VECTOR( 3 DOWNTO  0);		--  SRAM Parity Data Bus

	-- Outputs
	readdata			:BUFFER	STD_LOGIC_VECTOR(31 DOWNTO  0);	
	readdatavalid	:BUFFER	STD_LOGIC;
	waitrequest		:BUFFER	STD_LOGIC;

	SRAM_CLK			:BUFFER	STD_LOGIC;								--	SRAM Clock
	SRAM_ADDR		:BUFFER	STD_LOGIC_VECTOR(18 DOWNTO  0);	--	SRAM Address bus 21 Bits
	SRAM_ADSC_N		:BUFFER	STD_LOGIC;								--	SRAM Controller Address Status 	
	SRAM_ADSP_N		:BUFFER	STD_LOGIC;								--	SRAM Processor Address Status
	SRAM_ADV_N		:BUFFER	STD_LOGIC;								--	SRAM Burst Address Advance
	SRAM_BE_N		:BUFFER	STD_LOGIC_VECTOR( 3 DOWNTO  0);		--	SRAM Byte Write Enable
	SRAM_CE1_N		:BUFFER	STD_LOGIC;								--	SRAM Chip Enable
	SRAM_CE2			:BUFFER	STD_LOGIC;								--	SRAM Chip Enable
	SRAM_CE3_N		:BUFFER	STD_LOGIC;								--	SRAM Chip Enable
	SRAM_GW_N		:BUFFER	STD_LOGIC;								--  SRAM Global Write Enable
	SRAM_OE_N		:BUFFER	STD_LOGIC;								--	SRAM Output Enable
	SRAM_WE_N		:BUFFER	STD_LOGIC								--	SRAM Write Enable

);

END altera_up_avalon_ssram;

ARCHITECTURE Behaviour OF altera_up_avalon_ssram IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************

-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	
	-- Internal Wires
	SIGNAL	internal_byteenable	:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	SIGNAL	start_write				:STD_LOGIC;
	
	-- Internal Registers
	SIGNAL	read_pipeline			:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	SIGNAL	byteenable_reg			:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	SIGNAL	writedata_reg			:STD_LOGIC_VECTOR(31 DOWNTO  0);	
	
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

	-- Output Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			readdata				<= SRAM_DQ;
		
			SRAM_ADDR			<= address;
			SRAM_ADSP_N			<= '1';
			SRAM_ADSC_N			<= NOT (read OR start_write);
			SRAM_ADV_N			<= '1';
			SRAM_BE_N(3)		<= NOT internal_byteenable(3);
			SRAM_BE_N(2)		<= NOT internal_byteenable(2);
			SRAM_BE_N(1)		<= NOT internal_byteenable(1);
			SRAM_BE_N(0)		<= NOT internal_byteenable(0);
			SRAM_CE1_N			<= NOT (read OR start_write);
			SRAM_CE2				<= (read OR start_write);
			SRAM_CE3_N			<= NOT (read OR start_write);
			SRAM_GW_N			<= '1';
			SRAM_OE_N			<= NOT (read_pipeline(1));
			SRAM_WE_N			<= NOT start_write;
		END IF;
	END PROCESS;


	-- Internal Registers
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				read_pipeline	<= B"0000";
			ELSE
				read_pipeline	<= (read_pipeline(2 DOWNTO 0) & read);
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (reset = '1') THEN
				byteenable_reg	<= B"0000";
			ELSE
				byteenable_reg	<= internal_byteenable;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			writedata_reg		<= writedata;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments
	readdatavalid 	<= read_pipeline(3);
	waitrequest 	<= write AND (OR_REDUCE(read_pipeline(2 DOWNTO 0)));

	SRAM_DQ(31 DOWNTO 24) <= writedata_reg(31 DOWNTO 24) WHEN (internal_byteenable(3) = '1') ELSE (OTHERS => 'Z');
	SRAM_DQ(23 DOWNTO 16) <= writedata_reg(23 DOWNTO 16) WHEN (internal_byteenable(2) = '1') ELSE (OTHERS => 'Z');
	SRAM_DQ(15 DOWNTO  8) <= writedata_reg(15 DOWNTO  8) WHEN (internal_byteenable(1) = '1') ELSE (OTHERS => 'Z');
	SRAM_DQ( 7 DOWNTO  0) <= writedata_reg( 7 DOWNTO  0) WHEN (internal_byteenable(0) = '1') ELSE (OTHERS => 'Z');

	SRAM_DPA <= (OTHERS => 'Z');

	SRAM_CLK <= clk;

	-- Internal Assignments
	internal_byteenable(3) <= byteenable(3) AND start_write;
	internal_byteenable(2) <= byteenable(2) AND start_write;
	internal_byteenable(1) <= byteenable(1) AND start_write;
	internal_byteenable(0) <= byteenable(0) AND start_write;

	start_write <= write AND NOT (OR_REDUCE(read_pipeline(2 DOWNTO 0)));

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************



END Behaviour;
