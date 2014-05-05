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
 * This module can read and/or write data to a parallel I/O port based        *
 *  on various user selected parameters. It has some predefined               *
 *  configurations for some devices on the Altera DE boards.                  *
 *                                                                            *
 ******************************************************************************/

module altera_up_avalon_parallel_port (
   // Inputs
	clk,
	reset,
	
	address,
	byteenable,
	chipselect,
	read,
	write,
	writedata,

`ifdef USE_INPUT_PORT
	in_port,
`elsif USE_SLIDER_SWITCHES
	SW,
`elsif USE_DIP_SWITCHES
	DIP,
`elsif USE_PUSHBUTTONS
	KEY,
`endif

	// Bidirectionals

`ifdef USE_BIDIR_PORT
	bidir_port,
`elsif USE_GPIO_S
	GPIO,
`elsif USE_GPIO_0
	GPIO_0,
`elsif USE_GPIO_1
	GPIO_1,
`elsif USE_GPIO_2
	GPIO_2,
`elsif USE_GPIO_2_IN
	GPIO_2_IN,
`endif

	// Outputs
`ifdef USE_OUTPUT_PORT
	out_port,
`elsif USE_TRI_COLOUR_LEDS
	LED,
`elsif USE_GREEN_LEDS
	LEDG,
`elsif USE_RED_LEDS
	LEDR,
`elsif USE_TWO_SEVEN_SEGMENT
	HEX0,
	HEX1,
`elsif USE_LOW_SEVEN_SEGMENT
	HEX0,
	HEX1,
	HEX2,
	HEX3,
`elsif USE_HIGH_SEVEN_SEGMENT
	HEX4,
	HEX5,
	HEX6,
	HEX7,
`endif

`ifdef USE_INTERRUPTS
	irq,
`endif
	readdata
);

/*****************************************************************************
 *                           Parameter Declarations                          *
 *****************************************************************************/

// DW represents the Data Width minus 1
parameter DW = 31;

/*****************************************************************************
 *                             Port Declarations                             *
 *****************************************************************************/
// Inputs
input						clk;
input						reset;

input			[ 1: 0]	address;
input			[ 3: 0]	byteenable;
input						chipselect;
input						read;
input						write;
input			[31: 0]	writedata;

`ifdef USE_INPUT_PORT
input			[DW: 0]	in_port;
`elsif USE_SLIDER_SWITCHES
input			[DW: 0]	SW;
`elsif USE_DIP_SWITCHES
input			[DW: 0]	DIP;
`elsif USE_PUSHBUTTONS
input			[DW: 0]	KEY;
`endif

// Bidirectionals
`ifdef USE_BIDIR_PORT
inout			[DW: 0]	bidir_port;
`elsif USE_GPIO_S
inout			[31: 0]	GPIO;
`elsif USE_GPIO_0
inout			[31: 0]	GPIO_0;
`elsif USE_GPIO_1
inout			[31: 0]	GPIO_1;
`elsif USE_GPIO_2
inout			[12: 0]	GPIO_2;
`elsif USE_GPIO_2_IN
input			[ 2: 0]	GPIO_2_IN;
`endif

// Outputs
`ifdef USE_OUTPUT_PORT
output		[DW: 0]	out_port;
`elsif USE_TRI_COLOUR_LEDS
output		[DW: 0]	LED;
`elsif USE_GREEN_LEDS
output		[DW: 0]	LEDG;
`elsif USE_RED_LEDS
output		[DW: 0]	LEDR;
`elsif USE_TWO_SEVEN_SEGMENT
output		[ 6: 0]	HEX0;
output		[ 6: 0]	HEX1;
`elsif USE_LOW_SEVEN_SEGMENT
`ifdef USE_DE2_70
output		[ 7: 0]	HEX0;
output		[ 7: 0]	HEX1;
output		[ 7: 0]	HEX2;
output		[ 7: 0]	HEX3;
`elsif USE_DE0
output		[ 7: 0]	HEX0;
output		[ 7: 0]	HEX1;
output		[ 7: 0]	HEX2;
output		[ 7: 0]	HEX3;
`else
output		[ 6: 0]	HEX0;
output		[ 6: 0]	HEX1;
output		[ 6: 0]	HEX2;
output		[ 6: 0]	HEX3;
`endif
`elsif USE_HIGH_SEVEN_SEGMENT
`ifdef USE_DE2_70
output		[ 7: 0]	HEX4;
output		[ 7: 0]	HEX5;
output		[ 7: 0]	HEX6;
output		[ 7: 0]	HEX7;
`else
output		[ 6: 0]	HEX4;
output		[ 6: 0]	HEX5;
output		[ 6: 0]	HEX6;
output		[ 6: 0]	HEX7;
`endif
`endif

`ifdef USE_INTERRUPTS
output reg				irq;
`endif
output reg	[31: 0]	readdata;

/*****************************************************************************
 *                           Constant Declarations                           *
 *****************************************************************************/

/*****************************************************************************
 *                 Internal Wires and Registers Declarations                 *
 *****************************************************************************/
// Internal Wires
`ifdef USE_CAPTURE
wire			[DW: 0]	new_capture;
`endif

// Internal Registers
reg			[31: 0]	data;
`ifdef USE_INOUT
reg			[31: 0]	direction;
`endif
`ifdef USE_INTERRUPTS
reg			[31: 0]	interrupt;
`endif
`ifdef USE_CAPTURE
reg			[DW: 0]	capture;
`endif

reg			[DW: 0]	data_in;
`ifdef USE_DATA_OUT
reg			[DW: 0]	data_out;
`endif

`ifdef USE_CAPTURE
reg			[DW: 0]	last_data_in;
`endif

// State Machine Registers

// Internal Variables
genvar					i;


/*****************************************************************************
 *                         Finite State Machine(s)                           *
 *****************************************************************************/


/*****************************************************************************
 *                             Sequential Logic                              *
 *****************************************************************************/

// Input Registers
always @(posedge clk)
begin
`ifdef USE_INPUT_PORT
	data_in <= in_port;
