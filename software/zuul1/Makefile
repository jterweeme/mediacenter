# Alex Aalbertsberg
# Jasper ter Weeme

MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc
LIBRARIES := -L ../bsp
NAAM = ofm1
LINKY := -T'../bsp//linker.x' $(MACHINE) $(LIBRARIES)

all: clean primes1.elf vgatest1.elf vgatest1.hex audiotest1.elf audiotest1.hex test1.hex primes1.hex

%.elf:
	nios2-elf-g++ -g -c $(INCLUDES) test1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) misc.cpp
	nios2-elf-g++ -g -c $(INCLUDES) primes1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) audiotest1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) jtagtest1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) sdcardtest1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) eepromtest1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) vgatest1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) ofm1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) AUDIO.cpp
	nios2-elf-g++ -g -c $(INCLUDES) I2C.cpp
	nios2-elf-g++ -g -c $(INCLUDES) main.cpp
	nios2-elf-g++ -g -c $(INCLUDES) LCD.cpp
	nios2-elf-g++ -g -c $(INCLUDES) debug.cpp
	nios2-elf-g++ -g -c $(INCLUDES) WaveLib.cpp
	nios2-elf-g++ -g -c $(INCLUDES) FatInternal.cpp
	nios2-elf-g++ -g -c $(INCLUDES) FatFileSystem.cpp
	nios2-elf-g++ -g -c $(INCLUDES) sd_lib.cpp
	nios2-elf-g++ -g -c $(INCLUDES) sd_hal.cpp
	nios2-elf-g++ -g -c $(INCLUDES) sd_protocol.cpp
	nios2-elf-g++ -g -c $(INCLUDES) crc7.cpp
	nios2-elf-g++ -g -c $(INCLUDES) crc16.cpp
	nios2-elf-g++ -o test1.elf -T'../bsp//linker.x' $(MACHINE) $(LIBRARIES) test1.o misc.o
	nios2-elf-g++ -o jtagtest1.elf -T'../bsp//linker.x' $(MACHINE) $(LIBRARIES) jtagtest1.o misc.o
	nios2-elf-g++ -o audiotest1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp audiotest1.o misc.o
	nios2-elf-g++ -o primes1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp primes1.o misc.o
	nios2-elf-g++ -o sdcardtest1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp sdcardtest1.o misc.o
	nios2-elf-g++ -o eepromtest1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp eepromtest1.o misc.o
	nios2-elf-g++ -o vgatest1.elf $(LINKY) vgatest1.o misc.o
	nios2-elf-g++ -o sdaudio1.elf $(LINKY) main.o AUDIO.o I2C.o LCD.o debug.o WaveLib.o FatInternal.o FatFileSystem.o sd_lib.o sd_protocol.o sd_hal.o crc7.o crc16.o
	nios2-elf-g++ -o ofm1.elf $(LINKY) ofm1.o misc.o

%.flash: %.elf
	elf2flash --epcs --after=../../de2_115.flash --input=$< --output=$@

%.hex: %.flash
	nios2-elf-objcopy -I srec -O ihex $< $@

doemaar.altijd:
	echo

zuul1.png: zuul1.uxf
	umlet -action=convert -format=png -filename=$< -output=$@

clean:
	rm -Rvf *.o *.elf *.hex *.flash *.png

download:
	nios2-download -g $(NAAM).elf

