module toetsenbord1(
    input csi_clk,
    input csi_reset_n,
    output reg ins_irq0_irq,
    //input avs_s1_cs_n,
    input avs_s1_read,
    output reg [7:0] avs_s1_readdata,
    //input avs_s1_write,
    //input [7:0] avs_s1_writedata,
    input coe_kc,
    input coe_kd,
    output [6:0] coe_sseg0,
    output [6:0] coe_sseg1
);

reg [7:0] scancode_data;
reg [31:0] cnt;


always @(posedge csi_clk or negedge csi_reset_n) begin
    if (~csi_reset_n)
        ins_irq0_irq <= 1'b0;
    else if (cnt == 32'b1)
        ins_irq0_irq <= 1'b1;
    //else if (~avs_s1_cs_n & avs_s1_write)
        //ins_irq0_irq <= 1'b0;
end

always @(posedge csi_clk or negedge csi_reset_n) begin
    if (~csi_reset_n)
        avs_s1_readdata = 8'b0;
    else if (avs_s1_read)
        avs_s1_readdata[7:0] <= scancode_data[7:0];
end

always @(posedge csi_clk or negedge csi_reset_n) begin
    if (~csi_reset_n)
        cnt <= 31'b0;
    else
        cnt <= cnt + 1'b1;
end

display2 display_inst1(
    .MUXOUT(scancode_data[3:0]),
    .seg(coe_sseg1)
);

display2 display_inst2(
    .MUXOUT(scancode_data[7:4]),
    .seg(coe_sseg0)
);

scancode scancode_inst(
    .pclk(cnt[3]),
    .rst(csi_reset_n),
    .s_readdata(scancode_data),
    .kc(coe_kc),
    .kd(coe_kd)
);

endmodule





