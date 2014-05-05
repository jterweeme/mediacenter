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
 * This module is designed for use on an Altera DE0-Nano Development Board.   *
 *  It allows a user to interface with the on-board Analog-to-Digital         *
 *  Converter.                                                                *
 *                                                                            *
 ******************************************************************************/
 
module altera_up_avalon_de0_nano_adc (
	clock, 
	reset, 
	read, 
	write, 
	readdata, 
	writedata, 
	address,
	waitrequest,
	adc_sclk,
	adc_cs_n,
	adc_din,
	adc_dout
);


/*****************************************************************************
 *                           Parameter Declarations                          *
 *****************************************************************************/
parameter tsclk = 8'd16;
parameter numch = 4'd8;

/*****************************************************************************
 *                             Port Declarations                             *
 *****************************************************************************/
	input clock, reset, read, write;
	input [31:0] writedata;
	input [2:0] address;
	output reg [31:0] readdata;
	output reg waitrequest;

	input adc_dout;
	output adc_sclk, adc_cs_n, adc_din;

	reg go;
	reg [11:0] values [7:0];
	reg auto_run;

	wire done;
	wire [11:0] outs [7:0];

	defparam ADC_CTRL.T_SCLK = tsclk, ADC_CTRL.NUM_CH = numch;
	adv_adc ADC_CTRL (clock, reset, go, adc_sclk, adc_cs_n, adc_din, adc_dout, done, outs[0],
						 outs[1], outs[2], outs[3], outs[4], outs [5], outs[6], outs[7]);
// - - - - - - - readdata & waitrequest - - - - - - -
	always @(*)
	begin
		readdata =0;
		waitrequest =0;
		if (write && done)	//need done to be lowered before re-raising go
			waitrequest=1; 
		if (read) begin
			if (go&&!auto_run)
				waitrequest=1;
			else
				readdata = {20'b0, values[address]};
		end
	end	
// - - - - - - - - - - go  - - - - - - - - - 	
	always @ (posedge clock)
	begin
		if (reset) 
			go<=1'b0;
		else if (done) 
			go<=1'b0;
		else if (write && address == 3'b0)
			go<=1'b1;
		else if (auto_run)
			go<=1'b1;
	end
// - - - - - - - values - - - - - - - - - - - - -	
	always @ (posedge clock) 
		if (done) begin
			values[0] <= outs[0];
			values[1] <= outs[1];
			values[2] <= outs[2];
			values[3] <= outs[3];
			values[4] <= outs[4];
			values[5] <= outs[5];
			values[6] <= outs[6];
			values[7] <= outs[7];
		end
// - - - - - - - - - - auto run - - - - - - - - - - 
	always @(posedge clock) 
		if (write && address == 3'd1)
			auto_run <= writedata[0];

endmodule
