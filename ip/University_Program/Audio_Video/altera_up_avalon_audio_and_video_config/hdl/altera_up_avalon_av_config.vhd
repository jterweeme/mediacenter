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
-- * This module sends and receives data from the audio's and TV in's           *
-- *  control registers for the chips on Altera's DE2 board. Plus, it can       *
-- *  send and receive data from the TRDB_DC2 and TRDB_LCM add-on modules.      *
-- *                                                                            *
-- ******************************************************************************

ENTITY altera_up_avalon_av_config IS 

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
	byteenable	:IN		STD_LOGIC_VECTOR( 3 DOWNTO  0);	
	read			:IN		STD_LOGIC;
	write			:IN		STD_LOGIC;
	writedata	:IN		STD_LOGIC_VECTOR(31 DOWNTO  0);	

`ifdef USE_MANUAL_EXPOSURE
	exposure		:IN		STD_LOGIC_VECTOR(15 DOWNTO  0);	
`endif

	-- Bidirectionals
	I2C_SDAT		:INOUT	STD_LOGIC;

	-- Outputs
	readdata		:BUFFER	STD_LOGIC_VECTOR(31 DOWNTO  0);	
	waitrequest	:BUFFER	STD_LOGIC;
	irq			:BUFFER	STD_LOGIC;

	I2C_SCEN		:BUFFER	STD_LOGIC;
	I2C_SCLK		:BUFFER	STD_LOGIC

);

END altera_up_avalon_av_config;

ARCHITECTURE Behaviour OF altera_up_avalon_av_config IS
-- *****************************************************************************
-- *                           Constant Declarations                           *
-- *****************************************************************************
		
	CONSTANT	DW								:INTEGER									:= 26;	-- Serial protocol's datawidth
	
	CONSTANT	CFG_TYPE						:STD_LOGIC_VECTOR(7 DOWNTO 0)		:= B"00000000";
	
	CONSTANT	READ_MASK					:STD_LOGIC_VECTOR(26 DOWNTO 0)	:= B"00000000" & '1' & B"11111111" & '0' & B"00000000" & '1';
	CONSTANT	WRITE_MASK					:STD_LOGIC_VECTOR(26 DOWNTO 0)	:= B"00000000" & '1' & B"00000000" & '1' & B"00000000" & '1';
	
	CONSTANT	RESTART_COUNTER			:STD_LOGIC_VECTOR(4 DOWNTO 0)		:= (0 => '1', 3 => '1', OTHERS => '0'); -- (SBCW DOWNTO 0)
	
`ifdef USE_AUTO_INIT
	-- Auto init parameters
	CONSTANT	AIRS							:INTEGER									:= 50;	-- Auto Init ROM's size
	CONSTANT	AIAW							:INTEGER									:= 5;		-- Auto Init ROM's address width 
	
`ifdef USE_OB_AUTO_INIT
	CONSTANT	AUD_LINE_IN_LC				:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"000011010";
	CONSTANT	AUD_LINE_IN_RC				:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"000011010";
	CONSTANT	AUD_LINE_OUT_LC			:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"001111011";
	CONSTANT	AUD_LINE_OUT_RC			:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"001111011";
	CONSTANT	AUD_ADC_PATH				:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"011111000";
	CONSTANT	AUD_DAC_PATH				:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"000000110";
	CONSTANT	AUD_POWER					:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"000000000";
	CONSTANT	AUD_DATA_FORMAT			:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"000000001";
	CONSTANT	AUD_SAMPLE_CTRL			:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"000000010";
	CONSTANT	AUD_SET_ACTIVE				:STD_LOGIC_VECTOR(8 DOWNTO 0)		:= B"000000001";
