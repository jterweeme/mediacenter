library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity display2 is
    port (MUXOUT: in std_logic_vector(3 downto 0);
          seg: out std_logic_vector(6 downto 0));
end display2;

architecture behavior of display2 is begin
    seg <= "1000000" when MUXOUT = x"0" else
           "1111001" when MUXOUT = x"1" else
           "0100100" when MUXOUT = x"2" else
           "0110000" when MUXOUT = x"3" else
           "0011001" when MUXOUT = x"4" else
           "0010010" when MUXOUT = x"5" else
           "0000010" when MUXOUT = x"6" else
           "1111000" when MUXOUT = x"7" else
           "0000000" when MUXOUT = x"8" else
           "0010000" when MUXOUT = x"9" else
           "0001000" when MUXOUT = x"a" else
           "0000011" when MUXOUT = x"b" else
           "1000110" when MUXOUT = x"c" else
           "0100001" when MUXOUT = x"d" else
           "0000110" when MUXOUT = x"e" else
           "0001110" when MUXOUT = x"f" else
           "1111111";

end behavior;

