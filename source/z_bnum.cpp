//	Zinc Interface Library - Z_BNUM.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

/*       This file is a part of OpenZinc

          OpenZinc is free software; you can redistribute it and/or modify it under
          the terms of the GNU Lessor General Public License as published by
          the Free Software Foundation, either version 3 of the License, or (at
          your option) any later version

	OpenZinc is distributed in the hope that it will be useful, but WITHOUT
          ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
          or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser 
          General Public License for more details.

          You should have received a copy of the GNU Lessor General Public License
	 along with OpenZinc. If not, see <http://www.gnu.org/licenses/>                          */


#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#	include <mem.h>
#elif defined(macintosh) || defined(__MWERKS__)
#	if !defined(EINVAL)
#		define EINVAL 22
#	endif
#	if !defined(ENOMEM)
#		define ENOMEM 8
#	endif
#endif
#include "ui_gen.hpp"

// WARNING: This must change if ZIL_DIGITS changes
// WHOLE_WORD is 10^ZIL_DIGITS, MOD_WORD is sqrt(WHOLE_WORD)
#if (ZIL_DIGITS == 8)
	const unsigned int WHOLE_WORD = 100000000U;
	const unsigned int MOD_WORD = 10000U;
#elif (ZIL_DIGITS == 4)
	const unsigned int WHOLE_WORD = 10000U;
	const unsigned int MOD_WORD = 100U;
#endif

const unsigned int MAXSTORED = (WHOLE_WORD-1);

#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))

// TO DO
// div
// mod
// sqrt

// DONE
// add		*
// sub		*
// neg		-
// abs		-
// zero		-
// ceil		-
// floor	-
// truncate	-
// round	-
// >		*
// >=		*
// <		*
// <=		*
// ==		*
// !=		*
// mul		*
// ascii to ui_num
// ui_num to ascii
// underflow/overflow exceptions

static ZIL_NUMBER poten[ZIL_DIGITS+1] = {
	1,		// 0
#if (ZIL_DIGITS > 0)
	10,		// 1
#endif
#if (ZIL_DIGITS > 1)
	100,		// 2
#endif
#if (ZIL_DIGITS > 2)
	1000,		// 3
#endif
#if (ZIL_DIGITS > 3)
	10000,		// 4
#endif
#if (ZIL_DIGITS > 4)
	100000,		// 5
#endif
#if (ZIL_DIGITS > 5)
	1000000,	// 6
#endif
#if (ZIL_DIGITS > 6)
	10000000,	// 7
#endif
#if (ZIL_DIGITS > 7)
	100000000,	// 8
#endif
#if (ZIL_DIGITS > 8)
	1000000000,	// 9
#endif
#if (ZIL_DIGITS > 9)
	10000000000,	// 10
#endif
};

static ZIL_ICHAR _bnumSpaceString[] 	= { ' ', 0 };
static ZIL_ICHAR _bnumPlus[] 		= { '+', 0 };
static ZIL_ICHAR _bnumMinValueus[] 		= { '-', 0 };
static ZIL_ICHAR _bnumTimes[] 		= { '*', 0 };
static ZIL_ICHAR _bnumfloor[]		= { 'f','l','o','o','r','/','c','e','i','l', 0 };
static ZIL_ICHAR _bnumAbsValue[]			= { 'a','b','s', 0 };
static ZIL_ICHAR _bnumTruncate[]		= { 't','r','u','n','c','a','t','e', 0 };
static ZIL_ICHAR _bnumRound[]		= { 'r','o','u','n','d', 0 };
static ZIL_ICHAR _bnumExport[]		= { 'E','x','p','o','r','t', 0 };
//static ZIL_ICHAR _bnumImport[]		= { 'I','m','p','o','r','t', 0 };

// ----- Stack support -------------------------------------------------------

const int STACKSIZE = 10;
#define INCSTACK(s, e)	if (++stackpos >= STACKSIZE) os_numerror(s, e)
static int stackpos = -1;
static ZIL_BIGNUM *stack = ZIL_NULLP(ZIL_BIGNUM);
static int useCount = 0;
//line below changed to conform to ISO C++ standards 
//static inMakeStack = FALSE;
static int inMakeStack = FALSE;

