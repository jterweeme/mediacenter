module vgacore1top(
    input csi_clk,
    input csi_reset,
    
);

alpha_blender ab1(
    .clk(csi_clk),
    .reset(csi_reset),
);

endmodule


