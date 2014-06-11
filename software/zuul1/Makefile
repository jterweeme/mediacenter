# Alex Aalbertsberg
# Jasper ter Weeme

MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc
LDFLAGS := -T'../bsp//linker.x' $(MACHINE) -L ../bsp
DOWNLOAD := app_karaoke1.elf

%.o:
	nios2-elf-g++ $(INCLUDES) -O0 -g -c -o $@ $<

%.elf:
	nios2-elf-g++ $(LDFLAGS) -o $@ $^

%.flash:
	elf2flash --epcs --after=../../de2_115.flash --input=$< --output=$@

%.hex:
	nios2-elf-objcopy -I srec -O ihex $< $@

.PHONY: all

all: app_primes1.hex sdcardtest1.hex app_test1.hex app_wavplay1.hex \
    app_md5test1.elf kbtest2.hex vgatest1.hex \
    app_karaoke1.hex app_ofm1.hex mytest1.hex app_eepromtest1.hex \
    sdcardtest3.hex app_graph1.hex

app_graph1.hex: app_graph1.flash
audiotest1.hex: audiotest1.flash
app_eepromtest1.hex: app_eepromtest1.flash
app_jtagtest1.hex: app_jtagtest1.flash
app_karaoke1.hex: app_karaoke1.flash
kbtest1.hex: kbtest1.flash
kbtest2.hex: kbtest2.flash
app_md5test1.hex: app_md5test1.flash
mytest1.hex: mytest1.flash
app_ofm1.hex: app_ofm1.flash
piano1.hex: piano1.flash
app_primes1.hex: app_primes1.flash
#sdaudio2.hex: sdaudio2.flash
sdcardtest1.hex: sdcardtest1.flash
sdcardtest2.hex: sdcardtest2.flash
sdcardtest3.hex: sdcardtest3.flash
app_test1.hex: app_test1.flash
vgatest1.hex: vgatest1.flash
vgatest2.hex: vgatest2.flash
app_wavplay1.hex: app_wavplay1.flash

app_graph1.flash: app_graph1.elf
app_audiotest1.flash: app_audiotest1.elf
app_eepromtest1.flash: app_eepromtest1.elf
jtagtest1.flash: jtagtest1.elf
app_karaoke1.flash: app_karaoke1.elf
kbtest1.flash: kbtest1.elf
kbtest2.flash: kbtest2.elf
app_md5test1.flash: app_md5test1.elf
mytest1.flash: mytest1.elf
app_ofm1.flash: app_ofm1.elf
piano1.flash: piano1.elf
app_primes1.flash: app_primes1.elf
#sdaudio2.flash: sdaudio2.elf
sdcardtest1.flash: sdcardtest1.elf
sdcardtest2.flash: sdcardtest2.elf
sdcardtest3.flash: sdcardtest3.elf
app_test1.flash: app_test1.elf
vgatest1.flash: vgatest1.elf
vgatest2.flash: vgatest2.elf
app_wavplay1.flash: app_wavplay1.elf

app_graph1.elf: app_graph1.o
audiotest1.elf: audiotest1.o misc.o sdcard.o
app_eepromtest1.elf: app_eepromtest1.o misc.o sdcard.o
jtagtest1.elf: jtagtest1.o sdcard.o
app_karaoke1.elf: app_karaoke1.o misc.o sdcard.o mymidi.o
kbtest1.elf: kbtest1.o misc.o sdcard.o
kbtest2.elf: kbtest2.o misc.o sdcard.o
app_md5test1.elf: app_md5test1.o misc.o sdcard.o
mytest1.elf: mytest1.o misc.o sdcard.o
app_ofm1.elf: app_ofm1.o misc.o sdcard.o
piano1.elf: piano1.o misc.o sdcard.o
app_primes1.elf: app_primes1.o misc.o sdcard.o
#sdaudio2.elf: sdaudio2.o AUDIO.o LCD.o WaveLib.o misc.o LED.o debug.o sd_lib.o sd_hal.o sd_protocol.o crc7.o crc16.o
sdcardtest1.elf: sdcardtest1.o misc.o sdcard.o
sdcardtest2.elf: sdcardtest2.o misc.o sdcard.o
sdcardtest3.elf: sdcardtest3.o misc.o sdcard.o
app_test1.elf: app_test1.o misc.o sdcard.o
vgatest1.elf: vgatest1.o misc.o sdcard.o
vgatest2.elf: vgatest2.o vgascreen.o sdcard.o
app_wavplay1.elf: app_wavplay1.o misc.o sdcard.o

app_graph1.o: app_graph1.cpp
AUDIO.o: AUDIO.cpp AUDIO.h
audiotest1.o: audiotest1.cpp
crc16.o: crc16.cpp
crc7.o: crc7.cpp
debug.o: debug.cpp
app_eepromtest1.o: app_eepromtest1.cpp
FatFileSystem.o: FatFileSystem.cpp
FatInternal.o: FatInternal.cpp
jtagtest1.o: jtagtest1.cpp
app_karaoke1.o: app_karaoke1.cpp mymidi.h misc.h sdcard.h
kbtest1.o: kbtest1.cpp misc.h
kbtest2.o: kbtest2.cpp
LCD.o: LCD.cpp LCD.h
LED.o: LED.cpp
md5.o: md5.cpp
app_md5test1.o: app_md5test1.cpp
misc.o: misc.cpp misc.h
mymidi.o: mymidi.cpp mymidi.h misc.h
mytest1.o: mytest1.cpp misc.h
app_ofm1.o: app_ofm1.cpp
app_piano1.o: app_piano1.cpp
app_primes1.o: app_primes1.cpp
ps2.o: ps2.cpp
sdaudio2.o: sdaudio2.cpp misc.h LED.h AUDIO.h AUDIO_REG.h
sdcard.o: sdcard.cpp sdcard.h
sdcardtest1.o: sdcardtest1.cpp
sdcardtest2.o: sdcardtest2.cpp
sdcardtest3.o: sdcardtest3.cpp
#sd_hal.o: sd_hal.cpp
#sd_lib.o: sd_lib.cpp
#sd_protocol.o: sd_protocol.cpp
SEG7.o: SEG7.cpp
app_test1.o: app_test1.cpp misc.h
vgascreen.o: vgascreen.cpp
vgatest1.o: vgatest1.cpp
vgatest2.o: vgatest2.cpp
WaveLib.o: WaveLib.cpp
app_wavplay1.o: app_wavplay1.cpp misc.h

zuul1.png: zuul1.uxf
	umlet -action=convert -format=png -filename=$< -output=$@

download: $(DOWNLOAD)
	nios2-download -g $<

clean:
	rm -Rvf *.o *.elf *.flash *.hex *.png


