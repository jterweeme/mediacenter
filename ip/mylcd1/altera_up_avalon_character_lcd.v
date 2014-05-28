module altera_up_avalon_character_lcd(
	clk,
	reset,
	address,
	chipselect,
	read,
	write,
	writedata,
	LCD_DATA,
	LCD_ON,
	LCD_BLON,
	LCD_EN,
	LCD_RS,
	LCD_RW,
	readdata,
	waitrequest
);

parameter	CURSOR_ON	= 1'b1;
parameter	BLINKING_ON	= 1'b1;

input						clk;
input						reset;
input						address;
input						chipselect;
input						read;
input						write;
input			[ 7: 0]	writedata;
inout			[ 7: 0]	LCD_DATA;
output					LCD_ON;				//	LCD Power ON/OFF
output					LCD_BLON;			//	LCD Back Light ON/OFF
output					LCD_EN;				//	LCD Enable
output					LCD_RS;				//	LCD 0-Command/1-Data Select
output					LCD_RW;				//	LCD 1-Read/0-Write Select

output		[ 7: 0]	readdata;
output					waitrequest;

/*****************************************************************************
 *                           Constant Declarations                           *
 *****************************************************************************/
// states
localparam	LCD_STATE_0_IDLE					= 3'h0,
				LCD_STATE_1_INITIALIZE			= 3'h1,
				LCD_STATE_2_START_CHECK_BUSY	= 3'h2,
				LCD_STATE_3_CHECK_BUSY			= 3'h3,
				LCD_STATE_4_BEGIN_TRANSFER		= 3'h4,
				LCD_STATE_5_TRANSFER				= 3'h5,
				LCD_STATE_6_COMPLETE				= 3'h6;

wire						transfer_complete;
wire						done_initialization;
wire						init_send_command;
wire			[ 8: 0]	init_command;
wire						send_data;
wire			[ 7: 0]	data_received;
reg						initialize_lcd_display;
reg			[ 7: 0]	data_to_send;
reg						rs;
reg						rw;
reg			[ 2: 0]	ns_lcd_controller;
reg			[ 2: 0]	s_lcd_controller;

always @(posedge clk)
begin
	if (reset)
		s_lcd_controller <= LCD_STATE_0_IDLE;
	else
		s_lcd_controller <= ns_lcd_controller;
end

always @(*) begin
	ns_lcd_controller = LCD_STATE_0_IDLE;

   case (s_lcd_controller)
	LCD_STATE_0_IDLE:
		begin
			if (initialize_lcd_display)
				ns_lcd_controller = LCD_STATE_1_INITIALIZE;
			else if (chipselect)
				ns_lcd_controller = LCD_STATE_2_START_CHECK_BUSY; 
			else
				ns_lcd_controller = LCD_STATE_0_IDLE;
		end
	LCD_STATE_1_INITIALIZE:
		begin
			if (done_initialization)
				ns_lcd_controller = LCD_STATE_6_COMPLETE;
			else
				ns_lcd_controller = LCD_STATE_1_INITIALIZE;
		end
	LCD_STATE_2_START_CHECK_BUSY:
		begin
			if (transfer_complete == 1'b0)
				ns_lcd_controller = LCD_STATE_3_CHECK_BUSY;
			else
				ns_lcd_controller = LCD_STATE_2_START_CHECK_BUSY;
		end
	LCD_STATE_3_CHECK_BUSY:
		begin
			if ((transfer_complete) && (data_received[7]))
				ns_lcd_controller = LCD_STATE_2_START_CHECK_BUSY;
			else if ((transfer_complete) && (data_received[7] == 1'b0))
				ns_lcd_controller = LCD_STATE_4_BEGIN_TRANSFER;
			else
				ns_lcd_controller = LCD_STATE_3_CHECK_BUSY;
		end
	LCD_STATE_4_BEGIN_TRANSFER:
		begin
			if (transfer_complete == 1'b0)
				ns_lcd_controller = LCD_STATE_5_TRANSFER;
			else
				ns_lcd_controller = LCD_STATE_4_BEGIN_TRANSFER;
		end
	LCD_STATE_5_TRANSFER:
		begin
			if (transfer_complete)
				ns_lcd_controller = LCD_STATE_6_COMPLETE;
			else
				ns_lcd_controller = LCD_STATE_5_TRANSFER;
		end
	LCD_STATE_6_COMPLETE:
		begin
			ns_lcd_controller = LCD_STATE_0_IDLE;
		end
	default:
		begin
			ns_lcd_controller = LCD_STATE_0_IDLE;
		end
	endcase
end

always @(posedge clk) begin
	if (reset)
		initialize_lcd_display <= 1'b1;
	else if (done_initialization)
		initialize_lcd_display <= 1'b0;
end

always @(posedge clk) begin
	if (reset)
		data_to_send <= 8'h00;
	else if (s_lcd_controller == LCD_STATE_1_INITIALIZE)
		data_to_send <= init_command[7:0];
	else if (s_lcd_controller == LCD_STATE_4_BEGIN_TRANSFER)
		data_to_send <= writedata[7:0];
end

always @(posedge clk)
begin
	if (reset)
		rs <= 1'b0;
	else if (s_lcd_controller == LCD_STATE_1_INITIALIZE)
		rs <= init_command[8];
	else if (s_lcd_controller == LCD_STATE_2_START_CHECK_BUSY)
		rs <= 1'b0;
	else if (s_lcd_controller == LCD_STATE_4_BEGIN_TRANSFER)
		rs <= address;
end

always @(posedge clk)
begin
	if (reset)
		rw <= 1'b0;
	else if (s_lcd_controller == LCD_STATE_1_INITIALIZE)
		rw <= 1'b0;
	else if (s_lcd_controller == LCD_STATE_2_START_CHECK_BUSY)
		rw <= 1'b1;
	else if (s_lcd_controller == LCD_STATE_4_BEGIN_TRANSFER)
		rw <= ~write;
end

assign readdata = data_received;
assign LCD_BLON = 1'b0;
assign waitrequest = chipselect & (s_lcd_controller != LCD_STATE_6_COMPLETE);

assign send_data	= 
	(s_lcd_controller == LCD_STATE_1_INITIALIZE) ?	init_send_command :
	(s_lcd_controller == LCD_STATE_3_CHECK_BUSY) ?	1'b1 :
	(s_lcd_controller == LCD_STATE_5_TRANSFER) ?	1'b1 : 1'b0;

altera_up_character_lcd_communication Char_LCD_Comm (
	.clk							(clk),
	.reset						(reset),

	.data_in						(data_to_send),
	.enable						(send_data),
	.rs							(rs),
	.rw							(rw),
	.display_on					(1'b1),
	.back_light_on				(1'b1),
	.LCD_DATA					(LCD_DATA),
	.LCD_ON						(LCD_ON),
	//.LCD_BLON					(LCD_BLON),
	.LCD_EN						(LCD_EN),
	.LCD_RS						(LCD_RS),
	.LCD_RW						(LCD_RW),
	.data_out					(data_received),
	.transfer_complete		(transfer_complete)
);

altera_up_character_lcd_initialization Char_LCD_Init(
	.clk(clk),
	.reset(reset),
	.initialize_LCD_display(initialize_lcd_display),
	.command_was_sent(transfer_complete),
	.done_initialization(done_initialization),
	.send_command(init_send_command),
	.the_command(init_command)
);
defparam
	Char_LCD_Init.CURSOR_ON		= CURSOR_ON,
	Char_LCD_Init.BLINKING_ON	= BLINKING_ON;

endmodule

