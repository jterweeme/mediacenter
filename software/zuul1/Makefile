# Jasper ter Weeme

MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc
LDFLAGS := -T'../bsp//linker.x' $(MACHINE) -L ../bsp
DOWNLOAD := app_karaoke1.elf

%.o:
	nios2-elf-g++ $(INCLUDES) -O2 -Wall -g -c -o $@ $<

%.elf:
	nios2-elf-g++ $(LDFLAGS) -o $@ $^

%.flash:
	elf2flash --epcs --after=../../de2_115.flash --input=$< --output=$@

%.hex:
	nios2-elf-objcopy -I srec -O ihex $< $@

.PHONY: all

all: app_primes1.hex app_sdcardtest1.hex app_test1.elf app_wavplay1.hex \
    app_md5test1.hex app_kbtest2.hex app_vgatest1.hex \
    app_karaoke1.hex app_ofm1.elf app_mytest1.elf app_eepromtest1.elf \
    app_sdcardtest3.hex app_graph1.elf app_fourier1.elf \
	app_wavplay2.elf app_wavmix1.elf app_jtagtest1.elf app_zuul1.elf

app_graph1.hex: app_graph1.flash
app_audiotest1.hex: app_audiotest1.flash
app_eepromtest1.hex: app_eepromtest1.flash
app_fourier1.hex: app_fourier1.flash
app_jtagtest1.hex: app_jtagtest1.flash
app_karaoke1.hex: app_karaoke1.flash
app_kbtest1.hex: app_kbtest1.flash
app_kbtest2.hex: app_kbtest2.flash
app_md5test1.hex: app_md5test1.flash
app_mytest1.hex: app_mytest1.flash
app_ofm1.hex: app_ofm1.flash
app_piano1.hex: app_piano1.flash
app_primes1.hex: app_primes1.flash
app_sdcardtest1.hex: app_sdcardtest1.flash
app_sdcardtest2.hex: app_sdcardtest2.flash
app_sdcardtest3.hex: app_sdcardtest3.flash
app_test1.hex: app_test1.flash
app_vgatest1.hex: app_vgatest1.flash
app_vgatest2.hex: app_vgatest2.flash
app_wavmix1.hex: app_wavmix1.flash
app_wavplay1.hex: app_wavplay1.flash
app_wavplay2.hex: app_wavplay2.flash

app_audiotest1.flash: app_audiotest1.elf
app_eepromtest1.flash: app_eepromtest1.elf
app_fourier1.flash: app_fourier1.elf
app_graph1.flash: app_graph1.elf
app_jtagtest1.flash: app_jtagtest1.elf
app_karaoke1.flash: app_karaoke1.elf
app_kbtest1.flash: app_kbtest1.elf
app_kbtest2.flash: app_kbtest2.elf
app_md5test1.flash: app_md5test1.elf
app_mytest1.flash: app_mytest1.elf
app_ofm1.flash: app_ofm1.elf
app_piano1.flash: app_piano1.elf
app_primes1.flash: app_primes1.elf
app_sdcardtest1.flash: app_sdcardtest1.elf
app_sdcardtest2.flash: app_sdcardtest2.elf
app_sdcardtest3.flash: app_sdcardtest3.elf
app_test1.flash: app_test1.elf
app_vgatest1.flash: app_vgatest1.elf
app_vgatest2.flash: app_vgatest2.elf
app_wavmix1.flash: app_wavmix1.elf
app_wavplay1.flash: app_wavplay1.elf
app_wavplay2.flash: app_wavplay2.elf

app_graph1.elf: app_graph1.o
app_audiotest1.elf: app_audiotest1.o misc.o sdcard.o
app_eepromtest1.elf: app_eepromtest1.o misc.o sdcard.o
app_fourier1.elf: app_fourier1.o misc.o
app_jtagtest1.elf: app_jtagtest1.o sdcard.o misc.o
app_karaoke1.elf: app_karaoke1.o misc.o sdcard.o mymidi.o
app_kbtest1.elf: app_kbtest1.o misc.o sdcard.o
app_kbtest2.elf: app_kbtest2.o misc.o sdcard.o
app_md5test1.elf: app_md5test1.o misc.o sdcard.o
app_mytest1.elf: app_mytest1.o misc.o sdcard.o
app_ofm1.elf: app_ofm1.o misc.o sdcard.o
app_piano1.elf: app_piano1.o misc.o sdcard.o
app_primes1.elf: app_primes1.o misc.o sdcard.o
app_sdcardtest1.elf: app_sdcardtest1.o misc.o sdcard.o
app_sdcardtest2.elf: app_sdcardtest2.o misc.o sdcard.o
app_sdcardtest3.elf: app_sdcardtest3.o misc.o sdcard.o
app_test1.elf: app_test1.o misc.o sdcard.o
app_vgatest1.elf: app_vgatest1.o misc.o sdcard.o
app_vgatest2.elf: app_vgatest2.o vgascreen.o sdcard.o
app_wavmix1.elf: app_wavmix1.o misc.o sdcard.o
app_wavplay1.elf: app_wavplay1.o misc.o sdcard.o
app_wavplay2.elf: app_wavplay2.o misc.o sdcard.o
app_zuul1.elf: app_zuul1.o misc.o

app_audiotest1.o: app_audiotest1.cpp
app_graph1.o: app_graph1.cpp
app_fourier1.o: app_fourier1.cpp misc.h
app_jtagtest1.o: app_jtagtest1.cpp
app_eepromtest1.o: app_eepromtest1.cpp
AUDIO.o: AUDIO.cpp AUDIO.h
FatFileSystem.o: FatFileSystem.cpp
FatInternal.o: FatInternal.cpp
app_karaoke1.o: app_karaoke1.cpp mymidi.h misc.h sdcard.h
app_kbtest1.o: app_kbtest1.cpp misc.h
app_kbtest2.o: app_kbtest2.cpp
md5.o: md5.cpp
app_md5test1.o: app_md5test1.cpp
misc.o: misc.cpp misc.h
mymidi.o: mymidi.cpp mymidi.h misc.h
ps2.o: ps2.cpp
sdaudio2.o: sdaudio2.cpp misc.h LED.h AUDIO.h AUDIO_REG.h
sdcard.o: sdcard.cpp sdcard.h
SEG7.o: SEG7.cpp
app_mytest1.o: app_mytest1.cpp misc.h
app_ofm1.o: app_ofm1.cpp
app_piano1.o: app_piano1.cpp
app_primes1.o: app_primes1.cpp
app_sdcardtest1.o: app_sdcardtest1.cpp
app_sdcardtest2.o: app_sdcardtest2.cpp
app_sdcardtest3.o: app_sdcardtest3.cpp
app_test1.o: app_test1.cpp misc.h mystl.h
app_vgatest1.o: app_vgatest1.cpp
app_vgatest2.o: app_vgatest2.cpp
app_wavmix1.o: app_wavmix1.cpp misc.h sdcard.h
app_wavplay1.o: app_wavplay1.cpp misc.h sdcard.h
app_wavplay2.o: app_wavplay2.cpp misc.h sdcard.h
app_zuul1.o: app_zuul1.cpp misc.h
vgascreen.o: vgascreen.cpp
WaveLib.o: WaveLib.cpp


zuul1.png: zuul1.uxf
	umlet -action=convert -format=png -filename=$< -output=$@

download: $(DOWNLOAD)
	nios2-download -g $<

clean:
	rm -Rvf *.o *.elf *.flash *.hex *.png


