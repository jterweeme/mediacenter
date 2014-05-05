/*
Jasper ter Weeme
Alex Aalbertsberg
*/

module mysram(input clk,
              input reset_n,
              input s_chipselect_n,
              input [1:0] s_byteenable_n,
              input s_write_n,
              input s_read_n,
              input [19:0] s_address,
              input [15:0] s_writedata,
              output [15:0] s_readdata,
              inout [15:0] SRAM_DQ,
              output [19:0] SRAM_ADDR,
              output SRAM_UB_n,
              output SRAM_LB_n,
              output SRAM_WE_n,
              output SRAM_CE_n,
              output SRAM_OE_n);
              
assign SRAM_DQ = SRAM_WE_n ? 'hz : s_writedata;
assign s_readdata = SRAM_DQ;
assign SRAM_ADDR = s_address;
assign SRAM_WE_n = s_write_n;
assign SRAM_OE_n = s_read_n;
assign SRAM_CE_n = s_chipselect_n;
assign {SRAM_UB_n, SRAM_LB_n} = s_byteenable_n;

endmodule

