/*
Jasper ter Weeme
*/

module slave_template(
	input clk,
	input reset,	
	input [1:0] slave_address,
	input slave_read,
	input slave_write,
	output reg [15:0] slave_readdata,
	input [15:0] slave_writedata,
	input [3:0] slave_byteenable,
	output wire [13:0] user_dataout_0
	//output wire [15:0] user_chipselect,
	//output wire [1:0] user_byteenable,
	//output wire user_write,
	//output wire user_read
);
	wire [15:0] internal_byteenable;
	reg [1:0] internal_byteenable_d1;
	wire [15:0] address_decode;
	reg [15:0] address_decode_d1; 
	wire [3:0] address_bank_decode;   
	reg [3:0] address_bank_decode_d1;   
    reg slave_read_d1;          
    reg slave_read_d2;           
    reg slave_write_d1;      
	reg [15:0] mux_first_stage_a;
	reg [15:0] mux_first_stage_b;
	reg [15:0] mux_first_stage_c;
    reg [15:0] mux_first_stage_d;
    assign internal_byteenable = slave_byteenable;
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
	
	always @ (posedge clk or posedge reset) begin
		if (reset == 1) begin
			slave_read_d1 <= 0;
			slave_read_d2 <= 0;
			slave_write_d1 <= 0;
			address_decode_d1 <= 0;
			address_bank_decode_d1 <= 0;
			internal_byteenable_d1 <= 0;
		end else begin
			slave_read_d1 <= slave_read;
			slave_read_d2 <= slave_read_d1;
			slave_write_d1 <= slave_write;
			internal_byteenable_d1 <= internal_byteenable;
            
			if((slave_read == 1) | (slave_write == 1))
				address_decode_d1 <= address_decode;
            
			if(slave_read_d1 == 1)
				address_bank_decode_d1 <= address_bank_decode;
		end
	end
	
	register_with_bytelanes r0(clk,reset,slave_writedata,slave_write&address_decode[0],internal_byteenable,user_dataout_0);
endmodule


module register_with_bytelanes(
	input clk,
	input reset,
	input [15:0] data_in,
	input write,
	input [1:0] byte_enables,
	output reg [13:0] data_out
);
    always @(posedge clk or posedge reset) begin
        if (reset == 1)
            data_out[6:0] <= 7'b0;
        else if (byte_enables[0] & write)
            data_out[6:0] <= data_in[6:0];
    end
    
    always @(posedge clk or posedge reset) begin
        if (reset == 1)
            data_out[13:7] <= 7'b0;
        else if (byte_enables[1] & write)
            data_out[13:7] <= data_in[14:8];
    end
endmodule


