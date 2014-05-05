MACHINE := -msys-crt0='../bsp//obj/HAL/src/crt0.o' -msys-lib=hal_bsp
INCLUDES := -I../bsp -I../bsp/drivers/inc -I../bsp/HAL/inc
DEFS := -DSYSTEM_BUS_WIDTH=32

all:
	nios2-elf-g++ -c $(INCLUDES) $(DEFS) main.cpp
	nios2-elf-g++ -c $(INCLUDES) $(DEFS) sd_hal.cpp
	nios2-elf-g++ -o main.elf -T'../bsp//linker.x' $(MACHINE) -L ../bsp main.o sd_hal.o
	#elf2flash --epcs --after=../../de2_115.flash --input=main.elf --output=main.flash
	#nios2-elf-objcopy -I srec -O ihex main.flash main.hex

clean:
	rm -Rvf *.o *.elf *.hex

download:
	nios2-download -g main.elf