`elsif USE_D5M_AUTO_INIT
	CONSTANT	D5M_COLUMN_SIZE			:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000101000011111";
	CONSTANT	D5M_ROW_SIZE				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000011110010111";
	CONSTANT	D5M_COLUMN_BIN				:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000000";
	CONSTANT	D5M_ROW_BIN					:STD_LOGIC_VECTOR(15 DOWNTO  0)	:= B"0000000000000000";
`endif
`endif
	
	-- Serial Bus Controller parameters

	CONSTANT	SBCW							:INTEGER									:= 4;		-- Serial bus counter's width
	CONSTANT	SCCW							:INTEGER									:= 11;		-- Slow clock's counter's width
	
	
	-- States for finite state machine
	TYPE State_Type IS (	STATE_0_IDLE,
								STATE_1_PRE_WRITE,
								STATE_2_WRITE_TRANSFER,
								STATE_3_POST_WRITE,
								STATE_4_PRE_READ,
								STATE_5_READ_TRANSFER,
								STATE_6_POST_READ
							);
	
-- *****************************************************************************
-- *                       Internal Signals Declarations                       *
-- *****************************************************************************
	-- Internal Wires
	SIGNAL	internal_reset						:STD_LOGIC;
	
	--  Auto init signals
`ifdef USE_AUTO_INIT
	SIGNAL	rom_address							:STD_LOGIC_VECTOR(AIAW DOWNTO 0);	
	SIGNAL	rom_data								:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`endif
	SIGNAL	ack									:STD_LOGIC;
	
	SIGNAL	auto_init_data						:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	SIGNAL	auto_init_transfer_en			:STD_LOGIC;
	SIGNAL	auto_init_complete				:STD_LOGIC;
	SIGNAL	auto_init_error					:STD_LOGIC;
	
	--  Serial controller signals
	SIGNAL	transfer_mask						:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	SIGNAL	data_to_controller				:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`ifndef USE_LTM_MODE
	SIGNAL	data_to_controller_on_restart	:STD_LOGIC_VECTOR(DW DOWNTO  0);	
`endif
	SIGNAL	data_from_controller				:STD_LOGIC_VECTOR(DW DOWNTO  0);	
	
	SIGNAL	start_transfer						:STD_LOGIC;
	
	SIGNAL	transfer_complete					:STD_LOGIC;
	
	-- Internal Registers
	SIGNAL	control_reg							:STD_LOGIC_VECTOR(31 DOWNTO  0);	
	SIGNAL	address_reg							:STD_LOGIC_VECTOR(31 DOWNTO  0);	
	SIGNAL	data_reg								:STD_LOGIC_VECTOR(31 DOWNTO  0);	
	
	SIGNAL	start_external_transfer			:STD_LOGIC;
	SIGNAL	external_read_transfer			:STD_LOGIC;
	SIGNAL	address_for_transfer				:STD_LOGIC_VECTOR( 7 DOWNTO  0);	
`ifdef USE_OB_MODE
	SIGNAL	device_for_transfer				:STD_LOGIC_VECTOR( 1 DOWNTO  0);	
`endif
	
	-- State Machine Registers
	SIGNAL	ns_serial_transfer				: State_Type;	
	SIGNAL	s_serial_transfer					: State_Type;	
	
-- *****************************************************************************
-- *                          Component Declarations                           *
-- *****************************************************************************
`ifdef USE_AUTO_INIT
	COMPONENT altera_up_av_config_auto_init
	GENERIC (	
		ROM_SIZE					:INTEGER;
		AW							:INTEGER;
		DW							:INTEGER
	);
	PORT (
		-- Inputs
		clk						:IN		STD_LOGIC;
		reset						:IN		STD_LOGIC;

		clear_error				:IN		STD_LOGIC;

		ack						:IN		STD_LOGIC;
		transfer_complete		:IN		STD_LOGIC;

		rom_data					:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);

		-- Bidirectionals

		-- Outputs
		data_out					:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);
		transfer_data			:BUFFER	STD_LOGIC;

		rom_address				:BUFFER	STD_LOGIC_VECTOR(AIAW DOWNTO 0);
	
		auto_init_complete	:BUFFER	STD_LOGIC;
		auto_init_error		:BUFFER	STD_LOGIC
	);
	END COMPONENT;

`ifdef USE_DC2_AUTO_INIT
	COMPONENT altera_up_av_config_auto_init_dc2
`elsif USE_D5M_AUTO_INIT
	COMPONENT altera_up_av_config_auto_init_d5m
