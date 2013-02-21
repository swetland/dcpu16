
	; set
	JSR		initLED

	JSR		setLED
	JSR		delay5
	JSR		clearLED
	JSR		delay5
	JSR		setLED
	JSR		delay5
	JSR		clearLED
	JSR		delay5
	JSR		setLED
	JSR		delay5
	JSR		clearLED
	JSR		delay5
	JSR		setLED
	JSR		delay5
	JSR		clearLED
	JSR		delay5
	JSR		setLED
	JSR		delay5
	JSR		clearLED
	WORD    0xeee0

:delay5
	SET		A, 10
:delay
	IFE		A, 0
	SET		PC, POP
	SUB		A, 1
	set		PC, delay

:setLED
	SET		A, 2
	SET		B, 0x4000	; D14
	HWI		1
	set		PC, POP

:clearLED
	SET		A, 3
	SET		B, 0x4000	; D14
	HWI		1
	set		PC, POP

:initLED
	JSR		clearLED
	; output
	SET		A, 1
	SET		B, 0x4000	; D14
	HWI		1
	set		PC, POP
