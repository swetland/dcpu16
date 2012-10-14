/*
 * Copyright (c) 2012, Brian Swetland
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright 
 *   notice, this list of conditions and the following disclaimer in the 
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* A DCPU-16 Implementation */

/* DCPU-16 Spec is Copyright 2012 Mojang */
/* http://0x10c.com/doc/dcpu-16.txt */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "emulator.h"

extern u16 *disassemble(u16 *pc, char *out);

static u16 lit[0x20] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

u16 *dcpu_opr(struct dcpu *d, u16 code) {
	switch (code) {
	case 0x00: case 0x01: case 0x02: case 0x03:
	case 0x04: case 0x05: case 0x06: case 0x07:
		return d->r + code;
	case 0x08: case 0x09: case 0x0a: case 0x0b:
	case 0x0c: case 0x0d: case 0x0e: case 0x0f:
		return d->m + d->r[code & 7];
	case 0x10: case 0x11: case 0x12: case 0x13:
	case 0x14: case 0x15: case 0x16: case 0x17:
		return d->m + ((d->r[code & 7] + d->m[d->pc++]) & 0xffff);
	case 0x18:
		return d->m + d->sp++;
	case 0x19:
		return d->m + d->sp;
	case 0x1a:
		return d->m + (--(d->sp));
	case 0x1b:
		return &d->sp;
	case 0x1c:
		return &d->pc;
	case 0x1d:
		return &d->ex;
	case 0x1e:
		return d->m + d->m[d->pc++];
	case 0x1f:
		return d->m + d->pc++;
	default:
		return lit + (code & 0x1F) - 1;
	}
}

static u8 skiptable[32] = { /* operand forms that advance pc */
	[0x10] = 1, [0x11] = 1, [0x12] = 1, [0x13] = 1,
	[0x14] = 1, [0x15] = 1, [0x16] = 1, [0x17] = 1,
	[0x1E] = 1, [0x1F] = 1,
};

void dcpu_skip(struct dcpu *d) {
	u16 op = d->m[d->pc++];
	d->pc += skiptable[op >> 10];
	if ((op & 15) == 0)
		d->pc += skiptable[(op >> 4) & 31];	
}

void dcpu_step(struct dcpu *d) {
	u16 op = d->m[d->pc++];
	u16 dst;
	u32 res;
	u16 a, b, *aa;

	if ((op & 0xF) == 0) goto extended;

	aa = dcpu_opr(d, dst = (op >> 4) & 0x3F);
	a = *aa;
	b = *dcpu_opr(d, op >> 10);

	switch (op & 0xF) {
	case 0x1: res = b; break;
	case 0x2: res = a + b; d->ex = res >> 16; break;	
	case 0x3: res = a - b; d->ex = res >> 16; break;
	case 0x4: res = a * b; d->ex = res >> 16; break;
	case 0x5: if (b) { res = a / b; } else { res = 0; } d->ex = res >> 16; break;
	case 0x6: if (b) { res = a % b; } else { res = 0; } break;
	case 0x7: res = a << b; d->ex = res >> 16; break;
	case 0x8: res = a >> b; d->ex = res >> 16; break;
	case 0x9: res = a & b; break;
	case 0xA: res = a | b; break;
	case 0xB: res = a ^ b; break;
	case 0xC: if (a!=b) dcpu_skip(d); return;
	case 0xD: if (a==b) dcpu_skip(d); return;
	case 0xE: if (a<=b) dcpu_skip(d); return;
	case 0xF: if ((a&b)==0) dcpu_skip(d); return;
	}

	if (dst < 0x1f) *aa = res;
	return;

extended:
	a = *dcpu_opr(d, op >> 10);
	aa = dcpu_opr(d, op >> 10);
	switch ((op >> 5) & 0x1F) {
	case 0x01:
		d->m[--(d->sp)] = d->pc;
		d->pc = a;
		return;
	case 0x08:		// TODO: Realize interrupt work
		return;
	case 0x0b:
		return;
	case 0x0c:
		return;
	case 0x09:
		*aa = d->ia;
		return;
	case 0x0a:
		d->ia = a;
		return;
	case 0x10:		// TODO: There is no devices at current time
		*aa = 0;
		return;
	case 0x11:		// HWQ
		memset(d->r,0x00,10);	// A,B,C,X,Y are set to zero
		return;
	case 0x12:		// HWI
		return;
	default:
		fprintf(stderr, "< ILLEGAL OPCODE >\n");
		exit(0);
	}
}
