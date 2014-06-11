module alpha_blender(
	clk,
	reset,
	background_data,
	background_startofpacket,
	background_endofpacket,
	background_empty,
	background_valid,
	foreground_data,
	foreground_startofpacket,
	foreground_endofpacket,
	foreground_empty,
	foreground_valid,
	output_ready,
	background_ready,
	foreground_ready,
	output_data,
	output_startofpacket,
	output_endofpacket,
	output_empty,
	output_valid
);

input clk;
input reset;
input [29: 0] background_data;
input background_startofpacket;
input background_endofpacket;
input [ 1: 0] background_empty;
input background_valid;
input [39: 0] foreground_data;
input foreground_startofpacket;
input foreground_endofpacket;
input [ 1: 0] foreground_empty;
input foreground_valid;
input output_ready;
output background_ready;
output					foreground_ready;
output		[29: 0]	output_data;
output					output_startofpacket;
output					output_endofpacket;
output		[ 1: 0]	output_empty;
output					output_valid;
wire			[ 9: 0]	new_red;
wire			[ 9: 0]	new_green;
wire [ 9: 0] new_blue;
wire sync_foreground;
wire sync_background;
wire valid;

// Internal Registers

// State Machine Registers

/*****************************************************************************
 *                         Finite State Machine(s)                           *
 *****************************************************************************/


/*****************************************************************************
 *                             Sequential Logic                              *
 *****************************************************************************/
// Output Registers

// Internal Registers

/*****************************************************************************
 *                            Combinational Logic                            *
 *****************************************************************************/
// Output Assignments
assign background_ready = (output_ready & output_valid) | sync_background;
assign foreground_ready = (output_ready & output_valid) | sync_foreground;

assign output_data				= {new_red, new_green, new_blue};
assign output_startofpacket 	= foreground_startofpacket;
assign output_endofpacket		= foreground_endofpacket;
assign output_empty				= 2'h0;
assign output_valid				= valid;

// Internal Assignments
assign sync_foreground = (foreground_valid & background_valid &
			((background_startofpacket & ~foreground_startofpacket) |
			(background_endofpacket & ~foreground_endofpacket)));
assign sync_background = (foreground_valid & background_valid &
			((foreground_startofpacket & ~background_startofpacket) |
			(foreground_endofpacket & ~background_endofpacket)));

assign valid =	foreground_valid & background_valid & 
				~sync_foreground & ~sync_background;

/*****************************************************************************
 *                              Internal Modules                             *
 *****************************************************************************/

`ifdef USE_NORMAL_MODE
altera_up_video_alpha_blender_normal alpha_blender (
	// Inputs
	.background_data	(background_data),
	.foreground_data	(foreground_data),

	// Bidirectionals

	// Outputs
	.new_red				(new_red),
	.new_green			(new_green),
	.new_blue			(new_blue)
);
`else
altera_up_video_alpha_blender_simple alpha_blender (
	// Inputs
	.background_data	(background_data),
	.foreground_data	(foreground_data),

	// Bidirectionals

	// Outputs
	.new_red				(new_red),
	.new_green			(new_green),
	.new_blue			(new_blue)
);
`endif


endmodule

