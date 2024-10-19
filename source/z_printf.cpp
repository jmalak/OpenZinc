//	Zinc Interface Library - Z_PRINTF.CPP
//
//	This file is a modified version of vfprintf.c obtained from the
//	Berkeley Source Tree administered by The Regents of the University of
//	California (the "Regents")
//
//	Modifications made by Zinc are:
//	  COPYRIGHT (C) 1994-1995.  All Rights Reserved.
//	  Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//
//	Permission from the Regents is conditioned on inclusion of the following
//	notice from the Regents which applies only to vfprintf.c:
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

extern "C"
{
#include <stdio.h>
}
#include "ui_gen.hpp"
#include "z_stdarg.hpp"

#if defined(ZIL_UNICODE) || defined(__DECCXX) || defined(_SUNOS4)
#	define REGENTS_CODE
#endif

#if !defined(REGENTS_CODE)

int ZIL_INTERNATIONAL::sprintf(ZIL_ICHAR *dst, const ZIL_ICHAR *format, ...)
{
	va_list args;
	va_start(args, format);
#if defined(ZIL_REARRANGEARGS)
	char *newFormat = new char[ZIL_MAXPARAMLEN];
	char buff[ZIL_MAXPARAMLEN];	// Borland expects this in the stack.
	va_list newArgs;
	ZIL_STDARG::RearrangeArgs(FALSE, buff, format, args, newFormat, &newArgs);
	int i = ::vsprintf(dst, newFormat, newArgs);
	delete newFormat;
#else
	int i = ::vsprintf(dst, format, args);
#endif
	va_end(args);
	return (i);
}

int ZIL_STDARG::vsprintf(ZIL_ICHAR *dst, const ZIL_ICHAR *format, va_list args)
{
#if defined(ZIL_REARRANGEARGS)
	char *newFormat = new char[ZIL_MAXPARAMLEN];
	char buff[ZIL_MAXPARAMLEN];	// Borland expects this in the stack.
	va_list newArgs;
	ZIL_STDARG::RearrangeArgs(FALSE, buff, format, args, newFormat, &newArgs);
	int i = ::vsprintf(dst, newFormat, newArgs);
	delete newFormat;
#else
	int i = ::vsprintf(dst, format, args);
#endif
	return (i);
}

#else

int ZIL_INTERNATIONAL::sprintf(ZIL_ICHAR *dst, const ZIL_ICHAR *format, ...)
{
	va_list args;
	va_start(args, format);
#if defined(ZIL_REARRANGEARGS)
	ZIL_ICHAR *newFormat = new ZIL_ICHAR[ZIL_MAXPARAMLEN];
	char buff[ZIL_MAXPARAMLEN];	// Borland expects this in the stack.
	va_list newArgs;
	ZIL_STDARG::RearrangeArgs(FALSE, buff, format, args, newFormat, &newArgs);
	int i = ZIL_STDARG::_vsprintf(dst, newFormat, newArgs);
	delete newFormat;
#else
	int i = ZIL_STDARG::_vsprintf(dst, format, args);
#endif
	va_end(args);
	return (i);
}

int ZIL_STDARG::vsprintf(ZIL_ICHAR *dst, const ZIL_ICHAR *format, va_list args)
{
#if defined(ZIL_REARRANGEARGS)
	ZIL_ICHAR *newFormat = new ZIL_ICHAR[ZIL_MAXPARAMLEN];
	char buff[ZIL_MAXPARAMLEN];	// Borland expects this in the stack.
	va_list newArgs;
	ZIL_STDARG::RearrangeArgs(FALSE, buff, format, args, newFormat, &newArgs);
	int i = ZIL_STDARG::_vsprintf(dst, newFormat, newArgs);
	delete newFormat;
#else
	int i = ZIL_STDARG::_vsprintf(dst, format, args);
#endif
	return (i);
}

#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))

static long POT[] =
{
	10L,
	100L,
	1000L,
	10000L,
	100000L,
	1000000L,
	10000000L,
	100000000L,
	1000000000L,
};

// -- Code below here was derived from BSD

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)vfprintf.c	5.49 (Berkeley) 6/2/92";
#endif /* LIBC_SCCS and not lint */

/*
 * Actual printf innards.
 *
 * This code is large and complicated...
 */

#if !defined(__MWERKS__)
extern "C"
{
#endif
#if !defined(ZIL_MACINTOSH)
#	include <sys/types.h>
#endif
#if !defined(__hpux)
#	include <math.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#if !defined(__MWERKS__)
}
#endif
#if defined(__hpux)
#	include <math.h>
#endif

