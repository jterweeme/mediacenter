MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc
NAAM = primes1

all: $(NAAM).hex

$(NAAM).elf: doemaar.altijd
	nios2-elf-gcc -g -c $(INCLUDES) test2.c
	nios2-elf-g++ -g -c $(INCLUDES) test1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) misc.cpp
	nios2-elf-g++ -g -c $(INCLUDES) primes1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) audiotest1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) jtagtest1.cpp
	nios2-elf-g++ -g -c $(INCLUDES) sdcardtest1.cpp
	nios2-elf-g++ -o test1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp test1.o misc.o
	nios2-elf-gcc -o test2.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp test2.o
	nios2-elf-g++ -o jtagtest1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp jtagtest1.o misc.o
	nios2-elf-g++ -o audiotest1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp audiotest1.o misc.o
	nios2-elf-g++ -o primes1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp primes1.o misc.o
	nios2-elf-g++ -o sdcardtest1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp sdcardtest1.o misc.o

$(NAAM).flash: $(NAAM).elf
	elf2flash --epcs --after=../../de2_115.flash --input=$(NAAM).elf --output=$@

$(NAAM).hex: $(NAAM).flash
	nios2-elf-objcopy -I srec -O ihex $< $@

doemaar.altijd:
	echo

zuul1.png: zuul1.uxf
	umlet -action=convert -format=png -filename=$< -output=$@

clean:
	rm -Rvf *.o *.elf *.hex *.flash *.png

download:
	nios2-download -g $(NAAM).elf

