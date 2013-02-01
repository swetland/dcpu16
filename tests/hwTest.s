
; query number of HW devices
	SET		A, 0x30
	JSR		countHW
	JSR		initMotors

; query X mag sensor
	SET		A, 0x00
	HWI		[nav_mod]
	SET		A, 0x01
	HWI		[nav_mod]
	SET		A, 0x02
	HWI		[nav_mod]

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
	HWQ		1
	HWQ		2
	JSR		goForwards
	SET		A, hello
	JSR		printString
	JSR		stop
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

; Motor Functions
; IO7:  Right Motor Direction
; IO8:  Left Motor Direction
; IO9:  Right Motor Power
; IO10: Left Motor Direction

:initMotors
; all forward and no power
	; clear power
	SET		A, 3
	SET		B, 0x0600
	HWI		1
	; set direction - forward
	SET		A, 2
	SET		B, 0x0180
	HWI		1
	; output all
	SET		A, 1
	SET		B, 0x0780
	HWI		1
	SET		PC, POP

:goForwards
	; set direction - forward
	SET		A, 2
	SET		B, 0x0180
	HWI		1
	; set power
	SET		A, 2
	SET		B, 0x0600
	HWI		1
	SET		PC, POP

; FIXME: Implement these... (dummy change to verify github authentication working again...)
:goBackwards
	SET		PC, POP

:turnLeft
	SET		PC, POP

:turnRight
	SET		PC, POP

:stop
	; clear power
	SET		A, 3
	SET		B, 0x0600
	HWI		1
	SET		PC, POP

:hello
	DAT		"Hello World\n", 0

:nav_mod
	DAT		2
