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

#if defined(ZIL_CURSES)
#	define ui_transCopy(dst, src)	::strcpy(dst, src)
#else
#	define ui_transCopy(dst, src)	UnMapText(src, dst)
#endif

void ZIL_INTERNATIONAL::OSI18nInitialize(ZIL_ICHAR *langName, ZIL_ICHAR *locName, int force)
{
	static int initialized = FALSE;
	// Make sure the country information has not already been initialized.
	if (initialized && !force)
		return;
	initialized = TRUE;

	char intlString[] = { 'I','n','t','l', 0 };
	char iCountry[] = { 'i','C','o','u','n','t','r','y', 0 };
	ZIL_LOCALE::oemCountryCode = GetProfileInt(intlString, iCountry, ZIL_LOCALE::oemCountryCode);

	const char *localeName= CountryCodeToDefaultLocale(ZIL_LOCALE::oemCountryCode);
	if (localeName != ZIL_NULLP(char))
	{
		ui_copy(locName, localeName);
		strcpy(localeManager.defaultOSName, locName);
	}

		char tmpBuff[20];
		char sLanguage[] = { 's','L','a','n','g','u','a','g','e', 0 };
		char sDecimal[] = { 's','D','e','c','i','m','a','l', 0 };
		char sThousand[] = { 's','T','h','o','u','s','a','n','d', 0 };
		char sCurrency[] = { 's','C','u','r','r','e','n','c','y', 0 };
		char sDate[] = { 's','D','a','t','e', 0 };
		char sTime[] = { 's','T','i','m','e', 0 };
		char iCurrency[] = { 'i','C','u','r','r','e','n','c','y', 0 };
		char iNegCurr[] = { 'i','N','e','g','C','u','r','r', 0 };
		char iCurrDigits[] = { 'i','C','u','r','r','D','i','g','i','t','s', 0 };
		char iDate[] = { 'i','D','a','t','e', 0 };
		char iTime[] = { 'i','T','i','m','e', 0 };

		GetProfileString(intlString, sLanguage, tmpBuff, tmpBuff,
		 	sizeof(tmpBuff));

		::strlwr(tmpBuff);
#		define TEXT_ tmpBuff
		const char *languageName = LanguageNameToZincLanguage(TEXT_);
		if (languageName != ZIL_NULLP(char))
		{
			ui_copy(langName, languageName);
			strcpy(languageManager.defaultOSName, langName);
		}

#if defined(ZIL_DO_OS_I18N)
		ZIL_LOCALE *locale = (ZIL_LOCALE *)defaultLocale;
		strcpy(locale->name, locName);
		strcpy(localeManager.defaultName, locName);

#	if defined(ZIL_UNICODE)
		GetProfileString(intlString, sDecimal, tmpBuff, tmpBuff,
			 	sizeof(tmpBuff));
		UnMapText(tmpBuff, locale->data.decimalSeparator);
		GetProfileString(intlString, sDecimal, tmpBuff, tmpBuff,
			 	sizeof(tmpBuff));
		UnMapText(tmpBuff, locale->data.monDecimalSeparator);
		GetProfileString(intlString, sThousand, tmpBuff, tmpBuff,
			 	sizeof(tmpBuff));
		UnMapText(tmpBuff, locale->data.thousandsSeparator);
		GetProfileString(intlString, sThousand, tmpBuff, tmpBuff,
			 	sizeof(tmpBuff));
		UnMapText(tmpBuff, locale->data.monThousandsSeparator);
		GetProfileString(intlString, sCurrency, tmpBuff, tmpBuff,
			 	sizeof(tmpBuff));
		UnMapText(tmpBuff, locale->data.currencySymbol);
		strcpy(locale->data.intCurrencySymbol, locale->data.currencySymbol);
		GetProfileString(intlString, sDate, tmpBuff, tmpBuff,
			 	sizeof(tmpBuff));
		UnMapText(tmpBuff, locale->data.dateSeparator);
		GetProfileString(intlString, sTime, tmpBuff, tmpBuff,
			 	sizeof(tmpBuff));
		UnMapText(tmpBuff, locale->data.timeSeparator);
#	else
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
#	endif
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