`elsif USE_LTM_AUTO_INIT
	COMPONENT altera_up_av_config_auto_init_ltm
`elsif USE_OB_DE1_AUTO_INIT
	COMPONENT altera_up_av_config_auto_init_ob_audio
`elsif USE_OB_DE2_35_AUTO_INIT
	COMPONENT altera_up_av_config_auto_init_ob_de2_35
`elsif USE_OB_DE2_70_AUTO_INIT
	COMPONENT altera_up_av_config_auto_init_ob_de2_70
`elsif USE_OB_DE2_115_AUTO_INIT
	COMPONENT altera_up_av_config_auto_init_ob_de2_115
`endif
`ifdef USE_OB_AUTO_INIT
	GENERIC (
`ifdef USE_OB_DE2_35_AUTO_INIT
`ifdef USE_NTSC_VIDEO
		VID_INPUT_CONTROL	:STD_LOGIC_VECTOR(15 DOWNTO 0);
		VID_CHROMA_GAIN_1	:STD_LOGIC_VECTOR(15 DOWNTO 0);
		VID_CHROMA_GAIN_2	:STD_LOGIC_VECTOR(15 DOWNTO 0);
`else
		VID_INPUT_CONTROL	:STD_LOGIC_VECTOR(15 DOWNTO 0);
		VID_CHROMA_GAIN_1	:STD_LOGIC_VECTOR(15 DOWNTO 0);
		VID_CHROMA_GAIN_2	:STD_LOGIC_VECTOR(15 DOWNTO 0);
`endif
`endif

		AUD_LINE_IN_LC		:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_LINE_IN_RC		:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_LINE_OUT_LC	:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_LINE_OUT_RC	:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_ADC_PATH		:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_DAC_PATH		:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_POWER			:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_DATA_FORMAT	:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_SAMPLE_CTRL	:STD_LOGIC_VECTOR(8 DOWNTO 0);
		AUD_SET_ACTIVE		:STD_LOGIC_VECTOR(8 DOWNTO 0)
	);
`elsif USE_D5M_AUTO_INIT
	GENERIC (
		D5M_COLUMN_SIZE	:STD_LOGIC_VECTOR(15 DOWNTO 0);
		D5M_ROW_SIZE		:STD_LOGIC_VECTOR(15 DOWNTO 0);
		D5M_COLUMN_BIN		:STD_LOGIC_VECTOR(15 DOWNTO 0);
		D5M_ROW_BIN			:STD_LOGIC_VECTOR(15 DOWNTO 0)
	);
`endif
	PORT (
		-- Inputs
		rom_address	:IN		STD_LOGIC_VECTOR(AIAW DOWNTO 0);

`ifdef USE_D5M_AUTO_INIT
		exposure		:IN		STD_LOGIC_VECTOR(15 DOWNTO  0);	
`endif
		-- Bidirectionals

		-- Outputs
		rom_data		:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0)
	);
	END COMPONENT;
`endif

	COMPONENT altera_up_av_config_serial_bus_controller
	GENERIC (
		DW								:INTEGER;
		CW								:INTEGER;
		SCCW							:INTEGER
	);
	PORT (
		-- Inputs
		clk							:IN		STD_LOGIC;
		reset							:IN		STD_LOGIC;

		start_transfer				:IN		STD_LOGIC;

		data_in						:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);
		transfer_mask				:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);

`ifndef USE_LTM_MODE
		restart_counter			:IN		STD_LOGIC_VECTOR(SBCW DOWNTO 0);
		restart_data_in			:IN		STD_LOGIC_VECTOR(DW DOWNTO  0);
		restart_transfer_mask	:IN		STD_LOGIC_VECTOR(26 DOWNTO 0);
