module mysegdisp(
    input clk,
    input reset_n,
    input [1:0] address,
    input [31:0] writedata,
    input write_n,
    input cs,
    output reg [7:0] most_sig,
    output reg [7:0] least_sig
);

always @(posedge clk or negedge reset_n) begin
    if (~reset) begin
        most_sig <= 8'b0;
        least_sig <= 8'b0;
    else
end

endmodule

