//	Zinc Interface Library - Z_SCANF.CPP
//
//	This file is a modified version of vfscanf.c obtained from the
//	Berkeley Source Tree administered by The Regents of the University of
//	California (the "Regents")
//
//	Modifications made by Zinc are:
//	  COPYRIGHT (C) 1994-1995.  All Rights Reserved.
//	  Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//
//	Permission from the Regents is conditioned on inclusion of the following
//	notice from the Regents which applies only to vfscanf.c:
//
//	  Copyright (c) 1990 The Regents of the University of California.
//	  All rights reserved.
//
//	  This code is derived from software contributed to Berkeley by
//	  Chris Torek.
//
//	  Redistribution and use in source and binary forms, with or without
//	  modification, are permitted provided that the following conditions
//	  are met:
//	  1. Redistributions of source code must retain the above copyright
//	     notice, this list of conditions and the following disclaimer.
//	  2. Redistributions in binary form must reproduce the above copyright
//	     notice, this list of conditions and the following disclaimer in the
//	     documentation and/or other materials provided with the distribution.
//	  3. All advertising materials mentioning features or use of this software
//	     must display the following acknowledgement:
//	       This product includes software developed by the University of
//	       California, Berkeley and its contributors.
//	  4. Neither the name of the University nor the names of its contributors
//	     may be used to endorse or promote products derived from this software
//	     without specific prior written permission.
//
//	  THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
//	  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//	  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//	  ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
//	  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//	  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//	  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//	  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//	  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//	  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//	  SUCH DAMAGE.

#include "ui_gen.hpp"
#include "z_stdarg.hpp"

#if defined(ZIL_UNICODE)
#	define REGENTS_CODE
#elif defined(applec) || defined(_MSC_VER) || defined(__MWERKS__) || defined(__ZTC__)
#	define REGENTS_CODE
#elif defined(__IBMCPP__) || defined(__GLOCKENSPIEL__)
#	define REGENTS_CODE
#elif defined(sun) || defined(_IBM_RS6000) || defined(__sgi) || defined(SCO_UNIX) || defined(_SINIX) || defined(__DECCXX) || defined(ZIL_NEXTSTEP)
#	define REGENTS_CODE
#endif

extern "C"
{
#include <stdio.h>
}

#if !defined(REGENTS_CODE)

int ZIL_INTERNATIONAL::sscanf(ZIL_ICHAR *dst, const ZIL_ICHAR *format, ...)
{
	va_list args;
	va_start(args, format);
#if defined(ZIL_REARRANGEARGS)
	char *newFormat = new char[ZIL_MAXPARAMLEN];
	char buff[ZIL_MAXPARAMLEN];	// Borland expects this in the stack.
	va_list newArgs;
	ZIL_STDARG::RearrangeArgs(TRUE, buff, format, args, newFormat, &newArgs);
	int i = ::vsscanf(dst, newFormat, newArgs);
	delete newFormat;
#else
	int i = ::vsscanf(dst, format, args);
#endif
	va_end(args);
	return (i);
}

int ZIL_STDARG::vsscanf(ZIL_ICHAR *dst, const ZIL_ICHAR *format, va_list args)
{
#if defined(ZIL_REARRANGEARGS)
	char *newFormat = new char[ZIL_MAXPARAMLEN];
	char buff[ZIL_MAXPARAMLEN];	// Borland expects this in the stack.
	va_list newArgs;
	ZIL_STDARG::RearrangeArgs(TRUE, buff, format, args, newFormat, &newArgs);
	int i = ::vsscanf(dst, newFormat, newArgs);
	delete newFormat;
#else
	int i = ::vsscanf(dst, format, args);
#endif
	return (i);
}

#else

int ZIL_INTERNATIONAL::sscanf(ZIL_ICHAR *dst, const ZIL_ICHAR *format, ...)
{
	va_list args;
	va_start(args, format);
#if defined(ZIL_REARRANGEARGS)
	ZIL_ICHAR *newFormat = new ZIL_ICHAR[ZIL_MAXPARAMLEN];
	char buff[ZIL_MAXPARAMLEN];	// Borland expects this in the stack.
	va_list newArgs;
	ZIL_STDARG::RearrangeArgs(TRUE, buff, format, args, newFormat, &newArgs);
	int i = ZIL_STDARG::_vsscanf(dst, newFormat, newArgs);
	delete newFormat;
#else
	int i = ZIL_STDARG::_vsscanf(dst, format, args);
#endif
	va_end(args);
	return i;
}

