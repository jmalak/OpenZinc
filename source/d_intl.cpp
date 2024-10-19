//	Zinc Interface Library - D_INTL.CPP
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


#include <locale.h>
#include <limits.h>
#include <stdlib.h>
#include "ui_gen.hpp"
#if defined(ZIL_MSDOS)
#	include <dos.h>
#endif
#if defined(_MSC_VER)
#	if defined(DOS386)
#		include <pldos32.h>
#	elif defined(DOSX286)
#		define	_int86x	int86x
#		define	_REGS	REGS
#		define	_SREGS	SREGS
#	endif
#elif defined(__WATCOMC__)
#	include <i86.h>
#endif
#if defined(DOSX286)
#	include <phapi.h>
#elif	defined(DOS16M)
#	include <dos16.h>
#endif

#if defined(__ZTC__) && defined(DOS386)
//	Zortech 32 bit mode doesn't support int 21h, function 38h right.
//	Remove these lines when it works.
#	define DISABLE_INT21
#endif

struct DOS3_COUNTRY
{
	short dateFormat;
	char currencySymbol[5];
	char thousandsSeparator[2];
	char decimalSeparator[2];
	char dateSeparator[2];
	char timeSeparator[2];
	unsigned char currencyStyle;
	unsigned char significantDigits;
	unsigned char timeFormat;
	long caseMap;
	char dataSeparator[2];
	char filler[10];
};

static void ui_copy(ZIL_ICHAR *dst, char const *src)
{
	if (*src)
#if defined(ZIL_UNICODE)
		ZIL_INTERNATIONAL::ISOtoUNICODE(src, dst);
#else
		strcpy(dst, src);
#endif
}

#if defined(ZIL_CURSES)
#	define ui_transCopy(dst, src)	::strcpy(dst, src)
#else
#	define ui_transCopy(dst, src)	UnMapText(src, dst)
#endif

