# Jasper ter Weeme
# Alex Aalbertsberg

all:
	quartus_sh --flow compile de2_115 -c de2_115

download:
	quartus_pgm de2_115_jtag.cdf
	

