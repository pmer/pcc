/*	$Id: table.c,v 1.150 2018/11/21 18:20:31 ragge Exp $	*/
/*
 * Copyright (c) 2003 Anders Magnusson (ragge@ludd.luth.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


# include "pass2.h"

# define TLL TLONGLONG|TULONGLONG
# define ANYSIGNED TINT|TLONG|TSHORT|TCHAR
# define ANYUSIGNED TUNSIGNED|TULONG|TUSHORT|TUCHAR
# define ANYFIXED ANYSIGNED|ANYUSIGNED
# define TUWORD TUNSIGNED|TULONG
# define TSWORD TINT|TLONG
# define TWORD TUWORD|TSWORD
#define	TAREG	TINT|TSHORT|TCHAR|TUNSIGNED|TUSHORT|TUCHAR
#define	 SHINT	SAREG	/* short and int */
#define	 ININT	INAREG
#ifndef NOBREGS
#define	 SHCH	SBREG	/* shape for char */
#define	 INCH	INBREG
#endif
#define	 SHLL	SCREG	/* shape for long long */
#define	 INLL	INCREG
#define	 SHFL	SDREG	/* shape for float/double */
#define	 INFL	INDREG	/* shape for float/double */

#define	XSL(c)	NEEDS(NREG(c, 1), NSL(c))

struct optab table[] = {
/* First entry must be an empty entry */
{ -1, FOREFF, SANY, TANY, SANY, TANY, 0, 0, "", },

/* PCONVs are usually not necessary */
{ PCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RLEFT,
		"", },

/*
 * A bunch conversions of integral<->integral types
 * There are lots of them, first in table conversions to itself
 * and then conversions from each type to the others.
 */

/* itself to itself, including pointers */

#ifdef NOBREGS
/* convert (u)char to (u)char. */
{ SCONV,	INAREG,
	SAREG,	TCHAR|TUCHAR,
	SAREG,	TCHAR|TUCHAR,
		0,	RLEFT,
		"", },
#else
/* convert (u)char to (u)char. */
{ SCONV,	INCH,
	SHCH,	TCHAR|TUCHAR,
	SHCH,	TCHAR|TUCHAR,
		0,	RLEFT,
		"", },
#endif

/* convert pointers to int. */
{ SCONV,	ININT,
	SHINT,	TPOINT|TWORD,
	SANY,	TWORD,
		0,	RLEFT,
		"", },

/* convert (u)longlong to (u)longlong. */
{ SCONV,	INLL,
	SHLL,	TLL,
	SHLL,	TLL,
		0,	RLEFT,
		"", },

/* convert between float/double/long double. */
{ SCONV,	INFL,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		0,	RLEFT,
		"ZI", },

/* convert pointers to pointers. */
{ SCONV,	ININT,
	SHINT,	TPOINT,
	SANY,	TPOINT,
		0,	RLEFT,
		"", },

/* char to something */

#ifdef NOBREGS
/* convert char to (unsigned) short. */
/* Done when reg is loaded */
{ SCONV,	INAREG,
	SAREG,	TCHAR|TUCHAR,
	SAREG,	TSHORT|TUSHORT,
		0,	RLEFT,
		"", },

/* convert char to (unsigned) short. */
{ SCONV,	INAREG,
	SOREG|SNAME,	TCHAR,
	SAREG,	TSHORT|TUSHORT,
		XSL(A),	RESC1,
		"	movsbl AL,A1\n", },

/* convert unsigned char to (u)short. */
{ SCONV,	INAREG,
	SOREG|SNAME,	TUCHAR,
	SAREG,	TSHORT|TUSHORT,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movzbl AL,A1\n", },
#else
/* convert char to (unsigned) short. */
{ SCONV,	ININT,
	SBREG|SOREG|SNAME,	TCHAR,
	SAREG,	TSHORT|TUSHORT,
		XSL(A),	RESC1,
		"	movsbw AL,A1\n", },

/* convert unsigned char to (u)short. */
{ SCONV,	ININT,
	SHCH|SOREG|SNAME,	TUCHAR,
	SAREG,	TSHORT|TUSHORT,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movzbw AL,A1\n", },
#endif

/* convert signed char to int (or pointer). */
#ifdef NOBREGS
{ SCONV,	ININT,
	SAREG,	TCHAR|TUCHAR,
	SAREG,	TWORD|TPOINT,
		0,	RLEFT,
		"", },

{ SCONV,	ININT,
	SOREG|SNAME,	TCHAR,
	SAREG,	TWORD|TPOINT,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movsbl AL,A1\n", },

/* convert unsigned char to (u)int. */
{ SCONV,	ININT,
	SOREG|SNAME,	TUCHAR,
	SAREG,	TWORD,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movzbl AL,A1\n", },

/* convert char to (u)long long */
{ SCONV,	INLL,
	SAREG,	TCHAR,
	SANY,	TLL,
		NEEDS(NREG(C, 1), NSL(C),
		    NRES(EAXEDX), NLEFT(EAX)),	RESC1,
		"	cltd\n", },

/* convert unsigned char (in mem) to (u)long long */
{ SCONV,	INLL,
	SOREG|SNAME,	TUCHAR,
	SANY,	TLL,
		NEEDS(NREG(C, 1), NSL(C)),	RESC1,
		"	movzbl AL,A1\n	xorl U1,U1\n", },

/* convert unsigned char to (u)long long */
{ SCONV,	INLL,
	SAREG,	TUCHAR,
	SANY,	TLL,
		NEEDS(NREG(C, 1), NSL(C)),	RESC1,
		"	movl AL,A1\n	xorl U1,U1\n", },

/* convert (u)char (in register) to double */
{ SCONV,	INFL,
	SAREG,	TCHAR|TUCHAR,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1), NTEMP(1)),	RESC1,
		"	movl AL,A2\n 	fildl A2\n", },
#else
{ SCONV,	ININT,
	SHCH|SOREG|SNAME,	TCHAR,
	SAREG,	TWORD|TPOINT,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movsbl AL,A1\n", },

/* convert unsigned char to (u)int. */
{ SCONV,	ININT,
	SHCH|SOREG|SNAME,	TUCHAR,
	SAREG,	TWORD,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movzbl AL,A1\n", },

/* convert char to (u)long long */
{ SCONV,	INLL,
	SHCH|SOREG|SNAME,	TCHAR,
	SANY,	TLL,
		NEEDS(NREG(C, 1), NSL(C), NRES(EAXEDX), 
		    NEVER(EAX), NEVER(EDX)),	RESC1,
		"	movsbl AL,%eax\n	cltd\n", },

/* convert unsigned char to (u)long long */
{ SCONV,	INLL,
	SHCH|SOREG|SNAME,	TUCHAR,
	SANY,			TLL,
		NEEDS(NREG(C, 1), NSL(C)),	RESC1,
		"	movzbl AL,A1\n	xorl U1,U1\n", },

/* convert char (in register) to double XXX - use NTEMP */
{ SCONV,	INFL,
	SHCH|SOREG|SNAME,	TCHAR,
	SHFL,			TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(A, 1), NSL(A), NREG(D, 1)),	RESC2,
		"	movsbl AL,A1\n	pushl A1\n"
		"	fildl (%esp)\n	addl $4,%esp\n", },

/* convert (u)char (in register) to double XXX - use NTEMP */
{ SCONV,	INFL,
	SHCH|SOREG|SNAME,	TUCHAR,
	SHFL,			TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(A, 1), NSL(A), NREG(D, 1)),	RESC2,
		"	movzbl AL,A1\n	pushl A1\n"
		"	fildl (%esp)\n	addl $4,%esp\n", },
#endif

/* short to something */

/* convert (u)short to (u)short. */
{ SCONV,	INAREG,
	SAREG,	TSHORT|TUSHORT,
	SAREG,	TSHORT|TUSHORT,
		0,	RLEFT,
		"", },

#ifdef NOBREGS
/* convert short (in memory) to char */
{ SCONV,	INAREG,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SAREG,		TCHAR|TUCHAR,
		XSL(A),		RESC1,
		"	movzbl AL,A1\n", },

/* convert short (in reg) to char. */
{ SCONV,	INAREG,
	SAREG,	TSHORT|TUSHORT,
	SAREG,	TCHAR|TUCHAR,
		NEEDS(NOLEFT(ESI), NOLEFT(EDI)),    RLEFT,
		"	movsbl AL,AL\n", },

/* convert short to (u)int. */
{ SCONV,	ININT,
	SOREG|SNAME,	TSHORT,
	SAREG,	TWORD,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movswl AL,A1\n", },

/* convert unsigned short to (u)int. */
{ SCONV,	ININT,
	SOREG|SNAME,	TUSHORT,
	SAREG,	TWORD,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movzwl AL,A1\n", },

/* convert unsigned short to (u)int (in reg) */
{ SCONV,	INAREG,
	SAREG,	TSHORT|TUSHORT,
	SAREG,	TWORD,
		0,	RLEFT,
		"", },
#else
/* convert short (in memory) to char */
{ SCONV,	INCH,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SHCH,		TCHAR|TUCHAR,
		NEEDS(NREG(B, 1), NSL(B)),	RESC1,
		"	movb AL,A1\n", },

/* convert short (in reg) to char. */
{ SCONV,	INCH,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NREG(B, 1), NSL(B),
		    NOLEFT(ESI), NOLEFT(EDI)),    RESC1,
		"ZM", },
/* convert short to (u)int. */
{ SCONV,	ININT,
	SAREG|SOREG|SNAME,	TSHORT,
	SAREG,	TWORD,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movswl AL,A1\n", },

/* convert unsigned short to (u)int. */
{ SCONV,	ININT,
	SAREG|SOREG|SNAME,	TUSHORT,
	SAREG,	TWORD,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movzwl AL,A1\n", },
#endif


/* convert short to (u)long long */
{ SCONV,	INLL,
	SAREG|SOREG|SNAME,	TSHORT,
	SHLL,			TLL,
		NEEDS(NREG(C, 1), NSL(C), NRES(EAXEDX), 
		    NEVER(EAX), NEVER(EDX)),	RESC1,
		"	movswl AL,%eax\n	cltd\n", },

/* convert unsigned short to (u)long long */
{ SCONV,	INLL,
	SAREG|SOREG|SNAME,	TUSHORT,
	SHLL,			TLL,
		NEEDS(NREG(C, 1), NSL(C)),	RESC1,
		"	movzwl AL,A1\n	xorl U1,U1\n", },

/* convert short (in memory) to float/double */
{ SCONV,	INFL,
	SOREG|SNAME,	TSHORT,
	SDREG,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1)),	RESC1,
		"	fild AL\n", },

/* convert short (in register) to float/double */
{ SCONV,	INFL,
	SAREG,	TSHORT,
	SDREG,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1), NTEMP(1)), RESC1,
		"	pushw AL\n	fild (%esp)\n	addl $2,%esp\n", },

/* convert unsigned short to double XXX - use NTEMP */
{ SCONV,	INFL,
	SAREG|SOREG|SNAME,	TUSHORT,
	SHFL,			TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(A, 1), NSL(A), 
		    NREG(D, 1), NTEMP(1)),	RESC2,
		"	movzwl AL,A1\n	movl A1,A3\n"
		"	fildl A3\n", },

/* int to something */

/* convert int to char. This is done when register is loaded */
#ifdef NOBREGS
{ SCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SAREG,	TCHAR|TUCHAR,
#if 0
		NEEDS(NOLEFT(ESI), NOLEFT(EDI)),	RLEFT,
		"	movsbl ZT,AL\n", },
#else
		0,	RLEFT,
		"", },
