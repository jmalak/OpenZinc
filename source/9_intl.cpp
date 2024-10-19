//	Zinc Interface Library - W_INTL.CPP
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


#if defined(__ZTC__) || defined (_MSC_VER) || defined(__WATCOMC__)
#	include <stdlib.h>
#endif
#include "ui_gen.hpp"

static void ui_copy(ZIL_ICHAR *dst, char const *src)
{
	if (*src)
#if defined(ZIL_UNICODE)
		ZIL_INTERNATIONAL::ISOtoUNICODE(src, dst);
#else
		strcpy(dst, src);
#endif
}

#define ui_transCopy(dst, src)	UnMapText(src, dst)

static int ui_watoi(wchar_t *buffer)
{
	int value = 0;

	while (*buffer >= '0' && *buffer <= '9')
	{
		value = value * 10 + *buffer - '0';
		buffer++;
	}

	return (value);
}

static void nt_strput(ZIL_ICHAR *dst, wchar_t *src)
{
	int i;
	for (i=0; src[i]; i++)
		dst[i] = (ZIL_ICHAR)src[i];
	dst[i] = (ZIL_ICHAR)src[i];
}

void ZIL_INTERNATIONAL::OSI18nInitialize(ZIL_ICHAR *langName, ZIL_ICHAR *locName, int force)
{
	static int initialized = FALSE;
	// Make sure the country information has not already been initialized.
	if (initialized && !force)
		return;
	initialized = TRUE;

	ZIL_ICHAR intlString[] = { 'I','n','t','l', 0 };
	ZIL_ICHAR iCountry[] = { 'i','C','o','u','n','t','r','y', 0 };
	ZIL_LOCALE::oemCountryCode = GetProfileInt(intlString, iCountry, ZIL_LOCALE::oemCountryCode);

	const char *localeName= CountryCodeToDefaultLocale(ZIL_LOCALE::oemCountryCode);
	if (localeName != ZIL_NULLP(char))
	{
		ui_copy(locName, localeName);
		strcpy(localeManager.defaultOSName, locName);
	}

	if (FlagSet(GetVersion(), 0x80000000))		// Win32s
	{
		ZIL_ICHAR tmpBuff[20];
		ZIL_ICHAR sLanguage[] = { 's','L','a','n','g','u','a','g','e', 0 };
		ZIL_ICHAR sDecimal[] = { 's','D','e','c','i','m','a','l', 0 };
		ZIL_ICHAR sThousand[] = { 's','T','h','o','u','s','a','n','d', 0 };
		ZIL_ICHAR sCurrency[] = { 's','C','u','r','r','e','n','c','y', 0 };
		ZIL_ICHAR sDate[] = { 's','D','a','t','e', 0 };
		ZIL_ICHAR sTime[] = { 's','T','i','m','e', 0 };
		ZIL_ICHAR iCurrency[] = { 'i','C','u','r','r','e','n','c','y', 0 };
		ZIL_ICHAR iNegCurr[] = { 'i','N','e','g','C','u','r','r', 0 };
		ZIL_ICHAR iCurrDigits[] = { 'i','C','u','r','r','D','i','g','i','t','s', 0 };
		ZIL_ICHAR iDate[] = { 'i','D','a','t','e', 0 };
		ZIL_ICHAR iTime[] = { 'i','T','i','m','e', 0 };

		GetProfileString(intlString, sLanguage, tmpBuff, tmpBuff,
		 	sizeof(tmpBuff));

#if defined(ZIL_UNICODE)
		strlwr(tmpBuff);
		char *mappedString = MapText(tmpBuff);
#		define TEXT_ mappedString
#else
		::strlwr(tmpBuff);
#		define TEXT_ tmpBuff
#endif
		const char *languageName = LanguageNameToZincLanguage(TEXT_);
		if (languageName != ZIL_NULLP(char))
		{
			ui_copy(langName, languageName);
			strcpy(languageManager.defaultOSName, langName);
		}
#if defined(ZIL_UNICODE)
		delete mappedString;
#endif

#if defined(ZIL_DO_OS_I18N)
		ZIL_LOCALE *locale = (ZIL_LOCALE *)defaultLocale;
		strcpy(locale->name, locName);
		strcpy(localeManager.defaultName, locName);

		GetProfileString(intlString, sDecimal, locale->data.decimalSeparator,
			 	locale->data.decimalSeparator, sizeof(locale->data.decimalSeparator));
		GetProfileString(intlString, sDecimal, locale->data.monDecimalSeparator,
			 	locale->data.monDecimalSeparator, sizeof(locale->data.monDecimalSeparator));
		GetProfileString(intlString, sThousand, locale->data.thousandsSeparator,
			 	locale->data.thousandsSeparator, sizeof(locale->data.thousandsSeparator));
		GetProfileString(intlString, sThousand, locale->data.monThousandsSeparator,
			 	locale->data.monThousandsSeparator, sizeof(locale->data.monThousandsSeparator));
		GetProfileString(intlString, sCurrency, locale->data.currencySymbol,
			 	locale->data.currencySymbol, sizeof(locale->data.currencySymbol));
		strcpy(locale->data.intCurrencySymbol, locale->data.currencySymbol);
		GetProfileString(intlString, sDate, locale->data.dateSeparator,
				 locale->data.dateSeparator, sizeof(locale->data.dateSeparator));
		GetProfileString(intlString, sTime, locale->data.timeSeparator,
				 locale->data.timeSeparator, sizeof(locale->data.timeSeparator));

		WORD tmpval;
		static struct {
			int spce;
			int curr;
		} pcstable[] = {
			{ 0, TRUE },
			{ 0, FALSE },
			{ 1, TRUE },
			{ 1, FALSE }
		};
		tmpval = GetProfileInt(intlString, iCurrency, -1);
		if (tmpval < 4) {
			locale->data.posSpaceSeparation = pcstable[tmpval].spce;
			locale->data.posCurrencyPrecedes = pcstable[tmpval].curr;
		}
		// ???? posSignPrecedes = 1;
		// ???? positiveSign
		// ???? negativeSign
		static struct {
			int spce;
			int curr;
			int sign;
		} ncstable[] = {
			{ 0, 1, 0 },	// 0
			{ 0, 1, 1 },	// 1
			{ 0, 1, 3 },	// 2
			{ 0, 1, 2 },	// 3
			{ 0, 0, 0 },	// 4
			{ 0, 0, 1 },	// 5
			{ 0, 0, 4 },	// 6
			{ 0, 0, 2 },	// 7
			{ 1, 0, 1 },	// 8
			{ 1, 1, 1 },	// 9
			{ 1, 0, 2 }		// 10
		};
		tmpval = GetProfileInt(intlString, iNegCurr, -1);
		if (tmpval < 11) {
			locale->data.negSpaceSeparation = ncstable[tmpval].spce;
			locale->data.negCurrencyPrecedes = ncstable[tmpval].curr;
			locale->data.negSignPrecedes = ncstable[tmpval].sign;
		}
		locale->data.fractionDigits =
			GetProfileInt(intlString, iCurrDigits, locale->data.fractionDigits);
		locale->data.intFractionDigits = locale->data.fractionDigits;
		// iDate and sDate should be replaced with sShortDate
		switch (GetProfileInt(intlString, iDate, 0))
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
		switch (GetProfileInt(intlString, iTime, 0))
		{
		case 0:
 			locale->data.defaultTimeFlags = TMF_TWELVE_HOUR;
			break;
		case 1:
			locale->data.defaultTimeFlags = TMF_TWENTY_FOUR_HOUR;
			break;
		}
#endif
	}
	else
	{
		wchar_t buffer[64];
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_ICOUNTRY, buffer, 16);
		ZIL_LOCALE::oemCountryCode = ui_watoi(buffer);

		const char *localeName= CountryCodeToDefaultLocale(ZIL_LOCALE::oemCountryCode);
		if (localeName != ZIL_NULLP(char))
		{
			ui_copy(locName, localeName);
			strcpy(localeManager.defaultOSName, locName);
		}

		ZIL_ICHAR tmpBuff[32];
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_ILANGUAGE, buffer, 32);
		nt_strput(tmpBuff, buffer);
		tmpBuff[0] = '0';	tmpBuff[1] = 'x';
		strlwr(tmpBuff);

		// Matching done by string to be compatible with Win32s.
