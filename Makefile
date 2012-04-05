
all: dcpu a16

dcpu: dcpu.c disassemble.c
	gcc -Wall -o dcpu dcpu.c disassemble.c

a16: a16.c disassemble.c
	gcc -Wall -o a16 a16.c disassemble.c

clean:
	rm -f dcpu a16
