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

/* A DCPU-16 Assembler */

/* DCPU-16 Spec is Copyright 2012 Mojang */
/* http://0x10c.com/doc/dcpu-16.txt */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define countof(a) (sizeof(a) / sizeof((a)[0]))

extern u16 *disassemble(u16 *pc, char *out);

static u16 image[65536] = { 0, };
static u8 note[65536] = { 0, };
static u16 PC = 0;
static FILE *fin;
static const char *filename = "";
static int linenumber = 0;

static char linebuffer[128] = { 0, };
static char *lineptr = linebuffer;
static int token;
static char tstring[128];
static u16 tnumber;

enum outformat {
	OUTFORMAT_PRETTY,
	OUTFORMAT_HEX,
	OUTFORMAT_BINARY,
};

void die(const char *fmt, ...) {
	va_list ap;
	fprintf(stderr,"%s:%d: ", filename, linenumber);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr,"\n");
	if (linebuffer[0])
		fprintf(stderr,"%s:%d: >> %s <<\n", filename, linenumber, linebuffer);
	exit(1);
}

struct fixup {
	struct fixup *next;
	struct label *label;
	u16 pc;
};

struct label {
	struct label *next;
	u16 pc;
	u16 defined;
	char name[1];
};

struct label *labels = 0;
struct fixup *fixups = 0;

struct label *mklabel(const char *name, u16 pc, u16 def) {
	struct label *l;
	for (l = labels; l; l = l->next) {
		if (!strcasecmp(name, l->name)) {
			if (def) {
				if (l->defined)
					die("cannot redefine label: %s", name);
				l->defined = def;
				l->pc = pc;
			}
			return l;
		}
	}
	l = malloc(sizeof(*l) + strlen(name));
	l->defined = def;
	l->pc = pc;
	strcpy(l->name, name);
	l->next = labels;
	labels = l;
	return l;
}

void use_label(const char *name, u16 pc) {
	struct label *l = mklabel(name, 0, 0);
	if (l->defined) {
		image[pc] = l->pc;
	} else {
		struct fixup *f = malloc(sizeof(*f));
		f->next = fixups;
		f->pc = pc;
		f->label = l;
		fixups = f;
	}	
}

void set_label(const char *name, u16 pc) {
	mklabel(name, pc, 1);
}

void resolve_fixups(void) {
	struct fixup *f;
	for (f = fixups; f; f = f->next) {
		if (f->label->defined) {
			image[f->pc] = f->label->pc;
		} else {
			die("undefined reference to '%s' at 0x%04x", f->label->name, f->pc);
		}
	}
}

enum tokens {
	tA, tB, tC, tX, tY, tZ, tI, tJ,
	tSET, tADD, tSUB, tMUL, tDIV, tMOD, tSHL,
	tSHR, tAND, tBOR, tXOR, tIFE, tIFN, tIFG, tIFB,
	tJSR,
	tPOP, tPEEK, tPUSH, tSP, tPC, tO,
	tJMP, tMOV,
	tDATA, tDAT, tDW, tWORD,
	tCOMMA, tOBRACK, tCBRACK, tCOLON, tPLUS,
	tSTRING, tQSTRING, tNUMBER, tEOF,
};
static const char *tnames[] = {
	"A", "B", "C", "X", "Y", "Z", "I", "J",
	"SET", "ADD", "SUB", "MUL", "DIV", "MOD", "SHL",
	"SHR", "AND", "BOR", "XOR", "IFE", "IFN", "IFG", "IFB",
	"JSR",
	"POP", "PEEK", "PUSH", "SP", "PC", "O",
	"JMP", "MOV",
	"DATA", "DAT", "DW", "WORD",
	",", "[", "]", ":", "+",
	"<STRING>", "<QUOTED-STRING>", "<NUMBER>", "<EOF>",
};
#define LASTKEYWORD	tWORD