void ZIL_BIGNUM::MakeStack(void)
{
	if (inMakeStack)
		return;
	inMakeStack = TRUE;
	if (useCount == 0)
		stack = new ZIL_BIGNUM[STACKSIZE];
	useCount++;
	inMakeStack = FALSE;
}

void ZIL_BIGNUM::DeleteStack(void)
{
	if (inMakeStack)
		return;
	inMakeStack = TRUE;
	useCount--;
	if (useCount == 0)
		delete []stack;
	inMakeStack = FALSE;
}

// ----- ZIL_BIGNUM ----------------------------------------------------------

ZIL_BIGNUM::ZIL_BIGNUM(void)
{
	MakeStack();
	myLocale = ZIL_LOCALE_MANAGER::UseLocale();
	Zero();
}

ZIL_BIGNUM::ZIL_BIGNUM(ZIL_IBIGNUM value)
{
	MakeStack();
	myLocale = ZIL_LOCALE_MANAGER::UseLocale();
	Import(value);
}

ZIL_BIGNUM::ZIL_BIGNUM(const ZIL_ICHAR *string, const ZIL_ICHAR *decimalString,
		     const ZIL_ICHAR *signString)
{
	MakeStack();
	myLocale = ZIL_LOCALE_MANAGER::UseLocale();
	Import(string, decimalString, signString);
}

ZIL_BIGNUM::ZIL_BIGNUM(const ZIL_BIGNUM &number)
{
	MakeStack();
	*this = number;
	myLocale = ZIL_LOCALE_MANAGER::UseLocale(number.myLocale);
}

ZIL_BIGNUM::~ZIL_BIGNUM(void)
{
	DeleteStack();
	ZIL_LOCALE_MANAGER::FreeLocale(myLocale);
}

//--------------------------------------------------------------------------
// Helper routines

// Add two bignums, ignoring sign.  This is used by both add
// and subtract routines, and each defines how the carry is set
// comming in.  Returns a new carry.
int ZIL_BIGNUM::DoAddLoop(const ZIL_NUMBER *aw, int carry)
{
	if (aw)
		aw += LENGTHOF(num)-1;
	ZIL_NUMBER *r = &num[LENGTHOF(num)-1];
	for (int i=0; i < LENGTHOF(num); i++)
	{
		*r += carry;
		if (aw)
			*r += *aw--;
		carry = (*r > MAXSTORED);
		if (carry)
			*r -= MAXSTORED+1;
		r--;
	}
	return (carry);
}

// Replace a number with its nines-complement.
void ZIL_BIGNUM::NinesComplement(void)
{
	ZIL_NUMBER *r = num;
	for (int i=0; i < LENGTHOF(num); i++, r++)
		*r = MAXSTORED - *r;
}

// Copy a number and add to it.
int ZIL_BIGNUM::AddSameSign(const ZIL_BIGNUM &b, ZIL_BIGNUM &result)
{
	result.Import(*this);
	return (result.DoAddLoop(b.num, 0));
}

// Zero out a bignum.
void ZIL_BIGNUM::Zero(void)
{
	sign = 0;
	for (int i=0; i < LENGTHOF(num); i++)
		num[i] = 0;
}

// Add two bignums of different sign (or subtract like signed values).
int ZIL_BIGNUM::AddDiffSign(const ZIL_BIGNUM &b, ZIL_BIGNUM &result)
{
	ZIL_NUMBER tmp = 0;

	int i = 0;
	for (; i < LENGTHOF(num); i++)
		if ((tmp = num[i] - b.num[i]) != 0)
			break;
	if (i >= LENGTHOF(num))
	{
		result.Zero();
		return (0);
	}
	const ZIL_BIGNUM *tmp1 = (tmp > 0 ? &b : this);
	const ZIL_BIGNUM *tmp2 = (tmp > 0 ? this : &b);
	result.Import(*tmp1);
	result.sign = tmp2->sign;
	// set 9's compliment
	result.NinesComplement();
	i = result.DoAddLoop(tmp2->num, 1);
	if (i == 0)
	{
		result.NinesComplement();
		i = result.DoAddLoop(ZIL_NULLP(ZIL_NUMBER), 1);
		result.sign = 1-result.sign;
		i = 1-i;
	}
	return (1-i);
}

