module infrared1top(
    input clk,
    input reset_n,
    output reg irq,
    input s_cs_n,
    input s_read,
    output [31:0] s_readdata,
    input s_write,
    input [31:0] s_writedata,
    input ir);

    wire data_ready;
    reg pre_data_ready;

    always @(posedge clk or negedge reset_n)
        pre_data_ready <= ~reset_n ? 1'b0 : data_ready;

    always @ (posedge clk or negedge reset_n) begin
        if (~reset_n)
            irq <= 1'b0;
        else if (~pre_data_ready & data_ready)
            irq <= 1'b1;
        else if (~s_cs_n & s_write)
            irq <= 1'b0;
    end

    infrared1 infrared1_inst(
        .iCLK(clk),
        .iRST_n(reset_n),
        .iIRDA(ir),
        .iREAD(~s_cs_n & s_read),
        .oDATA_REAY(data_ready),
        .oDATA(s_readdata));

endmodule


