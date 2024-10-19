//	Zinc Interface Library - O_INTL.CPP
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


#define INCL_DOSNLS				// OS/2 Dos calls.
#define INCL_WINSHELLDATA		// OS/2 Dos calls.
#include <stdio.h>
#include <stdarg.h>
#include <locale.h>
#include <limits.h>
#include <stdlib.h>
#include "ui_gen.hpp"
#if !defined(__IBMCPP__) && !defined(__GLOCKENSPIEL__)
#	include <dos.h>
#endif

static void ui_copy(ZIL_ICHAR *dst, char const *src)
{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
	if (*src)
		ZIL_INTERNATIONAL::UnMapText(src, dst);
#else
	if (*src)
		strcpy(dst, src);
#endif
}

void ZIL_INTERNATIONAL::OSI18nInitialize(ZIL_ICHAR *langName, ZIL_ICHAR *locName, int force)
{
	static int initialized = FALSE;
	// Make sure the country information has not already been initialized.
	if (initialized && !force)
		return;
	initialized = TRUE;

	PSZ intlString = (PSZ)"PM_National";
	ZIL_LOCALE::oemCountryCode = PrfQueryProfileInt(HINI_PROFILE, intlString,
					 (PSZ)"iCountry", ZIL_LOCALE::oemCountryCode);

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

#if defined(ZIL_DO_OS_I18N)
	ZIL_LOCALE *locale = (ZIL_LOCALE *)defaultLocale;
	strcpy(locale->name, locName);
	strcpy(localeManager.defaultName, locName);

	// Override it with the OS/2 country information
	COUNTRYCODE tmp1;
	COUNTRYINFO os2Info;
	ULONG tmp2, tmp3;
	
	DosQueryCp(2L, &tmp2, &tmp3);
	tmp1.country = 0;
	tmp1.codepage = (ULONG)HIUSHORT(tmp2);
	DosQueryCtryInfo(sizeof(os2Info), &tmp1, &os2Info, &tmp2);
	switch (os2Info.fsDateFmt)
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
	ui_copy(locale->data.currencySymbol, os2Info.szCurrency);
	ui_copy(locale->data.intCurrencySymbol, os2Info.szCurrency);
	ui_copy(locale->data.thousandsSeparator, os2Info.szThousandsSeparator);
	ui_copy(locale->data.monThousandsSeparator, os2Info.szThousandsSeparator);
	ui_copy(locale->data.decimalSeparator, os2Info.szDecimal);
	ui_copy(locale->data.monDecimalSeparator, os2Info.szDecimal);
	ui_copy(locale->data.dateSeparator, os2Info.szDateSeparator);
	ui_copy(locale->data.timeSeparator, os2Info.szTimeSeparator);
	locale->data.posSignPrecedes = locale->data.negSignPrecedes = 1;

	static struct
	{
		int spce;
		int curr;
	} cstable[] =
	{
		{ 0, 1 },
		{ 0, 0 },
		{ 1, 1 },
		{ 1, 0 },
		{ 0, 1 }
	};
	if (os2Info.fsCurrencyFmt <= 4)
	{
		locale->data.posSpaceSeparation = locale->data.negSpaceSeparation =
			cstable[os2Info.fsCurrencyFmt].spce;
		locale->data.posCurrencyPrecedes = locale->data.negCurrencyPrecedes =
			cstable[os2Info.fsCurrencyFmt].curr;
		if (os2Info.fsCurrencyFmt == 4)
		{
			strcpy(locale->data.monDecimalSeparator, locale->data.currencySymbol);
			strcpy(locale->data.currencySymbol, _blankString);

		}
	}
	locale->data.intFractionDigits = locale->data.fractionDigits = os2Info.cDecimalPlace;
	switch (os2Info.fsTimeFmt)
	{
	case 0:
		locale->data.defaultTimeFlags = TMF_TWELVE_HOUR;
		break;
	case 1:
		locale->data.defaultTimeFlags = TMF_TWENTY_FOUR_HOUR;
		break;
	}
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
	char tmpBuff[20];

	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sDecimal",
			      (PSZ)tmpBuff, (PSZ)tmpBuff,
			      sizeof(tmpBuff));
	UnMapText(tmpBuff, locale->data.decimalSeparator);
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sDecimal",
			      (PSZ)tmpBuff,
			      (PSZ)tmpBuff,
			      sizeof(tmpBuff));
	UnMapText(tmpBuff, locale->data.monDecimalSeparator);
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sThousand",
			      (PSZ)tmpBuff,
			      (PSZ)tmpBuff,
			      sizeof(tmpBuff));
	UnMapText(tmpBuff, locale->data.thousandsSeparator);
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sThousand",
			      (PSZ)tmpBuff,
			      (PSZ)tmpBuff,
			      sizeof(tmpBuff));
	UnMapText(tmpBuff, locale->data.monThousandsSeparator);
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sCurrency",
			      (PSZ)tmpBuff,
			      (PSZ)tmpBuff, sizeof(tmpBuff));
	UnMapText(tmpBuff, locale->data.currencySymbol);
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sDate",
			      (PSZ)tmpBuff, (PSZ)tmpBuff,
			      sizeof(tmpBuff));
	UnMapText(tmpBuff, locale->data.dateSeparator);
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sTime",
			      (PSZ)tmpBuff, (PSZ)tmpBuff,
			      sizeof(tmpBuff));
	UnMapText(tmpBuff, locale->data.timeSeparator);
