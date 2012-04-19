
CFLAGS := -Wall -g

all: dcpu a16

DCPU_OBJS := dcpu.o emulator.o disassemble.o
dcpu: $(DCPU_OBJS)
	$(CC) -o dcpu $(DCPU_OBJS)

A16_OBJS := assembler.o disassemble.o
a16: $(A16_OBJS)
	$(CC) -o a16 $(A16_OBJS)

dcpu.c: emulator.h
emulator.c: emulator.h

clean:
	rm -f dcpu a16 *.o