#endif
#else
{ SCONV,	INCH,
	SAREG,	TWORD|TPOINT,
	SANY,	TCHAR|TUCHAR,
		NEEDS(NREG(B, 1), NSL(B),
		    NOLEFT(ESI), NOLEFT(EDI)),	RESC1,
		"ZM", },
#endif

/* convert int to short. Nothing to do */
{ SCONV,	INAREG,
	SAREG,	TWORD|TPOINT,
	SANY,	TSHORT|TUSHORT,
		0,	RLEFT,
		"", },

/* convert signed int to (u)long long */
{ SCONV,	INLL,
	SHINT,	TSWORD,
	SHLL,	TLL,
		NEEDS(NREG(C, 1), NSL(C), NLEFT(EAX),
		    NEVER(EAX), NEVER(EDX), NRES(EAXEDX)),	RESC1,
		"	cltd\n", },

/* convert unsigned int to (u)long long */
{ SCONV,	INLL,
	SHINT|SOREG|SNAME,	TUWORD|TPOINT,
	SHLL,	TLL,
		NEEDS(NREG(C, 1), NSL(C)),	RESC1,
		"	movl AL,A1\n	xorl U1,U1\n", },

/* convert signed int (in memory) to double */
{ SCONV,	INFL,
	SOREG|SNAME,	TSWORD,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1)),	RESC1,
		"	fildl AL\n", },

/* convert signed int (in register) to double */
{ SCONV,	INFL,
	SAREG,	TSWORD,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1), NTEMP(1)),	RESC1,
		"	movl AL,A2\n	fildl A2\n", },

/* convert unsigned int (reg&mem) to double */
{ SCONV,       INFL,
	SOREG|SNAME|SAREG,	TUWORD,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1), NTEMP(1)),	RESC1,
		"	pushl $0\n"
		"	pushl AL\n"
		"	fildq (%esp)\n"
		"	addl $8,%esp\n", },

/* long long to something */

#ifdef NOBREGS
/* convert (u)long long to (u)char (mem->reg) */
{ SCONV,	INAREG,
	SOREG|SNAME,	TLL,
	SANY,	TUCHAR,
		XSL(A),	RESC1,
		"	movzbl AL,A1\n", },

{ SCONV,	INAREG,
	SOREG|SNAME,	TLL,
	SANY,	TCHAR,
		XSL(A),	RESC1,
		"	movsbl AL,A1\n", },

/* convert (u)long long to (u)char (reg->reg, hopefully nothing) */
{ SCONV,	INAREG,
	SHLL,	TLL,
	SANY,	TCHAR|TUCHAR,
		NEEDS(NREG(A, 1), NSL(A), NTEMP(1)),	RESC1,
		"ZS", },
#else
/* convert (u)long long to (u)char (mem->reg) */
{ SCONV,	INCH,
	SOREG|SNAME,	TLL,
	SANY,	TCHAR|TUCHAR,
		NEEDS(NREG(B, 1), NSL(B)),	RESC1,
		"	movb AL,A1\n", },

/* convert (u)long long to (u)char (reg->reg, hopefully nothing) */
{ SCONV,	INCH,
	SHLL,	TLL,
	SANY,	TCHAR|TUCHAR,
		NEEDS(NREG(B, 1), NSL(B), NTEMP(1)),	RESC1,
		"ZS", },
#endif

/* convert (u)long long to (u)short (mem->reg) */
{ SCONV,	INAREG,
	SOREG|SNAME,	TLL,
	SAREG,	TSHORT|TUSHORT,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,
		"	movw AL,A1\n", },

/* convert (u)long long to (u)short (reg->reg, hopefully nothing) */
{ SCONV,	INAREG,
	SHLL|SOREG|SNAME,	TLL,
	SAREG,	TSHORT|TUSHORT,
		NEEDS(NREG(A, 1), NSL(A), NTEMP(1)),	RESC1,
		"ZS", },

/* convert long long to int (mem->reg) */
{ SCONV,	INAREG,
	SOREG|SNAME,	TLL,
	SAREG,	TWORD|TPOINT,
		NEEDS(NREG(A, 1), NSL(A)),    RESC1,
		"	movl AL,A1\n", },

/* convert long long to int (reg->reg, hopefully nothing) */
{ SCONV,	INAREG,
	SHLL|SOREG|SNAME,	TLL,
	SAREG,	TWORD|TPOINT,
		NEEDS(NREG(A, 1), NSL(A), NTEMP(1)),	RESC1,
		"ZS", },

/* convert long long (in memory) to floating */
{ SCONV,	INFL,
	SOREG|SNAME,	TLONGLONG,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1)),	RESC1,
		"	fildq AL\n", },

/* convert long long (in register) to floating */
{ SCONV,	INFL,
	SHLL,	TLONGLONG,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1), NTEMP(1)),	RESC1,
		"	pushl UL\n	pushl AL\n"
		"	fildq (%esp)\n	addl $8,%esp\n", },

