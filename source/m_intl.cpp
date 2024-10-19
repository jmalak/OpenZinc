//	Zinc Interface Library - M_INTL.CPP
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


#include <stdio.h>
#include <stdarg.h>
#if !(defined(__GNUC__) && defined(__DVX__))
#	include <locale.h>
#endif
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include "ui_gen.hpp"

#if defined(ZIL_DO_OS_I18N)
static void ui_copy(ZIL_ICHAR *dst, char const *src)
{
	if (*src)
#if defined(ZIL_UNICODE)
		ZIL_INTERNATIONAL::ISOtoUNICODE(src, dst);
#else
		strcpy(dst, src);
#endif
}

#if defined(ZIL_UNICODE)
#	define ui_transCopy(dst, src)	UnMapText(src, dst)
#else
#	define ui_transCopy(dst, src)	::strcpy(dst, src)
#endif

static ZIL_ICHAR langString[] = { 'L','A','N','G', 0 };

void ZIL_INTERNATIONAL::OSI18nInitialize(ZIL_ICHAR *langName, ZIL_ICHAR *locName, int force)
{
	// Make sure the country information has not already been initialized.
	static int initialized = FALSE;
	if (initialized && !force)
		return;
	initialized = TRUE;

	ZIL_ICHAR *lang = getenv(langString);
	if (lang && *lang)
	{
#if defined(__hpux) || defined(SCO_UNIX)
		const char *_lang = LanguageNameToZincLanguage((const char *)lang);
		strcpy(langName, (ZIL_ICHAR *)_lang);
		strcpy(locName, LanguageToDefaultLocale(langName));
#elif defined(_SINIX)
		strcpy(langName, lang);
		langName[0] = ToLower(langName[0]);
		langName[1] = ToLower(langName[1]);
		strcpy(locName, LanguageToDefaultLocale(langName));
#elif defined(sun) || defined(__alpha) || defined(__sgi) || defined(_IBM_RS6000)
		lang[0] = ToLower(lang[0]);
		ZIL_ICHAR *_langName = strdup(lang);
		ZIL_ICHAR *ptr = strchr(_langName, '_');
		if (ptr)
		{
			// LANG contains locale.
			*ptr++ = 0;
			const ZIL_ICHAR *_locName = ptr;
			if ((ptr = strchr(ptr, '.')) != ZIL_NULLP(ZIL_ICHAR))
				*ptr++ = 0;
			strcpy(locName, _locName);
			strcpy(localeManager.defaultOSName, _locName);
		}
		strcpy(langName, _langName);
		strcpy(languageManager.defaultOSName, _langName);
		delete _langName;
#elif defined(__QNX__)
	// Watcom C lib doesn't support locales yet.
#elif defined(__DVX__)
	// This is really a DOS call, see d_intl.cpp.
#elif defined(__linux__)
	//need to look into internationalization in linux
	//NOTE TO SELF jdh
#else
	????; // This code needs to be fixed up for your system.
#endif
	}

	setlocale(LC_ALL, "");
	
	// Get the locale information.
	ZIL_LOCALE *locale = (ZIL_LOCALE *)defaultLocale;

	struct lconv *conv = localeconv();
	ui_copy(locale->data.decimalSeparator, conv->decimal_point);
	ui_copy(locale->data.monDecimalSeparator, conv->mon_decimal_point);
	ui_copy(locale->data.thousandsSeparator, conv->thousands_sep);
	ui_copy(locale->data.monThousandsSeparator, conv->mon_thousands_sep);
	::strcpy(locale->data.grouping, conv->grouping);
	::strcpy(locale->data.monGrouping, conv->mon_grouping);
	ui_copy(locale->data.intCurrencySymbol, conv->int_curr_symbol);
	if (conv->p_cs_precedes != CHAR_MAX)
		locale->data.posCurrencyPrecedes = conv->p_cs_precedes;
	if (conv->n_cs_precedes != CHAR_MAX)
		locale->data.negCurrencyPrecedes = conv->n_cs_precedes;
	if (conv->frac_digits != CHAR_MAX)
		locale->data.fractionDigits = conv->frac_digits;
	if (conv->int_frac_digits != CHAR_MAX)
		locale->data.intFractionDigits = conv->int_frac_digits;
	ui_copy(locale->data.positiveSign, conv->positive_sign);
	if (conv->p_sign_posn != CHAR_MAX)
		locale->data.posSignPrecedes = conv->p_sign_posn;
	if (conv->p_sep_by_space != CHAR_MAX)
		locale->data.posSpaceSeparation = conv->p_sep_by_space;
	ui_copy(locale->data.negativeSign, conv->negative_sign);
	if (conv->n_sign_posn != CHAR_MAX)
		locale->data.negSignPrecedes = conv->n_sign_posn;
	if (conv->n_sep_by_space != CHAR_MAX)
		locale->data.negSpaceSeparation = conv->n_sep_by_space;
	locale->data.defaultTimeFlags = TMF_TWELVE_HOUR;
	locale->data.defaultDateFlags = DTF_US_FORMAT;
}
#else
void ZIL_INTERNATIONAL::OSI18nInitialize(ZIL_ICHAR *, ZIL_ICHAR *, int)
{ }
#endif
