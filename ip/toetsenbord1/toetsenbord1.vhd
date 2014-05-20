-- Jasper ter Weeme

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity scancode is
    port (pclk, rst, kd, kc: in std_logic;
        s_readdata: out std_logic_vector(7 downto 0));
end scancode;

architecture behavior of scancode is
    signal kdi, kci, dff1, dff2: std_logic;
    signal shiftRegSig1: std_logic_vector(10 downto 0);
    signal shiftRegSig2: std_logic_vector(10 downto 1);
begin
    process (pclk, rst, kc, kd) begin
        if rst = '0' then
            dff1 <= '0';
            dff2 <= '0';
            kdi <= '0';
            kci <= '0';
        elsif rising_edge(pclk) then
            dff1 <= kd;
            kdi <= dff1;
            dff2 <= kc;
            kci <= dff2;
        end if;
    end process;

    process (kdi, kci, rst) begin
        if rst = '0' then
            ShiftRegSig1 <= (others => '0');
            ShiftRegSig2 <= (others => '0');
        elsif falling_edge(pclk) then
            ShiftRegSig1(10 downto 0) <= KDI & ShiftRegSig1(10 downto 1);
            ShiftRegSig2(10 downto 1) <= ShiftRegSig1(0) & ShiftRegSig2(10 downto 2);
        end if;
    end process;

    process (ShiftRegSig1, ShiftRegSig2, rst, kci) begin
        if rst = '0' then
            s_readdata <= "10000000";
        elsif rising_edge(pclk) and ShiftRegSig2(8 downto 1) = "11110000" then
            s_readdata <= ShiftRegSig1(8 downto 1);
        end if;
    end process;
    
end behavior;


