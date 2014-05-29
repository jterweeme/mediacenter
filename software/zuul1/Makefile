# Alex Aalbertsberg
# Jasper ter Weeme

MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc
LDFLAGS := -T'../bsp//linker.x' $(MACHINE) -L ../bsp
DOWNLOAD := ofm1.elf

%.o:
	nios2-elf-g++ $(INCLUDES) -O0 -g -c -o $@ $<

%.elf:
	nios2-elf-g++ $(LDFLAGS) -o $@ $^

%.flash:
	elf2flash --epcs --after=../../de2_115.flash --input=$< --output=$@

%.hex:
	nios2-elf-objcopy -I srec -O ihex $< $@

.PHONY: all

all: primes1.hex sdcardtest1.hex test1.hex wavplay1.hex \
    md5test1.elf kbtest2.hex vgatest1.hex \
    karaoke1.hex ofm1.hex mytest1.hex

audiotest1.hex: audiotest1.flash
eepromtest1.hex: eepromtest1.flash
jtagtest1.hex: jtagtest1.flash
karaoke1.hex: karaoke1.flash
kbtest1.hex: kbtest1.flash
kbtest2.hex: kbtest2.flash
md5test1.hex: md5test1.flash
mytest1.hex: mytest1.flash
ofm1.hex: ofm1.flash
piano1.hex: piano1.flash
primes1.hex: primes1.flash
#sdaudio2.hex: sdaudio2.flash
sdcardtest1.hex: sdcardtest1.flash
test1.hex: test1.flash
vgatest1.hex: vgatest1.flash
vgatest2.hex: vgatest2.flash
wavplay1.hex: wavplay1.flash

audiotest1.flash: audiotest1.elf
eepromtest1.flash: eepromtest1.elf
jtagtest1.flash: jtagtest1.elf
karaoke1.flash: karaoke1.elf
kbtest1.flash: kbtest1.elf
kbtest2.flash: kbtest2.elf
md5test1.flash: md5test1.elf
mytest1.flash: mytest1.elf
ofm1.flash: ofm1.elf
piano1.flash: piano1.elf
primes1.flash: primes1.elf
#sdaudio2.flash: sdaudio2.elf
sdcardtest1.flash: sdcardtest1.elf
test1.flash: test1.elf
vgatest1.flash: vgatest1.elf
vgatest2.flash: vgatest2.elf
wavplay1.flash: wavplay1.elf

audiotest1.elf: audiotest1.o misc.o sdcard.o
eepromtest1.elf: eepromtest1.o misc.o sdcard.o
jtagtest1.elf: jtagtest1.o sdcard.o
karaoke1.elf: karaoke1.o misc.o sdcard.o
kbtest1.elf: kbtest1.o misc.o sdcard.o
kbtest2.elf: kbtest2.o misc.o sdcard.o
md5test1.elf: md5test1.o misc.o sdcard.o
mytest1.elf: mytest1.o misc.o sdcard.o
ofm1.elf: ofm1.o misc.o sdcard.o
piano1.elf: piano1.o misc.o sdcard.o
primes1.elf: primes1.o misc.o sdcard.o
#sdaudio2.elf: sdaudio2.o AUDIO.o LCD.o WaveLib.o misc.o LED.o debug.o sd_lib.o sd_hal.o sd_protocol.o crc7.o crc16.o
sdcardtest1.elf: sdcardtest1.o misc.o sdcard.o
test1.elf: test1.o misc.o sdcard.o
vgatest1.elf: vgatest1.o misc.o sdcard.o
vgatest2.elf: vgatest2.o vgascreen.o sdcard.o
wavplay1.elf: wavplay1.o misc.o sdcard.o

AUDIO.o: AUDIO.cpp AUDIO.h
audiotest1.o: audiotest1.cpp
crc16.o: crc16.cpp
crc7.o: crc7.cpp
debug.o: debug.cpp
eepromtest1.o: eepromtest1.cpp
FatFileSystem.o: FatFileSystem.cpp
FatInternal.o: FatInternal.cpp
jtagtest1.o: jtagtest1.cpp
karaoke1.o: karaoke1.cpp
kbtest1.o: kbtest1.cpp
kbtest2.o: kbtest2.cpp
LCD.o: LCD.cpp LCD.h
LED.o: LED.cpp
md5.o: md5.cpp
md5test1.o: md5test1.cpp
misc.o: misc.cpp misc.h
mytest1.o: mytest1.cpp misc.h
ofm1.o: ofm1.cpp
piano1.o: piano1.cpp
primes1.o: primes1.cpp
ps2.o: ps2.cpp
sdaudio2.o: sdaudio2.cpp misc.h LED.h AUDIO.h AUDIO_REG.h
sdcard.o: sdcard.cpp sdcard.h
sdcardtest1.o: sdcardtest1.cpp
#sd_hal.o: sd_hal.cpp
#sd_lib.o: sd_lib.cpp
#sd_protocol.o: sd_protocol.cpp
SEG7.o: SEG7.cpp
test1.o: test1.cpp misc.h
vgascreen.o: vgascreen.cpp
vgatest1.o: vgatest1.cpp
vgatest2.o: vgatest2.cpp
WaveLib.o: WaveLib.cpp
wavplay1.o: wavplay1.cpp misc.h

zuul1.png: zuul1.uxf
	umlet -action=convert -format=png -filename=$< -output=$@

download: $(DOWNLOAD)
	nios2-download -g $<

clean:
	rm -Rvf *.o *.elf *.flash *.hex *.png