#ifdef TESTING
static void os_numerror(ZIL_ICHAR *s, int error)
{
	errno = error;
	perror(s);
	abort();
}
#else
static void os_numerror(ZIL_ICHAR *, int error)
{
	errno = error;
}
#endif

// Calculate the Floor (nsign = 1) or the Ceiling (nsign = 0).
ZIL_BIGNUM &ZIL_BIGNUM::FloorCeil(int nsign) const
{
	INCSTACK(_bnumfloor, ENOMEM);
	stack[stackpos].Import(*this);
	int j = LENGTHOF(num) - (ZIL_NUMBER_DECIMAL+ZIL_DIGITS) / ZIL_DIGITS;
	int k = ZIL_NUMBER_DECIMAL % ZIL_DIGITS;
	ZIL_NUMBER tmp = stack[stackpos].num[j] % poten[k];
	stack[stackpos].num[j] -= tmp;
	if (k == 0)
		// Can't use the ?: operator here because GNC++ 2.5.8 is easily confused.
		if (j+1 < LENGTHOF(num))
			tmp = stack[stackpos].num[j+1];
		else
			tmp = 0;
	int i = j + 1;
	for (; i < LENGTHOF(num); i++)
		stack[stackpos].num[i] = 0;
	if ((stack[stackpos].sign == nsign) && tmp != 0)
	{
		ZIL_NUMBER n[LENGTHOF(num)];

		for (i=0; i < LENGTHOF(n); i++)
			n[i] = 0;
		n[j] = poten[k];
		(void) stack[stackpos].DoAddLoop(n, 0);
	}
	return (stack[stackpos]);
}

// Compare two bignums.  Return appropriate value.
int ZIL_BIGNUM::Compare(const ZIL_BIGNUM &b, int gt, int ls, int eq)
{
	if (sign < b.sign)
		return (gt);
	if (sign > b.sign)
		return (ls);
	for (int i=0; i < LENGTHOF(num); i++)
	{
		if (num[i] > b.num[i])
			return (sign ? ls : gt);
		if (num[i] < b.num[i])
			return (sign ? gt : ls);
	}
	return (eq);
}

// If this bignum is zero, clear the sign flag.
void ZIL_BIGNUM::CheckZero(void)
{
	// Don't allow "-0", it is flag for empty
	for (int i=0; i < LENGTHOF(num); i++)
		if (num[i] != 0)
			return;
	sign = 0;
}	

//--------------------------------------------------------------------------
// Regular ZIL_BIGNUM op ZIL_BIGNUM operations

ZIL_BIGNUM &ZIL_BIGNUM::operator=(const ZIL_BIGNUM &number)
{
	Import(number);
	// set stack to empty
	stackpos = -1;
	return (*this);
}

ZIL_BIGNUM &ZIL_BIGNUM::operator+(const ZIL_BIGNUM &number)
{
	INCSTACK(_bnumPlus, ENOMEM);
	if (sign == number.sign)
	{
		if (AddSameSign(number, stack[stackpos]))
			os_numerror(_bnumPlus, ERANGE);
	}
	else
	{
		if (AddDiffSign(number, stack[stackpos]))
			os_numerror(_bnumPlus, ERANGE);
	}
	return (stack[stackpos]);
}

ZIL_BIGNUM &ZIL_BIGNUM::operator-(const ZIL_BIGNUM &number)
{
	INCSTACK(_bnumMinValueus, ENOMEM);
	if (sign != number.sign)
	{
		if (AddSameSign(number, stack[stackpos]))
			os_numerror(_bnumMinValueus, ERANGE);
	}
	else
	{
		if (AddDiffSign(number, stack[stackpos]))
			os_numerror(_bnumMinValueus, ERANGE);
	}
	return (stack[stackpos]);
}

ZIL_BIGNUM &ZIL_BIGNUM::operator++(void)
{
	return *this += ZIL_BIGNUM((ZIL_INT32)1);
}

ZIL_BIGNUM &ZIL_BIGNUM::operator--(void)
{
	return *this -= ZIL_BIGNUM((ZIL_INT32)1);
}

ZIL_BIGNUM &ZIL_BIGNUM::abs(void) const
{
	INCSTACK(_bnumAbsValue, ENOMEM);
	stack[stackpos].Import(*this);
	stack[stackpos].sign = 0;
	return (stack[stackpos]);
}