/* Define FLOATING_POINT to get floating point. */
#define FLOATING_POINT

typedef unsigned long U_QUAD_T;
typedef long QUAD_T;

/*
 * Macros for converting digits to letters and vice versa
 */
// For format string (always in Arabic numerals)
#define to_digit(c)	((c) - '0')
#define is_digit(c)	((unsigned)to_digit(c) <= 9)
// For floating point number (always in Arabic numerals)
#define to_char(n)	((n) + '0')

/*
 * Flags used during conversion.
 */
#define	ALT		0x001		/* alternate form */
#define	HEXPREFIX	0x002		/* add 0x or 0X prefix */
#define	LADJUST		0x004		/* left adjustment */
#define	LONGDBL		0x008		/* long double; unimplemented */
#define	LONGINT		0x010		/* long integer */
#define	QUADINT		0x020		/* quad integer; unimplemented */
#define	SHORTINT	0x040		/* short integer */
#define	ZEROPAD		0x080		/* zero (as opposed to blank) pad */
#define ALTZIL_DIGITS	0x100		// Use alternate digits

#ifdef FLOATING_POINT

#if defined(__hpux) || defined(__MWERKS__)
#	include <math.h>
#else
extern "C"
{
#	include <math.h>
}
#endif

#define MAXEXP		4
#define MAXFRACT	24
//#define	BUF		(MAXEXP+MAXFRACT+1)	/* + decimal point */
#define	BUF		350
#define	DEFPREC		6

int isInf(double x)
{
	union {
		short ints[4];
		double x;
	} convert;

	convert.x = x;
	if ((convert.ints[3] & 0x7fff) == 0x7ff0 &&
	    convert.ints[2] == 0 && convert.ints[1] == 0 &&
	    convert.ints[0] == 0)
		return (convert.ints[3] & 0x8000 ? -1 : 1);
	return (0);
}

int isNan(double x)
{
	union {
		short ints[4];
		double x;
	} convert;

	convert.x = x;
	return ((convert.ints[3] & 0x7fff) == 0x7ff0 &&
		(convert.ints[2] != 0 || convert.ints[1] != 0 ||
		 convert.ints[0] != 0));
}

static ZIL_ICHAR *exponent(ZIL_ICHAR *p, int exp, int fmtch)
{
	ZIL_ICHAR *t;
	ZIL_ICHAR expbuf[MAXEXP];

	*p++ = fmtch;
	if (exp < 0) {
		exp = -exp;
		*p++ = '-';
	}
	else
		*p++ = '+';
	t = expbuf + MAXEXP;
	if (exp > 9) {
		do {
			*--t = to_char(exp % 10);
		} while ((exp /= 10) > 9);
		*--t = to_char(exp);
		for (; t < expbuf + MAXEXP; *p++ = *t++);
	}
	else {
		*p++ = '0';
		*p++ = to_char(exp);
	}
	return (p);
}

static ZIL_ICHAR *round(double fract, int *exp, ZIL_ICHAR *start, ZIL_ICHAR *end,
	ZIL_ICHAR ch, ZIL_ICHAR *signp)
{
	double tmp;
	if (fract)
		(void)modf(fract * 10, &tmp);
	else
		tmp = to_digit(ch);
	if (tmp > 4)
		for (;; --end) {
			if (*end == '.')
				--end;
			if (++*end <= '9')
				break;
			*end = '0';
			if (end == start) {
				if (exp) {	/* e/E; increment exponent */
					*end = '1';
					++*exp;
				}
				else {		/* f; add extra digit */
				*--end = '1';
				--start;
				}
				break;
			}
		}
	/* ``"%.3f", (double)-0.0004'' gives you a negative 0. */
	else if (*signp == '-')
		for (;; --end) {
			if (*end == '.')
				--end;
			if (*end != '0')
				break;
			if (end == start)
				*signp = 0;
		}
	return (start);
}

