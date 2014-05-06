# Alex Aalbertsberg
# Jasper ter Weeme

all:
	cd bsp && ./create-this-bsp
	make -C zuul1
	#make -C lsdir1
	make -C primes1

clean:
	make -C zuul1 clean
	make -C primes1 clean
	make -C lsdir1 clean
	rm -Rvf bsp/Makefile bsp/*.c bsp/*.h bsp/*.html bsp/drivers bsp/HAL bsp/obj bsp/*.mk bsp/*.x bsp/*.a bsp/*.gdb

