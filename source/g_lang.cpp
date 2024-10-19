//	Zinc Interface Library - G_LANG.CPP
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
	char *osLanguageName;
	char *languageName;
};

static struct langMap ZIL_FARDATA _langMap[] = {
#if defined(__hpux)
	{ "american",		"en" },
	{ "arabic",			"ar" },
	{ "arabic-w",		"ar" },
	{ "bulgarian",		"bg" },
	{ "c-french",		"fr" },
	{ "chinese-s",		"zh" },
	{ "chinese-t",		"zh" },
	{ "czech",			"cs" },
	{ "danish",			"da" },
	{ "dutch",			"nl" },
	{ "english",		"en" },
	{ "finnish",		"fi" },
	{ "french",			"fr" },
	{ "german",			"de" },
	{ "greek",			"el" },
	{ "hebrew",			"iw" },
	{ "hungarian",		"hu" },
	{ "icelandic",		"is" },
	{ "italian",		"it" },
	{ "japanese",		"ja" },
	{ "katakana",		"ja" },
	{ "korean",			"ko" },
	{ "norwegian",		"no" },
	{ "polish",			"pl" },
	{ "portuguese",		"pt" },
	{ "rumanian",		"ro" },
	{ "russian",		"ru" },
	{ "serbocroatian",	"sh" },
	{ "slovene",		"sl" },
	{ "spanish",		"es" },
	{ "swedish",		"sv" },
	{ "thai",			"th" },
	{ "turkish",		"tr" },
#elif defined(ZIL_MSWINDOWS)
#	if defined(ZIL_WINNT)
	{ "0x01",			"ar" },	// LANG_ARABIC
	{ "0x02",			"bg" },	// LANG_BULGARIAN
	{ "0x03",			"ca" },	// LANG_CATALAN
	{ "0x04",			"zh" },	// LANG_CHINESE
	{ "0x05",			"cs" },	// LANG_CZECH
	{ "0x06",			"da" },	// LANG_DANISH
	{ "0x07",			"de" },	// LANG_GERMAN
	{ "0x08",			"el" },	// LANG_GREEK
	{ "0x09",			"en" },	// LANG_ENGLISH
	{ "0x0a",			"es" },	// LANG_SPANISH
	{ "0x0b",			"fi" },	// LANG_FINNISH
	{ "0x0c",			"fr" },	// LANG_FRENCH
	{ "0x0d",			"iw" },	// LANG_HEBREW
	{ "0x0e",			"hu" },	// LANG_HUNGARIAN
	{ "0x0f",			"is" },	// LANG_ICELANDIC
	{ "0x10",			"it" },	// LANG_ITALIAN
	{ "0x11",			"ja" },	// LANG_JAPANESE
	{ "0x12",			"ko" },	// LANG_KOREAN
	{ "0x13",			"nl" },	// LANG_DUTCH
	{ "0x14",			"no" },	// LANG_NORWEGIAN
	{ "0x15",			"pl" },	// LANG_POLISH
	{ "0x16",			"pt" },	// LANG_PORTUGUESE
	{ "0x17",			"rm" },	// LANG_RHAETO_ROMAN
	{ "0x18",			"ro" },	// LANG_ROMANIAN
	{ "0x19",			"ru" },	// LANG_RUSSIAN
	{ "0x1a",			"sh" },	// LANG_SERBO_CROATIAN
	{ "0x1b",			"sk" },	// LANG_SLOVAK
	{ "0x1c",			"sq" },	// LANG_ALBANIAN
	{ "0x1d",			"sv" },	// LANG_SWEDISH
	{ "0x1e",			"th" },	// LANG_THAI
	{ "0x1f",			"tr" },	// LANG_TURKISH
	{ "0x20",			"ur" },	// LANG_URDU
//	{ "0x21",			"" },	// LANG_BAHASA
#	endif
	// This must be included for Win32s
	{ "chs", "zh" },	// China PROC
	{ "cht", "zh" },	// China ROC
	{ "csy", "cs" },	// Czech Republic.
	{ "dan", "da" },	// Denmark.
	{ "dea", "de" },	// Austria.
	{ "des", "de" },	// German Switzerland.
	{ "deu", "de" },	// Germany.
	{ "ell", "el" },	// Greece.
	{ "eng", "en" },	// England.
	{ "eni", "en" },	// Ireland.
	{ "ena", "en" },	// Australia.
	{ "enc", "en" },	// English Canada.
	{ "enu", "en" },	// United States.
	{ "enz", "en" },	// New Zealand.
	{ "esm", "es" },	// Mexico.
	{ "esn", "es" },	// Modern Spain.
	{ "esp", "es" },	// Traditional Spain.
	{ "fin", "fi" },	// Finland.
	{ "fra", "fr" },	// France.
	{ "frb", "fr" },	// French Belgium.
	{ "frc", "fr" },	// French Canada.
	{ "frs", "fr" },	// French Switzerland.
	{ "hun", "hu" },	// Hungary.
	{ "isl", "is" },	// Iceland.
	{ "ita", "it" },	// Italy.
	{ "its", "it" },	// Italian Switzerland.
	{ "jpn", "ja" },	// Japan.
	{ "kor", "ko" },	// Japan.
	{ "nlb", "nl" },	// Dutch Belgium.
	{ "nld", "nl" },	// Netherlands.
	{ "nor", "no" },	// Norway (Bokmal).
	{ "non", "no" },	// Norway (Nynorsk).
	{ "plk", "pl" },	// Poland.
	{ "ptb", "pt" },	// Brazil.
	{ "ptg", "pt" },	// Portugal.
	{ "rus", "ru" },	// Russia.
	{ "sky", "sk" },	// Slovak Republic.
	{ "sve", "sv" },	// Sweden.
	{ "trk", "tr" },	// Turkey.
#endif
	{  ZIL_NULLP(char),		ZIL_NULLP(char)		}
};

const char *ZIL_INTERNATIONAL::LanguageNameToZincLanguage(const char *osLanguage)
{
	struct langMap *mapPtr = NULL;
	for (mapPtr = _langMap; mapPtr->osLanguageName; mapPtr++)
		if (!::strcmp(osLanguage, mapPtr->osLanguageName))
			break;
	return (mapPtr->languageName);
}