static int cvt(double number, int prec, int flags, ZIL_ICHAR *signp, int fmtch,
	ZIL_ICHAR *startp, ZIL_ICHAR *endp)
{
	ZIL_ICHAR *p, *t;
	double fract;
	int dotrim, expcnt, gformat;
	double integer, tmp;

	dotrim = expcnt = gformat = 0;
	if (number < 0) {
		number = -number;
		*signp = '-';
	} else
		*signp = 0;

	fract = modf(number, &integer);

	/* get an extra slot for rounding. */
	t = ++startp;

	/*
	 * get integer portion of number; put into the end of the buffer; the
	 * .01 is added for modf(356.0 / 10, &integer) returning .59999999...
	 */
	for (p = endp - 1; integer; ++expcnt) {
		tmp = modf(integer / 10, &integer);
		*p-- = to_char((int)((tmp + .01) * 10));
	}
	switch (fmtch) {
	case 'f':
		/* reverse integer into beginning of buffer */
		if (expcnt)
			for (; ++p < endp; *t++ = *p);
		else
			*t++ = '0';
		/*
		 * if precision required or alternate flag set, add in a
		 * decimal point.
		 */
		if (prec || flags&ALT)
			*t++ = '.';
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec)
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				} while (--prec && fract);
			if (fract)
				startp = round(fract, ZIL_NULLP(int), startp,
				    t - 1, (ZIL_ICHAR)0, signp);
		}
		for (; prec--; *t++ = '0');
		// Make sure end of formatted string has null character.
		*t = '\0';
		break;
	case 'e':
	case 'E':
eformat:	if (expcnt) {
			*t++ = *++p;
			if (prec || flags&ALT)
				*t++ = '.';
			/* if requires more precision and some integer left */
			for (; prec && ++p < endp; --prec)
				*t++ = *p;
			/*
			 * if done precision and more of the integer component,
			 * round using it; adjust fract so we don't re-round
			 * later.
			 */
			if (!prec && ++p < endp) {
				fract = 0;
				startp = round((double)0, &expcnt, startp,
				    t - 1, *p, signp);
			}
			/* adjust expcnt for digit in front of decimal */
			--expcnt;
		}
		/* until first fractional digit, decrement exponent */
		else if (fract) {
			/* adjust expcnt for digit in front of decimal */
			for (expcnt = -1;; --expcnt) {
				fract = modf(fract * 10, &tmp);
				if (tmp)
					break;
			}
			*t++ = to_char((int)tmp);
			if (prec || flags&ALT)
				*t++ = '.';
		}
		else {
			*t++ = '0';
			if (prec || flags&ALT)
				*t++ = '.';
		}
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec)
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				} while (--prec && fract);
			if (fract)
				startp = round(fract, &expcnt, startp,
				    t - 1, (ZIL_ICHAR)0, signp);
		}
		/* if requires more precision */
		for (; prec--; *t++ = '0');

		/* unless alternate flag, trim any g/G format trailing 0's */
		if (gformat && !(flags&ALT)) {
			while (t > startp && *--t == '0');
			if (*t == '.')
				--t;
			++t;
		}
		t = exponent(t, expcnt, fmtch);
		break;
	case 'g':
	case 'G':
		/* a precision of 0 is treated as a precision of 1. */
		if (!prec)
			++prec;
		/*
		 * ``The style used depends on the value converted; style e
		 * will be used only if the exponent resulting from the
		 * conversion is less than -4 or greater than the precision.''
		 *	-- ANSI X3J11
		 */
		if (expcnt > prec || !expcnt && fract && fract < .0001) {
			/*
			 * g/G format counts "significant digits, not digits of
			 * precision; for the e/E format, this just causes an
			 * off-by-one problem, i.e. g/G considers the digit
			 * before the decimal point significant and e/E doesn't
			 * count it as precision.
			 */
			fmtch -= 2;		/* G->E, g->e */
			gformat = 1;
			goto eformat;
		}
		/*
		 * reverse integer into beginning of buffer,
		 * note, decrement precision
		 */
		if (expcnt)
			for (; ++p < endp; *t++ = *p, --prec);
		else
			*t++ = '0';
		/*
		 * if precision required or alternate flag set, add in a
		 * decimal point.  If no digits yet, add in leading 0.
		 */
		if (prec > 0 || flags&ALT) {
			dotrim = 1;
			*t++ = '.';
		}
		else
			dotrim = 0;
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec > 0) {
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				} while(!tmp);
				while (--prec && fract) {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				}
			}
			if (fract)
				startp = round(fract, ZIL_NULLP(int), startp,
				    t - 1, (ZIL_ICHAR)0, signp);
		}
		/* alternate format, adds 0's for precision, else trim 0's */
		if (flags&ALT)
			for (; prec--; *t++ = '0');
		else if (dotrim) {
			while (t > startp && *--t == '0');
			if (*t != '.')
				++t;
		}
		// Make sure end of formatted string has null character.
		*t = '\0';
	}
	return (int)(t - startp + 1);
}