`endif

		-- Bidirectionals
		serial_data					:INOUT	STD_LOGIC;

		-- Outputs
		serial_clk					:BUFFER	STD_LOGIC;
		serial_en					:BUFFER	STD_LOGIC;

		data_out						:BUFFER	STD_LOGIC_VECTOR(DW DOWNTO  0);
		transfer_complete			:BUFFER	STD_LOGIC
	);
	END COMPONENT;

BEGIN
-- *****************************************************************************
-- *                         Finite State Machine(s)                           *
-- *****************************************************************************

	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (internal_reset = '1') THEN
				s_serial_transfer <= STATE_0_IDLE;
			ELSE
				s_serial_transfer <= ns_serial_transfer;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (ns_serial_transfer, s_serial_transfer, transfer_complete, 
				auto_init_complete, write, address, read, control_reg)
	BEGIN
		-- Defaults
		ns_serial_transfer <= STATE_0_IDLE;
	
	   CASE (s_serial_transfer) IS
		WHEN STATE_0_IDLE =>
			IF ((transfer_complete = '1') OR (auto_init_complete = '0')) THEN
				ns_serial_transfer <= STATE_0_IDLE;
			ELSIF ((write = '1') AND (address = B"11")) THEN
				ns_serial_transfer <= STATE_1_PRE_WRITE;
			ELSIF ((read = '1') AND (address = B"11")) THEN
`ifdef USE_OB_MODE
				IF (control_reg(17 DOWNTO 16) = B"00") THEN
					ns_serial_transfer <= STATE_6_POST_READ;
				ELSE
					ns_serial_transfer <= STATE_4_PRE_READ;
			END IF;
`else
				ns_serial_transfer <= STATE_4_PRE_READ;
`endif
			ELSE
				ns_serial_transfer <= STATE_0_IDLE;
			END IF;
		WHEN STATE_1_PRE_WRITE =>
			ns_serial_transfer <= STATE_2_WRITE_TRANSFER;
		WHEN STATE_2_WRITE_TRANSFER =>
			IF (transfer_complete = '1') THEN
				ns_serial_transfer <= STATE_3_POST_WRITE;
			ELSE
				ns_serial_transfer <= STATE_2_WRITE_TRANSFER;
			END IF;
		WHEN STATE_3_POST_WRITE =>
			ns_serial_transfer <= STATE_0_IDLE;
		WHEN STATE_4_PRE_READ =>
			ns_serial_transfer <= STATE_5_READ_TRANSFER;
		WHEN STATE_5_READ_TRANSFER =>
			IF (transfer_complete = '1') THEN
				ns_serial_transfer <= STATE_6_POST_READ;
			ELSE
				ns_serial_transfer <= STATE_5_READ_TRANSFER;
			END IF;
		WHEN STATE_6_POST_READ =>
			ns_serial_transfer <= STATE_0_IDLE;
		WHEN OTHERS =>
			ns_serial_transfer <= STATE_0_IDLE;
		END CASE;
	END PROCESS;


-- *****************************************************************************
-- *                             Sequential Logic                              *
-- *****************************************************************************

	-- Output regsiters
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (internal_reset = '1') THEN
				readdata		<= B"00000000000000000000000000000000";
			ELSIF (read = '1') THEN
				IF (address = B"00") THEN
					readdata	<= control_reg;
				ELSIF (address = B"01") THEN
					readdata	<= B"00000000" & CFG_TYPE & B"0000000" & 
									(auto_init_complete AND NOT auto_init_error) & B"000000" & 
									(NOT start_external_transfer AND auto_init_complete) & 
									ack;
				ELSIF (address = B"10") THEN
					readdata	<= address_reg;
`ifdef USE_OB_MODE
				ELSIF (control_reg(17 DOWNTO 16) = B"00") THEN
					readdata	<= B"00000000000000000000000" & 
									data_from_controller(10) & 
									data_from_controller( 8 DOWNTO  1);
				ELSE
					readdata	<= B"000000000000000000000000" & data_from_controller( 8 DOWNTO  1);
`elsif USE_LTM_MODE
				ELSE
					readdata	<= B"000000000000000000000000" & data_from_controller( 7 DOWNTO  0);
