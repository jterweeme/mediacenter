module toetsenbord1(
    input clk,
    input reset_n,
    output reg irq,
    input s_cs_n,
    input s_read,
    output [7:0] s_readdata,
    input s_write,
    input [7:0] s_writedata,
    input kc,
    input kd,
    output [6:0] sseg0,
    output [6:0] sseg1
);

wire [7:0] scancode_data;
reg [31:0] cnt;

assign s_readdata = scancode_data;

always @(posedge clk or negedge reset_n) begin
    if (~reset_n)
        irq <= 1'b0;
    else if (cnt == 26'b1)
        irq <= 1'b1;
    else if (~s_cs_n & s_write)
        irq <= 1'b0;
end

always @(posedge clk or negedge reset_n) begin
    if (~reset_n)
        cnt <= 4'b0;
    else
        cnt <= cnt + 1'b1;
end

display2 display_inst1(
    .MUXOUT(scancode_data[3:0]),
    .seg(sseg0)
);

display2 display_inst2(
    .MUXOUT(scancode_data[7:4]),
    .seg(sseg1)
);

scancode scancode_inst(
    .pclk(cnt[3]),
    .rst(reset_n),
    .s_readdata(scancode_data),
    .kc(kc),
    .kd(kd)
);

endmodule