ZIL_BIGNUM &ZIL_BIGNUM::Truncate(int places) const
{
	INCSTACK(_bnumTruncate, ENOMEM);
	stack[stackpos].Import(*this);
	places = ZIL_NUMBER_DECIMAL - places;
	if (places <= 0)
		return (stack[stackpos]);
	if (places > ZIL_NUMBER_DECIMAL + ZIL_NUMBER_WHOLE)
	{
		stack[stackpos].Zero();
		return (stack[stackpos]);
	}
	int j = LENGTHOF(num) - (places+ZIL_DIGITS) / ZIL_DIGITS;
	for (int i=j+1; i < LENGTHOF(num); i++)
		stack[stackpos].num[i] = 0;
#if defined(__ZTC__) && defined(DOS386)
	// This seems to prevent the optimizer from not compiling the
	// other line right.
	int tmp = poten[places % ZIL_DIGITS];
	stack[stackpos].num[j] -= (stack[stackpos].num[j] % tmp);
#else
	stack[stackpos].num[j] -= (stack[stackpos].num[j] % poten[places % ZIL_DIGITS]);
#endif
	stack[stackpos].CheckZero();
	return (stack[stackpos]);
}

ZIL_BIGNUM &ZIL_BIGNUM::Round(int places) const
{
	INCSTACK(_bnumRound, ENOMEM);
	stack[stackpos].Import(*this);
	places = ZIL_NUMBER_DECIMAL - places;
	if (places <= 0)
		return (stack[stackpos]);
	if (places > ZIL_NUMBER_DECIMAL + ZIL_NUMBER_WHOLE)
	{
		stack[stackpos].Zero();
		return (stack[stackpos]);
	}
	int j = LENGTHOF(num) - (places+ZIL_DIGITS) / ZIL_DIGITS;
	int k = (places+ZIL_DIGITS) % ZIL_DIGITS;
	ZIL_NUMBER tmp;
	if (k == 0)
	{
		tmp = stack[stackpos].num[j+1] % poten[ZIL_DIGITS];
		k = ZIL_DIGITS;
	}
	else
	{
		tmp = stack[stackpos].num[j] % poten[k];
		stack[stackpos].num[j] -= tmp;
	}
	int i = j + 1;
	for (; i < LENGTHOF(num); i++)
		stack[stackpos].num[i] = 0;
	if (tmp >= poten[k] / 2)
	{
		ZIL_NUMBER n[LENGTHOF(num)];

		for (i=0; i < LENGTHOF(n); i++)
			n[i] = 0;
		n[j] = (k == ZIL_DIGITS ? poten[0] : poten[k]);
		(void) stack[stackpos].DoAddLoop(n, 0);
	}
	stack[stackpos].CheckZero();
	return (stack[stackpos]);
}

// Support for mixed mode (number op int) operations
void ZIL_BIGNUM::Itob(ZIL_IBIGNUM value)
{
	Zero();
	if (value < 0)
	{
		sign = 1;
		value = -value;
	}
	ZIL_NUMBER *numbase = &num[LENGTHOF(num)-1-(ZIL_NUMBER_DECIMAL/ZIL_DIGITS)];
#if ZIL_DIGITS == 4
#	if (ZIL_NUMBER_DECIMAL % ZIL_DIGITS == 0)
	numbase[0]  = (ZIL_NUMBER)(value % WHOLE_WORD);
	numbase[-1] = (ZIL_NUMBER)((value / WHOLE_WORD) % WHOLE_WORD);
	numbase[-2] = (ZIL_NUMBER)((value / WHOLE_WORD) / WHOLE_WORD);
#	else
	ZIL_IBIGNUM potbase = poten[ZIL_NUMBER_DECIMAL%ZIL_DIGITS];
	ZIL_IBIGNUM potpowr = poten[ZIL_DIGITS-ZIL_NUMBER_DECIMAL%ZIL_DIGITS];
	numbase[0]  = (ZIL_NUMBER)((value % potpowr) * potbase);
	numbase[-1] = (ZIL_NUMBER)((value / potpowr) % WHOLE_WORD);
	numbase[-2] = (ZIL_NUMBER)(((value / potpowr) / WHOLE_WORD) % WHOLE_WORD);
	numbase[-3] = (ZIL_NUMBER)(((value / potpowr) / WHOLE_WORD) / WHOLE_WORD);
#	endif
#endif
#if ZIL_DIGITS == 8
#	if (ZIL_NUMBER_DECIMAL % ZIL_DIGITS == 0)
	numbase[0]  = (ZIL_NUMBER)(value % WHOLE_WORD);
	numbase[-1] = (ZIL_NUMBER)(value / WHOLE_WORD);
#	else
	ZIL_IBIGNUM potbase = poten[ZIL_NUMBER_DECIMAL%ZIL_DIGITS];
	ZIL_IBIGNUM potpowr = poten[ZIL_DIGITS-ZIL_NUMBER_DECIMAL%ZIL_DIGITS];
	numbase[0]  = (ZIL_NUMBER)((value % potpowr) * potbase);
	numbase[-1] = (ZIL_NUMBER)((value / potpowr) % WHOLE_WORD);
	numbase[-2] = (ZIL_NUMBER)((value / potpowr) / WHOLE_WORD);
#	endif
#endif
}

