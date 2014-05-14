-- Jasper ter Weeme

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity toetsenbord1 is
    port (clk, rst, kd, kc: in std_logic;
        WaitReg: out std_logic_vector(7 downto 0));
end toetsenbord;

architecture behavior of toetsenbord1 is
begin
end behavior;