int _next(void) {
	char c, *x;
nextline:
	if (!*lineptr) {
		if (feof(fin)) return tEOF;
		if (fgets(linebuffer, 128, fin) == 0) return tEOF;
		lineptr = linebuffer;
		linenumber++;
	}
	while (*lineptr <= ' ') {
		if (*lineptr == 0) goto nextline;
		lineptr++;
	}
	switch ((c = *lineptr++)) {
	case ',': return tCOMMA;
	case '+': return tPLUS;
	case '[': return tOBRACK;
	case ']': return tCBRACK;
	case ':': return tCOLON;
	case '/': case ';': case '#': *lineptr = 0; goto nextline;
	case '"':
		x = tstring;
		for (;;) {
			switch((c = *lineptr++)) {
			case 0:
				die("unterminated string");
			case '"':
				*x = 0;
				return tQSTRING;
			case '\\':
				switch((c = *lineptr++)) {
				case 'n': *x++ = '\n'; break;
				case 't': *x++ = '\t'; break;
				case 'r': *x++ = '\r'; break;
				default:
					*x++ = c; break;
				}
				break;
			default:
				*x++ = c;
			}
		}
	default:
		if (isdigit(c) || ((c == '-') && isdigit(*lineptr))) {
			tnumber = strtoul(lineptr-1, &lineptr, 0);
			return tNUMBER;
		}
		if (isalpha(c) || c == '_') {
			int n;
			x = tstring;
			lineptr--;
			while (isalnum(*lineptr) || *lineptr == '_')
				*x++ = tolower(*lineptr++);
			*x = 0;
			for (n = 0; n <= LASTKEYWORD; n++)
				if (!strcasecmp(tnames[n], tstring))
					return n;
			return tSTRING;
		}
		if ((c == '\'') && (*(lineptr+1) == '\'')) {
			tnumber = (u16) (*lineptr++);
			lineptr++;
			return tNUMBER;
		}
		die("illegal character '%c'", c);
		return tEOF;
	}
}

int next(void) {
	token = _next();

	//fprintf(stderr,"%3d %s\n", token, tnames[token]);
	return token;
}

void expect(int t) {
	if (next() != t)
		die("expecting %s, found %s", tnames[t], tnames[token]);
}

void assemble_imm_or_label(void) {
	do {
		next();
		if (token == tNUMBER) {
			note[PC] = 'd';
			image[PC++] = tnumber;
		} else if (token == tSTRING) {
			note[PC] = 'd';
			image[PC] = 0;
			use_label(tstring, PC++);
		} else if (token == tQSTRING) {
			char *x = tstring;
			while (*x) {
				note[PC] = 'd';
				image[PC++] = *x++;
			}
		} else {
			die("expected number or label");
		}
		next();
	} while (token == tCOMMA);
}

int assemble_operand(void) {
	u16 n;
	next();
	switch (token) {
	case tA: case tB: case tC: case tX:
	case tY: case tZ: case tI: case tJ:
		return token & 7;
	case tPOP: return 0x18;
	case tPEEK: return 0x19;
	case tPUSH: return 0x1a;
	case tSP: return 0x1b;
	case tPC: return 0x1c;
	case tO: return 0x1d;
	case tNUMBER:
		if (tnumber < 0x20)
			return tnumber + 0x20;
		image[PC++] = tnumber;
		return 0x1f;
	case tSTRING:
		image[PC] = 0;
		use_label(tstring, PC++);
		return 0x1f;
	default:
		if (token != tOBRACK)
			die("expected [");
	}
	next();
	switch (token) {
	case tA: case tB: case tC: case tX:
	case tY: case tZ: case tI: case tJ:
		n = token & 7;
		next();
		if (token == tCBRACK)
			return 0x08 | n;
		if ((token != tCOMMA) && (token != tPLUS))
			die("expected , or +");
		next();
		if (token == tSTRING) {
			use_label(tstring, PC);
			image[PC++] = 0;
		} else if (token == tNUMBER) {
			image[PC++] = tnumber;
		} else {
			die("expected immediate value");
		}
		expect(tCBRACK);
		return 0x10 | n;
	case tSTRING:
		use_label(tstring, PC);
	case tNUMBER:
		image[PC++] = tnumber;
		next();
		if (token == tCBRACK) {
			return 0x1e;
		} else if ((token == tCOMMA) || (token == tPLUS)) {
			next();
			if ((token >= tA) && (token <= tJ)) {
				n = 0x10 | (token & 7);
			} else {
				die("invalid register");
			}
			expect(tCBRACK);
			return n;
		} else {
			die("invalid operand");
		}
	default:
		die("invalid operand");
	}
	return 0;
}

void assemble_binop(void) {
	u16 pc = PC++;
	int a, b;
	int op = token;

	/* alias for push x, pop x */
	if (token == tPUSH) {
		op = tSET;
		a = 0x1a; // push
		b = assemble_operand();
	} else if (token == tPOP) {
		op = tSET;
		a = assemble_operand();
		b = 0x18; // pop
	} else {
		a = assemble_operand();
		expect(tCOMMA);
		b = assemble_operand();
	}

	/* token to opcode */
	op -= (tSET - 1);
	image[pc] = op | (a << 4) | (b << 10);
}