//--------------------------------------------------------------------------
// Conversion routines

ZIL_ICHAR *ZIL_BIGNUM::Copy(ZIL_ICHAR *to, const ZIL_ICHAR *from)
{
	if (!*from)
		return (to);
	const ZIL_ICHAR *_from = from + strlen(from);

	do {
		*--to = *--_from;
	} while (_from != from);
	return (to);
}

void ZIL_BIGNUM::Export(ZIL_ICHAR *string, NMF_FLAGS nmFlags,
		       const ZIL_ICHAR *decimalStr, const ZIL_ICHAR *signStr)
{
	int i, dgt, j, comma;
	ZIL_NUMBER tmp = 0;
	const ZIL_ICHAR *t;
	const ZIL_ICHAR *ps1, *pc, *ps, *ps2;
	const ZIL_ICHAR *ss2, *ss, *sc, *ss1;
	const ZIL_ICHAR *pt, *cm;
	const char *gp; int oldgp;

	if (sign == -1)
	{
		// Unanswered value
		*string = 0;
		return;
	}
	if (!myLocale)
	{
		myLocale = ZIL_LOCALE_MANAGER::UseLocale();
		if (!myLocale)
			return;
	}
	int digits = NMF_DIGITS(nmFlags);
	// bigger than the output string?
	ZIL_ICHAR *buff = new ZIL_ICHAR[2*(ZIL_NUMBER_WHOLE+digits)];
	ZIL_ICHAR *cpos = &buff[2*(ZIL_NUMBER_WHOLE+digits)-1];

	t = ps1 = pc = ps = ps2 = ss1 = sc = ss = ss2 = pt = cm = _blankString;
	if (FlagSet(nmFlags, NMF_CURRENCY))
	{
		pt = (decimalStr ? decimalStr : myLocale->data.monDecimalSeparator);
		cm = myLocale->data.monThousandsSeparator;
		gp = myLocale->data.monGrouping;
		if (sign)
		{
			if (myLocale->data.negCurrencyPrecedes)
			{
				pc = myLocale->data.currencySymbol;
				if (myLocale->data.negSpaceSeparation)
					ps = _bnumSpaceString;
			}
			else
			{
				sc = myLocale->data.currencySymbol;
				if (myLocale->data.negSpaceSeparation)
					ss = _bnumSpaceString;
			}
		}
		else
		{
			if (myLocale->data.posCurrencyPrecedes)
			{
				pc = myLocale->data.currencySymbol;
				if (myLocale->data.posSpaceSeparation)
					ps = _bnumSpaceString;
			}
			else
			{
				sc = myLocale->data.currencySymbol;
				if (myLocale->data.posSpaceSeparation)
					ss = _bnumSpaceString;
			}
		}
		if (digits < 0)
			digits = myLocale->data.fractionDigits;
	}
	else
	{
		pt = (decimalStr ? decimalStr : myLocale->data.decimalSeparator);
		cm = myLocale->data.thousandsSeparator;
		gp = myLocale->data.grouping;
		if (digits < 0)
			digits = ZIL_NUMBER_DECIMAL;
	}
	if (sign)
	{
		i = myLocale->data.negSignPrecedes;
		t = (signStr ? signStr : myLocale->data.negativeSign);
		if (FlagSet(nmFlags, NMF_CREDIT))
			i = 0;
	}
	else
	{
		i = myLocale->data.posSignPrecedes;
		t = myLocale->data.positiveSign;
	}
	switch (i)
	{
	case 0:
		if (sign)
		{
			ps1 = myLocale->data.bnumLeftParen;
			ss1 = myLocale->data.bnumRightParen;
		}
		break;

	case 1:
		ps1 = t;
		break;

	case 2:
		ss1 = t;
		break;

	case 3:
		ps2 = t;
		break;

	case 4:
		ss2 = t;
		break;

	default:
		os_numerror(_bnumExport, EINVAL);
	}
	*cpos = '\0';
	cpos = Copy(cpos, ss1);
	if (FlagSet(nmFlags, NMF_PERCENT))
		*--cpos = '%';
	cpos = Copy(cpos, sc);
	cpos = Copy(cpos, ss);
	cpos = Copy(cpos, ss2);

	t = cpos;		// to see if anything is added
	while (digits > ZIL_NUMBER_DECIMAL)
	{
		*--cpos = '0';
		digits--;
	}
	dgt = ZIL_NUMBER_DECIMAL;
	i = LENGTHOF(num);
	while (dgt > digits)
	{
		if (dgt % ZIL_DIGITS == 0)
		{
			i--;
			tmp = num[i];
		}
		tmp /= 10;
		dgt--;
	}
	while (dgt > 0)
	{
		if (dgt % ZIL_DIGITS == 0)
		{
			i--;
			tmp = num[i];
		}
		*--cpos = (char)(tmp % 10) + '0';
		tmp /= 10;
		dgt--;
	}
	if (cpos != t)
		cpos = Copy(cpos, pt);

	t = cpos;		// to see if anything is added
	j = 0;
	while (j < LENGTHOF(num) && num[j] == 0) j++;
	oldgp = (unsigned)*gp;
	if (oldgp == 0 || !FlagSet(nmFlags, NMF_COMMAS))
		oldgp = CHAR_MAX;
	else
		 gp++;
	comma = 0;
	while (i >= j)
	{
		if (dgt % ZIL_DIGITS == 0)
		{
			i--;
			tmp = num[i];
		}
		*--cpos = (char)(tmp % 10) + '0';
		tmp /= 10;
	if (i == j && tmp == 0) break;
		comma++;
		if (oldgp != CHAR_MAX && comma == oldgp)
		{
			cpos = Copy(cpos, cm);
			comma = 0;
			if (*gp)
				oldgp = (unsigned)*gp++;
		}
		dgt--;
	}
	if (cpos == t)
		*--cpos = '0';	// need a leading zero
	cpos = Copy(cpos, ps2);
	cpos = Copy(cpos, ps);
	cpos = Copy(cpos, pc);
	cpos = Copy(cpos, ps1);
	strcpy(string, cpos);
	delete buff;
}