#if defined(ZIL_UNICODE)
		char *mappedString = MapText(tmpBuff);
#		define TEXT_ mappedString
#else
#			define TEXT_ tmpBuff
#endif
		const char *languageName = LanguageNameToZincLanguage(TEXT_);
		if (languageName != ZIL_NULLP(char))
		{
			ui_copy(langName, languageName);
			strcpy(languageManager.defaultOSName, langName);
		}
#if defined(ZIL_UNICODE)
		delete mappedString;
#endif

#if defined(ZIL_DO_OS_I18N)
		ZIL_LOCALE *locale = (ZIL_LOCALE *)defaultLocale;
		strcpy(locale->name, locName);
		strcpy(localeManager.defaultName, locName);

		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SDECIMAL, buffer, 4);
		nt_strput(locale->data.decimalSeparator, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SMONDECIMALSEP, buffer, 4);
		nt_strput(locale->data.monDecimalSeparator, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_STHOUSAND, buffer, 4);
		nt_strput(locale->data.thousandsSeparator, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SMONTHOUSANDSEP, buffer, 4);
		nt_strput(locale->data.monThousandsSeparator, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SCURRENCY, buffer, 8);
		nt_strput(locale->data.currencySymbol, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SGROUPING, buffer, 19);
		wchar_t *buff = buffer;
		int i = 0;
		do
		{
			locale->data.grouping[i] = 0;
			while (*buff >= '0' && *buff <= '9')
			{
				locale->data.grouping[i] = locale->data.grouping[i] * 10 + *buff - '0';
				buff++;
			}
			buff++;
		} while (locale->data.grouping[i++]);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SMONGROUPING, buffer, 19);
		i = 0;
		do
		{
			locale->data.monGrouping[i] = 0;
			while (*buff >= '0' && *buff <= '9')
			{
				locale->data.monGrouping[i] = locale->data.monGrouping[i] * 10 + *buff - '0';
				buff++;
			}
			buff++;
		} while (locale->data.monGrouping[i++]);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SINTLSYMBOL, buffer, 8);
		nt_strput(locale->data.intCurrencySymbol, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_IPOSSYMPRECEDES, buffer, 16);
		locale->data.posCurrencyPrecedes = ui_watoi(buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_INEGSYMPRECEDES, buffer, 16);
		locale->data.negCurrencyPrecedes = ui_watoi(buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_ICURRDIGITS, buffer, 16);
		locale->data.fractionDigits = ui_watoi(buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_IINTLCURRDIGITS, buffer, 16);
		locale->data.intFractionDigits = ui_watoi(buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SPOSITIVESIGN, buffer, 4);
		nt_strput(locale->data.positiveSign, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_IPOSSYMPRECEDES, buffer, 16);
		locale->data.posSignPrecedes = ui_watoi(buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_IPOSSEPBYSPACE, buffer, 16);
		locale->data.posSpaceSeparation = ui_watoi(buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SNEGATIVESIGN, buffer, 4);
		nt_strput(locale->data.negativeSign, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_INEGSYMPRECEDES, buffer, 16);
		locale->data.negSignPrecedes = ui_watoi(buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_INEGSEPBYSPACE, buffer, 16);
		locale->data.negSpaceSeparation = ui_watoi(buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_IDATE, buffer, 16);
		switch (ui_watoi(buffer))
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
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_SDATE, buffer, 4);
		nt_strput(locale->data.dateSeparator, buffer);
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_ITIME, buffer, 16);
		switch (ui_watoi(buffer))
		{
		case 0:
 			locale->data.defaultTimeFlags = TMF_TWELVE_HOUR;
			break;
		case 1:
			locale->data.defaultTimeFlags = TMF_TWENTY_FOUR_HOUR;
			break;
		}
		GetLocaleInfoW(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT), LOCALE_STIME, buffer, 4);
		nt_strput(locale->data.timeSeparator, buffer);
#endif
	}
}

