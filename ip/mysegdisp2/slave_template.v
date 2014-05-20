/*
  Legal Notice: (C)2008 Altera Corporation. All rights reserved.  Your
  use of Altera Corporation's design tools, logic functions and other
  software and tools, and its AMPP partner logic functions, and any
  output files any of the foregoing (including device programming or
  simulation files), and any associated documentation or information are
  expressly subject to the terms and conditions of the Altera Program
  License Subscription Agreement or other applicable license agreement,
  including, without limitation, that your use is for the sole purpose
  of programming logic devices manufactured by Altera and sold by Altera
  or its authorized distributors.  Please refer to the applicable
  agreement for further details.
*/

/*

  Author:  JCJB
  Date:  09/03/2008
  
  This slave component has a parameterizable data width and 16 input/output
  words.  There are five modes for each addressable word in this component
  as follows:
  
  Mode = 0  --> Output only
  Mode = 1  --> Input only
  Mode = 2  --> Output and input (independent I/O, default)
  Mode = 3  --> Output with loopback (software readable output registers)
  Mode = 4	--> Disabled

  This component is always available so the waitrequest signal is not used
  and it has fixed read and write latencies.  The write latency is 0 and the
  read latency is 3 cycles.  If you attempt to access a location that doesn't
  support the necessary functionality then you will either write to a
  non-existent register (write to space) or will readback 0 as the inputs
  will be grounded if they are disabled.  Inputs or outputs that are removed
  will be due to the component tcl file stubbing the signals.  Disabled outputs
  will not be exposed at the top of the system and the Quartus II software
  will optimize the register away.  Disabled inputs (except in the loopback
  mode) will not be exposed at the top and internally be wired to ground.
  The Quartus II software as a result will optimize the input registers to
  be hardcoded wires set to ground automatically.
  
  In order for your external logic to know which register is being accessed
  you will need to enable 'ENABLE_SYNC_SIGNALS' by setting it to 1.  When
  enabled, the user_chipselect/byteenable/read/write signals will be exposed to your
  external logic which you can use to determine which register is being accessed
  and whether it's a read or write access.
  
  If you use the syncronization signals use the following to qualify them:
  
  Read:  user_chipselect[x] AND user_read
  Write:  user_chipselect[x] AND user_write AND user_byteenable
  
  Note: Reads return the full word regardless of the byteenables presented.
*/




