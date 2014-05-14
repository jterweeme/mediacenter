# Alex Aalbertsberg
# Jasper ter Weeme

MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc
LDFLAGS := -T'../bsp//linker.x' $(MACHINE) -L ../bsp

%.o:
	nios2-elf-g++ $(INCLUDES) -O2 -g -c -o $@ $<

%.elf:
	nios2-elf-g++ $(LDFLAGS) -o $@ $^

%.flash:
	elf2flash --epcs --after=../../de2_115.flash --input=$< --output=$@

%.hex:
	nios2-elf-objcopy -I srec -O ihex $< $@

.PHONY: all

all: audiotest1.hex eepromtest1.hex jtagtest1.hex ofm1.hex piano1.hex primes1.hex sdaudio2.hex sdcardtest1.hex test1.hex

audiotest1.hex: audiotest1.flash
eepromtest1.hex: eepromtest1.flash
jtagtest1.hex: jtagtest1.flash
ofm1.hex: ofm1.flash
piano1.hex: piano1.flash
primes1.hex: primes1.flash
sdaudio2.hex: sdaudio2.flash
sdcardtest1.hex: sdcardtest1.flash
test1.hex: test1.flash

audiotest1.flash: audiotest1.elf
eepromtest1.flash: eepromtest1.elf
jtagtest1.flash: jtagtest1.elf
ofm1.flash: ofm1.elf
piano1.flash: piano1.elf
primes1.flash: primes1.elf
sdaudio2.flash: sdaudio2.elf
sdcardtest1.flash: sdcardtest1.elf
test1.flash: test1.elf

audiotest1.elf: audiotest1.o misc.o
eepromtest1.elf: eepromtest1.o misc.o
jtagtest1.elf: jtagtest1.o
ofm1.elf: ofm1.o misc.o
piano1.elf: piano1.o
primes1.elf: primes1.o misc.o
sdaudio2.elf: sdaudio2.o AUDIO.o LCD.o WaveLib.o misc.o LED.o debug.o sd_lib.o sd_hal.o sd_protocol.o crc7.o crc16.o
sdcardtest1.elf: sdcardtest1.o misc.o
test1.elf: test1.o misc.o

AUDIO.o: AUDIO.cpp AUDIO.h
audiotest1.o: audiotest1.cpp
crc16.o: crc16.cpp
crc7.o: crc7.cpp
debug.o: debug.cpp
eepromtest1.o: eepromtest1.cpp
FatFileSystem.o: FatFileSystem.cpp
FatInternal.o: FatInternal.cpp
jtagtest1.o: jtagtest1.cpp
LCD.o: LCD.cpp LCD.h
LED.o: LED.cpp
misc.o: misc.cpp misc.h
ofm1.o: ofm1.cpp
piano1.o: piano1.cpp
primes1.o: primes1.cpp
sdaudio2.o: sdaudio2.cpp misc.h LED.h AUDIO.h AUDIO_REG.h
sdcardtest1.o: sdcardtest1.cpp
sd_hal.o: sd_hal.cpp
sd_lib.o: sd_lib.cpp
sd_protocol.o: sd_protocol.cpp
SEG7.o: SEG7.cpp
test1.o: test1.cpp misc.h
vgatest1.o: vgatest1.cpp
WaveLib.o: WaveLib.cpp

download: sdaudio2.elf
	nios2-download -g sdaudio2.elf

clean:
	rm -Rvf *.o *.elf *.flash *.hex


