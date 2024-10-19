//	Zinc Interface Library - Z_INTL.CPP
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


extern "C"
{
#	include <time.h>
}
#include "ui_dsp.hpp"

ZIL_INTERNATIONAL::ZIL_INTERNATIONAL(void)
{ }

ZIL_INTERNATIONAL::~ZIL_INTERNATIONAL(void)
{ }

void ZIL_INTERNATIONAL::TimeStamp(ZIL_UINT32 *value)
{
	(void) time((time_t *)value);
#if defined(ZIL_MACINTOSH)
	// Fix for time value since Apple uses 1904.
	*value -= 31554100L * 66L + 249000L;
#elif defined(__ZTC__)
	// Help for any system that can't do GMT right
	*value += minutesWestGMT * 60L;
#	if __ZTC__ > 0x0310
	// Fix for time value since Symantec deemed 1968 better than 1970
	*value -= 63108000L;
#	endif
#endif
}

#if defined(ZIL_MACINTOSH)
void ZIL_INTERNATIONAL::ParseLangEnv(ZIL_ICHAR *, ZIL_ICHAR *, ZIL_ICHAR *) { }
#else
void ZIL_INTERNATIONAL::ParseLangEnv(ZIL_ICHAR *_codeSet, ZIL_ICHAR *_locName, ZIL_ICHAR *_langName)
{
	ZIL_ICHAR *lang = getenv(zincLangString);
	// Parse input of the format: language[_locale][.codeSet], where
	// language and locale are the ISO 2 character standard.
	if (lang && *lang)
	{
		ZIL_ICHAR *langName = strdup(lang);
		ZIL_ICHAR *ptr = strchr(langName, '_');
		if (ptr)
		{
			// ptr now contains locale.
			*ptr++ = 0;
			const ZIL_ICHAR *locName = ptr;
			if ((ptr = strchr(ptr, '.')) != ZIL_NULLP(ZIL_ICHAR))
			{
				// ptr now contains codeSet.
				*ptr++ = 0;
				if (_codeSet)
					strcpy(_codeSet, ptr);
			}
			strcpy(_locName, locName);
			strcpy(localeManager.defaultOSName, locName);
		}
		strcpy(_langName, langName);
		strcpy(languageManager.defaultOSName, langName);
		delete langName;
	}
}
#endif

void ZIL_INTERNATIONAL::CharMapInitialize(void)
{
#if defined(ZIL_LOAD) && defined(ZIL_DO_FILE_I18N) && (defined(ZIL_ISO8859_1) || defined(ZIL_UNICODE))
#	if defined(ZIL_UNICODE)
	static ZIL_ICHAR stdFontName[] = { 'U','N','I','C','O','D','E',0 };
#	else
	static ZIL_ICHAR stdFontName[] = { 'I','S','O','8','8','5','9','-','1',0 };
#	endif
	if (streq(UI_DISPLAY::codeSet, defaultCharMap->name) != 0 &&
	    streq(UI_DISPLAY::codeSet, stdFontName) != 0)
  	{
#	if defined(ZIL_UNICODE)
		ZIL_ICHAR tmpDirSepChar = defaultCharMap->dirSepStr[0];
#	endif
		ZIL_MAP_CHARS *tmp = new ZIL_MAP_CHARS(UI_DISPLAY::codeSet, machineName);
		if (tmp->error == 0)
		{
			delete defaultCharMap;
			defaultCharMap = tmp;
#	if defined(ZIL_UNICODE)
			// Fix up the stored paths to use the "new" separator.
			if (tmpDirSepChar != defaultCharMap->dirSepStr[0])
				for (const ZIL_ICHAR *path = ZIL_STORAGE_READ_ONLY::searchPath->FirstPathName(); path; path = ZIL_STORAGE_READ_ONLY::searchPath->NextPathName())
					strrepc((ZIL_ICHAR *)path, tmpDirSepChar, defaultCharMap->dirSepStr[0]);
#	endif
		}
		else
			delete tmp;
	}
#endif
}

void ZIL_INTERNATIONAL::I18nInitialize(const ZIL_ICHAR *localeName, const ZIL_ICHAR *languageName)
{
#if defined(ZIL_LOAD) && defined(ZIL_DO_FILE_I18N) && (defined(ZIL_ISO8859_1) || defined(ZIL_UNICODE))
	ZIL_LOCALE_MANAGER::LoadDefaultLocale(localeName);
	ZIL_LANGUAGE_MANAGER::LoadDefaultLanguage(languageName);
	// If there is a special language_locale entry, load it
	ZIL_ICHAR specialName[40];
	strcpy(specialName, languageName);
	int i = strlen(languageName);
	specialName[i] = '_';
	strcpy(&specialName[i+1], localeName);
	ZIL_LOCALE_MANAGER::LoadDefaultLocale(specialName);
	ZIL_LANGUAGE_MANAGER::LoadDefaultLanguage(specialName);
#endif
}