#else /* no FLOATING_POINT */

#define	BUF		40

#endif /* FLOATING_POINT */

int ZIL_STDARG::_vsprintf(ZIL_ICHAR *fp, const ZIL_ICHAR *fmt, va_list ap)
{
	int ch;			/* character from fmt */
	int n;			/* handy integer (short term usage) */
	ZIL_ICHAR *cp;		/* handy char pointer (short term usage) */
	int flags;		/* flags as above */
	int ret;		/* return value accumulator */
	int width;		/* width from format (%8d), or 0 */
	int prec;		/* precision from format (%.3d), or -1 */
	ZIL_ICHAR sign;		/* sign prefix (' ', '+', '-', or \0) */
#ifdef FLOATING_POINT
	ZIL_ICHAR softsign;	/* temporary negative sign for floats */
	double _double;		/* double precision arguments %[eEfgG] */
	int fpprec;		/* `extra' floating precision in [eEfgG] */
#endif
	U_QUAD_T _uquad;	/* integer arguments %[diouxX] */
	enum { OCT, DEC, HEX } base;/* base for [diouxX] conversion */
	int dprec;		/* a copy of prec if [diouxX], 0 otherwise */
	int fieldsz;		/* field size expanded by sign, etc */
	int realsz;		/* field size expanded by dprec */
	int size;		/* size of converted field or string */
	char *xdigs;		/* digits for [xX] conversion */
	ZIL_ICHAR buf[BUF];	/* space for %c, %[diouxX], %[eEfgG] */
	int deleteCP;		// delete cp (it was converted from a char *).

	/*
	 * Choose PADSIZE to trade efficiency vs. size.  If larger printf
	 * fields occur frequently, increase PADSIZE and make the initialisers
	 * below longer.
	 */

	/*
	 * BEWARE, these `goto error' on error, and PAD uses `n'.
	 */
#define	PRINT(ptr, len) strncpy(fp, ptr, len), fp += len;
#define	PAD(howmany, with) { \
	n = (howmany); \
	while (n > 0) { \
		*fp++ = with; \
		n--; \
	} \
}
#define	FLUSH() *fp = 0
	/*
	 * To extend shorts properly, we need both signed and unsigned
	 * argument extraction methods.
	 */
#define	SARG() \
	(flags&LONGINT ? va_arg(ap, long) : \
	    flags&SHORTINT ? (long)(short)va_arg(ap, int) : \
	    (long)va_arg(ap, int))
