//	Zinc Interface Library - G_LANG1.CPP
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


#include "ui_gen.hpp"

struct langMap {
	char *localeName;
	char *languageName;
};

static struct langMap ZIL_FARDATA _langMap[] = {
	{ "AR", "es" },	// Argentina
	{ "AU", "en" },	// Australia
	{ "BE", "fr" },	// Belgium
	{ "BR", "pt" },	// Brazil
	{ "CA", "fr" },	// Canada
	{ "CH", "de" },	// Switzerland
	{ "CN", "zh" },	// PROC
	{ "CY", "el" },	// Cyprus
	{ "CZ", "cs" },	// Czechoslavakia
	{ "DE", "de" },	// Germany
	{ "DK", "da" },	// Denmark
	{ "EE", "et" },	// Estonia
	{ "ES", "es" },	// Spain
	{ "FI", "fi" },	// Finland
	{ "FO", "fo" },	// Faeroe Islands
	{ "FR", "fr" },	// France
	{ "GB", "en" },	// United Kingdom
	{ "GR", "el" },	// Greece
	{ "HU", "hu" },	// Hungary
	{ "IE", "ga" },	// Ireland
	{ "IL", "iw" },	// Israel
	{ "IN", "hi" },	// India
	{ "IR", "fa" },	// Iran
	{ "IS", "is" },	// Iceland
	{ "IT", "it" },	// Italy
	{ "JP", "ja" },	// Japan
	{ "KR", "ko" },	// Korea
	{ "LT", "lt" },	// Lithuania
	{ "LV", "lv" },	// Latvia
	{ "MT", "mt" },	// Malta
	{ "NL", "nl" },	// Netherlands
	{ "NO", "no" },	// Norway
	{ "PK", "ar" },	// Pakistan
	{ "PL", "pl" },	// Poland
	{ "PT", "pt" },	// Portugal
	{ "RU", "ru" },	// Russia
	{ "SA", "ar" },	// Saudi Arabia
	{ "SE", "sv" },	// Sweden
	{ "TH", "th" },	// Thailand
	{ "TR", "tr" },	// Turkey
	{ "TW", "zh" },	// ROC
	{ "US", "en" },	// United States
	{ "YU", "sh" },	// Yugoslavia
	{  ZIL_NULLP(char),		ZIL_NULLP(char)		}
};


const char *ZIL_INTERNATIONAL::ZincLocaleToDefaultLanguage(const char *locale)
{
	struct langMap *mapPtr = _langMap;
	for (; mapPtr->localeName; mapPtr++)
		if (!::strcmp(locale, mapPtr->localeName))
			break;
	return (mapPtr->languageName);
}