int ZIL_STDARG::vsscanf(ZIL_ICHAR *dst, const ZIL_ICHAR *format, va_list args)
{
#if defined(ZIL_REARRANGEARGS)
	ZIL_ICHAR *newFormat = new ZIL_ICHAR[ZIL_MAXPARAMLEN];
	char buff[ZIL_MAXPARAMLEN];	// Borland expects this in the stack.
	va_list newArgs;
	ZIL_STDARG::RearrangeArgs(TRUE, buff, format, args, newFormat, &newArgs);
	int i = ZIL_STDARG::_vsscanf(dst, newFormat, newArgs);
	delete newFormat;
#else
	int i = ZIL_STDARG::_vsscanf(dst, format, args);
#endif
	return (i);
}

// -- Code below here was derived from BSD

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)vfscanf.c	5.6 (Berkeley) 2/24/91";
#endif /* LIBC_SCCS and not lint */

#if !defined(__MWERKS__) && !(__BORLANDC__ >= 0x0500)
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#if !defined(__MWERKS__) && !(__BORLANDC__ >= 0x0500)
}
#endif
#if defined(__ZTC__) && !defined(macintosh)
#	undef va_arg
#	define va_arg(ap,type)	   (*(type *)((*(char **)&(ap)+=__va_size(type))-(__va_size(type))))
#endif

/* Define FLOATING_POINT to get floating point. */
#define FLOATING_POINT

#ifdef FLOATING_POINT
#define MAXEXP		4
#define MAXFRACT	24
#define	BUF	(MAXEXP+MAXFRACT+3)	/* 3 = sign + decimal point + NUL */
#else
#define	BUF	40
#endif

/*
 * Flags used during conversion.
 */
#define	LONGVAL		0x01	/* l: long or double */
#define	LONGDBL		0x02	/* L: long double; unimplemented */
#define	SHORTVAL	0x04	/* h: short */
#define	SUPPRESS	0x08	/* suppress assignment */
#define	POINTER		0x10	/* weird %p pointer (`fake hex') */
#define	NOSKIP		0x20	/* do not skip blanks */

/*
 * The following are used in numeric conversions only:
 * SIGNOK, NZIL_DIGITS, DPTOK, and EXPOK are for floating point;
 * SIGNOK, NZIL_DIGITS, PFXOK, and NZZIL_DIGITS are for integral.
 */
#define	SIGNOK		0x40	/* +/- is (still) legal */
#define	NZIL_DIGITS		0x80	/* no digits detected */

#define	DPTOK		0x100	/* (float) decimal point is still legal */
#define	EXPOK		0x200	/* (float) exponent (e+3, etc) still legal */

#define	PFXOK		0x100	/* 0x prefix is (still) legal */
#define	NZZIL_DIGITS	0x200	/* no zero digits detected */

/*
 * Conversion types.
 */
#define	CT_CHAR		0	/* %c conversion */
#define	CT_CCL		1	/* %[...] conversion */
#define	CT_STRING	2	/* %s conversion */
#define	CT_INT		3	/* integer, i.e., strtol or strtoul */
#define	CT_FLOAT	4	/* floating, i.e., strtod */

/*
 * Fill in the given table from the scanset at the given format
 * (just after `[').  Return a pointer to the character past the
 * closing `]'.  The table has a 1 wherever characters should be
 * considered part of the scanset.
 */
