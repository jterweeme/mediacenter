# Jasper ter Weeme
# Alex Aalbertsberg

all: de2_115.flash

de2_115.sof:
	quartus_sh --flow compile mediacenter -c de2_115

de2_115.flash: de2_115.sof
	sof2flash --epcs --input=$< --output=$@

de2_115.pof: de2_115.flash
	quartus_cpf --convert de2_115.cof

download:
	quartus_pgm de2_115_jtag.cdf

urjtag:
	jtag de2_115_urjtag.cmd

clean:
	rm -Rvf db incremental_db *.rpt *.summary *.smsg *.sof *.pof
	rm -Rvf *.sopcinfo *.pin *.svf *.done *.jam *.qws *.qdf *.jdi *.flash
	rm -Rvf *.ttf *.rbf *.jbc