#else
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sDecimal",
			      (PSZ)locale->data.decimalSeparator, (PSZ)locale->data.decimalSeparator,
			      sizeof(locale->data.decimalSeparator));
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sDecimal",
			      (PSZ)locale->data.monDecimalSeparator,
			      (PSZ)locale->data.monDecimalSeparator,
			      sizeof(locale->data.monDecimalSeparator));
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sThousand",
			      (PSZ)locale->data.thousandsSeparator,
			      (PSZ)locale->data.thousandsSeparator,
			      sizeof(locale->data.thousandsSeparator));
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sThousand",
			      (PSZ)locale->data.monThousandsSeparator,
			      (PSZ)locale->data.monThousandsSeparator,
			      sizeof(locale->data.monThousandsSeparator));
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sCurrency",
			      (PSZ)locale->data.currencySymbol,
			      (PSZ)locale->data.currencySymbol, sizeof(locale->data.currencySymbol));
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sDate",
			      (PSZ)locale->data.dateSeparator, (PSZ)locale->data.dateSeparator,
			      sizeof(locale->data.dateSeparator));
	PrfQueryProfileString(HINI_PROFILE, intlString, (PSZ)"sTime",
			      (PSZ)locale->data.timeSeparator, (PSZ)locale->data.timeSeparator,
			      sizeof(locale->data.timeSeparator));
#endif
	// ???? intCurrencySymbol
	LONG tmpval;
	static struct
	{
		int spce;
		int curr;
	} pcstable[] =
	{
		{ 0, TRUE },
		{ 0, FALSE },
		{ 1, TRUE },
		{ 1, FALSE }
	};
	tmpval = PrfQueryProfileInt(HINI_PROFILE, intlString,
				    (PSZ)"iCurrency", -1);
	if (tmpval < 4)
	{
		locale->data.posSpaceSeparation = pcstable[tmpval].spce;
		locale->data.posCurrencyPrecedes = pcstable[tmpval].curr;
	}
	// ???? posSignPrecedes = 1;
	// ???? positiveSign
	// ???? negativeSign
	// ???? negativeCurrancy
	locale->data.fractionDigits = PrfQueryProfileInt(HINI_PROFILE, intlString,
					    (PSZ)"iDigits", locale->data.fractionDigits);
	// ???? intFractionDigits
	// iDate and sDate should be replaced with sShortDate
	switch (PrfQueryProfileInt(HINI_PROFILE, intlString, (PSZ)"iDate", 0))
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
	switch (PrfQueryProfileInt(HINI_PROFILE, intlString, (PSZ)"iTime", 0))
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
