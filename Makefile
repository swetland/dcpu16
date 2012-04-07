
CFLAGS := -std=c99 -Wall -Wextra
CC     ?= gcc

all: dcpu a16

dcpu: emulator.c disassemble.c
	$(CC) $(CFLAGS) -o $@ emulator.c disassemble.c

a16: assembler.c disassemble.c
	$(CC) $(CFLAGS) -o $@ assembler.c disassemble.c

clean:
	rm -f dcpu a16
