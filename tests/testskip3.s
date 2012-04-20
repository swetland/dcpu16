	SET A, 0x10

	IFN A, 0x10
		SUB A, 0
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 1
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 2
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 3
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 4
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 5
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 6
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 7
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 8
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 9
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 10
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 11
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 12
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 13
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 14
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 15
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 16
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 17
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 18
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 19
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 20
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 21
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 22
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 23
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 24
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 25
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 26
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 27
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 28
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 29
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 30
	IFN A, 0x10
		SET PC, fail

	IFN A, 0x10
		SUB A, 31
	IFN A, 0x10
		SET PC, fail

:pass
	WORD 0x3FF0

:fail
	WORD 0xEEE0
	