module slave_template (
	// signals to connect to an Avalon clock source interface
	clk,
	reset,
	
	// signals to connect to an Avalon-MM slave interface
	slave_address,
	slave_read,
	slave_write,
	slave_readdata,
	slave_writedata,
	slave_byteenable,

	// signals to connect to custom user logic (up to 16 input and output pairs)
	user_dataout_0,
	user_dataout_1,
	user_dataout_2,
	user_dataout_3,
	user_dataout_4,
	user_dataout_5,
	user_dataout_6,
	user_dataout_7,
	user_dataout_8,
	user_dataout_9,
	user_dataout_10,
	user_dataout_11,
	user_dataout_12,
	user_dataout_13,
	user_dataout_14,
	user_dataout_15,
	user_datain_0,
	user_datain_1,
	user_datain_2,
	user_datain_3,
	user_datain_4,
	user_datain_5,
	user_datain_6,
	user_datain_7,
	user_datain_8,
	user_datain_9,
	user_datain_10,
	user_datain_11,
	user_datain_12,
	user_datain_13,
	user_datain_14,
	user_datain_15,
	
	// optional signals so that your external logic knows what location is being accessed
	user_chipselect,
	user_byteenable,
	user_write,
	user_read
);

	// most of the set values will only be used by the component .tcl file.  The DATA_WIDTH and MODE_X = 3 influence the hardware created.
	// ENABLE_SYNC_SIGNALS isn't used by this hardware at all but it provided anyway so that it can be exposed in the component .tcl file
	// to control the stubbing of certain signals.
	parameter DATA_WIDTH = 32;          // word size of each input and output register
	parameter ENABLE_SYNC_SIGNALS = 0;  // only used by the component .tcl file, 1 to expose user_chipselect/write/read, 0 to stub them
	parameter MODE_0 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_1 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_2 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_3 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_4 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_5 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_6 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_7 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_8 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_9 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_10 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_11 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_12 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_13 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_14 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled
	parameter MODE_15 = 2;               // 0 = Output, 1 = Input, 2 = Ouput and Input, 3 = Output with loopback, 4 = Disabled



	// clock interface
	input clk;
	input reset;
	
	
	// slave interface
	input [3:0] slave_address;
	input slave_read;
	input slave_write;
	output reg [DATA_WIDTH-1:0] slave_readdata;
	input [DATA_WIDTH-1:0] slave_writedata;
	input [(DATA_WIDTH/8)-1:0] slave_byteenable;


	// user interface
	output wire [DATA_WIDTH-1:0] user_dataout_0;
	output wire [DATA_WIDTH-1:0] user_dataout_1;
	output wire [DATA_WIDTH-1:0] user_dataout_2;
	output wire [DATA_WIDTH-1:0] user_dataout_3;
	output wire [DATA_WIDTH-1:0] user_dataout_4;
	output wire [DATA_WIDTH-1:0] user_dataout_5;
	output wire [DATA_WIDTH-1:0] user_dataout_6;
	output wire [DATA_WIDTH-1:0] user_dataout_7;
	output wire [DATA_WIDTH-1:0] user_dataout_8;
	output wire [DATA_WIDTH-1:0] user_dataout_9;
	output wire [DATA_WIDTH-1:0] user_dataout_10;
	output wire [DATA_WIDTH-1:0] user_dataout_11;
	output wire [DATA_WIDTH-1:0] user_dataout_12;
	output wire [DATA_WIDTH-1:0] user_dataout_13;
	output wire [DATA_WIDTH-1:0] user_dataout_14;
	output wire [DATA_WIDTH-1:0] user_dataout_15;
	input [DATA_WIDTH-1:0] user_datain_0;
	input [DATA_WIDTH-1:0] user_datain_1;
	input [DATA_WIDTH-1:0] user_datain_2;
	input [DATA_WIDTH-1:0] user_datain_3;
	input [DATA_WIDTH-1:0] user_datain_4;
	input [DATA_WIDTH-1:0] user_datain_5;
	input [DATA_WIDTH-1:0] user_datain_6;
	input [DATA_WIDTH-1:0] user_datain_7;
	input [DATA_WIDTH-1:0] user_datain_8;
	input [DATA_WIDTH-1:0] user_datain_9;
	input [DATA_WIDTH-1:0] user_datain_10;
	input [DATA_WIDTH-1:0] user_datain_11;
	input [DATA_WIDTH-1:0] user_datain_12;
	input [DATA_WIDTH-1:0] user_datain_13;
	input [DATA_WIDTH-1:0] user_datain_14;
	input [DATA_WIDTH-1:0] user_datain_15;
	output wire [15:0] user_chipselect;
	output wire [(DATA_WIDTH/8)-1:0] user_byteenable;
	output wire user_write;
	output wire user_read;
	
	
	// internal logic signals
	wire [(DATA_WIDTH/8)-1:0] internal_byteenable;  // when DATA_WIDTH is 8 bits need to hardcode this signal to 1 since the fabric doesn't support 1 bit byte enables
	reg [(DATA_WIDTH/8)-1:0] internal_byteenable_d1;
	wire [15:0] address_decode;
	reg [15:0] address_decode_d1;         // used to select the first stage of mux pipelining (a, b, c, or d)
	wire [3:0] address_bank_decode;       // used to select the second stage of mux pipelining (mux a, b, c, or d)
	reg [3:0] address_bank_decode_d1;     // used to select the second stage of mux pipelining (mux a, b, c, or d)
    reg slave_read_d1;                    // used to qualify the first stage of mux pipelining (a, b, c, or d)
    reg slave_read_d2;                    // used to qualify the second stage of mux pipelining (slave_readdata)
    reg slave_write_d1;                   // used by the option user_write signal
	reg [DATA_WIDTH-1:0] user_datain_0_d1;
	reg [DATA_WIDTH-1:0] user_datain_1_d1;
	reg [DATA_WIDTH-1:0] user_datain_2_d1;
	reg [DATA_WIDTH-1:0] user_datain_3_d1;
	reg [DATA_WIDTH-1:0] user_datain_4_d1;
	reg [DATA_WIDTH-1:0] user_datain_5_d1;
	reg [DATA_WIDTH-1:0] user_datain_6_d1;
	reg [DATA_WIDTH-1:0] user_datain_7_d1;
	reg [DATA_WIDTH-1:0] user_datain_8_d1;
	reg [DATA_WIDTH-1:0] user_datain_9_d1;
	reg [DATA_WIDTH-1:0] user_datain_10_d1;
	reg [DATA_WIDTH-1:0] user_datain_11_d1;
	reg [DATA_WIDTH-1:0] user_datain_12_d1;
	reg [DATA_WIDTH-1:0] user_datain_13_d1;
	reg [DATA_WIDTH-1:0] user_datain_14_d1;
	reg [DATA_WIDTH-1:0] user_datain_15_d1;
	reg [DATA_WIDTH-1:0] mux_first_stage_a;      // muxed inputs 0-3
	reg [DATA_WIDTH-1:0] mux_first_stage_b;      // muxed inputs 4-7
	reg [DATA_WIDTH-1:0] mux_first_stage_c;      // muxed inputs 8-11
	reg [DATA_WIDTH-1:0] mux_first_stage_d;      // muxed inputs 12-15
	

	// when the data width is 8 need to hardcode the 1 bit byteenable to high
	generate
		if (DATA_WIDTH == 8)
		begin
			assign internal_byteenable = 1'b1;
		end
		else
		begin
			assign internal_byteenable = slave_byteenable;
		end
	endgenerate


	// sixteen address decodes (using one-hot encoding)  A bank is considered to be a grouping of four addresses.
	assign address_decode[0] = (slave_address == 4'b0000) & (slave_write | slave_read);
	assign address_decode[1] = (slave_address == 4'b0001) & (slave_write | slave_read);
	assign address_decode[2] = (slave_address == 4'b0010) & (slave_write | slave_read);
	assign address_decode[3] = (slave_address == 4'b0011) & (slave_write | slave_read);
	assign address_decode[4] = (slave_address == 4'b0100) & (slave_write | slave_read);
	assign address_decode[5] = (slave_address == 4'b0101) & (slave_write | slave_read);
	assign address_decode[6] = (slave_address == 4'b0110) & (slave_write | slave_read);
	assign address_decode[7] = (slave_address == 4'b0111) & (slave_write | slave_read);
	assign address_decode[8] = (slave_address == 4'b1000) & (slave_write | slave_read);
	assign address_decode[9] = (slave_address == 4'b1001) & (slave_write | slave_read);
	assign address_decode[10] = (slave_address == 4'b1010) & (slave_write | slave_read);
	assign address_decode[11] = (slave_address == 4'b1011) & (slave_write | slave_read);
	assign address_decode[12] = (slave_address == 4'b1100) & (slave_write | slave_read);
	assign address_decode[13] = (slave_address == 4'b1101) & (slave_write | slave_read);
	assign address_decode[14] = (slave_address == 4'b1110) & (slave_write | slave_read);
	assign address_decode[15] = (slave_address == 4'b1111) & (slave_write | slave_read);
	assign address_bank_decode[0] = (address_decode_d1[3:0] != 0)? 1'b1 : 1'b0;
	assign address_bank_decode[1] = (address_decode_d1[7:4] != 0)? 1'b1 : 1'b0;
	assign address_bank_decode[2] = (address_decode_d1[11:8] != 0)? 1'b1 : 1'b0;
	assign address_bank_decode[3] = (address_decode_d1[15:12] != 0)? 1'b1 : 1'b0;
		
	
	// registering various address decoding registers and the input data
	always @ (posedge clk or posedge reset)
	begin
		if (reset == 1)
		begin
			slave_read_d1 <= 0;
			slave_read_d2 <= 0;
			slave_write_d1 <= 0;
			address_decode_d1 <= 0;
			address_bank_decode_d1 <= 0;
			internal_byteenable_d1 <= 0;
			user_datain_0_d1 <= 0;
			user_datain_1_d1 <= 0;
			user_datain_2_d1 <= 0;
			user_datain_3_d1 <= 0;
			user_datain_4_d1 <= 0;
			user_datain_5_d1 <= 0;
			user_datain_6_d1 <= 0;
			user_datain_7_d1 <= 0;
			user_datain_8_d1 <= 0;
			user_datain_9_d1 <= 0;
			user_datain_10_d1 <= 0;
			user_datain_11_d1 <= 0;
			user_datain_12_d1 <= 0;
			user_datain_13_d1 <= 0;
			user_datain_14_d1 <= 0;
			user_datain_15_d1 <= 0;
		end
		else
		begin
			slave_read_d1 <= slave_read;
			slave_read_d2 <= slave_read_d1;
			slave_write_d1 <= slave_write;
			internal_byteenable_d1 <= internal_byteenable;
			if((slave_read == 1) | (slave_write == 1))
			begin
				address_decode_d1 <= address_decode;
			end
			if(slave_read_d1 == 1)
			begin
				address_bank_decode_d1 <= address_bank_decode;
			end
			if((address_decode[0] == 1) & (slave_read == 1))
			begin
				user_datain_0_d1 <= user_datain_0;
			end
			if((address_decode[1] == 1) & (slave_read == 1))
			begin
				user_datain_1_d1 <= user_datain_1;
			end
			if((address_decode[2] == 1) & (slave_read == 1))
			begin
				user_datain_2_d1 <= user_datain_2;
			end
			if((address_decode[3] == 1) & (slave_read == 1))
			begin
				user_datain_3_d1 <= user_datain_3;
			end
			if((address_decode[4] == 1) & (slave_read == 1))
			begin
				user_datain_4_d1 <= user_datain_4;
			end
			if((address_decode[5] == 1) & (slave_read == 1))
			begin
				user_datain_5_d1 <= user_datain_5;
			end
			if((address_decode[6] == 1) & (slave_read == 1))
			begin
				user_datain_6_d1 <= user_datain_6;
			end
			if((address_decode[7] == 1) & (slave_read == 1))
			begin
				user_datain_7_d1 <= user_datain_7;
			end
			if((address_decode[8] == 1) & (slave_read == 1))
			begin
				user_datain_8_d1 <= user_datain_8;
			end
			if((address_decode[9] == 1) & (slave_read == 1))
			begin
				user_datain_9_d1 <= user_datain_9;
			end
			if((address_decode[10] == 1) & (slave_read == 1))
			begin
				user_datain_10_d1 <= user_datain_10;
			end
			if((address_decode[11] == 1) & (slave_read == 1))
			begin
				user_datain_11_d1 <= user_datain_11;
			end
			if((address_decode[12] == 1) & (slave_read == 1))
			begin
				user_datain_12_d1 <= user_datain_12;
			end
			if((address_decode[13] == 1) & (slave_read == 1))
			begin
				user_datain_13_d1 <= user_datain_13;
			end
			if((address_decode[14] == 1) & (slave_read == 1))
			begin
				user_datain_14_d1 <= user_datain_14;
			end
			if((address_decode[15] == 1) & (slave_read == 1))
			begin
				user_datain_15_d1 <= user_datain_15;
			end
		end
	end
	


	// sixteen output registers which use byteenables to register each byte.  Disabling the write enable for output registers that were not needed.
	register_with_bytelanes register_0 (clk,reset,slave_writedata,slave_write&address_decode[0],internal_byteenable,user_dataout_0);
		defparam register_0.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_1 (clk,reset,slave_writedata,slave_write&address_decode[1],internal_byteenable,user_dataout_1);
		defparam register_1.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_2 (clk,reset,slave_writedata,slave_write&address_decode[2],internal_byteenable,user_dataout_2);
		defparam register_2.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_3 (clk,reset,slave_writedata,slave_write&address_decode[3],internal_byteenable,user_dataout_3);
		defparam register_3.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_4 (clk,reset,slave_writedata,slave_write&address_decode[4],internal_byteenable,user_dataout_4);
		defparam register_4.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_5 (clk,reset,slave_writedata,slave_write&address_decode[5],internal_byteenable,user_dataout_5);
		defparam register_5.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_6 (clk,reset,slave_writedata,slave_write&address_decode[6],internal_byteenable,user_dataout_6);
		defparam register_6.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_7 (clk,reset,slave_writedata,slave_write&address_decode[7],internal_byteenable,user_dataout_7);
		defparam register_7.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_8 (clk,reset,slave_writedata,slave_write&address_decode[8],internal_byteenable,user_dataout_8);
		defparam register_8.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_9 (clk,reset,slave_writedata,slave_write&address_decode[9],internal_byteenable,user_dataout_9);
		defparam register_9.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_10 (clk,reset,slave_writedata,slave_write&address_decode[10],internal_byteenable,user_dataout_10);
		defparam register_10.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_11 (clk,reset,slave_writedata,slave_write&address_decode[11],internal_byteenable,user_dataout_11);
		defparam register_11.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_12 (clk,reset,slave_writedata,slave_write&address_decode[12],internal_byteenable,user_dataout_12);
		defparam register_12.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_13 (clk,reset,slave_writedata,slave_write&address_decode[13],internal_byteenable,user_dataout_13);
		defparam register_13.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_14 (clk,reset,slave_writedata,slave_write&address_decode[14],internal_byteenable,user_dataout_14);
		defparam register_14.DATA_WIDTH = DATA_WIDTH;
	register_with_bytelanes register_15 (clk,reset,slave_writedata,slave_write&address_decode[15],internal_byteenable,user_dataout_15);
		defparam register_15.DATA_WIDTH = DATA_WIDTH;


	// registered slave_readdata mux for all the return values, if an input register is disabled then a zero will be passed in
	always @ (posedge clk or posedge reset)
	begin
		if (reset == 1)
		begin
			slave_readdata <= 0;
		end
		else
		begin
			if (slave_read_d1 == 1)  // first multiplexer stage
			begin
				// when the mode parameters are set to 3 then the outputs are looped back as inputs
				case (address_decode_d1[3:0])
					4'b0001: mux_first_stage_a <= (MODE_0 == 3)? user_dataout_0 : user_datain_0_d1;
					4'b0010: mux_first_stage_a <= (MODE_1 == 3)? user_dataout_1 : user_datain_1_d1;
					4'b0100: mux_first_stage_a <= (MODE_2 == 3)? user_dataout_2 : user_datain_2_d1;
					4'b1000: mux_first_stage_a <= (MODE_3 == 3)? user_dataout_3 : user_datain_3_d1;
				endcase
				case (address_decode_d1[7:4])
					4'b0001: mux_first_stage_b <= (MODE_4 == 3)? user_dataout_4 : user_datain_4_d1;
					4'b0010: mux_first_stage_b <= (MODE_5 == 3)? user_dataout_5 : user_datain_5_d1;
					4'b0100: mux_first_stage_b <= (MODE_6 == 3)? user_dataout_6 : user_datain_6_d1;
					4'b1000: mux_first_stage_b <= (MODE_7 == 3)? user_dataout_7 : user_datain_7_d1;
				endcase
				case (address_decode_d1[11:8])
					4'b0001: mux_first_stage_c <= (MODE_8 == 3)? user_dataout_8 : user_datain_8_d1;
					4'b0010: mux_first_stage_c <= (MODE_9 == 3)? user_dataout_9 : user_datain_9_d1;
					4'b0100: mux_first_stage_c <= (MODE_10 == 3)? user_dataout_10 : user_datain_10_d1;
					4'b1000: mux_first_stage_c <= (MODE_11 == 3)? user_dataout_11 : user_datain_11_d1;
				endcase
				case (address_decode_d1[15:12])
					4'b0001: mux_first_stage_d <= (MODE_12 == 3)? user_dataout_12 : user_datain_12_d1;
					4'b0010: mux_first_stage_d <= (MODE_13 == 3)? user_dataout_13 : user_datain_13_d1;
					4'b0100: mux_first_stage_d <= (MODE_14 == 3)? user_dataout_14 : user_datain_14_d1;
					4'b1000: mux_first_stage_d <= (MODE_15 == 3)? user_dataout_15 : user_datain_15_d1;
				endcase
			end
			
			if (slave_read_d2 == 1)  // second multiplexer stage
			begin
				case (address_bank_decode_d1[3:0])
					4'b0001: slave_readdata <= mux_first_stage_a;
					4'b0010: slave_readdata <= mux_first_stage_b;
					4'b0100: slave_readdata <= mux_first_stage_c;
					4'b1000: slave_readdata <= mux_first_stage_d;
				endcase
			end
		end
	end
	
	
	assign user_write = slave_write_d1;  // outputs are registed so need a delayed copy of the write signal
	assign user_read = slave_read;
	assign user_chipselect = (slave_write_d1 == 1)? address_decode_d1 : address_decode;  // for write cycles need the delayed copy of the address decode since outputs are registered
	assign user_byteenable = (slave_write_d1 == 1)? internal_byteenable_d1 : internal_byteenable;  // for write cycles need the delayed copy of the byteenables, don't use the byteenables for reads since the full word is always sent on read transfers
	
endmodule



// helper module to simplify having a register of variable width and containing independent byte lanes
module register_with_bytelanes (
	clk,
	reset,
	
	data_in,
	write,
	byte_enables,
	data_out
);

	parameter DATA_WIDTH = 32;
	
	input clk;
	input reset;
	
	input [DATA_WIDTH-1:0] data_in;
	input write;
	input [(DATA_WIDTH/8)-1:0] byte_enables;
	output reg [DATA_WIDTH-1:0] data_out;
	
	// generating write logic for each group of 8 bits for 'data_out'
	generate
	genvar LANE;
		for(LANE = 0; LANE < (DATA_WIDTH/8); LANE = LANE+1)
		begin: register_bytelane_generation	
			always @ (posedge clk or posedge reset)
			begin
				if(reset == 1)
				begin
					data_out[(LANE*8)+7:(LANE*8)] <= 0;
				end
				else
				begin
					if((byte_enables[LANE] == 1) & (write == 1))
					begin
						data_out[(LANE*8)+7:(LANE*8)] <= data_in[(LANE*8)+7:(LANE*8)];  // write to each byte lane with write = 1 and the lane byteenable = 1
					end
				end
			end
		end
	endgenerate
	
endmodule