/* convert unsigned long long to floating */
{ SCONV,	INFL,
	SCREG,	TULONGLONG,
	SDREG,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NREG(D, 1), NTEMP(1)),	RESC1,
		"ZJ", },

/* float to something */

#if 0 /* go via int by adding an extra sconv in clocal() */
/* convert float/double to (u) char. XXX should use NTEMP here */
{ SCONV,	INCH,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHCH,	TWORD|TSHORT|TUSHORT|TCHAR|TUCHAR,
		NEEDS(NREG(B, 1)),	RESC1,
		"	subl $4,%esp\n	fistpl (%esp)\n	popl A1\n", },

/* convert float/double to (u) int/short/char. XXX should use NTEMP here */
{ SCONV,	INCH,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHCH,	TWORD|TSHORT|TUSHORT|TCHAR|TUCHAR,
		NEEDS(NREG(C, 1)),	RESC1,
		"	subl $4,%esp\n	fistpl (%esp)\n	popl A1\n", },
#endif

/* convert float/double to int. XXX should use NTEMP here */
{ SCONV,	INAREG,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SAREG,	TSWORD,
		NEEDS(NREG(A, 1), NTEMP(3)),	RESC1,
		"	fnstcw A2\n"
		"	fnstcw 4+A2\n"
		"	movb $12,1+A2\n"
		"	fldcw A2\n"
		"	fistpl 8+A2\n"
		"	movl 8+A2,A1\n"
		"	fldcw 4+A2\n", },

/* convert float/double to unsigned int. */
{ SCONV,       INAREG,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SAREG,	TUWORD,
		NEEDS(NREG(A, 1), NTEMP(4)),	RESC1,
		"	fnstcw A2\n"
		"	fnstcw 4+A2\n"
		"	movb $12,1+A2\n"
		"	fldcw A2\n"
		"	fistpq 8+A2\n"
		"	movl 8+A2,A1\n"
		"	fldcw 4+A2\n", },

/* convert float/double (in register) to long long */
{ SCONV,	INLL,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHLL,	TLONGLONG,
		NEEDS(NREG(C, 1), NTEMP(4)),	RESC1,
		"	fnstcw A2\n"
		"	fnstcw 4+A2\n"
		"	movb $12,1+A2\n"
		"	fldcw A2\n"
		"	fistpq 8+A2\n"
		"	movl 8+A2,A1\n"
		"	movl 12+A2,U1\n"
		"	fldcw 4+A2\n", },

/* convert float/double (in register) to unsigned long long */
{ SCONV,	INLL,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHLL,	TULONGLONG,
		NEEDS(NREG(C, 1), NTEMP(4)),	RESC1,
		"	fnstcw A2\n"
		"	fnstcw 4+A2\n"
		"	movb $7,1+A2\n"	/* 64-bit, round down  */
		"	fldcw A2\n"
		"	movl $0x5f000000, 8+A2\n"	/* (float)(1<<63) */
		"	fsubs 8+A2\n"	/* keep in range of fistpq */
		"	fistpq 8+A2\n"
		"	xorb $0x80,15+A2\n"	/* addq $1>>63 to 8(%esp) */
		"	movl 8+A2,A1\n"
		"	movl 12+A2,U1\n"
		"	fldcw 4+A2\n", },

/* slut sconv */

/*
 * Subroutine calls.
 */

