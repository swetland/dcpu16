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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "emulator.h"

extern u16 *disassemble(u16 *pc, char *out);

void dumpheader(void) {
	fprintf(stderr,
		"PC   SP   EX   IA   A    B    C    X    Y    Z    I    J    Instruction\n"
		"---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- -----------\n");
}

void dumpstate(struct dcpu *d) {
	char out[128];
	disassemble(d->m + d->pc, out);
	fprintf(stderr,
		"%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %s\n",
		d->pc, d->sp, d->ex, d->ia,
		d->r[0], d->r[1], d->r[2], d->r[3],
		d->r[4], d->r[5], d->r[6], d->r[7],
		out);
}

void load(struct dcpu *d, const char *fn) {
	FILE *fp;
	char buf[128];
	u16 n = 0;
	if (!(fp = fopen(fn, "r"))) {
		fprintf(stderr, "cannot open: %s\n", fn);
		exit(1);
	}	
	while (!feof(fp) && fgets(buf, 128, fp)) {
		if (!isalnum(buf[0]))
			continue;
		d->m[n++] = strtoul(buf, 0, 16);
	}
	fprintf(stderr,"< LOADED %d WORDS >\n", n);
	fclose(fp);
}
	
int main(int argc, char **argv) {
	struct dcpu d;
	memset(&d, 0, sizeof(d));

	load(&d, argc > 1 ? argv[1] : "out.hex");

	dumpheader();
	for (;;) {
		dumpstate(&d);
		dcpu_step(&d);
	}		
	return 0;
}