void ZIL_INTERNATIONAL::OSI18nInitialize(ZIL_ICHAR *langName, ZIL_ICHAR *locName, int force)
{
	// Make sure the country information has not already been initialized.
	static int initialized = FALSE;
	if (initialized && !force)
		return;
	initialized = TRUE;

#if defined(DISABLE_INT21)
	const char *localeName= CountryCodeToDefaultLocale(ZIL_LOCALE::oemCountryCode);
	if (localeName != ZIL_NULLP(char))
	{
		ui_copy(locName, localeName);
		strcpy(localeManager.defaultOSName, locName);
		const char *languageName = ZincLocaleToDefaultLanguage(localeName);
		if (languageName != ZIL_NULLP(char))
		{
			ui_copy(langName, languageName);
			strcpy(languageManager.defaultOSName, langName);
		}
	}

#	if defined(ZIL_DO_OS_I18N)
	ZIL_LOCALE *locale = (ZIL_LOCALE *)defaultLocale;
	strcpy(locale->name, locName);
	strcpy(localeManager.defaultName, locName);

	// Get the locale information.  (Probably brain-damaged.)
	struct lconv *conv = localeconv();
	ui_copy(locale->data.decimalSeparator, conv->decimal_point);
	ui_copy(locale->data.monDecimalSeparator, conv->mon_decimal_point);
	ui_copy(locale->data.thousandsSeparator, conv->thousands_sep);
	ui_copy(locale->data.monThousandsSeparator, conv->mon_thousands_sep);
	ui_copy(locale->data.intCurrencySymbol, conv->int_curr_symbol);
	if (conv->p_cs_precedes != CHAR_MAX)
		locale->data.posCurrencyPrecedes = conv->p_cs_precedes;
	if (conv->frac_digits != CHAR_MAX)
		locale->data.fractionDigits = conv->frac_digits;
	if (conv->p_sep_by_space != CHAR_MAX)
		locale->data.posSpaceSeparation = conv->p_sep_by_space;
	ui_copy(locale->data.negativeSign, conv->negative_sign);
	if (conv->n_sign_posn != CHAR_MAX)
		locale->data.negSignPrecedes = conv->n_sign_posn;
	if (conv->n_sep_by_space != CHAR_MAX)
		locale->data.negSpaceSeparation = conv->n_sep_by_space;

#		if defined(__ZTC__) && __ZTC__ < 0x310
	if (conv->n_sc_precedes != CHAR_MAX)
		locale->data.negCurrencyPrecedes = conv->n_sc_precedes;
#		else	// !ZTC 3.1
	::strcpy(locale->data.grouping, conv->grouping);
	::strcpy(locale->data.monGrouping, conv->mon_grouping);
	if (conv->n_cs_precedes != CHAR_MAX)
		locale->data.negCurrencyPrecedes = conv->n_cs_precedes;
	if (conv->int_frac_digits != CHAR_MAX)
		locale->data.intFractionDigits = conv->int_frac_digits;
	ui_transCopy(locale->data.positiveSign, conv->positive_sign);
	if (conv->p_sign_posn != CHAR_MAX)
		locale->data.posSignPrecedes = conv->p_sign_posn;
#		endif	// !ZTC 3.1
#	endif

#else
	// Override it with the DOS country information
	static DOS3_COUNTRY dos3Info;

#if defined(_MSC_VER)
#	if defined(DOS386)
		union _REGS regs;
		regs.d.edx = (unsigned int)&dos3Info;
		// Determine the country code.	
		regs.w.ax = 0x3800;
		_int86(0x21, &regs, &regs);
		ZIL_LOCALE::oemCountryCode = (regs.h.al == 0xFF) ? regs.w.bx : regs.h.al;
#	else
		union _REGS regs;
		struct _SREGS segregs;
		void far* ptr = (void *)&dos3Info;
		regs.x.dx = _FP_OFF(ptr);
		segregs.ds = _FP_SEG(ptr);
		segregs.es = 0;
		// Determine the country code.
		regs.x.ax = 0x3800;
		_int86x(0x21, &regs, &regs, &segregs);
		ZIL_LOCALE::oemCountryCode = (regs.h.al == 0xFF) ? regs.x.bx : regs.h.al;
#	endif
#elif defined(__WATCOMC__)
	union REGS regs;
	regs.x.edx = (unsigned int)&dos3Info;
	// Determine the country code.
	regs.w.ax = 0x3800;
	int386(0x21, &regs, &regs);
	ZIL_LOCALE::oemCountryCode = (regs.h.al == 0xFF) ? regs.w.bx : regs.h.al;
#elif defined(ZIL_CURSES)
	ZIL_LOCALE::oemCountryCode = 01;
#elif defined(__ZTC__)
	union REGS regs;
	regs.x.dx = (ZIL_UINT16)&dos3Info;
	// Determine the country code.
	regs.x.ax = 0x3800;
	int86(0x21, &regs, &regs);
	ZIL_LOCALE::oemCountryCode = (regs.h.al == 0xFF) ? regs.x.bx : regs.h.al;
#elif defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#	if defined(DOS32PP)
		union REGS regs;
		regs.x.edx = (unsigned int)&dos3Info;
		// Determine the country code.
		regs.w.ax = 0x3800;
		int386(0x21, &regs, &regs);
		ZIL_LOCALE::oemCountryCode = (regs.h.al == 0xFF) ? regs.w.bx : regs.h.al;
#	else
		union REGS regs;
		struct SREGS segregs;
		void far* ptr = (void *)&dos3Info;
		regs.x.dx = FP_OFF(ptr);
		segregs.ds = FP_SEG(ptr);
		segregs.es = 0;
		// Determine the country code.
		regs.x.ax = 0x3800;
		int86x(0x21, &regs, &regs, &segregs);
		ZIL_LOCALE::oemCountryCode = (regs.h.al == 0xFF) ? regs.x.bx : regs.h.al;
#	endif
#else
	???? who is this ????;
#endif
#if defined(ZIL_UNICODE)
	// It seems that MS-DOS/V 5.0J doesn't set the
	// country code to 1 when in "chev us" mode.
	int *table = I_CheckLeadIn();
	if (ZIL_LOCALE::oemCountryCode == 81 && table && *table == 0)
		ZIL_LOCALE::oemCountryCode = 1;
#endif

	const char *localeName= CountryCodeToDefaultLocale(ZIL_LOCALE::oemCountryCode);
	if (localeName != ZIL_NULLP(char))
	{
		ui_copy(locName, localeName);
		strcpy(localeManager.defaultOSName, locName);

		const char *languageName = ZincLocaleToDefaultLanguage(localeName);
		if (languageName != ZIL_NULLP(char))
		{
			ui_copy(langName, languageName);
			strcpy(languageManager.defaultOSName, langName);
		}
	}

#if defined(ZIL_DO_OS_I18N) && !defined(ZIL_CURSES)
	ZIL_LOCALE *locale = (ZIL_LOCALE *)defaultLocale;
	strcpy(locale->name, locName);
	strcpy(localeManager.defaultName, locName);

	switch (dos3Info.dateFormat)
	{
	case 0:
		locale->data.defaultDateFlags = DTF_US_FORMAT;
		break;
	case 1:
		locale->data.defaultDateFlags = DTF_EUROPEAN_FORMAT;
		break;
	case 2:
		locale->data.defaultDateFlags = DTF_ASIAN_FORMAT;
		break;
	}
	ui_transCopy(locale->data.currencySymbol, dos3Info.currencySymbol);
	ui_transCopy(locale->data.intCurrencySymbol, dos3Info.currencySymbol);
	ui_transCopy(locale->data.thousandsSeparator, dos3Info.thousandsSeparator);
	ui_transCopy(locale->data.monThousandsSeparator, dos3Info.thousandsSeparator);
	ui_transCopy(locale->data.decimalSeparator, dos3Info.decimalSeparator);
	ui_transCopy(locale->data.monDecimalSeparator, dos3Info.decimalSeparator);
	ui_transCopy(locale->data.dateSeparator, dos3Info.dateSeparator);
	ui_transCopy(locale->data.timeSeparator, dos3Info.timeSeparator);
	locale->data.posSignPrecedes = locale->data.negSignPrecedes = 1;
	static struct {
		int spce;
		int curr;
	} cstable[] = {
		{ 0, 1 },
		{ 0, 0 },
		{ 1, 1 },
		{ 1, 0 },
		{ 0, 1 }
	};
	if (dos3Info.currencyStyle <= 4)
	{
		locale->data.posSpaceSeparation = locale->data.negSpaceSeparation =
			cstable[dos3Info.currencyStyle].spce;
		locale->data.posCurrencyPrecedes = locale->data.negCurrencyPrecedes =
			cstable[dos3Info.currencyStyle].curr;
		if (dos3Info.currencyStyle == 4)
		{
			ui_transCopy(locale->data.monDecimalSeparator, dos3Info.currencySymbol);
			locale->data.currencySymbol[0] = 0;
		}
	}
	locale->data.intFractionDigits = locale->data.fractionDigits =
		dos3Info.significantDigits;
	switch (dos3Info.timeFormat)
	{
	case 0:
		locale->data.defaultTimeFlags = TMF_TWELVE_HOUR;
		break;
	case 1:
		locale->data.defaultTimeFlags = TMF_TWENTY_FOUR_HOUR;
		break;
	}
#endif
#endif	// !DISABLE_INT21
}
