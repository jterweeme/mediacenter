/*
Jasper ter Weeme
*/

module mytimer1(
    clk,
    reset_n,
    irq,
    s_cs_n,
    s_read,
    s_readdata,
    s_write,
    s_writedata
);

input           clk;
input           reset_n;
output  reg     irq;
input           s_cs_n;
input           s_read;
output  [31:0]  s_readdata;
input           s_write;
input   [31:0]  s_writedata;

reg [26:0] cnt;

always @ (posedge clk or negedge reset_n)
begin
    if (~reset_n) begin
        cnt <= 26'b0;
        irq <= 1'b0;
    end else begin
        cnt <= cnt + 1;
        
        if (cnt == 26'b1)
            irq <= 1'b1;
        else if (~s_cs_n & s_write)
            irq <= 1'b0;
    
    end    
end

endmodule