{ UCALL,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	call CL\nZC", },

{ CALL,		FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	0,
		"	call CL\nZC", },

{ UCALL,	FOREFF,
	SCON,	TANY,
	SAREG,	TWORD|TPOINT,
		0,	0,
		"	call CL\nZC", },

{ CALL,	INAREG,
	SCON,	TANY,
	SAREG,	TSHORT|TUSHORT|TWORD|TPOINT,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ UCALL,	INAREG,
	SCON,	TANY,
	SAREG,	TSHORT|TUSHORT|TWORD|TPOINT,
		NEEDS(NREG(A, 1), NSL(A)),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ CALL,	INBREG,
	SCON,	TANY,
	SBREG,	TCHAR|TUCHAR,
		NEEDS(NREG(B, 1)),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ UCALL,	INBREG,
	SCON,	TANY,
	SBREG,	TCHAR|TUCHAR,
		NEEDS(NREG(B, 1)),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ CALL,		INCREG,
	SCON,	TANY,
	SCREG,	TANY,
		NEEDS(NREG(C, 1), NSL(C)),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ UCALL,	INCREG,
	SCON,	TANY,
	SCREG,	TANY,
		NEEDS(NREG(C, 1), NSL(C)),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ CALL,	INDREG,
	SCON,	TANY,
	SDREG,	TANY,
		NEEDS(NREG(D, 1), NSL(D)),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ UCALL,	INDREG,
	SCON,	TANY,
	SDREG,	TANY,
		NEEDS(NREG(D, 1), NSL(D)),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ CALL,		FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	0,
		"	call *AL\nZC", },

{ UCALL,	FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	0,
		"	call *AL\nZC", },

{ CALL,		INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		XSL(A),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

{ UCALL,	INAREG,
	SAREG,	TANY,
	SANY,	TANY,
		XSL(A),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

{ CALL,		INBREG,
	SAREG,	TANY,
	SANY,	TANY,
		XSL(B),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

{ UCALL,	INBREG,
	SAREG,	TANY,
	SANY,	TANY,
		XSL(B),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

{ CALL,		INCREG,
	SAREG,	TANY,
	SANY,	TANY,
		XSL(C),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

{ UCALL,	INCREG,
	SAREG,	TANY,
	SANY,	TANY,
		XSL(C),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

{ CALL,		INDREG,
	SAREG,	TANY,
	SANY,	TANY,
		XSL(D),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

{ UCALL,	INDREG,
	SAREG,	TANY,
	SANY,	TANY,
		XSL(D),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

{ STCALL,	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		XSL(A),	0,
		"	call CL\nZC", },

{ STCALL,	INAREG,
	SCON,	TANY,
	SANY,	TANY,
		XSL(A),	RESC1,	/* should be 0 */
		"	call CL\nZC", },

{ STCALL,	INAREG,
	SNAME|SAREG,	TANY,
	SANY,	TANY,
		XSL(A),	RESC1,	/* should be 0 */
		"	call *AL\nZC", },

/*
 * The next rules handle all binop-style operators.
 */
/* Special treatment for long long */
{ PLUS,		INLL|FOREFF,
	SHLL,		TLL,
	SHLL|SNAME|SOREG,	TLL,
		0,	RLEFT,
		"	addl AR,AL\n	adcl UR,UL\n", },

{ PLUS,		INLL|FOREFF,
	SHLL|SNAME|SOREG,	TLL,
	SHLL|SCON,		TLL,
		0,	RLEFT,
		"	addl AR,AL\n	adcl UR,UL\n", },

/* Special treatment for long long  XXX - fix commutative check */
{ PLUS,		INLL|FOREFF,
	SHLL|SNAME|SOREG,	TLL,
	SHLL,			TLL,
		0,	RRIGHT,
		"	addl AL,AR\n	adcl UL,UR\n", },

{ PLUS,		INFL,
	SHFL,		TDOUBLE,
	SNAME|SOREG,	TDOUBLE,
		0,	RLEFT,
		"	faddl AR\n", },

{ PLUS,		INFL|FOREFF,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		0,	RLEFT,
		"	faddp\n", },

#ifdef NOBREGS
{ PLUS,		INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TAREG|TPOINT,
	SONE,	TANY,
		0,	RLEFT,
		"	incl AL\n", },

{ PLUS,		FOREFF,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SONE,	TANY,
		0,	RLEFT,
		"	incw AL\n", },

{ PLUS,		FOREFF,
	SNAME|SOREG,	TCHAR|TUCHAR,
	SONE,	TANY,
		0,	RLEFT,
		"	incb AL\n", },
#else
{ PLUS,		INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
	SONE,	TANY,
		0,	RLEFT,
		"	incl AL\n", },

{ PLUS,		INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SONE,	TANY,
		0,	RLEFT,
		"	incw AL\n", },

{ PLUS,		INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SONE,	TANY,
		0,	RLEFT,
		"	incb AL\n", },
#endif

{ PLUS,		INAREG,
	SAREG,	TAREG,
	SAREG,	TAREG,
		NEEDS(NREG(A,1),NSL(A),NSR(A)),	RESC1,
		"	leal (AL,AR),A1\n", },

#ifdef NOBREGS
{ MINUS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TAREG|TPOINT,
	SONE,			TANY,
		0,	RLEFT,
		"	decl AL\n", },

{ MINUS,	FOREFF,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SONE,			TANY,
		0,	RLEFT,
		"	decw AL\n", },

{ MINUS,	FOREFF,
	SNAME|SOREG,	TCHAR|TUCHAR,
	SONE,	TANY,
		0,	RLEFT,
		"	decb AL\n", },
#else
{ MINUS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
	SONE,			TANY,
		0,	RLEFT,
		"	decl AL\n", },

{ MINUS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SONE,			TANY,
		0,	RLEFT,
		"	decw AL\n", },

{ MINUS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SONE,	TANY,
		0,	RLEFT,
		"	decb AL\n", },
#endif

/* address as register offset, negative */
{ MINUS,	INLL|FOREFF,
	SHLL,	TLL,
	SHLL|SNAME|SOREG,	TLL,
		0,	RLEFT,
		"	subl AR,AL\n	sbbl UR,UL\n", },

{ MINUS,	INLL|FOREFF,
	SHLL|SNAME|SOREG,	TLL,
	SHLL|SCON,	TLL,
		0,	RLEFT,
		"	subl AR,AL\n	sbbl UR,UL\n", },

{ MINUS,	INFL,
	SHFL,	TDOUBLE,
	SNAME|SOREG,	TDOUBLE,
		0,	RLEFT,
		"	fsubl AR\n", },

{ MINUS,	INFL|FOREFF,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		0,	RLEFT,
		"	fsubZAp\n", },

/* Simple r/m->reg ops */
/* m/r |= r */
{ OPSIMP,	INAREG|FOREFF|FORCC,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
	SAREG,			TWORD|TPOINT,
		0,	RLEFT|RESCC,
		"	Ol AR,AL\n", },

/* r |= r/m */
{ OPSIMP,	INAREG|FOREFF|FORCC,
	SAREG,			TWORD|TPOINT,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
		0,	RLEFT|RESCC,
		"	Ol AR,AL\n", },

#ifdef NOBREGS
/* m/r |= r */
{ OPSIMP,	INAREG|FOREFF|FORCC,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SAREG,		TSHORT|TUSHORT,
		0,	RLEFT|RESCC,
		"	Ow AR,AL\n", },

/* r |= r/m */
{ OPSIMP,	INAREG|FOREFF|FORCC,
	SAREG,		TSHORT|TUSHORT,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
		0,	RLEFT|RESCC,
		"	Ow AR,AL\n", },

/* m/r |= r */
{ OPSIMP,	INAREG|FOREFF|FORCC,
	SAREG,	TCHAR|TUCHAR,
	SAREG,	TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"	Ol AR,AL\n", },

{ OPSIMP,	FOREFF|FORCC,
	SNAME|SOREG,	TCHAR|TUCHAR,
	SCON,	TANY,
		0,	RLEFT|RESCC,
		"	Ob AR,AL\n", },

{ OPSIMP,	INAREG|FOREFF|FORCC,
	SAREG,	TSHORT|TUSHORT,
	SCON,	TANY,
		0,	RLEFT|RESCC,
		"	Ol AR,AL\n", },
#else
/* m/r |= r */
{ OPSIMP,	INAREG|FOREFF|FORCC,
	SHINT|SNAME|SOREG,	TSHORT|TUSHORT,
	SHINT,		TSHORT|TUSHORT,
		0,	RLEFT|RESCC,
		"	Ow AR,AL\n", },

/* r |= r/m */
{ OPSIMP,	INAREG|FOREFF|FORCC,
	SHINT,		TSHORT|TUSHORT,
	SHINT|SNAME|SOREG,	TSHORT|TUSHORT,
		0,	RLEFT|RESCC,
		"	Ow AR,AL\n", },

/* m/r |= r */
{ OPSIMP,	INCH|FOREFF|FORCC,
	SHCH,		TCHAR|TUCHAR,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"	Ob AR,AL\n", },

/* r |= r/m */
{ OPSIMP,	INCH|FOREFF|FORCC,
	SHCH,		TCHAR|TUCHAR,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
		0,	RLEFT|RESCC,
		"	Ob AR,AL\n", },

{ OPSIMP,	INCH|FOREFF|FORCC,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SCON,	TANY,
		0,	RLEFT|RESCC,
		"	Ob AR,AL\n", },

{ OPSIMP,	INAREG|FOREFF|FORCC,
	SHINT|SNAME|SOREG,	TSHORT|TUSHORT,
	SCON,	TANY,
		0,	RLEFT|RESCC,
		"	Ow AR,AL\n", },
#endif

/* m/r |= const */
{ OPSIMP,	INAREG|FOREFF|FORCC,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
	SCON,	TWORD|TPOINT,
		0,	RLEFT|RESCC,
		"	Ol AR,AL\n", },

/* r |= r/m */
{ OPSIMP,	INLL|FOREFF,
	SHLL,	TLL,
	SHLL|SNAME|SOREG,	TLL,
		0,	RLEFT,
		"	Ol AR,AL\n	Ol UR,UL\n", },

/* m/r |= r/const */
{ OPSIMP,	INLL|FOREFF,
	SHLL|SNAME|SOREG,	TLL,
	SHLL|SCON,	TLL,
		0,	RLEFT,
		"	Ol AR,AL\n	Ol UR,UL\n", },

/* Try use-reg instructions first */
{ PLUS,		INAREG,
	SAREG,	TWORD|TPOINT,
	SCON,	TANY,
		XSL(A),	RESC1,
		"	leal CR(AL),A1\n", },

{ MINUS,	INAREG,
	SAREG,	TWORD|TPOINT,
	SPCON,	TANY,
		XSL(A),	RESC1,
		"	leal -CR(AL),A1\n", },


/*
 * The next rules handle all shift operators.
 */
#ifdef NOBREGS
/* (u)longlong left shift is emulated */
{ LS,	INCREG,
	SCREG,	TLL,
	SAREG,	TAREG,
		NEEDS(NLEFT(EAXEDX), NRIGHT(ECX), NRES(EAXEDX)),	RLEFT,
		"ZO", },
/* r/m <<= r */
{ LS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TWORD,
	SAREG,			TAREG,
		NEEDS(NOLEFT(ECX), NRIGHT(ECX)),	RLEFT,
		"	sall ZT,AL\n", },

/* r/m <<= const */
{ LS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TWORD,
	SCON,	TANY,
		0,	RLEFT,
		"	sall AR,AL\n", },

/* r/m <<= r */
{ LS,	FOREFF,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SAREG,		TAREG,
		NEEDS(NOLEFT(ECX), NRIGHT(ECX)),	RLEFT,
		"	shlw ZT,AL\n", },

/* r/m <<= r */
{ LS,	FOREFF,
	SNAME|SOREG,	TCHAR|TUCHAR,
	SAREG,		TAREG,
		NEEDS(NOLEFT(ECX), NRIGHT(ECX)),	RLEFT,
		"	shlb ZT,AL\n", },

/* r/m <<= const */
{ LS,	INAREG|FOREFF,
	SAREG,	TAREG,
	SCON,	TANY,
		0,	RLEFT,
		"	shll AR,AL\n", },

{ LS,	INAREG|FOREFF,
	SAREG,	TAREG,
	SAREG,	TAREG,
		NEEDS(NOLEFT(ECX), NRIGHT(ECX)),	RLEFT,
		"	shll ZT,AL\n", },
#else
/* (u)longlong left shift is emulated */
{ LS,	INCREG,
	SCREG,	TLL,
	SHCH,	TCHAR|TUCHAR,
		NEEDS(NLEFT(EAXEDX), NRIGHT(CL),NRES(EAXEDX)),	RLEFT,
		"ZO", },
/* r/m <<= r */
{ LS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TWORD,
	SHCH,		TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	sall AR,AL\n", },
/* r/m <<= const */
{ LS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TWORD,
	SCON,	TANY,
		0,	RLEFT,
		"	sall AR,AL\n", },
/* r/m <<= r */
{ LS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	shlw AR,AL\n", },
/* r/m <<= const */
{ LS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SCON,	TANY,
		0,	RLEFT,
		"	shlw AR,AL\n", },
{ LS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	salb AR,AL\n", },

{ LS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SCON,			TANY,
		0,	RLEFT,
		"	salb AR,AL\n", },
#endif

#ifdef NOBREGS
/* (u)longlong right shift is emulated */
{ RS,	INCREG,
	SCREG,	TLL,
	SAREG,	TAREG,
		NEEDS(NLEFT(EAXEDX), NRIGHT(ECX),NRES(EAXEDX)),	RLEFT,
		"ZO", },

/* right-shift word in memory or reg */
{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TWORD,
	SAREG,			TAREG,
		NEEDS(NOLEFT(ECX), NRIGHT(ECX)),	RLEFT,
		"	sZUrl ZT,AL\n", },

/* as above but using a constant */
{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TWORD,
	SCON,			TAREG,
		0,		RLEFT,
		"	sZUrl AR,AL\n", },

/* short in memory */
{ RS,	FOREFF,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SAREG,		TAREG,
		NEEDS(NOLEFT(ECX), NRIGHT(ECX)),	RLEFT,
		"	sZUrw ZT,AL\n", },

{ RS,	FOREFF,
	SNAME|SOREG,	TSHORT|TUSHORT,
	SCON,		TAREG,
		0,	RLEFT,
		"	sZUrw AR,AL\n", },

{ RS,	FOREFF,
	SNAME|SOREG,	TCHAR|TUCHAR,
	SAREG,		TAREG,
		NEEDS(NOLEFT(ECX), NRIGHT(ECX)),	RLEFT,
		"	sZUrl ZT,AL\n", },
#else
/* (u)longlong right shift is emulated */
{ RS,	INCREG,
	SCREG,	TLL,
	SHCH,	TCHAR|TUCHAR,
		NEEDS(NLEFT(EAXEDX), NRIGHT(CL),NRES(EAXEDX)),	RLEFT,
		"ZO", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSWORD,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	sarl AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSWORD,
	SCON,			TANY,
		0,		RLEFT,
		"	sarl AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TUWORD,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	shrl AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TUWORD,
	SCON,			TANY,
		0,		RLEFT,
		"	shrl AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	sarw AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TSHORT,
	SCON,			TANY,
		0,		RLEFT,
		"	sarw AR,AL\n", },
{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TUSHORT,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	shrw AR,AL\n", },

{ RS,	INAREG|FOREFF,
	SAREG|SNAME|SOREG,	TUSHORT,
	SCON,			TANY,
		0,		RLEFT,
		"	shrw AR,AL\n", },
{ RS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	sarb AR,AL\n", },

{ RS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TCHAR,
	SCON,			TANY,
		0,		RLEFT,
		"	sarb AR,AL\n", },
{ RS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TUCHAR,
	SHCH,			TCHAR|TUCHAR,
		NEEDS(NOLEFT(ECX), NRIGHT(CL)),	RLEFT,
		"	shrb AR,AL\n", },

{ RS,	INCH|FOREFF,
	SHCH|SNAME|SOREG,	TUCHAR,
	SCON,			TANY,
		0,		RLEFT,
		"	shrb AR,AL\n", },
#endif

/*
 * The next rules takes care of assignments. "=".
 */
{ ASSIGN,	FORCC|FOREFF|INLL,
	SHLL,		TLL,
	SZERO,		TANY,
		0,	RDEST,
		"	xorl AL,AL\n	xorl UL,UL\n", },

{ ASSIGN,	FORCC|FOREFF|INLL,
	SHLL,		TLL,
	SMILWXOR,	TANY,
		0,	RDEST,
		"	xorl AL,AL\n	movl UR,UL\n", },

{ ASSIGN,	FORCC|FOREFF|INLL,
	SHLL,		TLL,
	SMIHWXOR,	TANY,
		0,	RDEST,
		"	movl AR,AL\n	xorl UL,UL\n", },

{ ASSIGN,	FOREFF|INLL,
	SHLL,		TLL,
	SCON,		TANY,
		0,	RDEST,
		"	movl AR,AL\n	movl UR,UL\n", },

{ ASSIGN,	FOREFF,
	SHLL|SNAME|SOREG,	TLL,
	SCON,		TANY,
		0,	0,
		"	movl AR,AL\n	movl UR,UL\n", },

{ ASSIGN,	FOREFF,
	SNAME|SOREG,	TLL,
	SZERO,		TANY,
		0,	0,
		"	andl $0,AL\n	andl $0,UL\n", },

{ ASSIGN,	FORCC|FOREFF|INAREG,
	SAREG,	TWORD|TPOINT,
	SZERO,		TANY,
		0,	RDEST,
		"	xorl AL,AL\n", },

{ ASSIGN,	FOREFF,
	SOREG|SNAME,	TWORD|TPOINT,
	SZERO,		TANY,
		0,	0,
		"	andl $0,AL\n", },

{ ASSIGN,	FOREFF,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
	SCON,		TANY,
		0,	0,
		"	movl AR,AL\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TWORD|TPOINT,
	SCON,		TANY,
		0,	RDEST,
		"	movl AR,AL\n", },

{ ASSIGN,	FORCC|FOREFF|INAREG,
	SAREG,	TSHORT|TUSHORT,
	SMIXOR,		TANY,
		0,	RDEST,
		"	xorw AL,AL\n", },

{ ASSIGN,	FOREFF,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SCON,		TANY,
		0,	0,
		"	movw AR,AL\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TSHORT|TUSHORT,
	SCON,		TANY,
		0,	RDEST,
		"	movw AR,AL\n", },

#ifdef NOBREGS
{ ASSIGN,	INAREG|FOREFF,
	SAREG,			TUCHAR,
	SNAME|SOREG,	TUCHAR,
		0,	RDEST,
		"	movzbl AR,AL\n", },

{ ASSIGN,	INAREG|FOREFF,
	SAREG,			TCHAR,
	SNAME|SOREG,	TCHAR,
		0,	RDEST,
		"	movsbl AR,AL\n", },

{ ASSIGN,	INAREG|FOREFF,
	SOREG|SNAME,	TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR,
		NEEDS(NORIGHT(ESI), NORIGHT(EDI)),	RDEST,
		"	movb ZT,AL\n", },
#else
{ ASSIGN,	FOREFF,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SCON,		TANY,
		0,	0,
		"	movb AR,AL\n", },

{ ASSIGN,	FOREFF|INCH,
	SHCH,		TCHAR|TUCHAR,
	SCON,		TANY,
		0,	RDEST,
		"	movb AR,AL\n", },
#endif

{ ASSIGN,	FOREFF|INLL,
	SNAME|SOREG,	TLL,
	SHLL,		TLL,
		0,	RDEST,
		"	movl AR,AL\n	movl UR,UL\n", },

{ ASSIGN,	FOREFF|INLL,
	SHLL,	TLL,
	SHLL,	TLL,
		0,	RDEST,
		"ZH", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
	SAREG,		TWORD|TPOINT,
		0,	RDEST,
		"	movl AR,AL\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,			TWORD|TPOINT,
	SAREG|SNAME|SOREG,	TWORD|TPOINT,
		0,	RDEST,
		"	movl AR,AL\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
	SAREG,		TSHORT|TUSHORT,
		0,	RDEST,
		"	movw AR,AL\n", },

#ifdef NOBREGS
{ ASSIGN,	FOREFF|INAREG,
	SNAME|SOREG,	TCHAR|TUCHAR,
	SAREG,		TCHAR|TUCHAR|TWORD,
		NEEDS(NORIGHT(ESI), NORIGHT(EDI)),	RDEST,
		"	movb ZT,AL\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TUCHAR,
	SAREG,	TCHAR|TUCHAR|TWORD,
		NEEDS(NORIGHT(ESI), NORIGHT(EDI)),	RDEST,
		"	movzbl ZT,AL\n", },

{ ASSIGN,	FOREFF|INAREG,
	SAREG,	TCHAR,
	SAREG,	TCHAR|TUCHAR|TWORD,
		NEEDS(NORIGHT(ESI), NORIGHT(EDI)),	RDEST,
		"	movsbl ZT,AL\n", },
#else
{ ASSIGN,	FOREFF|INCH,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
	SHCH,		TCHAR|TUCHAR|TWORD,
		0,	RDEST,
		"	movb AR,AL\n", },
#endif

{ ASSIGN,	INDREG|FOREFF,
	SHFL,	TFLOAT|TDOUBLE|TLDOUBLE,
	SHFL,	TFLOAT|TDOUBLE|TLDOUBLE,
		0,	RDEST,
		"", }, /* This will always be in the correct register */

/* order of table entries is very important here! */
{ ASSIGN,	INFL,
	SNAME|SOREG,	TLDOUBLE,
	SHFL,	TFLOAT|TDOUBLE|TLDOUBLE,
		0,	RDEST,
		"	fstpt AL\n	fldt AL\n", }, /* XXX */

{ ASSIGN,	FOREFF,
	SNAME|SOREG,	TLDOUBLE,
	SHFL,	TFLOAT|TDOUBLE|TLDOUBLE,
		0,	0,
		"	fstpt AL\n", },

{ ASSIGN,	INFL,
	SNAME|SOREG,	TDOUBLE,
	SHFL,	TFLOAT|TDOUBLE|TLDOUBLE,
		0,	RDEST,
		"	fstl AL\n", },

{ ASSIGN,	FOREFF,
	SNAME|SOREG,	TDOUBLE,
	SHFL,	TFLOAT|TDOUBLE|TLDOUBLE,
		0,	0,
		"	fstpl AL\n", },

{ ASSIGN,	INFL,
	SNAME|SOREG,	TFLOAT,
	SHFL,	TFLOAT|TDOUBLE|TLDOUBLE,
		0,	RDEST,
		"	fsts AL\n", },

{ ASSIGN,	FOREFF,
	SNAME|SOREG,	TFLOAT,
	SHFL,	TFLOAT|TDOUBLE|TLDOUBLE,
		0,	0,
		"	fstps AL\n", },
/* end very important order */

{ ASSIGN,	INFL|FOREFF,
	SHFL,		TLDOUBLE,
	SHFL|SOREG|SNAME,	TLDOUBLE,
		0,	RDEST,
		"	fldt AR\n", },

{ ASSIGN,	INFL|FOREFF,
	SHFL,		TDOUBLE,
	SHFL|SOREG|SNAME,	TDOUBLE,
		0,	RDEST,
		"	fldl AR\n", },

{ ASSIGN,	INFL|FOREFF,
	SHFL,		TFLOAT,
	SHFL|SOREG|SNAME,	TFLOAT,
		0,	RDEST,
		"	flds AR\n", },

/* Do not generate memcpy if return from funcall */
#if 0
{ STASG,	INAREG|FOREFF,
	SOREG|SNAME|SAREG,	TPTRTO|TSTRUCT,
	SFUNCALL,	TPTRTO|TSTRUCT,
		0,	RRIGHT,
		"", },
#endif

{ STASG,	INAREG|FOREFF,
	SOREG|SNAME,	TANY,
	SAREG,		TPTRTO|TANY,
		NEEDS(NEVER(EDI), NEVER(ESI), NRIGHT(ESI), NOLEFT(ESI),
		    NOLEFT(ECX), NORIGHT(ECX), NEVER(ECX), NREG(A,1)),	RDEST,
		"F	movl %esi,A1\nZQF	movl A1,%esi\n", },

/*
 * DIV/MOD/MUL 
 */
/* long long div is emulated */
{ DIV,	INCREG,
	SCREG|SNAME|SOREG|SCON, TLL,
	SCREG|SNAME|SOREG|SCON, TLL,
		NEEDS(NREG(C,1), NSL(C), NSR(C), NEVER(EAX), 
		    NEVER(EDX), NEVER(ECX), NRES(EAXEDX)), RESC1,
		"ZO", },

{ DIV,	INAREG,
	SAREG,			TSWORD,
	SAREG|SNAME|SOREG,	TWORD,
		NEEDS(NEVER(EAX), NEVER(EDX), NLEFT(EAX), 
		    NRES(EAX), NORIGHT(EAX), NORIGHT(EDX)), RDEST,
		"	cltd\n	idivl AR\n", },

{ DIV,	INAREG,
	SAREG,			TUWORD|TPOINT,
	SAREG|SNAME|SOREG,	TUWORD|TPOINT,
		NEEDS(NEVER(EAX), NEVER(EDX), NLEFT(EAX), 
		    NRES(EAX), NORIGHT(EAX), NORIGHT(EDX)), RDEST,
		"	xorl %edx,%edx\n	divl AR\n", },

{ DIV,	INAREG,
	SAREG,			TUSHORT,
	SAREG|SNAME|SOREG,	TUSHORT,
		NEEDS(NEVER(EAX), NEVER(EDX), NLEFT(EAX), 
		    NRES(EAX), NORIGHT(EAX), NORIGHT(EDX)), RDEST,
		"	xorl %edx,%edx\n	divw AR\n", },

#ifdef NOBREGS
{ DIV,	INAREG,
	SAREG,	TUCHAR,
	SAREG,	TUCHAR,
		NEEDS(NEVER(EAX), NEVER(EDX), NLEFT(EAX), 
		    NRES(EAX), NORIGHT(EAX), NORIGHT(EDX)), RDEST,
		"	xorl %edx,%edx\n	divl AR\n", },
#else
{ DIV,	INCH,
	SHCH,			TUCHAR,
	SHCH|SNAME|SOREG,	TUCHAR,
		NEEDS(NEVER(AL), NEVER(AH), NLEFT(AL), 
		    NRES(AL), NORIGHT(AL), NORIGHT(AH)), RDEST,
		"	xorb %ah,%ah\n	divb AR\n", },
#endif

{ DIV,	INFL,
	SHFL,		TDOUBLE,
	SNAME|SOREG,	TDOUBLE,
		0,	RLEFT,
		"	fdivl AR\n", },

{ DIV,	INFL,
	SHFL,		TLDOUBLE|TDOUBLE|TFLOAT,
	SHFL,		TLDOUBLE|TDOUBLE|TFLOAT,
		0,	RLEFT,
		"	fdivZAp\n", },

/* (u)longlong mod is emulated */
{ MOD,	INCREG,
	SCREG|SNAME|SOREG|SCON, TLL,
	SCREG|SNAME|SOREG|SCON, TLL,
		NEEDS(NREG(C,1), NSL(C), NSR(C), NEVER(EAX), 
		    NEVER(EDX), NEVER(ECX), NRES(EAXEDX)), RESC1,
		"ZO", },

{ MOD,	INAREG,
	SAREG,			TSWORD,
	SAREG|SNAME|SOREG,	TSWORD,
		NEEDS(NREG(A,1), NEVER(EAX), NEVER(EDX), NLEFT(EAX), 
		    NRES(EDX), NORIGHT(EAX), NORIGHT(EDX)), RESC1,
		"	cltd\n	idivl AR\n", },

{ MOD,	INAREG,
	SAREG,			TWORD|TPOINT,
	SAREG|SNAME|SOREG,	TUWORD|TPOINT,
		NEEDS(NREG(A,1), NEVER(EAX), NEVER(EDX), NLEFT(EAX), 
		    NRES(EDX), NORIGHT(EAX), NORIGHT(EDX)), RESC1,
		"	xorl %edx,%edx\n	divl AR\n", },

{ MOD,	INAREG,
	SAREG,			TUSHORT,
	SAREG|SNAME|SOREG,	TUSHORT,
		NEEDS(NREG(A,1), NEVER(EAX), NEVER(EDX), NLEFT(EAX), 
		    NRES(EDX), NORIGHT(EAX), NORIGHT(EDX)), RESC1,
		"	xorl %edx,%edx\n	divw AR\n", },

#ifdef NOBREGS
{ MOD,	INAREG,
	SAREG,	TUCHAR,
	SAREG,	TUCHAR,
		NEEDS(NREG(A,1), NEVER(EAX), NEVER(EDX), NLEFT(EAX), 
		    NRES(EDX), NORIGHT(EAX), NORIGHT(EDX)), RESC1,
		"	xorl %edx,%edx\n	divl AR\n", },
#else
{ MOD,	INCH,
	SHCH,			TUCHAR,
	SHCH|SNAME|SOREG,	TUCHAR,
		NEEDS(NREG(B,1), NEVER(AL), NEVER(AH), NLEFT(AL), 
		    NRES(AH), NORIGHT(AL), NORIGHT(AH)), RESC1,
		"	xorb %ah,%ah\n	divb AR\n", },
#endif

/* (u)longlong mul is emulated */
{ MUL,	INCREG,
	SCREG,	TLL,
	SCREG,	TLL,
		NEEDS(NEVER(ESI), NRIGHT(ECXESI), NLEFT(EAXEDX), 
		    NRES(EAXEDX)), RDEST,
		"ZO", },

{ MUL,	INAREG,
	SAREG,				TWORD|TPOINT,
	SAREG|SNAME|SOREG|SCON,		TWORD|TPOINT,
		0,	RLEFT,
		"	imull AR,AL\n", },

{ MUL,	INAREG,
	SAREG,			TSHORT|TUSHORT,
	SAREG|SNAME|SOREG,	TSHORT|TUSHORT,
		0,	RLEFT,
		"	imulw AR,AL\n", },

#ifdef NOBREGS
{ MUL,	INAREG,
	SAREG,		TCHAR|TUCHAR,
	SAREG|SCON,	TCHAR|TUCHAR,
		0,	RLEFT,
		"	imull AR,AL\n", },
#else
{ MUL,	INCH,
	SHCH,			TCHAR|TUCHAR,
	SHCH|SNAME|SOREG,	TCHAR|TUCHAR,
		NEEDS(NLEFT(AL), NEVER(AL), NEVER(AH), 
		    NRES(AL)), RDEST,
		"	imulb AR\n", },
#endif

{ MUL,	INFL,
	SHFL,		TDOUBLE,
	SNAME|SOREG,	TDOUBLE,
		0,	RLEFT,
		"	fmull AR\n", },

{ MUL,	INFL,
	SHFL,		TLDOUBLE|TDOUBLE|TFLOAT,
	SHFL,		TLDOUBLE|TDOUBLE|TFLOAT,
		0,	RLEFT,
		"	fmulp\n", },

/*
 * Indirection operators.
 */
{ UMUL,	INLL,
	SANY,	TANY,
	SOREG,	TLL,
		NEEDS(NREG(C, 1)),	RESC1,
		"	movl UL,U1\n	movl AL,A1\n", },

{ UMUL,	INAREG,
	SANY,	TPOINT|TWORD,
	SOREG,	TPOINT|TWORD,
		XSL(A),	RESC1,
		"	movl AL,A1\n", },

#ifdef NOBREGS
{ UMUL,	INAREG,
	SANY,	TANY,
	SOREG,	TCHAR,
		XSL(A),	RESC1,
		"	movsbl AL,A1\n", },
{ UMUL,	INAREG,
	SANY,	TANY,
	SOREG,	TUCHAR,
		XSL(A),	RESC1,
		"	movzbl AL,A1\n", },
#else
{ UMUL,	INCH,
	SANY,	TANY,
	SOREG,	TCHAR|TUCHAR,
		XSL(B),	RESC1,
		"	movb AL,A1\n", },
#endif

{ UMUL,	INAREG,
	SANY,	TANY,
	SOREG,	TSHORT|TUSHORT,
		XSL(A),	RESC1,
		"	movw AL,A1\n", },

{ UMUL,	INFL,
	SANY,	TANY,
	SOREG,	TLDOUBLE,
		XSL(D),	RESC1,
		"	fldt AL\n", },

{ UMUL,	INFL,
	SANY,	TANY,
	SOREG,	TDOUBLE,
		XSL(D),	RESC1,
		"	fldl AL\n", },

{ UMUL,	INFL,
	SANY,	TANY,
	SOREG,	TFLOAT,
		XSL(D),	RESC1,
		"	flds AL\n", },

/*
 * Logical/branching operators
 */

/* Comparisions, take care of everything */
{ OPLOG,	FORCC,
	SHLL|SOREG|SNAME,	TLL,
	SHLL,			TLL,
		0,	0,
		"ZD", },

{ OPLOG,	FORCC,
	SAREG|SOREG|SNAME,	TWORD|TPOINT,
	SCON|SAREG,	TWORD|TPOINT,
		0, 	RESCC,
		"	cmpl AR,AL\n", },

#if 0
{ OPLOG,	FORCC,
	SCON|SAREG,	TWORD|TPOINT,
	SAREG|SOREG|SNAME,	TWORD|TPOINT,
		0, 	RESCC,
		"	cmpl AR,AL\n", },
#endif

{ OPLOG,	FORCC,
	SAREG|SOREG|SNAME,	TSHORT|TUSHORT,
	SCON|SAREG,	TANY,
		0, 	RESCC,
		"	cmpw AR,AL\n", },

{ OPLOG,	FORCC,
	SBREG|SOREG|SNAME,	TCHAR|TUCHAR,
	SCON|SBREG,	TANY,
		0, 	RESCC,
		"	cmpb AR,AL\n", },

{ OPLOG,	FORCC,
	SDREG,	TLDOUBLE|TDOUBLE|TFLOAT,
	SDREG,	TLDOUBLE|TDOUBLE|TFLOAT,
		NEEDS(NEVER(EAX)), 	RNOP,
		"ZG", },

{ OPLOG,	FORCC,
	SANY,	TANY,
	SANY,	TANY,
		NEEDS(NREWRITE),	0,
		"diediedie!", },

/* AND/OR/ER/NOT */
{ AND,	INAREG|FOREFF,
	SAREG|SOREG|SNAME,	TWORD,
	SCON|SAREG,		TWORD,
		0,	RLEFT,
		"	andl AR,AL\n", },

{ AND,	INCREG|FOREFF,
	SCREG,			TLL,
	SCREG|SOREG|SNAME,	TLL,
		0,	RLEFT,
		"	andl AR,AL\n	andl UR,UL\n", },

{ AND,	INAREG|FOREFF,
	SAREG,			TWORD,
	SAREG|SOREG|SNAME,	TWORD,
		0,	RLEFT,
		"	andl AR,AL\n", },

{ AND,	INAREG|FOREFF,  
	SAREG|SOREG|SNAME,	TSHORT|TUSHORT,
	SCON|SAREG,		TSHORT|TUSHORT,
		0,	RLEFT,
		"	andw AR,AL\n", },

{ AND,	INAREG|FOREFF,  
	SAREG,			TSHORT|TUSHORT,
	SAREG|SOREG|SNAME,	TSHORT|TUSHORT,
		0,	RLEFT,
		"	andw AR,AL\n", },

{ AND,	INBREG|FOREFF,
	SBREG|SOREG|SNAME,	TCHAR|TUCHAR,
	SCON|SBREG,		TCHAR|TUCHAR,
		0,	RLEFT,
		"	andb AR,AL\n", },

{ AND,	INBREG|FOREFF,
	SBREG,			TCHAR|TUCHAR,
	SBREG|SOREG|SNAME,	TCHAR|TUCHAR,
		0,	RLEFT,
		"	andb AR,AL\n", },
/* AND/OR/ER/NOT */

/*
 * Jumps.
 */
{ GOTO, 	FOREFF,
	SCON,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	jmp LL\n", },

#if defined(GCC_COMPAT) || defined(LANG_F77)
{ GOTO, 	FOREFF,
	SAREG,	TANY,
	SANY,	TANY,
		0,	RNOP,
		"	jmp *AL\n", },
#endif

/*
 * Convert LTYPE to reg.
 */
{ OPLTYPE,	FORCC|INLL,
	SCREG,	TLL,
	SMIXOR,	TANY,
		NEEDS(NREG(C,1)),	RESC1,
		"	xorl U1,U1\n	xorl A1,A1\n", },

{ OPLTYPE,	FORCC|INLL,
	SCREG,	TLL,
	SMILWXOR,	TANY,
		NEEDS(NREG(C,1)),	RESC1,
		"	movl UL,U1\n	xorl A1,A1\n", },

{ OPLTYPE,	FORCC|INLL,
	SCREG,	TLL,
	SMIHWXOR,	TANY,
		NEEDS(NREG(C,1)),	RESC1,
		"	xorl U1,U1\n	movl AL,A1\n", },

{ OPLTYPE,	INLL,
	SANY,	TANY,
	SCREG,	TLL,
		NEEDS(NREG(C,1)),	RESC1,
		"ZK", },

{ OPLTYPE,	INLL,
	SANY,	TANY,
	SCON|SOREG|SNAME,	TLL,
		NEEDS(NREG(C,1)),	RESC1,
		"	movl UL,U1\n	movl AL,A1\n", },

{ OPLTYPE,	FORCC|INAREG,
	SAREG,	TWORD|TPOINT,
	SMIXOR,	TANY,
		XSL(A),	RESC1,
		"	xorl A1,A1\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SAREG|SCON|SOREG|SNAME,	TWORD|TPOINT,
		XSL(A),	RESC1,
		"	movl AL,A1\n", },

#ifdef NOBREGS
{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SCON,	TCHAR|TUCHAR,
		XSL(A),	RESC1,
		"	movl AL,A1\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SOREG|SNAME,	TUCHAR,
		XSL(A),	RESC1,
		"	movzbl AL,A1\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SOREG|SNAME,	TCHAR,
		XSL(A),	RESC1,
		"	movsbl AL,A1\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SAREG,	TCHAR|TUCHAR,
		XSL(A),	RESC1,
		"	movl AL,A1\n", },
#else
{ OPLTYPE,	INBREG,
	SANY,	TANY,
	SBREG|SOREG|SNAME|SCON,	TCHAR|TUCHAR,
		XSL(B),	RESC1,
		"	movb AL,A1\n", },
#endif

{ OPLTYPE,	FORCC|INAREG,
	SAREG,	TSHORT|TUSHORT,
	SMIXOR,	TANY,
		XSL(A),	RESC1,
		"	xorw A1,A1\n", },

{ OPLTYPE,	INAREG,
	SANY,	TANY,
	SAREG|SOREG|SNAME|SCON,	TSHORT|TUSHORT,
		XSL(A),	RESC1,
		"	movw AL,A1\n", },

{ OPLTYPE,	INDREG,
	SANY,		TLDOUBLE,
	SOREG|SNAME,	TLDOUBLE,
		XSL(D),	RESC1,
		"	fldt AL\n", },

{ OPLTYPE,	INDREG,
	SANY,		TDOUBLE,
	SOREG|SNAME,	TDOUBLE,
		XSL(D),	RESC1,
		"	fldl AL\n", },

{ OPLTYPE,	INDREG,
	SANY,		TFLOAT,
	SOREG|SNAME,	TFLOAT,
		XSL(D),	RESC1,
		"	flds AL\n", },

/* Only used in ?: constructs. The stack already contains correct value */
{ OPLTYPE,	INDREG,
	SANY,	TFLOAT|TDOUBLE|TLDOUBLE,
	SDREG,	TFLOAT|TDOUBLE|TLDOUBLE,
		XSL(D),	RESC1,
		"", },

/*
 * Negate a word.
 */

{ UMINUS,	INCREG|FOREFF,
	SCREG,	TLL,
	SCREG,	TLL,
		0,	RLEFT,
		"	negl AL\n	adcl $0,UL\n	negl UL\n", },

{ UMINUS,	INAREG|FOREFF,
	SAREG,	TWORD|TPOINT,
	SAREG,	TWORD|TPOINT,
		0,	RLEFT,
		"	negl AL\n", },

{ UMINUS,	INAREG|FOREFF,
	SAREG,	TSHORT|TUSHORT,
	SAREG,	TSHORT|TUSHORT,
		0,	RLEFT,
		"	negw AL\n", },

{ UMINUS,	INBREG|FOREFF,
	SBREG,	TCHAR|TUCHAR,
	SBREG,	TCHAR|TUCHAR,
		0,	RLEFT,
		"	negb AL\n", },

{ UMINUS,	INFL|FOREFF,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
	SHFL,	TLDOUBLE|TDOUBLE|TFLOAT,
		0,	RLEFT,
		"	fchs\n", },

{ COMPL,	INCREG,
	SCREG,	TLL,
	SANY,	TANY,
		0,	RLEFT,
		"	notl AL\n	notl UL\n", },

{ COMPL,	INAREG,
	SAREG,	TWORD,
	SANY,	TANY,
		0,	RLEFT,
		"	notl AL\n", },

{ COMPL,	INAREG,
	SAREG,	TSHORT|TUSHORT,
	SANY,	TANY,
		0,	RLEFT,
		"	notw AL\n", },

{ COMPL,	INBREG,
	SBREG,	TCHAR|TUCHAR,
	SANY,	TANY,
		0,	RLEFT,
		"	notb AL\n", },

/*
 * Arguments to functions.
 */
{ FUNARG,	FOREFF,
	SCON|SCREG|SNAME|SOREG,	TLL,
	SANY,	TLL,
		0,	RNULL,
		"	pushl UL\n	pushl AL\n", },

{ FUNARG,	FOREFF,
	SCON|SAREG|SNAME|SOREG,	TWORD|TPOINT,
	SANY,	TWORD|TPOINT,
		0,	RNULL,
		"	pushl AL\n", },

{ FUNARG,	FOREFF,
	SCON,	TWORD|TSHORT|TUSHORT|TCHAR|TUCHAR,
	SANY,	TWORD|TSHORT|TUSHORT|TCHAR|TUCHAR,
		0,	RNULL,
		"	pushl AL\n", },

{ FUNARG,	FOREFF,
	SAREG|SNAME|SOREG,	TSHORT,
	SANY,	TSHORT,
		XSL(A),	0,
		"	movswl AL,ZN\n	pushl ZN\n", },

{ FUNARG,	FOREFF,
	SAREG|SNAME|SOREG,	TUSHORT,
	SANY,	TUSHORT,
		XSL(A),	0,
		"	movzwl AL,ZN\n	pushl ZN\n", },

#ifdef NOBREGS
{ FUNARG,	FOREFF,
	SAREG|SNAME|SOREG,	TUCHAR|TCHAR,
	SANY,			TUCHAR|TCHAR,
		XSL(A),	0,
		"	pushl AL\n", },
#else
{ FUNARG,	FOREFF,
	SHCH|SNAME|SOREG,	TCHAR,
	SANY,			TCHAR,
		XSL(A),	0,
		"	movsbl AL,A1\n	pushl A1\n", },

{ FUNARG,	FOREFF,
	SHCH|SNAME|SOREG,	TUCHAR,
	SANY,	TUCHAR,
		XSL(A),	0,
		"	movzbl AL,A1\n	pushl A1\n", },
#endif

{ FUNARG,	FOREFF,
	SNAME|SOREG,	TDOUBLE,
	SANY,	TDOUBLE,
		0,	0,
		"	pushl UL\n	pushl AL\n", },

{ FUNARG,	FOREFF,
	SDREG,	TDOUBLE,
	SANY,		TDOUBLE,
		0,	0,
		"	subl $8,%esp\n	fstpl (%esp)\n", },

{ FUNARG,	FOREFF,
	SNAME|SOREG,	TFLOAT,
	SANY,		TFLOAT,
		0,	0,
		"	pushl AL\n", },

{ FUNARG,	FOREFF,
	SDREG,	TFLOAT,
	SANY,		TFLOAT,
		0,	0,
		"	subl $4,%esp\n	fstps (%esp)\n", },

{ FUNARG,	FOREFF,
	SDREG,	TLDOUBLE,
	SANY,		TLDOUBLE,
		0,	0,
		"	subl $12,%esp\n	fstpt (%esp)\n", },

{ STARG,	FOREFF,
	SAREG,	TPTRTO|TSTRUCT,
	SANY,	TSTRUCT,
		NEEDS(NEVER(EDI), NLEFT(ESI), NEVER(ECX)),	0,
		"ZF", },

# define DF(x) FORREW,SANY,TANY,SANY,TANY,NEEDS(NREWRITE),x,""

{ UMUL, DF( UMUL ), },

{ ASSIGN, DF(ASSIGN), },

{ STASG, DF(STASG), },

{ FLD, DF(FLD), },

{ OPLEAF, DF(NAME), },

/* { INIT, DF(INIT), }, */

{ OPUNARY, DF(UMINUS), },

{ OPANY, DF(BITYPE), },

{ FREE,	FREE,	FREE,	FREE,	FREE,	FREE,	0,	FREE,	"help; I'm in trouble\n" },
};

int tablesize = sizeof(table)/sizeof(table[0]);