void ZIL_BIGNUM::Export(ZIL_IBIGNUM *value)
{
	ZIL_IBIGNUM l;

	ZIL_NUMBER *numbase = &num[LENGTHOF(num)-1-(ZIL_NUMBER_DECIMAL/ZIL_DIGITS)];
#if ZIL_DIGITS == 4
#	if (ZIL_NUMBER_DECIMAL % ZIL_DIGITS == 0)
	l = numbase[0] + (ZIL_INT32)numbase[-1] * WHOLE_WORD;
#	else
	ZIL_IBIGNUM potbase = poten[ZIL_NUMBER_DECIMAL%ZIL_DIGITS];
	ZIL_IBIGNUM potpowr = poten[ZIL_DIGITS-ZIL_NUMBER_DECIMAL%ZIL_DIGITS];
	l = (numbase[0] / potbase) + ((ZIL_INT32)numbase[-1] * potpowr) +
		(ZIL_INT32)(numbase[-2] % potbase) * potpowr * WHOLE_WORD;
#	endif
#endif
#if ZIL_DIGITS == 8
#	if (ZIL_NUMBER_DECIMAL % ZIL_DIGITS == 0)
	l = numbase[0];
#	else
	ZIL_IBIGNUM potbase = poten[ZIL_NUMBER_DECIMAL%ZIL_DIGITS];
	ZIL_IBIGNUM potpowr = poten[ZIL_DIGITS-ZIL_NUMBER_DECIMAL%ZIL_DIGITS];
	l = (numbase[0] / potbase) + (numbase[-1] % potbase) * potpowr;
#	endif
#endif
	*value = (sign ? -l : l);
}

