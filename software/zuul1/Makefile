MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc

CURRENT = test1

all: $(CURRENT).hex

$(CURRENT).elf: doemaar.altijd
	nios2-elf-g++ -c $(INCLUDES) test1.cpp
	nios2-elf-g++ -c $(INCLUDES) misc.cpp
	nios2-elf-g++ -c $(INCLUDES) primes1.cpp
	nios2-elf-g++ -c $(INCLUDES) sdcardtest1.cpp
	nios2-elf-g++ -o test1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp test1.o misc.o
	nios2-elf-g++ -o primes1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp primes1.o misc.o
	nios2-elf-g++ -o sdcardtest1.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp sdcardtest1.o misc.o

$(CURRENT).flash: $(CURRENT).elf
	elf2flash --epcs --after=../../de2_115.flash --input=test1.elf --output=test1.flash

$(CURRENT).hex: $(CURRENT).flash
	nios2-elf-objcopy -I srec -O ihex $(CURRENT).flash $(CURRENT).hex

doemaar.altijd:
	echo

clean:
	rm -Rvf *.o *.elf *.hex *.flash

download:
	nios2-download -g $(CURRENT).elf