`elsif USE_SLIDER_SWITCHES
	data_in <= SW;
`elsif USE_DIP_SWITCHES
	data_in	<= DIP;
`elsif USE_PUSHBUTTONS
	data_in <= ~KEY;
`elsif USE_BIDIR_PORT
	data_in <= bidir_port;
`elsif USE_GPIO_S
	data_in <= GPIO;
`elsif USE_GPIO_0
	data_in <= GPIO_0;
`elsif USE_GPIO_1
	data_in <= GPIO_1;
`elsif USE_GPIO_2
	data_in <= GPIO_2;
`elsif USE_GPIO_2_IN
	data_in <= GPIO_2_IN;
`else
	data_in <= data[DW: 0];
`endif
end

// Output Registers
`ifdef USE_INTERRUPTS
always @(posedge clk)
begin
	if (reset == 1'b1)
		irq <= 1'b0;
	else
`ifdef USE_LEVEL_INTERRUPTS
		irq <= (|(interrupt[DW:0] & data_in));
`else
		irq <= (|(interrupt[DW:0] & capture));
`endif
end
`endif

always @(posedge clk)
begin
	if (reset == 1'b1)
		readdata <= 32'h00000000;
	else if (chipselect == 1'b1)
	begin
		if (address == 2'h0)
			readdata <= {{(31-DW){1'b0}}, data_in};
`ifdef USE_INOUT
		else if (address == 2'h1)
			readdata <= {{(31-DW){1'b0}}, direction[DW:0]};
`endif
`ifdef USE_INTERRUPTS
		else if (address == 2'h2)
			readdata <= {{(31-DW){1'b0}}, interrupt[DW:0]};
`endif
`ifdef USE_CAPTURE
		else if (address == 2'h3)
			readdata <= {{(31-DW){1'b0}}, capture};
`endif
		else
			readdata <= 32'h00000000;
	end
end

// Internal Registers
always @(posedge clk)
begin
	if (reset == 1'b1)
		data <= {(DW + 1){1'b0}};
	else if ((chipselect == 1'b1) &&
			(write == 1'b1) &&
			(address == 2'h0))
	begin
		if (byteenable[0])
			data[ 7: 0] <= writedata[ 7: 0];
			
		if (byteenable[1])
			data[15: 8] <= writedata[15: 8];

		if (byteenable[2])
			data[23:16] <= writedata[23:16];
			
		if (byteenable[3])
			data[31:24] <= writedata[31:24];
	end
end

`ifdef USE_INOUT
always @(posedge clk)
begin
	if (reset == 1'b1)
		direction <= {(DW + 1){1'b0}};
	else if ((chipselect == 1'b1) &&
			(write == 1'b1) &&
			(address == 2'h1))
	begin
		if (byteenable[0])
			direction[ 7: 0] <= writedata[ 7: 0];
			
		if (byteenable[1])
			direction[15: 8] <= writedata[15: 8];

		if (byteenable[2])
			direction[23:16] <= writedata[23:16];
			
		if (byteenable[3])
			direction[31:24] <= writedata[31:24];
	end
end
`endif

`ifdef USE_INTERRUPTS
always @(posedge clk)
begin
	if (reset == 1'b1)
		interrupt <= {(DW + 1){1'b0}};
	else if ((chipselect == 1'b1) &&
			(write == 1'b1) &&
			(address == 2'h2))
	begin
		if (byteenable[0])
			interrupt[ 7: 0] <= writedata[ 7: 0];
			
		if (byteenable[1])
			interrupt[15: 8] <= writedata[15: 8];

		if (byteenable[2])
			interrupt[23:16] <= writedata[23:16];
			
		if (byteenable[3])
			interrupt[31:24] <= writedata[31:24];
	end
end
`endif

`ifdef USE_CAPTURE
always @(posedge clk)
begin
	if (reset == 1'b1)
		capture <= {(DW + 1){1'b0}};
	else if ((chipselect == 1'b1) &&
			(write == 1'b1) &&
			(address == 2'h3))
		capture <= {(DW + 1){1'b0}};
	else
		capture <= capture | new_capture;
end
`endif

`ifdef USE_DATA_OUT
always @(posedge clk)
	data_out <= data[DW: 0];
`endif

`ifdef USE_CAPTURE
always @(posedge clk)
begin
	if (reset == 1'b1)
		last_data_in <= {DW{1'b0}};
	else
		last_data_in <= data_in;
end
`endif

/*****************************************************************************
 *                            Combinational Logic                            *
 *****************************************************************************/

// Output Assignments
`ifdef USE_BIDIR_PORT
generate
	for (i=0; i <= DW; i = i + 1)
	begin : assign_data_out
		assign bidir_port[i] = direction[i] ? data_out[i] : 1'bZ;
	end
endgenerate
`elsif USE_GPIO_S
generate
	for (i=0; i <= DW; i = i + 1)
	begin : assign_data_out
		assign GPIO[i]	= direction[i] ? data_out[i] : 1'bZ;
	end
endgenerate
`elsif USE_GPIO_0
generate
	for (i=0; i <= DW; i = i + 1)
	begin : assign_data_out
		assign GPIO_0[i]	= direction[i] ? data_out[i] : 1'bZ;
	end
endgenerate
`elsif USE_GPIO_1
generate
	for (i=0; i <= DW; i = i + 1)
	begin : assign_data_out
		assign GPIO_1[i]	= direction[i] ? data_out[i] : 1'bZ;
	end
endgenerate
`elsif USE_GPIO_2
generate
	for (i=0; i <= DW; i = i + 1)
	begin : assign_data_out
		assign GPIO_2[i]	= direction[i] ? data_out[i] : 1'bZ;
	end
endgenerate
`endif

`ifdef USE_OUTPUT_PORT
assign out_port = data_out;
`elsif USE_TRI_COLOUR_LEDS
assign LED = data_out;
`elsif USE_GREEN_LEDS
assign LEDG = data_out;
`elsif USE_RED_LEDS
assign LEDR = data_out;
`elsif USE_TWO_SEVEN_SEGMENT
assign HEX0 = ~data_out[ 6: 0];
assign HEX1 = ~data_out[14: 8];
`elsif USE_LOW_SEVEN_SEGMENT
`ifdef USE_DE2_70
assign HEX0 = ~data_out[ 7: 0];
assign HEX1 = ~data_out[15: 8];
assign HEX2 = ~data_out[23:16];
assign HEX3 = ~data_out[31:24];
`elsif USE_DE0
assign HEX0 = ~data_out[ 7: 0];
assign HEX1 = ~data_out[15: 8];
assign HEX2 = ~data_out[23:16];
assign HEX3 = ~data_out[31:24];
`else
assign HEX0 = ~data_out[ 6: 0];
assign HEX1 = ~data_out[14: 8];
assign HEX2 = ~data_out[22:16];
assign HEX3 = ~data_out[30:24];
`endif
`elsif USE_HIGH_SEVEN_SEGMENT
`ifdef USE_DE2_70
assign HEX4 = ~data_out[ 7: 0];
assign HEX5 = ~data_out[15: 8];
assign HEX6 = ~data_out[23:16];
assign HEX7 = ~data_out[31:24];
`else
assign HEX4 = ~data_out[ 6: 0];
assign HEX5 = ~data_out[14: 8];
assign HEX6 = ~data_out[22:16];
assign HEX7 = ~data_out[30:24];
`endif
`endif

// Internal Assignments
`ifdef USE_EDGE_HIGH_CAPTURE
assign new_capture = data_in & ~last_data_in;
`elsif USE_EDGE_LOW_CAPTURE
assign new_capture = ~data_in & last_data_in;
`elsif USE_EDGE_BOTH_CAPTURE
assign new_capture = data_in ^ last_data_in;
`endif

/*****************************************************************************
 *                              Internal Modules                             *
 *****************************************************************************/


endmodule

