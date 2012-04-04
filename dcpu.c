/* Copyright 2012, Brian Swetland.  BSD Licensed.  Share and Enjoy! */
/* A DCPU-16 Implementation */

/* DCPU-16 Spec is Copyright 2012 Mojang */
/* http://0x10c.com/doc/dcpu-16.txt */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

typedef uint16_t u16;
typedef uint32_t u32;

struct dcpu {
	u16 r[8];
	u16 pc;
	u16 sp;
	u16 ov;
	u16 skip;
	u16 m[65536];
};

static u16 lit[0x20] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
};

enum opcode {
    RESERVED = 0x0,
    SET = 0x1,
    ADD = 0x2,
    SUB = 0x3,
    MUL = 0x4,
    DIV = 0x5,
    MOD = 0x6,
    SHL = 0x7,
    SHR = 0x8,
    AND = 0x9,
    BOR = 0xa,
    XOR = 0xb,
    IFE = 0xc,
    IFN = 0xd,
    IFG = 0xe,
    IFB = 0xf
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
		return &d->ov;
	case 0x1e:
		return d->m + d->m[d->pc++];
	case 0x1f:
		return d->m + d->pc++;
	default:
		return lit + (code & 0x1F);
	}
}

void dcpu_step(struct dcpu *d) {
	enum opcode op = d->m[d->pc++];
	u16 dst;
	u32 res;
	u16 a, b, *aa;

	if ((op & 0xF) == 0) {
		switch ((op >> 4) & 0x3F) {
		case 0x01:
			a = *dcpu_opr(d, op >> 10);
			if (d->skip) {
				d->skip = 0;
			} else {
				d->m[--(d->sp)] = d->pc;
				d->pc = a;
			}
			return;
		default:
			fprintf(stderr, "< ILLEGAL OPCODE >\n");
			exit(0);
		}
	}

	aa = dcpu_opr(d, dst = (op >> 4) & 0x3F);
	a = *aa;
	b = *dcpu_opr(d, op >> 10);

	switch (op & 0xF) {
	case SET: res = b;
		break;
	case ADD: res = a + b;
		break;	
	case SUB: res = a - b;
		break;
	case MUL: res = a * b;
		break;
	case DIV: if (b) { res = a / b; } else { res = 0; }
		break;
	case MOD: if (b) { res = a % b; } else { res = 0; }
		break;
	case SHL: res = a << b;
		break;
	case SHR: res = a >> b;
		break;
	case AND: res = a & b;
		break;
	case BOR: res = a | b;
		break;
	case XOR: res = a ^ b;
		break;
	case IFE: res = (a==b);
		goto cond;
	case IFN: res = (a!=b);
		goto cond;
	case IFG: res = (a>b);
		goto cond;
	case IFB: res = ((a&b)!=0);
		goto cond;
	}

	if (d->skip) {
		d->skip = 0;
		return;
	}
	if (dst < 0x1f) {
		*aa = res;
		d->ov = res >> 16;
	}
	return;

cond:
	if (d->skip) {
		d->skip = 0;
		return;
	} 
	d->skip = !res;
}

void dumpheader(void) {
	fprintf(stderr,
		"PC   SP   OV   SKIP A    B    C    X    Y    Z    I    J\n"
		"---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----\n");
}

void dumpstate(struct dcpu *d) {
	fprintf(stderr,
		"%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\n",
		d->pc, d->sp, d->ov, d->skip,
		d->r[0], d->r[1], d->r[2], d->r[3], d->r[4], d->r[5], d->r[6], d->r[7]);
}

void load(struct dcpu *d, FILE *fp) {
	char buf[128];
	u16 n = 0;
	while (!feof(fp) && fgets(buf, 128, fp)) {
		if (!isalnum(buf[0]))
			continue;
		d->m[n++] = strtoul(buf, 0, 16);
	}
	fprintf(stderr,"< LOADED %d WORDS >\n", n);
}
	
int main(int argc, char **argv) {
	struct dcpu d;

	memset(&d, 0, sizeof(d));
	d.sp = 0xffff;

	load(&d, stdin);

	dumpheader();
	for (;;) {
		dumpstate(&d);
		dcpu_step(&d);
	}		
	return 0;
}

