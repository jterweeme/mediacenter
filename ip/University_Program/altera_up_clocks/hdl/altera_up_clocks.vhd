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
-- *   This module generates the clocks needed for the I/O devices on           *
-- * Altera's DE-series boards.                                                 *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_clocks IS 

-- *****************************************************************************
-- *                             Generic Declarations                          *
-- *****************************************************************************

-- *****************************************************************************
-- *                             Port Declarations                             *
-- *****************************************************************************
PORT (
	-- Inputs
	CLOCK_50		:IN		STD_LOGIC;
`ifdef USE_AUDIO_CLK
	CLOCK_27		:IN		STD_LOGIC;
`endif
	reset			:IN		STD_LOGIC;

	-- Bidirectionals

	-- Outputs
`ifdef USE_SDRAM_CLK
	SDRAM_CLK	:BUFFER	STD_LOGIC;
`endif
`ifdef USE_VGA_CLK
`ifdef USE_TPAD
	VGA_CLK_40	:BUFFER	STD_LOGIC;
`elsif USE_VEEK_MT
	VGA_CLK_33	:BUFFER	STD_LOGIC;
`endif
	VGA_CLK		:BUFFER	STD_LOGIC;
`endif
`ifdef USE_AUDIO_CLK
	AUD_CLK		:BUFFER	STD_LOGIC;
`endif
	sys_clk		:BUFFER	STD_LOGIC;
	sys_reset_n	:BUFFER	STD_LOGIC

);

END altera_up_clocks;

ARCHITECTURE Behaviour OF altera_up_clocks IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
	
	CONSTANT	SYS_CLK_MULT	:INTEGER									:= 1;
	CONSTANT	SYS_CLK_DIV		:INTEGER									:= 1;
	
`ifdef USE_AUDIO_CLK
	CONSTANT	AUD_CLK_MULT	:INTEGER									:= 14;
	CONSTANT	AUD_CLK_DIV		:INTEGER									:= 31;
`endif
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
`ifdef USE_TPAD
	SIGNAL	sys_mem_clks		:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
`ifdef USE_AUDIO_CLK
	SIGNAL	audio_clks			:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
`endif
`elsif USE_VEEK_MT
	SIGNAL	sys_mem_clks		:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
`ifdef USE_AUDIO_CLK
	SIGNAL	audio_clks			:STD_LOGIC_VECTOR( 3 DOWNTO  0);	
`endif
`else
	SIGNAL	sys_mem_clks		:STD_LOGIC_VECTOR( 2 DOWNTO  0);	
`ifdef USE_AUDIO_CLK
	SIGNAL	audio_clks			:STD_LOGIC_VECTOR( 2 DOWNTO  0);	
`endif
`endif
	
	SIGNAL	clk_locked			:STD_LOGIC;
`ifdef USE_AUDIO_CLK
	SIGNAL	audio_clk_locked	:STD_LOGIC;
`endif
	
	SIGNAL	video_in_clk		:STD_LOGIC;
`ifndef USE_SDRAM_CLK
	SIGNAL	SDRAM_CLK			:STD_LOGIC;
`endif
`ifndef USE_VGA_CLK
`ifdef USE_TPAD
	SIGNAL	VGA_CLK_40			:STD_LOGIC;
`elsif USE_VEEK_MT
	SIGNAL	VGA_CLK_33			:STD_LOGIC;
`endif
	SIGNAL	VGA_CLK				:STD_LOGIC;
`endif
	
	-- Internal Registers
	
	-- State Machine Registers
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
	COMPONENT altpll
	GENERIC (
		clk0_divide_by				:INTEGER;
		clk0_duty_cycle			:INTEGER;
		clk0_multiply_by			:INTEGER;
		clk0_phase_shift			:STRING;
		clk1_divide_by				:INTEGER;
		clk1_duty_cycle			:INTEGER;
		clk1_multiply_by			:INTEGER;
		clk1_phase_shift			:STRING;
		clk2_divide_by				:INTEGER;
		clk2_duty_cycle			:INTEGER;
		clk2_multiply_by			:INTEGER;
		clk2_phase_shift			:STRING;
`ifdef USE_TPAD
		clk3_divide_by				:INTEGER;
		clk3_duty_cycle			:INTEGER;
		clk3_multiply_by			:INTEGER;
		clk3_phase_shift			:STRING;
`elsif USE_VEEK_MT
		clk3_divide_by				:INTEGER;
		clk3_duty_cycle			:INTEGER;
		clk3_multiply_by			:INTEGER;
		clk3_phase_shift			:STRING;
`endif
		compensate_clock			:STRING;
		gate_lock_signal			:STRING;
		inclk0_input_frequency	:INTEGER;
`ifdef USE_CYCLONE_III
		intended_device_family	:STRING;
`elsif USE_CYCLONE_IV
		intended_device_family	:STRING;
`else
		intended_device_family	:STRING;
`endif
		invalid_lock_multiplier	:INTEGER;
		lpm_type						:STRING;
		operation_mode				:STRING;
		pll_type						:STRING;
		port_activeclock			:STRING;
		port_areset					:STRING;
		port_clkbad0				:STRING;
		port_clkbad1				:STRING;
		port_clkloss				:STRING;
		port_clkswitch				:STRING;
		port_fbin					:STRING;
		port_inclk0					:STRING;
		port_inclk1					:STRING;
		port_locked					:STRING;
		port_pfdena					:STRING;
		port_pllena					:STRING;
		port_scanaclr				:STRING;
		port_scanclk				:STRING;
		port_scandata				:STRING;
		port_scandataout			:STRING;
		port_scandone				:STRING;
		port_scanread				:STRING;
		port_scanwrite				:STRING;
		port_clk0					:STRING;
		port_clk1					:STRING;
		port_clk2					:STRING;
		port_clk3					:STRING;
		port_clk4					:STRING;
		port_clk5					:STRING;
		port_clkena0				:STRING;
		port_clkena1				:STRING;
		port_clkena2				:STRING;
		port_clkena3				:STRING;
		port_clkena4				:STRING;
		port_clkena5				:STRING;
		port_enable0				:STRING;
		port_enable1				:STRING;
		port_extclk0				:STRING;
		port_extclk1				:STRING;
		port_extclk2				:STRING;
		port_extclk3				:STRING;
		port_extclkena0			:STRING;
		port_extclkena1			:STRING;
		port_extclkena2			:STRING;
		port_extclkena3			:STRING;
		port_sclkout0				:STRING;
		port_sclkout1				:STRING;
		valid_lock_multiplier	:INTEGER
	);
	PORT (
		-- Inputs
		inclk			:IN		STD_LOGIC_VECTOR( 1 DOWNTO  0);

		-- Outputs
`ifdef USE_TPAD
		clk			:BUFFER	STD_LOGIC_VECTOR( 3 DOWNTO  0);
`elsif USE_VEEK_MT
		clk			:BUFFER	STD_LOGIC_VECTOR( 3 DOWNTO  0);
`else
		clk			:BUFFER	STD_LOGIC_VECTOR( 2 DOWNTO  0);
`endif
		locked		:BUFFER	STD_LOGIC
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

	sys_reset_n <= clk_locked;

	sys_clk <= sys_mem_clks(0);
	SDRAM_CLK <= sys_mem_clks(1);
	VGA_CLK <= sys_mem_clks(2);
`ifdef USE_TPAD
	VGA_CLK_40 <= sys_mem_clks(3);
`elsif USE_VEEK_MT
	VGA_CLK_33 <= sys_mem_clks(3);
`endif

`ifdef USE_AUDIO_CLK
	video_in_clk <= audio_clks(0);
	AUD_CLK <= audio_clks(1);
`endif

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************

	DE_Clock_Generator_System : altpll 
	GENERIC MAP (
		clk0_divide_by				=> SYS_CLK_DIV,
		clk0_duty_cycle			=> 50,
		clk0_multiply_by			=> SYS_CLK_MULT,
		clk0_phase_shift			=> "0",
		clk1_divide_by				=> SYS_CLK_DIV,
		clk1_duty_cycle			=> 50,
		clk1_multiply_by			=> SYS_CLK_MULT,
		clk1_phase_shift			=> "-3000",
		clk2_divide_by				=> 2,
		clk2_duty_cycle			=> 50,
		clk2_multiply_by			=> 1,
		clk2_phase_shift			=> "20000",
`ifdef USE_TPAD
		clk3_divide_by				=> 5,
		clk3_duty_cycle			=> 50,
		clk3_multiply_by			=> 4,
		clk3_phase_shift			=> "0",
`elsif USE_VEEK_MT
		clk3_divide_by				=> 3,
		clk3_duty_cycle			=> 50,
		clk3_multiply_by			=> 2,
		clk3_phase_shift			=> "0",
`endif
		compensate_clock			=> "CLK0",
		gate_lock_signal			=> "NO",
		inclk0_input_frequency	=> 20000,
`ifdef USE_CYCLONE_III
		intended_device_family	=> "Cyclone III",
`elsif USE_CYCLONE_IV
		intended_device_family	=> "Cyclone IV",
`else
		intended_device_family	=> "Cyclone II",
`endif
		invalid_lock_multiplier	=> 5,
		lpm_type						=> "altpll",
		operation_mode				=> "NORMAL",
		pll_type						=> "FAST",
		port_activeclock			=> "PORT_UNUSED",
		port_areset					=> "PORT_UNUSED",
		port_clkbad0				=> "PORT_UNUSED",
		port_clkbad1				=> "PORT_UNUSED",
		port_clkloss				=> "PORT_UNUSED",
		port_clkswitch				=> "PORT_UNUSED",
		port_fbin					=> "PORT_UNUSED",
		port_inclk0					=> "PORT_USED",
		port_inclk1					=> "PORT_UNUSED",
		port_locked					=> "PORT_USED",
		port_pfdena					=> "PORT_UNUSED",
		port_pllena					=> "PORT_UNUSED",
		port_scanaclr				=> "PORT_UNUSED",
		port_scanclk				=> "PORT_UNUSED",
		port_scandata				=> "PORT_UNUSED",
		port_scandataout			=> "PORT_UNUSED",
		port_scandone				=> "PORT_UNUSED",
		port_scanread				=> "PORT_UNUSED",
		port_scanwrite				=> "PORT_UNUSED",
		port_clk0					=> "PORT_USED",
		port_clk1					=> "PORT_USED",
		port_clk2					=> "PORT_USED",
		port_clk3					=> "PORT_UNUSED",
		port_clk4					=> "PORT_UNUSED",
		port_clk5					=> "PORT_UNUSED",
		port_clkena0				=> "PORT_UNUSED",
		port_clkena1				=> "PORT_UNUSED",
		port_clkena2				=> "PORT_UNUSED",
		port_clkena3				=> "PORT_UNUSED",
		port_clkena4				=> "PORT_UNUSED",
		port_clkena5				=> "PORT_UNUSED",
		port_enable0				=> "PORT_UNUSED",
		port_enable1				=> "PORT_UNUSED",
		port_extclk0				=> "PORT_UNUSED",
		port_extclk1				=> "PORT_UNUSED",
		port_extclk2				=> "PORT_UNUSED",
		port_extclk3				=> "PORT_UNUSED",
		port_extclkena0			=> "PORT_UNUSED",
		port_extclkena1			=> "PORT_UNUSED",
		port_extclkena2			=> "PORT_UNUSED",
		port_extclkena3			=> "PORT_UNUSED",
		port_sclkout0				=> "PORT_UNUSED",
		port_sclkout1				=> "PORT_UNUSED",
		valid_lock_multiplier	=> 1
	)
	PORT MAP (
		-- Inputs
		inclk			=> '0' & CLOCK_50,
	
		-- Outputs
		clk			=> sys_mem_clks,
		locked		=> clk_locked
	);

`ifdef USE_AUDIO_CLK
	DE_Clock_Generator_Audio : altpll 
	GENERIC MAP (
		clk0_divide_by				=> 1,
		clk0_duty_cycle			=> 50,
		clk0_multiply_by			=> 1,
		clk0_phase_shift			=> "0",
		clk1_divide_by				=> AUD_CLK_DIV,
		clk1_duty_cycle			=> 50,
		clk1_multiply_by			=> AUD_CLK_MULT,
		clk1_phase_shift			=> "0",
		clk2_divide_by				=> 1,
		clk2_duty_cycle			=> 50,
		clk2_multiply_by			=> 1,
		clk2_phase_shift			=> "0",
`ifdef USE_TPAD
		clk3_divide_by				=> 1,
		clk3_duty_cycle			=> 50,
		clk3_multiply_by			=> 1,
		clk3_phase_shift			=> "0",
`elsif USE_VEEK_MT
		clk3_divide_by				=> 1,
		clk3_duty_cycle			=> 50,
		clk3_multiply_by			=> 1,
		clk3_phase_shift			=> "0",
`endif
		compensate_clock			=> "CLK0",
		gate_lock_signal			=> "NO",
		inclk0_input_frequency	=> 37037,
`ifdef USE_CYCLONE_III
		intended_device_family	=> "Cyclone III",
`elsif USE_CYCLONE_IV
		intended_device_family	=> "Cyclone IV",
`else
		intended_device_family	=> "Cyclone II",
`endif
		invalid_lock_multiplier	=> 5,
		lpm_type						=> "altpll",
		operation_mode				=> "NORMAL",
		pll_type						=> "FAST",
		port_activeclock			=> "PORT_UNUSED",
		port_areset					=> "PORT_UNUSED",
		port_clkbad0				=> "PORT_UNUSED",
		port_clkbad1				=> "PORT_UNUSED",
		port_clkloss				=> "PORT_UNUSED",
		port_clkswitch				=> "PORT_UNUSED",
		port_fbin					=> "PORT_UNUSED",
		port_inclk0					=> "PORT_USED",
		port_inclk1					=> "PORT_UNUSED",
		port_locked					=> "PORT_USED",
		port_pfdena					=> "PORT_UNUSED",
		port_pllena					=> "PORT_UNUSED",
		port_scanaclr				=> "PORT_UNUSED",
		port_scanclk				=> "PORT_UNUSED",
		port_scandata				=> "PORT_UNUSED",
		port_scandataout			=> "PORT_UNUSED",
		port_scandone				=> "PORT_UNUSED",
		port_scanread				=> "PORT_UNUSED",
		port_scanwrite				=> "PORT_UNUSED",
		port_clk0					=> "PORT_USED",
		port_clk1					=> "PORT_USED",
		port_clk2					=> "PORT_UNUSED",
		port_clk3					=> "PORT_UNUSED",
		port_clk4					=> "PORT_UNUSED",
		port_clk5					=> "PORT_UNUSED",
		port_clkena0				=> "PORT_UNUSED",
		port_clkena1				=> "PORT_UNUSED",
		port_clkena2				=> "PORT_UNUSED",
		port_clkena3				=> "PORT_UNUSED",
		port_clkena4				=> "PORT_UNUSED",
		port_clkena5				=> "PORT_UNUSED",
		port_enable0				=> "PORT_UNUSED",
		port_enable1				=> "PORT_UNUSED",
		port_extclk0				=> "PORT_UNUSED",
		port_extclk1				=> "PORT_UNUSED",
		port_extclk2				=> "PORT_UNUSED",
		port_extclk3				=> "PORT_UNUSED",
		port_extclkena0			=> "PORT_UNUSED",
		port_extclkena1			=> "PORT_UNUSED",
		port_extclkena2			=> "PORT_UNUSED",
		port_extclkena3			=> "PORT_UNUSED",
		port_sclkout0				=> "PORT_UNUSED",
		port_sclkout1				=> "PORT_UNUSED",
		valid_lock_multiplier	=> 1
	)
	PORT MAP (
		-- Inputs
		inclk			=> '0' & CLOCK_27,
	
		-- Outputs
		clk			=> audio_clks,
		locked		=> audio_clk_locked
	
	);
`endif


END Behaviour;
