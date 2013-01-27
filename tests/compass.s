
	JSR		initMotors

	; turn left until we face "North"
:goNorth
	JSR		isNorth
	IFE		A, 1
	SET		PC, goWest
	JSR		turnLeft
	SET		PC, goNorth

:goWest
	JSR		stop
	JSR		isWest

:end
	WORD	0xeee0

:isNorth
	; north is defined as X >= 300 && y <= -300
	; unsigned comparisons though!
	SET		A, 0
	HWI		[nav_mod]
	IFG		300, A
	SET		PC, notDir
	SET		A, 1
	HWI		[nav_mod]
	IFG		A, -300
	SET		PC, notDir
	SET		PC, isDir

:isWest
	; west is defined as X >= 300 && -100 < y < 100
	SET		A, 0
	HWI		[nav_mod]
	IFG		300, A
	SET		PC, notDir
	SET		A, 1
	HWI		[nav_mod]
	IFG		-100, A		; A < -100
	SET		PC, notDir
	; check for A < 0
	IFG		A, 0x7FFF
	SET		PC, isDir
	IFG		A, 100		; A > 100
	SET		PC, notDir
	SET		PC, isDir

:isDir
	SET		A, 1
	SET		PC, POP

:notDir
	SET		A, 0
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

:goBackwards
	SET		PC, POP

:turnLeft
	; set right forward, left back
	SET		A, 2
	SET		B, 0x0100
	HWI		1
	SET		A, 3
	SET		B, 0x0080
	HWI		1
	; set power
	SET		A, 2
	SET		B, 0x0600
	HWI		1
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
