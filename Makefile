
all: dcpu a16

dcpu: emulator.c disassemble.c
	gcc -Wall -o dcpu emulator.c disassemble.c

a16: assembler.c disassemble.c
	gcc -Wall -o a16 assembler.c disassemble.c

clean:
	rm -f dcpu a16