#define	UARG() \
	(flags&LONGINT ? va_arg(ap, unsigned long) : \
	    flags&SHORTINT ? (unsigned long)(unsigned short)va_arg(ap, int) : \
	    (unsigned long)va_arg(ap, int))

	ret = 0;

	/*
	 * Scan the format for conversions (`%' character).
	 */
	for (;;) {
		deleteCP = FALSE;
		for (cp = (ZIL_ICHAR *)fmt; (ch = *fmt) != '\0' && ch != '%'; fmt++)
			/* void */;
		if ((n = (int)(fmt - cp)) != 0) {
			PRINT(cp, n);
			ret += n;
		}
		if (ch == '\0')
			goto done;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
#ifdef FLOATING_POINT
		fpprec = 0;
#endif
		width = 0;
		prec = -1;
		sign = '\0';

rflag:		ch = *fmt++;
reswitch:	switch (ch) {
		case ' ':
			/*
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
			 *	-- ANSI X3J11
			 */
			if (!sign)
				sign = ' ';
			goto rflag;
		case '#':
			flags |= ALT;
			goto rflag;
		case '*':
			/*
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			if ((width = va_arg(ap, int)) >= 0)
				goto rflag;
			width = -width;
			/* FALLTHROUGH */
		case '-':
			flags |= LADJUST;
			goto rflag;
		case '+':
			sign = '+';
			goto rflag;
		case '.':
			if ((ch = *fmt++) == '*') {
				n = va_arg(ap, int);
				prec = n < 0 ? -1 : n;
				goto rflag;
			}
			n = 0;
			while (is_digit(ch)) {
				n = 10 * n + to_digit(ch);
				ch = *fmt++;
			}
			prec = n < 0 ? -1 : n;
			goto reswitch;
		case '0':
			/*
			 * ``Note that 0 is taken as a flag, not as the
			 * beginning of a field width.''
			 *	-- ANSI X3J11
			 */
			flags |= ZEROPAD;
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do {
				n = 10 * n + to_digit(ch);
				ch = *fmt++;
			} while (is_digit(ch));
			width = n;
			goto reswitch;
#ifdef FLOATING_POINT
		case 'L':
			flags |= LONGDBL;
			goto rflag;
#endif
		case 'h':
			flags |= SHORTINT;
			goto rflag;
		case 'l':
			flags |= LONGINT;
			goto rflag;
		case 'q':
			flags |= QUADINT;
			goto rflag;
		case 'c':
			*(cp = buf) = va_arg(ap, int);
			size = 1;
			sign = '\0';
			break;
		case 'D':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'd':
		case 'i':
			_uquad = SARG();
			if ((QUAD_T)_uquad < 0) {
				_uquad = -_uquad;
				sign = '-';
			}
			base = DEC;
			goto number;
		case 'a':
			flags |= ALTZIL_DIGITS;
			goto rflag;
#ifdef FLOATING_POINT
		case 'e':
		case 'E':
		case 'f':
		case 'g':
		case 'G':
			_double = va_arg(ap, double);
			/* do this before tricky precision changes */
			if (isInf(_double)) {
				if (_double < 0)
					sign = '-';
#if defined(ZIL_UNICODE)
				cp = ISOtoUNICODE("Inf");
				deleteCP = TRUE;
#else
				cp = "Inf";
#endif
				size = 3;
				break;
			}
			if (isNan(_double)) {
#if defined(ZIL_UNICODE)
				cp = ISOtoUNICODE("NaN");
				deleteCP = TRUE;
#else
				cp = "Nan";
#endif
				size = 3;
				break;
			}
			/*
			 * don't do unrealistic precision; just pad it with
			 * zeroes later, so buffer size stays rational.
			 */
			if (prec > MAXFRACT) {
				if (ch != 'g' && ch != 'G' || (flags&ALT))
					fpprec = prec - MAXFRACT;
				prec = MAXFRACT;
			} else if (prec == -1)
				prec = DEFPREC;
			/*
			 * cvt may have to round up before the "start" of
			 * its buffer, i.e. ``intf("%.2f", (double)9.999);'';
			 * if the first character is still NUL, it did.
			 * softsign avoids negative 0 if _double < 0 but
			 * no significant digits will be shown.
			 */
			cp = buf;
			*cp = '\0';
			size = cvt(_double, prec, flags, &softsign, ch,
			    cp, buf + LENGTHOF(buf));
			if (softsign)
				sign = '-';
			if (*cp == '\0')
				cp++;
			break;
#endif /* FLOATING_POINT */
		case 'n':
			if (flags & QUADINT)
				*va_arg(ap, QUAD_T *) = ret;
			else if (flags & LONGINT)
				*va_arg(ap, long *) = ret;
			else if (flags & SHORTINT)
				*va_arg(ap, short *) = ret;
			else
				*va_arg(ap, int *) = ret;
			continue;	/* no output */
		case 'O':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG();
			base = OCT;
			goto nosign;
		case 'p':
			/*
			 * ``The argument shall be a pointer to void.  The
			 * value of the pointer is converted to a sequence
			 * of printable characters, in an implementation-
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_uquad = (U_QUAD_T)va_arg(ap, void *);
			base = HEX;
			xdigs = "0123456789abcdef";
			flags |= HEXPREFIX;
			ch = 'x';
			goto nosign;
		case 's':
			if ((cp = va_arg(ap, ZIL_ICHAR *)) == ZIL_NULLP(ZIL_ICHAR))
			{
#if defined(ZIL_UNICODE)
				cp = ISOtoUNICODE("(null)");
				deleteCP = TRUE;
#else
				cp = "(null)";
#endif
			}
			if (prec >= 0) {
				/*
				* can't use strlen; can only look for the
				* NUL in the first `prec' characters, and
				* strlen() will go further.
				*/
				ZIL_ICHAR *p = cp;
				for (int _prec = prec; *p && _prec; p++, _prec--)
					;
				if (*p != '\0') {
					size = (int)(p - cp);
					if (size > prec)
						size = prec;
				} else
					size = prec;
			} else
				size = strlen(cp);
			sign = '\0';
			break;
		case 'U':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG();
			base = DEC;
			goto nosign;
		case 'X':
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG();
			base = HEX;
			/* leading 0x/X only if non-zero */
			if (flags & ALT && _uquad != 0)
				flags |= HEXPREFIX;

			/* unsigned conversions */
nosign:			sign = '\0';
			/*
			 * ``... diouXx conversions ... if a precision is
			 * specified, the 0 flag will be ignored.''
			 *	-- ANSI X3J11
			 */
number:			if ((dprec = prec) >= 0)
				flags &= ~ZEROPAD;

			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
			if (_uquad != 0 || prec != 0) {
				/*
				 * Unsigned mod is hard, and unsigned mod
				 * by a constant is easier than that by
				 * a variable; hence this switch.
				 */
				switch (base) {
				case OCT:
					do {
						*--cp = (char)to_char(_uquad & 7);
						_uquad >>= 3;
					} while (_uquad);
					/* handle octal leading 0 */
					if (flags & ALT && *cp != '0')
						*--cp = '0';
					break;

				case DEC:
					{
					ZIL_ICHAR *useDigits = (flags & ALTZIL_DIGITS ? defaultLocale->data.altDigits : defaultLocale->data.defDigits);
					if (_uquad == 0)
					{
						*--cp = useDigits[0];
						break;
					}
					int dontUsePOT = FALSE, i = 0;
					for (; i < LENGTHOF(POT); i++)
						if (useDigits[10+i] == 0)
						{
							dontUsePOT = (POT[i] <= _uquad);
							break;
						}
					i = 10;
					do {
						int tmp = (int)(_uquad % 10);
						if (dontUsePOT || (tmp != 0 && !(tmp == 1 && i == 12 && (_uquad / 10 == 0))))
							*--cp = useDigits[tmp];
						_uquad /= 10;
						if (!dontUsePOT && (_uquad % 10))
							*--cp = useDigits[i];
						i++;
					} while (_uquad);
					}
					break;

				case HEX:
					do {
						*--cp = xdigs[(int)(_uquad & 15)];
						_uquad >>= 4;
					} while (_uquad);
					break;

				default:
//					cp = ISOtoUNICODE("bug in vfprintf: bad base");
#if defined(ZIL_UNICODE)
					cp = ISOtoUNICODE("*****");
					deleteCP = TRUE;
#else
					cp = "*****";
#endif
					size = strlen(cp);
					goto skipsize;
				}
			}
			size = (int)(buf + BUF - cp);
		skipsize:
			break;
		default:	/* "%?" prints ?, unless ? is NUL */
			if (ch == '\0')
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
			*cp = ch;
			size = 1;
			sign = '\0';
			break;
		}

		/*
		 * All reasonable formats wind up here.  At this point,
		 * `cp' points to a string which (if not flags&LADJUST)
		 * should be padded out to `width' places.  If
		 * flags&ZEROPAD, it should first be prefixed by any
		 * sign or other prefix; otherwise, it should be blank
		 * padded before the prefix is emitted.  After any
		 * left-hand padding and prefixing, emit zeroes
		 * required by a decimal [diouxX] precision, then print
		 * the string proper, then emit zeroes required by any
		 * leftover floating precision; finally, if LADJUST,
		 * pad with blanks.
		 */

		/*
		 * compute actual size, so we know how much to pad.
		 * fieldsz excludes decimal prec; realsz includes it
		 */
