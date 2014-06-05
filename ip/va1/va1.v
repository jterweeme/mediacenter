module va1(
    input csi_clk,
    input csi_reset,
    
    input wire [1:0] avs_s1_address,
    input wire avs_s1_read,
    input wire avs_s1_write,
    output wire [15:0] avs_s1_readdata,
    input wire [15:0] avs_s1_writedata,
    input wire  [1:0] avs_s1_byteenable,
    output wire[13:0] coe_links,

    input wire [1:0] avs_s2_address,
    input wire avs_s2_read,
    input wire avs_s2_write,
    output wire  [15:0] avs_s2_readdata,
    input wire [15:0] avs_s2_writedata,
    input wire [1:0] avs_s2_byteenable,
    output wire [13:0] coe_rechts
);

    myduosegx links(
        .clk(csi_clk),
        .reset(csi_reset),
        .slave_address(avs_s1_address),
        .slave_read(avs_s1_read),
        .slave_write(avs_s1_write),
        .slave_readdata(avs_s1_readdata),
        .slave_writedata(avs_s1_writedata),
        .slave_byteenable(avs_s1_byteenable),
        .user_dataout_0(coe_links));

    myduosegx rechts(
        .clk(csi_clk),
        .reset(csi_reset),
        .slave_address(avs_s2_address),
        .slave_read(avs_s2_read),
        .slave_write(avs_s2_write),
        .slave_readdata(avs_s2_readdata),
        .slave_writedata(avs_s2_writedata),
        .slave_byteenable(avs_s2_byteenable),
        .user_dataout_0(coe_rechts));


endmodule

