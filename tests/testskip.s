; Skip test.  Assumes skipping a 3-word instruction may be faulty, but 
; skipping a 2-word instruction works.

	SET A, 0x10			; c001
        SET [0x1000], A			; 01e1, 1000
        SET [0xEEE0], 0			; 81e1, eee0
        IFN A, 0x10			; c00d
		SET [0x1000], [0xEEE0]	; 79e1, 1000, eee0
        SET A, [0x1000]			; 7801, 1000
        ; A should still be 0x10

        IFN A, 0x10			; c00d
		SET PC, fail		; 7dc1, 000f

:pass
	WORD 0x3FF0			; 3ff0

:fail
	WORD 0xEEE0			; eee0
		  