NMI_RESULT ZIL_BIGNUM::Import(const ZIL_BIGNUM &number)
{
	memcpy(num, number.num, sizeof(num));
	sign = number.sign;
	return (NMI_OK);
}

NMI_RESULT ZIL_BIGNUM::Import(ZIL_IBIGNUM value)
{
	Itob(value);
	return (NMI_OK);
}

NMI_RESULT ZIL_BIGNUM::Import(const ZIL_ICHAR *string, const ZIL_ICHAR *decimalstr, const ZIL_ICHAR *signstr)
{
	int i, j, decimal;
	const ZIL_ICHAR *tmp;
	const ZIL_ICHAR *monstr;

	Zero();
	if (!string)
	{
		// Unanswered value
		sign = -1;
		return (NMI_OK);
	}
	if (!myLocale)
	{
		myLocale = ZIL_LOCALE_MANAGER::UseLocale();
		if (!myLocale)
			return (NMI_INVALID);
	}
	if (!decimalstr)
	{
		decimalstr = myLocale->data.decimalSeparator;
		monstr = myLocale->data.monDecimalSeparator;
		if (!monstr[0])
			monstr = decimalstr;
	}
	else
		monstr = decimalstr;
	if (!signstr)
		signstr = myLocale->data.negativeSign;
	tmp = string;
	sign = (*string == '(');
	while (*tmp && !sign)
	{
		if (strncmp(tmp, signstr, strlen(signstr)) == 0)
			sign = 1;
		tmp++;
	}
	tmp = string;
	decimal = 0;
	while (*tmp && strncmp(tmp, monstr, strlen(monstr)) != 0 &&
		strncmp(tmp, decimalstr, strlen(decimalstr)) != 0)
	{
		if (IsAscii(*tmp) && IsDigit(*tmp))
			decimal++;
		tmp++;
	}
	if (decimal > ZIL_NUMBER_WHOLE)
		return (NMI_OUT_OF_RANGE);
		// os_numerror(_bnumImport, ERANGE);
	i = LENGTHOF(num) - (decimal + ZIL_NUMBER_DECIMAL + ZIL_DIGITS-1) / ZIL_DIGITS;
	j = (decimal + ZIL_NUMBER_DECIMAL - 1) % ZIL_DIGITS;
	while (*string && i < LENGTHOF(num))
	{
		if (IsAscii(*string) && IsDigit(*string))
			num[i] += poten[j] * (*string - '0');
		else
			j++;
		string++;
		j--;
		if (j < 0)
		{
			j = ZIL_DIGITS-1;
			i++;
		}
	}
	CheckZero();
	return (NMI_OK);
}