static const ZIL_ICHAR *__sccl(char *table, const ZIL_ICHAR *fmt)
{
	int c, n, v;

	/* first `clear' the whole table */
	c = *fmt++;		/* first char hat => negated scanset */
	if (c == '^') {
		v = 1;		/* default => accept */
		c = *fmt++;	/* get new first char */
	} else
		v = 0;		/* default => reject */
	/* should probably use memset here */
	for (n = 0; n < 256; n++)
		table[n] = v;
	if (c == 0)
		return (fmt - 1);/* format ended before closing ] */

	/*
	 * Now set the entries corresponding to the actual scanset
	 * to the opposite of the above.
	 *
	 * The first character may be ']' (or '-') without being special;
	 * the last character may be '-'.
	 */
	v = 1 - v;
	for (;;) {
		table[c] = v;		/* take character c */
doswitch:
		n = *fmt++;		/* and examine the next */
		switch (n) {

		case 0:			/* format ended too soon */
			return (fmt - 1);

		case '-':
			/*
			 * A scanset of the form
			 *	[01+-]
			 * is defined as `the digit 0, the digit 1,
			 * the character +, the character -', but
			 * the effect of a scanset such as
			 *	[a-zA-Z0-9]
			 * is implementation defined.  The V7 Unix
			 * scanf treats `a-z' as `the letters a through
			 * z', but treats `a-a' as `the letter a, the
			 * character -, and the letter a'.
			 *
			 * For compatibility, the `-' is not considerd
			 * to define a range if the character following
			 * it is either a close bracket (required by ANSI)
			 * or is not numerically greater than the character
			 * we just stored in the table (c).
			 */
			n = *fmt;
			if (n == ']' || n < c) {
				c = '-';
				break;	/* resume the for(;;) */
			}
			fmt++;
			do {		/* fill in the range */
				table[++c] = v;
			} while (c < n);
#if 1	/* XXX another disgusting compatibility hack */
			/*
			 * Alas, the V7 Unix scanf also treats formats
			 * such as [a-c-e] as `the letters a through e'.
			 * This too is permitted by the standard....
			 */
			goto doswitch;
#else
			c = *fmt++;
			if (c == 0)
				return (fmt - 1);
			if (c == ']')
				return (fmt);
			break;
#endif

		case ']':		/* end of scanset */
			return (fmt);

		default:		/* just another character */
			c = n;
			break;
		}
	}
#if defined(_MSC_VER)
	return ZIL_NULLP(ZIL_ICHAR);
#endif
}

#if defined(_SUNOS4)
unsigned long strtoul(const ZIL_ICHAR *str, ZIL_ICHAR **ptr, int base)
{
	return (unsigned long)strtol(str, ptr, base);
}
#endif

#if !defined(ZIL_UNICODE)
#	if defined(__IBMCPP__) || defined(__ZTC__)
	unsigned long (*uccfn)(const ZIL_ICHAR *, ZIL_ICHAR **, int);	/* conversion function (strtol/strtoul) */
	long (*ccfn)(const ZIL_ICHAR *, ZIL_ICHAR **, int);	/* conversion function (strtol/strtoul) */
//#	elif defined(__ZTC__)
//	unsigned long (__CLIB *uccfn)(const ZIL_ICHAR *, ZIL_ICHAR **, int);	/* conversion function (strtol/strtoul) */
//	long (__CLIB *ccfn)(const ZIL_ICHAR *, ZIL_ICHAR **, int);	/* conversion function (strtol/strtoul) */
#	else
extern "C" {
	unsigned long (*uccfn)(const ZIL_ICHAR *, ZIL_ICHAR **, int);	/* conversion function (strtol/strtoul) */
	long (*ccfn)(const ZIL_ICHAR *, ZIL_ICHAR **, int);	/* conversion function (strtol/strtoul) */
}
#	endif
#endif
/*
 * vfscanf
 */
