all: compile test

compile:
	g++ emulator.cpp -o emulator -I ./include -Wall -D DEBUG

test:
	./emulator -b std_bios.bin