`else  --USE_D5M_MODE and USE_DC2_MODE
				ELSE
					readdata	<= B"0000000000000000" & 
									data_from_controller(17 DOWNTO 10) & 
									data_from_controller( 8 DOWNTO  1);
`endif
				END IF;
			END IF;
		END IF;
	END PROCESS;


	-- Internal regsiters
	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (internal_reset = '1') THEN
				control_reg					<= B"00000000000000000000000000000000";
				address_reg					<= B"00000000000000000000000000000000";
				data_reg						<= B"00000000000000000000000000000000";
			
			ELSIF ((write = '1') AND (waitrequest = '0')) THEN
				-- Write to control register
				IF ((address = B"00") AND (byteenable(0) = '1')) THEN
					control_reg( 2 DOWNTO  1)	<= writedata( 2 DOWNTO  1);
`ifdef USE_OB_MODE
			END IF;
				IF ((address = B"00") AND (byteenable(2) = '1')) THEN
					control_reg(17 DOWNTO 16)	<= writedata(17 DOWNTO 16);
`endif
	
			-- Write to address register
			END IF;
				IF ((address = B"10") AND (byteenable(0) = '1')) THEN
					address_reg( 7 DOWNTO  0)	<= writedata( 7 DOWNTO  0);
	
			-- Write to data register
			END IF;
				IF ((address = B"11") AND (byteenable(0) = '1')) THEN
					data_reg( 7 DOWNTO  0)		<= writedata( 7 DOWNTO  0);
			END IF;
				IF ((address = B"11") AND (byteenable(1) = '1')) THEN
					data_reg(15 DOWNTO  8)		<= writedata(15 DOWNTO  8);
			END IF;
				IF ((address = B"11") AND (byteenable(2) = '1')) THEN
					data_reg(23 DOWNTO 16)		<= writedata(23 DOWNTO 16);
			END IF;
				IF ((address = B"11") AND (byteenable(3) = '1')) THEN
					data_reg(31 DOWNTO 24)		<= writedata(31 DOWNTO 24);
				END IF;
			END IF;
		END IF;
	END PROCESS;


	PROCESS (clk)
	BEGIN
		IF clk'EVENT AND clk = '1' THEN
			IF (internal_reset = '1') THEN
				start_external_transfer <= '0';
				external_read_transfer	<= '0';
				address_for_transfer		<= B"00000000";
`ifdef USE_OB_MODE
				device_for_transfer		<= B"00";
`endif
			ELSIF (transfer_complete = '1') THEN
				start_external_transfer <= '0';
				external_read_transfer	<= '0';
				address_for_transfer		<= B"00000000";
			ELSIF (s_serial_transfer = STATE_1_PRE_WRITE) THEN
				start_external_transfer <= '1';
				external_read_transfer	<= '0';
				address_for_transfer		<= address_reg(7 DOWNTO 0);
`ifdef USE_OB_MODE
				device_for_transfer		<= control_reg(17 DOWNTO 16);
`endif
			ELSIF (s_serial_transfer = STATE_4_PRE_READ) THEN
				start_external_transfer <= '1';
				external_read_transfer	<= '1';
				address_for_transfer		<= address_reg(7 DOWNTO 0);
`ifdef USE_OB_MODE
				device_for_transfer		<= control_reg(17 DOWNTO 16);
`endif
			END IF;
		END IF;
	END PROCESS;


-- *****************************************************************************
-- *                            Combinational Logic                            *
-- *****************************************************************************

	-- Output Assignments
	waitrequest <= '1' WHEN
		((address = B"11") AND (write = '1') AND (s_serial_transfer /= STATE_1_PRE_WRITE)) OR 
		((address = B"11") AND (read = '1')  AND (s_serial_transfer /= STATE_6_POST_READ))
		ELSE '0';
	irq <= '1' WHEN (control_reg(1) = '1') AND (start_external_transfer = '0') AND (auto_init_complete = '1');

	-- Internal Assignments
	internal_reset <= '1' WHEN (reset = '1') OR 
			((address = B"00") AND (write = '1') AND (byteenable(0) = '1') AND (writedata(0) = '1'))
			ELSE '0';

