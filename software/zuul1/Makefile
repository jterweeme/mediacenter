MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc

all: main.hex

main.elf: doemaar.altijd
	nios2-elf-g++ -c $(INCLUDES) main.cpp
	nios2-elf-g++ -c $(INCLUDES) misc.cpp
	nios2-elf-g++ -c $(INCLUDES) primes1.cpp
	nios2-elf-g++ -o main.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp main.o misc.o
	nios2-elf-g++ -o primes1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp primes1.o misc.o

main.flash: main.elf
	elf2flash --epcs --after=../../de2_115.flash --input=main.elf --output=main.flash

main.hex: main.flash
	nios2-elf-objcopy -I srec -O ihex main.flash main.hex

doemaar.altijd:
	echo

clean:
	rm -Rvf *.o *.elf *.hex *.flash

download:
	nios2-download -g main.elf

