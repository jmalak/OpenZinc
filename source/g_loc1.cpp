//	Zinc Interface Library - G_LOC1.CPP
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

#if defined(ZIL_MACINTOSH)
#	include <Packages.h>
#endif

struct countryCodeMap {
	int osCountryCode;
	char *defaultLocale;
};

static struct countryCodeMap ZIL_FARDATA _countryCodeMap[] = {
#if defined(ZIL_MACINTOSH)
	{ verUS,			"US" },
	{ verFrance,		"FR" },
	{ verBritain,		"GB" },
	{ verGermany,		"DE" },
	{ verItaly,			"IT" },
	{ verNetherlands,	"NL" },
	{ verFrBelgiumLux,	"BE" },
	{ verSweden,		"SE" },
	{ verSpain,			"ES" },
	{ verDenmark,		"DK" },
	{ verPortugal,		"PT" },
	{ verFrCanada,		"CA" },
	{ verNorway,		"NO" },
	{ verIsrael,		"IL" },
	{ verJapan,			"JP" },
	{ verAustralia,		"AU" },
	{ verArabic,		"SA" },
	{ verFinland,		"FI" },
	{ verFrSwiss,		"CH" },
	{ verGrSwiss,		"CH" },
	{ verGreece,		"GR" },
	{ verIceland,		"IS" },
	{ verMalta,			"MT" },
	{ verCyprus,		"CY" },
	{ verTurkey,		"TR" },
	{ verYugoCroatian,	"YU" },
	{ verIndiaHindi,	"IN" },
	{ verPakistan,		"PK" },
	{ verLithuania,		"LT" },
	{ verPoland,		"PL" },
	{ verHungary,		"HU" },
	{ verEstonia,		"EE" },
	{ verLatvia,		"LV" },
	{ verLapland,		"NO" },
	{ verFaeroeIsl,		"FO" },
	{ verIran,			"IR" },
	{ verRussia,		"RU" },
	{ verIreland,		"IE" },
	{ verKorea,			"KR" },
	{ verChina,			"CN" },
	{ verTaiwan,		"TW" },
	{ verThailand,		"TH" },
#else
	{   1, "US" },	// United States
	{   2, "CA" },	// Canada
	{   3, "AR" },	// South America (Argentina?)
	{  31, "NL" },	// Netherlands
	{  32, "BE" },	// Belgium
	{  33, "FR" },	// France
	{  34, "ES" },	// Spain
	{  36, "HU" },	// Hungary
	{  38, "YU" },	// Yugoslavia
	{  39, "IT" },	// Italy
	{  41, "CH" },	// Switzerland
	{  42, "CZ" },	// Czechoslavakia
	{  44, "GB" },	// United Kingdom
	{  45, "DK" },	// Denmark
	{  46, "SE" },	// Sweden
	{  47, "NO" },	// Norway
	{  48, "PL" },	// Poland
	{  49, "DE" },	// Germany
	{  55, "BR" },	// Brazil
	{  61, "AU" },	// Australia
	{  81, "JP" },	// Japan
	{  82, "KR" },	// Korea
	{  86, "CN" },	// PROC
	{  88, "TW" },	// ROC
	{  90, "TR" },	// Turkey
	{ 351, "PT" },	// Portugal
	{ 354, "IS" },	// Iceland
	{ 358, "FI" },	// Finland
	{ 785, "SA" },	// Saudi Arabia
	{ 886, "TW" },	// ROC
	{ 972, "IL" },	// Israel
#endif
	{  0,  ZIL_NULLP(char) }
};

const char *ZIL_INTERNATIONAL::CountryCodeToDefaultLocale(int countryCode)
{
	struct countryCodeMap *mapPtr = _countryCodeMap;
	for (; mapPtr->defaultLocale; mapPtr++)
		if (countryCode == mapPtr->osCountryCode)
			break;
	return (mapPtr->defaultLocale);
}
