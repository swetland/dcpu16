
all: dcpu a16

dcpu: dcpu.c
	gcc -Wall -o dcpu dcpu.c

a16: a16.c
	gcc -Wall -o a16 a16.c

clean:
	rm -f dcpu a16