ZIL_BIGNUM &ZIL_BIGNUM::operator*(const ZIL_BIGNUM &number)
// If we have two 2-word bignum, A and B, we can break the multiplication
// problem down into the following:
//    A = K^3*A0 + K^2*A1 + K*A2 + A3 and
//    B = K^3*B0 + K^2*B1 + K*B2 + B3,
// where K is a half word modulus (sqrt(WHOLE_WORD)).  The solution for
// (A*B) is now:
//    K^6*(A0*B0) +
//    K^5*(A0*B1 + A1*B0) +
//    K^4*(A0*B2 + A1*B1 + A2*B0) +
//    K^3*(A0*B3 + A1*B2 + A2*B1 + A3*B0) +
//    K^2*(A1*B3 + A2*B2 + A3*B1) +
//    K^1*(A2*B3 + A3*B2) +
//    K^0*(A3*B3)
// We can also optimize this by checking for overflow before multiplying
// and not working on the truncated digits.
{
#define XTENDED	(2*LENGTHOF(num))
#define RESULT	(2*XTENDED)
	int i, j, carry;
	ZIL_NUMBER ax[XTENDED], bx[XTENDED], rx[RESULT], *ap, *rp;
	const ZIL_NUMBER *bp;

	INCSTACK(_bnumTimes, ENOMEM);
	stack[stackpos].Import(*this);
	// split number, initialize arrays
	for (i=0, ap = &num[0], bp = &number.num[0];
	     i < LENGTHOF(num); i++, ap++, bp++)
	{
		ax[2*i+1] = *ap % MOD_WORD;
		ax[2*i+0] = *ap / MOD_WORD;
		bx[2*i+1] = *bp % MOD_WORD;
		bx[2*i+0] = *bp / MOD_WORD;
	}

	// init for summation
	for (i=0, rp = rx; i < LENGTHOF(rx); i++, rp++)
		*rp = 0;

	// cross multiply
	for (i=0, ap = ax; i < LENGTHOF(ax); i++, ap++)
		for (j=0, bp = bx; j < LENGTHOF(bx); j++, bp++)
		{
			// Using temps because of bug in Symantec 7.0 on the Macintosh.
			ZIL_NUMBER aTemp = *ap;
			ZIL_NUMBER bTemp = *bp;
			rx[i+j+1] += aTemp * bTemp;
		}

	// combine terms (doing this first makes propogate-carry shorter)
	for (i=LENGTHOF(rx)-2; i > 0; i -= 2)
	{
		ax[i >> 1] = rx[i+1] + (rx[i] % MOD_WORD) * MOD_WORD;

		// Not using += because of bug in Symantec 7.0 on the Macintosh.
		rx[i-1] = rx[i-1] + (rx[i] / MOD_WORD);
	}

	// propogate carry
	for (i=LENGTHOF(ax)-1, ap = ax + i; i > 0; i--, ap--)
		if (*ap >= WHOLE_WORD)
		{
			ap[-1] += *ap / WHOLE_WORD;
			*ap %= WHOLE_WORD;
		}
	if (ax[0] >= MOD_WORD)
		os_numerror(_bnumTimes, ERANGE);

	// cut ZIL_NUMBER_DECIMAL / ZIL_DIGITS words out of result
	// cut ZIL_NUMBER_DECIMAL % ZIL_DIGITS digits out of that result
	// round the whole thing
#if (ZIL_NUMBER_DECIMAL % ZIL_DIGITS == 0)
	carry = ax[LENGTHOF(ax)-(ZIL_NUMBER_DECIMAL/ZIL_DIGITS)] % poten[ZIL_DIGITS-1] >= 5;
#else
	carry = ax[LENGTHOF(ax)-(ZIL_NUMBER_DECIMAL/ZIL_DIGITS)] % poten[ZIL_NUMBER_DECIMAL % ZIL_DIGITS - 1] >= (poten[ZIL_NUMBER_DECIMAL % ZIL_DIGITS] >> 1);
#endif
	for (j=LENGTHOF(num)-1,
	     i=LENGTHOF(ax)-1-(ZIL_NUMBER_DECIMAL/ZIL_DIGITS), ap = ax + i;
	     j >= 0; j--, i--, ap--)
	{
		*ap /= poten[ZIL_NUMBER_DECIMAL % ZIL_DIGITS];
		*ap += (ap[1] % poten[ZIL_NUMBER_DECIMAL % ZIL_DIGITS]) * poten[ZIL_DIGITS - ZIL_NUMBER_DECIMAL % ZIL_DIGITS] + carry;
		stack[stackpos].num[j] = *ap;
		carry = 0;
	}
	stack[stackpos].sign = (sign != number.sign);
	return (stack[stackpos]);
}

ZIL_BIGNUM &abs(const ZIL_BIGNUM &number)
{
	return number.abs();
}

ZIL_BIGNUM &ceil(const ZIL_BIGNUM &number)
{
	return (number.FloorCeil(0));
}

ZIL_BIGNUM &floor(const ZIL_BIGNUM &number)
{
	return (number.FloorCeil(1));
}

ZIL_BIGNUM &round(const ZIL_BIGNUM &number, int places)
{
	return (number.Round(places));
}

ZIL_BIGNUM &truncate(const ZIL_BIGNUM &number, int places)
{
	return (number.Truncate(places));
}
