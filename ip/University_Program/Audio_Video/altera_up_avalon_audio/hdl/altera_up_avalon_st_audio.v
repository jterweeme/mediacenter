/******************************************************************************
 * License Agreement                                                          *
 *                                                                            *
 * Copyright (c) 1991-2013 Altera Corporation, San Jose, California, USA.     *
 * All rights reserved.                                                       *
 *                                                                            *
 * Any megafunction design, and related net list (encrypted or decrypted),    *
 *  support information, device programming or simulation file, and any other *
 *  associated documentation or information provided by Altera or a partner   *
 *  under Altera's Megafunction Partnership Program may be used only to       *
 *  program PLD devices (but not masked PLD devices) from Altera.  Any other  *
 *  use of such megafunction design, net list, support information, device    *
 *  programming or simulation file, or any other related documentation or     *
 *  information is prohibited for any other purpose, including, but not       *
 *  limited to modification, reverse engineering, de-compiling, or use with   *
 *  any other silicon devices, unless such use is explicitly licensed under   *
 *  a separate agreement with Altera or a megafunction partner.  Title to     *
 *  the intellectual property, including patents, copyrights, trademarks,     *
 *  trade secrets, or maskworks, embodied in any such megafunction design,    *
 *  net list, support information, device programming or simulation file, or  *
 *  any other related documentation or information provided by Altera or a    *
 *  megafunction partner, remains with Altera, the megafunction partner, or   *
 *  their respective licensors.  No other licenses, including any licenses    *
 *  needed under any third party's intellectual property, are provided herein.*
 *  Copying or modifying any file, or portion thereof, to which this notice   *
 *  is attached violates this copyright.                                      *
 *                                                                            *
 * THIS FILE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    *
 * FROM, OUT OF OR IN CONNECTION WITH THIS FILE OR THE USE OR OTHER DEALINGS  *
 * IN THIS FILE.                                                              *
 *                                                                            *
 * This agreement shall be governed in all respects by the laws of the State  *
 *  of California and by the laws of the United States of America.            *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
 *                                                                            *
 * This module reads and writes data to the Audio chip on Altera's DE2        *
 *  Development and Education Board. The audio chip must be in master mode    *
 *  and the digital format must be left justified.                            *
 *                                                                            *
 ******************************************************************************/

module altera_up_avalon_st_audio (
	// Inputs
	clk,
	reset,
	
`ifdef USE_AUDIO_IN
	from_adc_left_channel_ready,
	from_adc_right_channel_ready,
`endif

`ifdef USE_AUDIO_OUT
	to_dac_left_channel_data,
	to_dac_left_channel_valid,
	to_dac_right_channel_data,
	to_dac_right_channel_valid,
`endif

`ifdef USE_AUDIO_IN
	AUD_ADCDAT,
`endif
`ifdef USE_AUDIO_IN
	AUD_ADCLRCK,
`endif
	AUD_BCLK,
`ifdef USE_AUDIO_OUT
	AUD_DACLRCK,
`endif

	// Bidirectionals

	// Outputs
`ifdef USE_AUDIO_IN
	from_adc_left_channel_data,
	from_adc_left_channel_valid,
	from_adc_right_channel_data,
	from_adc_right_channel_valid,
`endif

`ifdef USE_AUDIO_OUT
	to_dac_left_channel_ready,
	to_dac_right_channel_ready,
`endif

`ifdef USE_AUDIO_OUT
	AUD_DACDAT
`endif
);

/*****************************************************************************
 *                           Parameter Declarations                          *
 *****************************************************************************/


/*****************************************************************************
 *                             Port Declarations                             *
 *****************************************************************************/
// Inputs
input						clk;
input						reset;

`ifdef USE_AUDIO_IN
input						from_adc_left_channel_ready;
input						from_adc_right_channel_ready;
`endif

`ifdef USE_AUDIO_OUT
input			[DW: 0]	to_dac_left_channel_data;
input						to_dac_left_channel_valid;
input			[DW: 0]	to_dac_right_channel_data;
input						to_dac_right_channel_valid;
`endif

`ifdef USE_AUDIO_IN
input						AUD_ADCDAT;
`endif
`ifdef USE_AUDIO_IN
input						AUD_ADCLRCK;
`endif
input						AUD_BCLK;
`ifdef USE_AUDIO_OUT
input						AUD_DACLRCK;
`endif

// Bidirectionals

// Outputs
`ifdef USE_AUDIO_IN
output		[DW: 0]	from_adc_left_channel_data;
output					from_adc_left_channel_valid;
output		[DW: 0]	from_adc_right_channel_data;
output					from_adc_right_channel_valid;
`endif

`ifdef USE_AUDIO_OUT
output					to_dac_left_channel_ready;
output					to_dac_right_channel_ready;
`endif

`ifdef USE_AUDIO_OUT
output					AUD_DACDAT;
`endif

/*****************************************************************************
 *                           Constant Declarations                           *
 *****************************************************************************/

localparam DW						= 15;
localparam BIT_COUNTER_INIT	= 5'h0F;

/*****************************************************************************
 *                 Internal Wires and Registers Declarations                 *
 *****************************************************************************/

// Internal Wires
wire						bclk_rising_edge;
wire						bclk_falling_edge;

