; DCPU-16 Example from Specification
; Copyright 2012 Mojang

; Try some basic stuff
	SET A, 0x30		; 7c01 0030
	SET [0x1000], 0x20	; 7de1 1000 0020
	SUB A, [0x1000]		; 7803 1000
	IFN A, 0x10		; c00d 
		SET PC, crash	; 7dc1 001a [*]

; Do a loopy thing
	SET I, 10		; a861
	SET A, 0x2000		; 7c01 2000
:loop
	SET [0x2000+I], [A]	; 2161 2000
	SUB I, 1		; 8463
	IFN I, 0		; 806d
		SET PC, loop	; 7dc1 000d [*]

; Call a subroutine
	SET X, 0x4		; 9031
	JSR testsub		; 7c10 0018 [*]
	SET PC, crash		; 7dc1 001a [*]

:testsub
	SHL X, 4		; 9037
	SET PC, POP		; 61c1

:crash
	WORD 0xeee0
