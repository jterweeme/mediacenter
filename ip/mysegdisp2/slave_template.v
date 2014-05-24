module slave_template(
	input clk,
	input reset,
	
	input [3:0] slave_address,
	input slave_read,
	input slave_write,
	output reg [31:0] slave_readdata,
	input [31:0] slave_writedata,
	input [3:0] slave_byteenable,

	output wire [31:0] user_dataout_0,
	output wire [15:0] user_chipselect,
	output wire [3:0] user_byteenable,
	output wire user_write,
	output wire user_read
);

	parameter DATA_WIDTH = 32;
	parameter ENABLE_SYNC_SIGNALS = 0;
	parameter MODE_0 = 2;

	wire [(DATA_WIDTH/8)-1:0] internal_byteenable;
	reg [(DATA_WIDTH/8)-1:0] internal_byteenable_d1;
	wire [15:0] address_decode;
	reg [15:0] address_decode_d1; 
	wire [3:0] address_bank_decode;   
	reg [3:0] address_bank_decode_d1;   
    reg slave_read_d1;          
    reg slave_read_d2;           
    reg slave_write_d1;      
	reg [DATA_WIDTH-1:0] mux_first_stage_a;
	reg [DATA_WIDTH-1:0] mux_first_stage_b;
	reg [DATA_WIDTH-1:0] mux_first_stage_c;
	reg [DATA_WIDTH-1:0] mux_first_stage_d;
	
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
		end
	end
	
	register_with_bytelanes r0(clk,reset,slave_writedata,slave_write&address_decode[0],internal_byteenable,user_dataout_0);
endmodule


module register_with_bytelanes(
	input clk,
	input reset,
	input [31:0] data_in,
	input write,
	input [3:0] byte_enables,
	output reg [31:0] data_out
);
	
	generate
	genvar LANE;
		for(LANE = 0; LANE < 4; LANE = LANE+1)
		begin: register_bytelane_generation	
			always @ (posedge clk or posedge reset) begin
				if(reset == 1) begin
					data_out[(LANE*8)+7:(LANE*8)] <= 0;
				end else begin
					if ((byte_enables[LANE] == 1) & (write == 1)) begin
						data_out[(LANE*8)+7:(LANE*8)] <= data_in[(LANE*8)+7:(LANE*8)];
					end
				end
			end
		end
	endgenerate
/*
    always @(posedge clk or posedge reset) begin
        if (reset == 1)
            data_out <= 32'b0;
        else if ((byte_enables[0] == 1) & (write == 1))
            data_out[7:0] <= data_in[7:0];
        else if ((byte_enables[1] == 1) & (write == 1))
            data_out[15:8] <= data_in[15:8];
        else if ((byte_enables[2] == 1) & (write == 1))
            data_out[23:16] <= data_in[23:16];
        else if ((byte_enables[3] == 1) & (write == 1))
            data_out[31:24] <= data_in[31:24];
    end*/
endmodule


