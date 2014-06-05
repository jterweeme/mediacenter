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
    output wire [13:0] coe_rechts,

    output wire avs_s3_irq,
    input wire avs_s3_cs_n,
    input wire avs_s3_read,
    output wire [31:0] avs_s3_readdata,
    input wire avs_s3_write,
    input wire [31:0] avs_s3_writedata,
    input wire coe_ir
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

    infrared1top ir1(
        .clk(csi_clk),
        .reset_n(~csi_reset),
        .irq(avs_s3_irq),
        .s_cs_n(avs_s3_cs_n),
        .s_read(avs_s3_read),
        .s_readdata(avs_s3_readdata),
        .s_write(avs_s3_write),
        .s_writedata(avs_s3_writedata),
        .ir(coe_ir));
endmodule

