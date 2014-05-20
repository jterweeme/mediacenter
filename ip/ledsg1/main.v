/*
Jasper ter Weeme
Alex Aalbertsberg
*/

module ledsg1(
    input csi_clk,
    input csi_reset_n,
    input [3:0] avs_s1_address,
    input avs_s1_write,
    input [7:0] avs_s1_writedata,
    output reg [7:0] coe_ledsg
);

always @(posedge csi_clk or negedge csi_reset_n) begin
    if (~csi_reset_n)
        coe_ledsg <= 8'b0;
    else if (avs_s1_write)
        coe_ledsg[7:0] <= avs_s1_writedata[7:0];
end

endmodule



