//	Zinc Interface Library - G_LOC.CPP
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

struct defLocaleMap {
	char *languageName;
	char *defaultLocale;
};

#if defined(__hpux) || defined(ZIL_NEXTSTEP)
static struct defLocaleMap ZIL_FARDATA _defLocaleMap[] = {
	{ "ar",	"SA"	},
	{ "bg",	"BG"	},
	{ "cs",	"CZ"	},
	{ "da",	"DK"	},
	{ "de",	"DE"	},
	{ "el",	"GR"	},
	{ "en",	"US"	},
	{ "es",	"ES"	},
	{ "fi",	"FI"	},
	{ "fr",	"FR"	},
	{ "hu",	"HU"	},
	{ "is",	"IS"	},
	{ "it",	"IT"	},
	{ "iw",	"IL"	},
	{ "ja",	"JP"	},
	{ "ko",	"KR"	},
	{ "nl",	"NL"	},
	{ "no",	"NO"	},
	{ "pl",	"PL"	},
	{ "pt",	"PT"	},
	{ "ro",	"RO"	},
	{ "ru",	"RU"	},
	{ "sh",	"YU"	},
	{ "sl",	"SI"	},
	{ "sv",	"SE"	},
	{ "th",	"TH"	},
	{ "tr",	"TR"	},
	{ "zh",	"CN"	},
	{  ZIL_NULLP(char),		"  " }
};

#else
static struct defLocaleMap ZIL_FARDATA _defLocaleMap[] = {
	{  ZIL_NULLP(char),		ZIL_NULLP(char)		}
};
#endif

const ZIL_ICHAR *ZIL_INTERNATIONAL::LanguageToDefaultLocale(const ZIL_ICHAR *language)
{
	static char charBuf[3];
	charBuf[0] = (char)language[0];
	charBuf[1] = (char)language[1];
	charBuf[2] = 0;

	struct defLocaleMap *mapPtr = NULL;
	for (mapPtr = _defLocaleMap; mapPtr->languageName; mapPtr++)
		if (!::strcmp(charBuf, mapPtr->languageName))
			break;

	static ZIL_ICHAR icharBuf[3];
	icharBuf[0] = mapPtr->defaultLocale[0];
	icharBuf[1] = mapPtr->defaultLocale[1];
	icharBuf[2] = 0;
	return (icharBuf);
}
