-- Jasper ter Weeme

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity mytimer2 is
    port (clk, reset_n, s_cs_n, s_read, s_write: in std_logic;
            s_readdata: out std_logic_vector(31 downto 0);
            s_writedata: in std_logic_vector(31 downto 0);
            irq: out std_logic);
end mytimer2;

architecture behavior of mytimer2 is
    signal cnt: unsigned(26 downto 0);
begin
    process (clk, reset_n) begin
        if reset_n = '0' then
            cnt <= (others => '0');
            irq <= '0';
        else
            cnt <= cnt + 1;

            if cnt = "111111111111111111111111111" then
                irq <= '1';
            elsif s_cs_n = '0' and s_write = '1' then
                irq <= '0';
            end if;
        end if;
    end process;
end behavior;