#ifdef FLOATING_POINT
		fieldsz = size + fpprec;
#else
		fieldsz = size;
#endif
		if (sign)
			fieldsz++;
		else if (flags & HEXPREFIX)
			fieldsz += 2;
		realsz = dprec > fieldsz ? dprec : fieldsz;

		/* right-adjusting blank padding */
		if ((flags & (LADJUST|ZEROPAD)) == 0)
			PAD(width - realsz, ' ');

		/* prefix */
		if (sign)
			PAD(1, sign)
		else if (flags & HEXPREFIX) {
			PAD(1, '0');
			PAD(1, ch);
		}

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD(width - realsz, '0');

		/* leading zeroes from decimal precision */
		PAD(dprec - fieldsz, '0');

		/* the string or number proper */
		PRINT(cp, size);
		if (deleteCP)
			delete cp;

#ifdef FLOATING_POINT
		/* trailing f.p. zeroes */
		PAD(fpprec, '0');
#endif

		/* left-adjusting padding (always blank) */
		if (flags & LADJUST)
			PAD(width - realsz, ' ');

		/* finally, adjust ret */
		ret += width > realsz ? width : realsz;

		FLUSH();	/* copy out the I/O vectors */
	}
done:
	FLUSH();
error:
	return (ret);
	/* NOTREACHED */
}
#endif

