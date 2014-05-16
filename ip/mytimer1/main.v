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

wire data_ready;
reg pre_data_ready;
always @ (posedge clk or negedge reset_n)
begin
    if (~reset_n)
        pre_data_ready <= 1'b0;
    else
        pre_data_ready <= data_ready;
end

always @ (posedge clk or negedge reset_n)
begin
    if (~reset_n)
        irq <= 1'b0;
    else if (~pre_data_ready & data_ready)
        irq <= 1'b1;
    else if (~s_cs_n & s_write)
        irq <= 1'b0;
end


endmodule