`ifndef USE_AUTO_INIT
	auto_init_transfer_en <= '0';
	auto_init_complete <= '1';
	auto_init_error <= '0';
`endif

	--  Signals to the serial controller
`ifdef USE_LTM_MODE
	transfer_mask <= WRITE_MASK WHEN ((auto_init_complete = '0') OR external_read_transfer = '0') 
							ELSE READ_MASK;
`else
	transfer_mask <= WRITE_MASK;
`endif

	data_to_controller <= 
`ifdef USE_AUTO_INIT
			auto_init_data WHEN (auto_init_complete = '0') ELSE 
`endif
`ifdef USE_OB_MODE
			(B"00110100" & '0' & address_for_transfer(6 DOWNTO 0) & data_reg(8) & '0' & data_reg(7 DOWNTO 0) & '0') 
				WHEN (device_for_transfer = B"00") ELSE 
			(B"01000000" & '0' & address_for_transfer(7 DOWNTO 0) & external_read_transfer & data_reg(7 DOWNTO 0) & '0') 
				WHEN (device_for_transfer = B"01") ELSE 
			(B"01000010" & '0' & address_for_transfer(7 DOWNTO 0) & external_read_transfer & data_reg(7 DOWNTO 0) & '0');
`elsif USE_LTM_MODE
				address_for_transfer(5 DOWNTO 0) & 
				external_read_transfer & '0' & 
				data_reg( 7 DOWNTO 0);
`else  -- USE_D5M_MODE and USE_DC2_MODE
				B"10111010" & '0' & 
				address_for_transfer(7 DOWNTO 0) & external_read_transfer & 
				data_reg(15 DOWNTO 8) & '0' & 
				data_reg( 7 DOWNTO 0) & '0';
`endif

`ifndef USE_LTM_MODE
`ifdef USE_OB_MODE
	data_to_controller_on_restart <= 
				(B"01000001" & '0' & B"00000000" & '0' & B"00000000" & ack) WHEN (device_for_transfer = B"01") ELSE 
				(B"01000011" & '0' & B"00000000" & '0' & B"00000000" & ack);

`else  -- USE_D5M_MODE and USE_DC2_MODE
	data_to_controller_on_restart <= B"10111011" & '0' & B"00000000" & '0' & B"00000000" & '0' & B"00000000" & '0';
`endif
`endif
			

	start_transfer <= start_external_transfer WHEN (auto_init_complete = '1') ELSE auto_init_transfer_en;

	--  Signals from the serial controller
`ifdef USE_OB_MODE
	ack <= data_from_controller(18) OR 
						data_from_controller( 9) OR 
						data_from_controller( 0);
`elsif USE_LTM_MODE
	ack <= data_from_controller( 8);
`else  -- USE_D5M_MODE and USE_DC2_MODE
	ack <= data_from_controller(27) OR 
						data_from_controller(18) OR 
						data_from_controller( 9) OR 
						data_from_controller( 0);
`endif

-- *****************************************************************************
-- *                          Component Instantiations                         *
-- *****************************************************************************

`ifdef USE_AUTO_INIT
	AV_Config_Auto_Init : altera_up_av_config_auto_init 
	GENERIC MAP (	
		ROM_SIZE					=> AIRS,
		AW							=> AIAW,
		DW							=> DW
	)
	PORT MAP (
		-- Inputs
		clk						=> clk,
		reset						=> internal_reset,
	
		clear_error				=> '0',
	
		ack						=> ack,
		transfer_complete		=> transfer_complete,
	
		rom_data					=> rom_data,
	
		-- Bidirectionals
	
		-- Outputs
		data_out					=> auto_init_data,
		transfer_data			=> auto_init_transfer_en,
	
		rom_address				=> rom_address,
		
		auto_init_complete	=> auto_init_complete,
		auto_init_error		=> auto_init_error
	);

`ifdef USE_DC2_AUTO_INIT
	Auto_Init_DC2_ROM : altera_up_av_config_auto_init_dc2 