int ZIL_STDARG::_vsscanf(ZIL_ICHAR *fp, const ZIL_ICHAR *fmt, va_list ap)
{
	int c;		/* character from format, or conversion */
	size_t width;	/* field width, or 0 */
	ZIL_ICHAR *p;	/* points into all kinds of strings */
	int n;		/* handy integer */
	int flags;	/* flags as defined above */
	ZIL_ICHAR *p0;	/* saves original value of p when necessary */
	int nassigned;		/* number of fields assigned */
	int nread;		/* number of characters consumed from fp */
	int base;		/* base argument to strtol/strtoul */
#if defined(ZIL_UNICODE)
	unsigned long (*uccfn)(const ZIL_ICHAR *, ZIL_ICHAR **, int);	/* conversion function (strtol/strtoul) */
	long (*ccfn)(const ZIL_ICHAR *, ZIL_ICHAR **, int);	/* conversion function (strtol/strtoul) */
#endif
	char ccltab[256];	/* character class table for %[...] */
	ZIL_ICHAR buf[BUF];		/* buffer for numeric conversions */

	/* `basefix' is used to avoid `if' tests in the integer scanner */
	static short basefix[17] =
		{ 10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

	nassigned = 0;
	nread = 0;
	base = 0;		/* XXX just to keep gcc happy */
	uccfn = ZIL_NULLF(unsigned long (*)(const ZIL_ICHAR *, ZIL_ICHAR **, int));
	ccfn = ZIL_NULLF(long (*)(const ZIL_ICHAR *, ZIL_ICHAR **, int));
	for (;;) {
		c = *fmt++;
		if (c == 0)
			return (nassigned);
		if (isspace(c)) {
			for (;;) {
				if (!isspace(*fp))
					break;
				nread++, fp++;
			}
			continue;
		}
		if (c != '%')
			goto literal;
		width = 0;
		flags = 0;
		/*
		 * switch on the format.  continue if done;
		 * break once format type is derived.
		 */
again:		c = *fmt++;
		switch (c) {
		case '%':
literal:
			if (*fp != c)
				goto match_failure;
			fp++;
			nread++;
			continue;

		case '*':
			flags |= SUPPRESS;
			goto again;
		case 'l':
			flags |= LONGVAL;
			goto again;
		case 'L':
			flags |= LONGDBL;
			goto again;
		case 'h':
			flags |= SHORTVAL;
			goto again;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			width = width * 10 + c - '0';
			goto again;

		/*
		 * Conversions.
		 * Those marked `compat' are for 4.[123]BSD compatibility.
		 *
		 * (According to ANSI, E and X formats are supposed
		 * to the same as e and x.  Sorry about that.)
		 */
		case 'D':	/* compat */
			flags |= LONGVAL;
			/* FALLTHROUGH */
		case 'd':
			c = CT_INT;
			ccfn = strtol;
			base = 10;
			break;

		case 'i':
			c = CT_INT;
			ccfn = strtol;
			base = 0;
			break;

		case 'O':	/* compat */
			flags |= LONGVAL;
			/* FALLTHROUGH */
		case 'o':
			c = CT_INT;
			uccfn = strtoul;
			base = 8;
			break;

		case 'u':
			c = CT_INT;
			uccfn = strtoul;
			base = 10;
			break;

		case 'X':	/* compat   XXX */
			flags |= LONGVAL;
			/* FALLTHROUGH */
		case 'x':
			flags |= PFXOK;	/* enable 0x prefixing */
			c = CT_INT;
			uccfn = strtoul;
			base = 16;
			break;

#ifdef FLOATING_POINT
		case 'E':	/* compat   XXX */
		case 'F':	/* compat */
			flags |= LONGVAL;
			/* FALLTHROUGH */
		case 'e': case 'f': case 'g':
			c = CT_FLOAT;
			break;
#endif

		case 's':
			c = CT_STRING;
			break;

		case '[':
			fmt = __sccl(ccltab, fmt);
			flags |= NOSKIP;
			c = CT_CCL;
			break;

		case 'c':
			flags |= NOSKIP;
			c = CT_CHAR;
			break;

		case 'p':	/* pointer format is like hex */
			flags |= POINTER | PFXOK;
			c = CT_INT;
			uccfn = strtoul;
			base = 16;
			break;

		case 'n':
			if (flags & SUPPRESS)	/* ??? */
				continue;
			if (flags & SHORTVAL)
				*va_arg(ap, short *) = nread;
			else if (flags & LONGVAL)
				*va_arg(ap, long *) = nread;
			else
				*va_arg(ap, int *) = nread;
			continue;

		/*
		 * Disgusting backwards compatibility hacks.	XXX
		 */
		case '\0':	/* compat */
			return (EOF);

		default:	/* compat */
			if (isupper(c))
				flags |= LONGVAL;
			c = CT_INT;
			ccfn = strtol;
			base = 10;
			break;
		}

		/*
		 * We have a conversion that requires input.
		 */

		/*
		 * Consume leading white space, except for formats
		 * that suppress this.
		 */
		if ((flags & NOSKIP) == 0) {
			while (isspace(*fp)) {
				nread++;
			}
			/*
			 * Note that there is at least one character in
			 * the buffer, so conversions that do not set NOSKIP
			 * ca no longer result in an input failure.
			 */
		}

		/*
		 * Do the conversion.
		 */
		switch (c) {

		case CT_CHAR:
			/* scan arbitrary characters (sets NOSKIP) */
			if (width == 0)
				width = 1;
			if (flags & SUPPRESS) {
				size_t sum = 0;
				sum += width;
				fp += width;
				nread += sum;
			} else {
				char *tmpStr = va_arg(ap, char *);
				size_t i = 0;
				for (; i < width; i++)
					*tmpStr++ = *fp++;
				nread += i;
				nassigned++;
			}
			break;

		case CT_CCL:
			/* scan a (nonempty) character class (sets NOSKIP) */
			if (width == 0)
				width = ~0;	/* `infinity' */
			/* take only those things in the class */
			if (flags & SUPPRESS) {
				n = 0;
				while (ccltab[*fp]) {
					n++, fp++;
					if (--width == 0)
						break;
				}
				if (n == 0)
					goto match_failure;
			} else {
				p0 = p = va_arg(ap, ZIL_ICHAR *);
				while (ccltab[*fp]) {
					*p++ = *fp++;
					if (--width == 0)
						break;
				}
				n = (int)(p - p0);
				if (n == 0)
					goto match_failure;
				*p = 0;
				nassigned++;
			}
			nread += n;
			break;

		case CT_STRING:
			/* like CCL, but zero-length string OK, & no NOSKIP */
			if (width == 0)
				width = ~0;
			if (flags & SUPPRESS) {
				n = 0;
				while (!isspace(*fp)) {
					n++, fp++;
					if (--width == 0)
						break;
				}
				nread += n;
			} else {
				p0 = p = va_arg(ap, ZIL_ICHAR *);
				while (ccltab[*fp]) {
					*p++ = *fp++;
					if (--width == 0)
						break;
				}
				n = (int)(p - p0);
				if (n == 0)
					goto match_failure;
				*p = 0;
				nassigned++;
			}
			continue;

		case CT_INT:
			/* scan an integer as if by strtol/strtoul */
#ifdef hardway
			if (width == 0 || width > sizeof(buf) - 1)
				width = sizeof(buf) - 1;
#else
			/* size_t is unsigned, hence this optimisation */
			if (--width > sizeof(buf) - 2)
				width = sizeof(buf) - 2;
			width++;
#endif
			flags |= SIGNOK | NZIL_DIGITS | NZZIL_DIGITS;
			for (p = buf; width; width--) {
				c = *fp++;
				/*
				 * Switch on the character; `goto ok'
				 * if we accept it as a part of number.
				 */
				switch (c) {

				/*
				 * The digit 0 is always legal, but is
				 * special.  For %i conversions, if no
				 * digits (zero or nonzero) have been
				 * scanned (only signs), we will have
				 * base==0.  In that case, we should set
				 * it to 8 and enable 0x prefixing.
				 * Also, if we have not scanned zero digits
				 * before this, do not turn off prefixing
				 * (someone else will turn it off if we
				 * have scanned any nonzero digits).
				 */
				case '0':
					if (base == 0) {
						base = 8;
						flags |= PFXOK;
					}
					if (flags & NZZIL_DIGITS)
					    flags &= ~(SIGNOK|NZZIL_DIGITS|NZIL_DIGITS);
					else
					    flags &= ~(SIGNOK|PFXOK|NZIL_DIGITS);
					goto ok;

				/* 1 through 7 always legal */
				case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					base = basefix[base];
					flags &= ~(SIGNOK | PFXOK | NZIL_DIGITS);
					goto ok;

				/* digits 8 and 9 ok iff decimal or hex */
				case '8': case '9':
					base = basefix[base];
					if (base <= 8)
						break;	/* not legal here */
					flags &= ~(SIGNOK | PFXOK | NZIL_DIGITS);
					goto ok;

				/* letters ok iff hex */
				case 'A': case 'B': case 'C':
				case 'D': case 'E': case 'F':
				case 'a': case 'b': case 'c':
				case 'd': case 'e': case 'f':
					/* no need to fix base here */
					if (base <= 10)
						break;	/* not legal here */
					flags &= ~(SIGNOK | PFXOK | NZIL_DIGITS);
					goto ok;

				/* sign ok only as first character */
				case '+': case '-':
					if (flags & SIGNOK) {
						flags &= ~SIGNOK;
						goto ok;
					}
					break;

				/* x ok iff flag still set & 2nd char */
				case 'x': case 'X':
					if (flags & PFXOK && p == buf + 1) {
						base = 16;	/* if %i */
						flags &= ~PFXOK;
						goto ok;
					}
					break;
				}

				/*
				 * If we got here, c is not a legal character
				 * for a number.  Stop accumulating digits.
				 */
				break;
		ok:
				/*
				 * c is legal: store it and look at the next.
				 */
				*p++ = c;
			}
			/*
			 * If we had only a sign, it is no good; push
			 * back the sign.  If the number ends in `x',
			 * it was [sign] '0' 'x', so push back the x
			 * and treat it as [sign] '0'.
			 */
			if (flags & NZIL_DIGITS) {
				if (p > buf)
					*--fp = *--p;
				goto match_failure;
			}
			c = ((ZIL_ICHAR *)p)[-1];
			if (c == 'x' || c == 'X') {
				--p;
				*--fp = c;
			}
			if ((flags & SUPPRESS) == 0) {
				unsigned long res;

				*p = 0;
				if (ccfn)
					res = (*ccfn)(buf, ZIL_NULLP(ZIL_ICHAR *), base);
				if (uccfn)
					res = (*uccfn)(buf, ZIL_NULLP(ZIL_ICHAR *), base);
				if (flags & POINTER)
					*va_arg(ap, void **) = (void *)res;
				else if (flags & SHORTVAL)
					*va_arg(ap, short *) = (short)res;
				else if (flags & LONGVAL)
					*va_arg(ap, long *) = res;
				else
					*va_arg(ap, int *) = (int)res;
				nassigned++;
			}
			nread += (int)(p - buf);
			break;

#ifdef FLOATING_POINT
		case CT_FLOAT:
			/* scan a floating point number as if by strtod */
#ifdef hardway
			if (width == 0 || width > sizeof(buf) - 1)
				width = sizeof(buf) - 1;
#else
			/* size_t is unsigned, hence this optimisation */
			if (--width > sizeof(buf) - 2)
				width = sizeof(buf) - 2;
			width++;
#endif
			flags |= SIGNOK | NZIL_DIGITS | DPTOK | EXPOK;
			for (p = buf; width; width--) {
				c = *fp++;
				/*
				 * This code mimicks the integer conversion
				 * code, but is much simpler.
				 */
				switch (c) {

				case '0': case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
				case '8': case '9':
					flags &= ~(SIGNOK | NZIL_DIGITS);
					goto fok;

				case '+': case '-':
					if (flags & SIGNOK) {
						flags &= ~SIGNOK;
						goto fok;
					}
					break;
				case '.':
					if (flags & DPTOK) {
						flags &= ~(SIGNOK | DPTOK);
						goto fok;
					}
					break;
				case 'e': case 'E':
					/* no exponent without some digits */
					if ((flags&(NZIL_DIGITS|EXPOK)) == EXPOK) {
						flags =
						    (flags & ~(EXPOK|DPTOK)) |
						    SIGNOK | NZIL_DIGITS;
						goto fok;
					}
					break;
				}
				break;
		fok:
				*p++ = c;
			}
			/*
			 * If no digits, might be missing exponent digits
			 * (just give back the exponent) or might be missing
			 * regular digits, but had sign and/or decimal point.
			 */
			if (flags & NZIL_DIGITS) {
				if (flags & EXPOK) {
					/* no digits at all */
					while (p > buf)
						*--fp = *(ZIL_ICHAR *)--p;
					goto match_failure;
				}
				/* just a bad exponent (e and maybe sign) */
				c = *(ZIL_ICHAR *)--p;
				if (c != 'e' && c != 'E') {
					*--fp = c;	/* sign */
					c = *(ZIL_ICHAR *)--p;
				}
				*--fp = c;
			}
			if ((flags & SUPPRESS) == 0) {
				double res;

				*p = 0;
				res = atof(buf);
				if (flags & LONGVAL)
					*va_arg(ap, double *) = res;
				else
					*va_arg(ap, float *) = (float)res;
				nassigned++;
			}
			nread += (int)(p - buf);
			break;
#endif /* FLOATING_POINT */
		}
	}
//input_failure:
//	return (nassigned ? nassigned : -1);
match_failure:
	return (nassigned);
}
#endif