void assemble_jump(void) {
	u16 pc = PC++;
	image[pc] = 0x01c1 | (assemble_operand() << 10);
}

void assemble(const char *fn) {
	u16 pc, n;
	fin = fopen(fn, "r");
	filename = fn;
	linenumber = 0;
	if (!fin) die("cannot read file");

	for (;;) {
		next();
again:
		switch (token) {
		case tEOF:
			goto done;
		case tSTRING:
			expect(tCOLON);
			set_label(tstring, PC);
			continue;
		case tCOLON:
			expect(tSTRING);
			set_label(tstring, PC);
			continue;
		case tWORD: case tDAT: case tDATA: case tDW:
			assemble_imm_or_label();
			goto again;
		case tJMP: // alias for SET PC, ...
			assemble_jump();
			continue;
		case tMOV: // alias for SET
			token = tSET;
		case tSET: case tADD: case tSUB: case tMUL:
		case tDIV: case tMOD: case tSHL: case tSHR:
		case tAND: case tBOR: case tXOR: case tIFE:
		case tIFN: case tIFG: case tIFB:
		case tPUSH: case tPOP:
			assemble_binop();
			continue;
		case tJSR:
			pc = PC++;
			n = assemble_operand();
			image[pc] = (n << 10) | 0x0010;
			continue;
		default:
			die("unexpected: %s", tnames[token]);
		}
	}
done:
	fclose(fin);
}

void emit(const char *fn, enum outformat format) {
	FILE *fp;
	u16 *pc = image;
	u16 *end = image + PC;
	u16 *dis = pc;
	filename = fn;
	linenumber = 0;

	if (!strcmp(fn, "-")) {
		fp = stdout;
	} else {
		fp = fopen(fn, "w");
	}
	if (!fp) die("cannot write file");

	while (pc < end) {
		if (format == OUTFORMAT_PRETTY) {
			if (note[pc-image] == 'd') {
				fprintf(fp, "%04x\n", *pc);
				dis = pc + 1;
			} else if (pc == dis) {
				char out[128];
				dis = disassemble(pc, out);
				fprintf(fp, "%04x\t%04x:\t%s\n", *pc, (unsigned)(pc-image), out);
			} else {
				fprintf(fp, "%04x\n", *pc);
			}
		} else if (format == OUTFORMAT_HEX) {
			fprintf(fp, "%04x\n", *pc);
		} else if (format == OUTFORMAT_BINARY) {
			/* XXX handle host endian */
			fwrite(pc, sizeof(*pc), 1, fp);
		}
		pc++;
	}
	if (fp != stdout)
		fclose(fp);
}

static void usage(int argc, char **argv)
{
	fprintf(stderr, "usage: %s [-o output] [-O output_format] <input file(s)>\n", argv[0]);
	fprintf(stderr, "\toutput_format can be one of: pretty, hex, binary\n");
}

int main(int argc, char **argv) {
	const char *outfn = "out.hex";
	enum outformat oformat = OUTFORMAT_PRETTY;

	for (;;) {
		int c;
		int option_index = 0;

		static struct option long_options[] = {
			{"help", 0, 0, 'h'},
			{"output", 1, 0, 'o'},
			{"outformat", 1, 0, 'O'},
			{0, 0, 0, 0},
		};

		c = getopt_long(argc, argv, "ho:O:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 'h':
				usage(argc, argv);
				return 0;
			case 'o':
				outfn = optarg;
				break;
			case 'O':
				if (!strcasecmp(optarg, "binary")) {
					oformat = OUTFORMAT_BINARY;
				} else if (!strcasecmp(optarg, "hex")) {
					oformat = OUTFORMAT_HEX;
				} else if (!strcasecmp(optarg, "pretty")) {
					oformat = OUTFORMAT_PRETTY;
				} else {
					usage(argc, argv);
					return 1;
				}
				break;
			default:
				usage(argc, argv);
				return 1;
		}
	}

	if (argc - optind < 1) {
		usage(argc, argv);
		return 1;
	}

	argc -= optind;
	argv += optind;

	while (argc >= 1) {
		assemble(argv[0]);
		argv++;
		argc--;
	}

	if (PC != 0) {
		linebuffer[0] = 0;
		resolve_fixups();
		emit(outfn, oformat);
	}
	return 0;
}

