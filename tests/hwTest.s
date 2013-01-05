
; query number of HW devices
	SET		A, 0x30
	JSR		countHW
	SET		A, 0x06
	INT		0x3
	IAS		intHandler
	INT		0x3
	IAQ		1
	INT		0x3
	SET		C, 0
	IAQ		C
	MOV		A, 0x2
	INT		0x3
	SET		A, 1
	IAG		B
	SET		A, 1
	SET		B, 2
	SET		C, 3
	SET		X, 4
	SET		Y, 5
	SET		Z, 6
	SET		I, 7
	SET		J, 8
	HWQ		0
	SET		A, hello
	JSR		printString
:end
	WORD	0xeee0

:countHW
	HWN		A
	SET		PC, POP

:intHandler
	SET		A, 0x31
	RFI		0

:printString
	SET		PUSH, B

	SET		B, A
:nextChar
	SET		A, [B]
	IFE		A, 0
	SET		PC, loopDone
	ADD		B, 1
	HWI		0
	SET		PC, nextChar

:loopDone
	SET		B, POP
	SET		PC, POP

:hello
	DAT		"Hello World", 0