`elsif USE_D5M_AUTO_INIT
	Auto_Init_D5M_ROM : altera_up_av_config_auto_init_d5m 
`elsif USE_LTM_AUTO_INIT
	Auto_Init_LTM_ROM : altera_up_av_config_auto_init_ltm 
`elsif USE_OB_DE1_AUTO_INIT
	Auto_Init_OB_Devices_ROM : altera_up_av_config_auto_init_ob_audio 
`elsif USE_OB_DE2_35_AUTO_INIT
	Auto_Init_OB_Devices_ROM : altera_up_av_config_auto_init_ob_de2_35 
`elsif USE_OB_DE2_70_AUTO_INIT
	Auto_Init_OB_Devices_ROM : altera_up_av_config_auto_init_ob_de2_70 
`elsif USE_OB_DE2_115_AUTO_INIT
	Auto_Init_OB_Devices_ROM : altera_up_av_config_auto_init_ob_de2_115 
`endif
`ifdef USE_OB_AUTO_INIT
	GENERIC MAP (
`ifdef USE_OB_DE2_35_AUTO_INIT
`ifdef USE_NTSC_VIDEO
		VID_INPUT_CONTROL	=> B"0000000001000000",
		VID_CHROMA_GAIN_1	=> B"0010110111110100",
		VID_CHROMA_GAIN_2	=> B"0010111000000000",
`else
		VID_INPUT_CONTROL	=> B"0000000010000000",
		VID_CHROMA_GAIN_1	=> B"0010110111111000",
		VID_CHROMA_GAIN_2	=> B"0010111011101110",
`endif
`endif
		AUD_LINE_IN_LC		=> AUD_LINE_IN_LC,
		AUD_LINE_IN_RC		=> AUD_LINE_IN_RC,
		AUD_LINE_OUT_LC	=> AUD_LINE_OUT_LC,
		AUD_LINE_OUT_RC	=> AUD_LINE_OUT_RC,
		AUD_ADC_PATH		=> AUD_ADC_PATH,
		AUD_DAC_PATH		=> AUD_DAC_PATH,
		AUD_POWER			=> AUD_POWER,
		AUD_DATA_FORMAT	=> AUD_DATA_FORMAT,
		AUD_SAMPLE_CTRL	=> AUD_SAMPLE_CTRL,
		AUD_SET_ACTIVE		=> AUD_SET_ACTIVE
	)
`elsif USE_D5M_AUTO_INIT
	GENERIC MAP (
		D5M_COLUMN_SIZE	=> D5M_COLUMN_SIZE,
		D5M_ROW_SIZE		=> D5M_ROW_SIZE,
		D5M_COLUMN_BIN		=> D5M_COLUMN_BIN,
		D5M_ROW_BIN			=> D5M_ROW_BIN
	)
`endif
	PORT MAP (

		-- Inputs
		rom_address	=> rom_address,
	
`ifdef USE_D5M_AUTO_INIT
`ifdef USE_MANUAL_EXPOSURE
		exposure	=> exposure,
`else
		exposure	=> B"0000011111000000",
`endif
`endif
		-- Bidirectionals
	
		-- Outputs
		rom_data		=> rom_data
	);
`endif


	Serial_Bus_Controller : altera_up_av_config_serial_bus_controller 
	GENERIC MAP (
		DW								=> DW,
		CW								=> SBCW,
		SCCW							=> SCCW
	)
	PORT MAP (
		-- Inputs
		clk							=> clk,
		reset							=> internal_reset,
	
		start_transfer				=> start_transfer,
	
		data_in						=> data_to_controller,
		transfer_mask				=> transfer_mask,
	
`ifndef USE_LTM_MODE
		restart_counter			=> RESTART_COUNTER,
		restart_data_in			=> data_to_controller_on_restart,
		restart_transfer_mask	=> READ_MASK,
`endif
	
		-- Bidirectionals
		serial_data					=> I2C_SDAT,
	
		-- Outputs
		serial_clk					=> I2C_SCLK,
		serial_en					=> I2C_SCEN,
	
		data_out						=> data_from_controller,
		transfer_complete			=> transfer_complete
	);


END Behaviour;