`ifdef USE_AUDIO_IN
wire						adc_lrclk_rising_edge;
wire						adc_lrclk_falling_edge;

wire			[ 7: 0]	left_channel_read_available;
wire			[ 7: 0]	right_channel_read_available;
`endif
`ifdef USE_AUDIO_OUT
wire						dac_lrclk_rising_edge;
wire						dac_lrclk_falling_edge;

wire			[ 7: 0]	left_channel_write_space;
wire			[ 7: 0]	right_channel_write_space;
`endif

// Internal Registers
`ifdef USE_AUDIO_IN
reg						done_adc_channel_sync;
`endif
`ifdef USE_AUDIO_OUT
reg						done_dac_channel_sync;
`endif

// State Machine Registers


/*****************************************************************************
 *                         Finite State Machine(s)                           *
 *****************************************************************************/


/*****************************************************************************
 *                             Sequential Logic                              *
 *****************************************************************************/

// Output Registers

// Internal Registers
`ifdef USE_AUDIO_IN
always @(posedge clk)
begin
	if (reset == 1'b1)
		done_adc_channel_sync <= 1'b0;
	else if (adc_lrclk_rising_edge == 1'b1)
		done_adc_channel_sync <= 1'b1;
end
`endif

`ifdef USE_AUDIO_OUT
always @(posedge clk)
begin
	if (reset == 1'b1)
		done_dac_channel_sync <= 1'b0;
	else if (dac_lrclk_falling_edge == 1'b1)
		done_dac_channel_sync <= 1'b1;
end
`endif

/*****************************************************************************
 *                            Combinational Logic                            *
 *****************************************************************************/

// Output Assignments
assign from_adc_left_channel_valid	= (|(left_channel_read_available));
assign from_adc_right_channel_valid	= (|(right_channel_read_available));

assign to_dac_left_channel_ready		= (|(left_channel_write_space));
assign to_dac_right_channel_ready	= (|(right_channel_write_space));

// Internal Assignments

/*****************************************************************************
 *                              Internal Modules                             *
 *****************************************************************************/

altera_up_clock_edge Bit_Clock_Edges (
	// Inputs
	.clk				(clk),
	.reset			(reset),
	
	.test_clk		(AUD_BCLK),
	
	// Bidirectionals

	// Outputs
	.rising_edge	(bclk_rising_edge),
	.falling_edge	(bclk_falling_edge)
);

`ifdef USE_AUDIO_IN
altera_up_clock_edge ADC_Left_Right_Clock_Edges (
	// Inputs
	.clk				(clk),
	.reset			(reset),
	
	.test_clk		(AUD_ADCLRCK),
	
	// Bidirectionals

	// Outputs
	.rising_edge	(adc_lrclk_rising_edge),
	.falling_edge	(adc_lrclk_falling_edge)
);
`endif

`ifdef USE_AUDIO_OUT
altera_up_clock_edge DAC_Left_Right_Clock_Edges (
	// Inputs
	.clk				(clk),
	.reset			(reset),
	
	.test_clk		(AUD_DACLRCK),
	
	// Bidirectionals

	// Outputs
	.rising_edge	(dac_lrclk_rising_edge),
	.falling_edge	(dac_lrclk_falling_edge)
);
`endif

`ifdef USE_AUDIO_IN
altera_up_audio_in_deserializer Audio_In_Deserializer (
	// Inputs
	.clk									(clk),
	.reset								(reset),
	
	.bit_clk_rising_edge				(bclk_rising_edge),
	.bit_clk_falling_edge			(bclk_falling_edge),
	.left_right_clk_rising_edge	(adc_lrclk_rising_edge),
	.left_right_clk_falling_edge	(adc_lrclk_falling_edge),

	.done_channel_sync				(done_adc_channel_sync),

	.serial_audio_in_data			(AUD_ADCDAT),

	.read_left_audio_data_en		(from_adc_left_channel_valid & from_adc_left_channel_ready),
	.read_right_audio_data_en		(from_adc_right_channel_valid & from_adc_right_channel_ready),

	// Bidirectionals

	// Outputs
	.left_audio_fifo_read_space	(left_channel_read_available),
	.right_audio_fifo_read_space	(right_channel_read_available),

	.left_channel_data				(from_adc_left_channel_data),
	.right_channel_data				(from_adc_right_channel_data)
);
defparam
	Audio_In_Deserializer.DW 					= DW,
	Audio_In_Deserializer.BIT_COUNTER_INIT = BIT_COUNTER_INIT;
`endif

`ifdef USE_AUDIO_OUT
altera_up_audio_out_serializer Audio_Out_Serializer (
	// Inputs
	.clk										(clk),
	.reset									(reset),
	
	.bit_clk_rising_edge					(bclk_rising_edge),
	.bit_clk_falling_edge				(bclk_falling_edge),
	.left_right_clk_rising_edge		(done_dac_channel_sync & dac_lrclk_rising_edge),
	.left_right_clk_falling_edge		(done_dac_channel_sync & dac_lrclk_falling_edge),
	
	.left_channel_data					(to_dac_left_channel_data),
	.left_channel_data_en				(to_dac_left_channel_valid & to_dac_left_channel_ready),

	.right_channel_data					(to_dac_right_channel_data),
	.right_channel_data_en				(to_dac_right_channel_valid & to_dac_right_channel_ready),
	
	// Bidirectionals

	// Outputs
	.left_channel_fifo_write_space	(left_channel_write_space),
	.right_channel_fifo_write_space	(right_channel_write_space),

	.serial_audio_out_data				(AUD_DACDAT)
);
defparam
	Audio_Out_Serializer.DW = DW;
`endif

endmodule

