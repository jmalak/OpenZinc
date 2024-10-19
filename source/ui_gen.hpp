//	Zinc Interface Library - UI_GEN.HPP
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


// ----- Include file and environment definition ----------------------------

#if !defined(UI_GEN_HPP)
#	define UI_GEN_HPP
//#	if !defined(UI_ENV_HPP)
#		include <ui_env.hpp>
//#	endif
#if defined(ZIL_NEXTSTEP)
extern "C"
{
#	include <stdlib.h>
#	include <sys/stat.h>
}
#endif

typedef ZIL_UINT16 ZIL_NUMBERID;

// --------------------------------------------------------------------------
// ----- ZIL_OBJECTID -------------------------------------------------------
// --------------------------------------------------------------------------
// --- The following object identifier values are reserved.
// --- values 1..999 			ZIL_SIMPLE_OBJECTID
// ---		values 1..499			objects derived from UI_WINDOW_OBJECT
// ---		values 500..999			user defined simple objects
// --- values 1000..1999		ZIL_COMPLEX_OBJECTID
// ---		values 1000..1499		objects derived from UIW_WINDOW
// ---		values 1500..1999		user defined complex objects
// --- values 2000..2999		ZIL_COMPLEX_OBJECTID
// --- values 3000..3999		ZIL_DESIGNER_OBJECTID
// --- values 4000..4999		ZIL_BASE_OBJECTID
// ---		values 4000..4099		simple base objects
// ---		values 4100..4199		message objects
// ---		values 4200..4299		help objects
// --- values 5000+				

typedef ZIL_INT16 ZIL_OBJECTID;
typedef ZIL_OBJECTID ZIL_BASE_OBJECTID;
typedef ZIL_OBJECTID ZIL_SIMPLE_OBJECTID;
typedef ZIL_OBJECTID ZIL_COMPLEX_OBJECTID;
typedef ZIL_OBJECTID ZIL_COMPOSITE_OBJECTID;
typedef ZIL_OBJECTID ZIL_DESIGNER_OBJECTID;
typedef ZIL_OBJECTID ZIL_USER_OBJECTID;

const ZIL_OBJECTID ID_END					= -1;	// Special values
const ZIL_OBJECTID ID_DEFAULT				= 0;

const ZIL_OBJECTID ID_BITMAP_IMAGE			= 4000;
const ZIL_OBJECTID ID_ICON_IMAGE			= 4001;
const ZIL_OBJECTID ID_IMT_MOUSE_IMAGE		= 4002;
const ZIL_OBJECTID ID_MESSAGE				= 4100;
const ZIL_OBJECTID ID_HELP_CONTEXT			= 4200;

// --------------------------------------------------------------------------
// ----- EVENT_TYPE ---------------------------------------------------------
// --------------------------------------------------------------------------
// --- The following object identifier values are reserved.
// --- values < -2999 			reserved for future use.
// ---		values -2999..-2000		ZIL_DIALOG_EVENT
// ---		values -2999..-2000		ZIL_DEVICE_STATE
// --- 	values -999 through -1 reserved for ZIL_LOGICAL_EVENT.
// --- values 0 through 99 reserved for ZIL_RAW_CODE.
// --- values 100 through 9999 reserved for ZIL_SYSTEM_EVENT.
// --- values 10000+ available for programmer's use.

// --------------------------------------------------------------------------
// ----- ZIL_INFO_REQUEST ---------------------------------------------------
// --------------------------------------------------------------------------
// --- The following information request values are reserved.
// --- values 0..499			UI_WINDOW_OBJECT
// ---		values 500..509 		UIW_BUTTON, UIW_ICON
// ---		values 510..519 		UI_CONSTRAINT
// ---		values 520..529 		UI_ATTACHMENT
// ---		values 530..539 		UI_DIMENSION_CONSTRAINT
// ---		values 540..549 		UI_RELATIVE_CONSTRAINT
// ---		values 1000..1499 		UIW_WINDOW
// ---		values 1500..1510 		UIW_COMBO_BOX

typedef unsigned ZIL_INFO_REQUEST;
typedef ZIL_INFO_REQUEST INFO_REQUEST; // Will be moved to olddefs in the future.

// --------------------------------------------------------------------------
// ----- UI_ELEMENT ---------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UI_ELEMENT
{
	friend class ZIL_EXPORT_CLASS UI_LIST;
public:
	virtual ~UI_ELEMENT(void);
	virtual ZIL_ICHAR *ClassName(void) { return (ZIL_NULLP(ZIL_ICHAR)); }
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	int ListIndex(void);
	UI_ELEMENT *Next(void) { return (next); }
	UI_ELEMENT *Previous(void) { return (previous); }

protected:
	UI_ELEMENT *previous, *next;

	UI_ELEMENT(void);
};

// --------------------------------------------------------------------------
// ----- UI_LIST ------------------------------------------------------------
// --------------------------------------------------------------------------

typedef int (*ZIL_COMPARE_FUNCTION)(void *, void *);

class ZIL_EXPORT_CLASS UI_LIST
{
	friend class ZIL_EXPORT_CLASS UI_LIST_BLOCK;
public:
	ZIL_COMPARE_FUNCTION compareFunction;

	UI_LIST(ZIL_COMPARE_FUNCTION compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION));
	virtual ~UI_LIST(void);
	UI_ELEMENT *Add(UI_ELEMENT *newElement);
	UI_ELEMENT *Add(UI_ELEMENT *element, UI_ELEMENT *newElement);
	int Count(void);
	UI_ELEMENT *Current(void) { return (current); }
	virtual void Destroy(void);
	UI_ELEMENT *First(void) { return (first); }
	UI_ELEMENT *Get(int index);
	UI_ELEMENT *Get(int (*findFunction)(void *element1, void *matchData), void *matchData);
	int Index(UI_ELEMENT const *element);
	UI_ELEMENT *Last(void) { return (last); }
	void SetCurrent(UI_ELEMENT *element) { current = element; }
	virtual void Sort(void);
	UI_ELEMENT *Subtract(UI_ELEMENT *element);
	UI_LIST &operator+(UI_ELEMENT *element) { Add(element); return (*this); }
	UI_LIST &operator-(UI_ELEMENT *element) { Subtract(element); return (*this); }

protected:
	UI_ELEMENT *first, *last, *current;
};

// --------------------------------------------------------------------------
// ----- UI_LIST_BLOCK ------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UI_LIST_BLOCK : public UI_LIST
{
public:
	virtual ~UI_LIST_BLOCK(void);
	UI_ELEMENT *Add(void);
	UI_ELEMENT *Add(UI_ELEMENT *element);
	int Full(void) { return (freeList.First() ? FALSE : TRUE); }
	UI_ELEMENT *Subtract(UI_ELEMENT *element);

protected:
	int noOfElements;
	void *elementArray;
	UI_LIST freeList;

	UI_LIST_BLOCK(int noOfElements, ZIL_COMPARE_FUNCTION compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION));
};

// --------------------------------------------------------------------------
// ----- ZIL_BIT_VECTOR -----------------------------------------------------
// --------------------------------------------------------------------------
class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT_READ_ONLY;
class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT;

class ZIL_EXPORT_CLASS ZIL_BIT_VECTOR
{
public:
	ZIL_BIT_VECTOR(ZIL_UINT16 _size=0);
	ZIL_BIT_VECTOR(ZIL_STORAGE_OBJECT_READ_ONLY *object);
	virtual ~ZIL_BIT_VECTOR();
	void Clear(ZIL_UINT16 pos);
	void Set(ZIL_UINT16 pos);
	int Test(ZIL_UINT16 pos, int _default = FALSE);

	void Store(ZIL_STORAGE_OBJECT *object);
private:
	ZIL_UINT8 *vector;
	ZIL_UINT16 size;
};

// --------------------------------------------------------------------------
// ----- ZIL_I18N, ZIL_LOCALE, ZIL_LANGUAGE, & ZIL_DECORATION ---------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZIL_INTERNATIONAL;
class ZIL_EXPORT_CLASS ZIL_STORAGE_READ_ONLY;
class ZIL_EXPORT_CLASS ZIL_STORAGE;

const int ZIL_STRINGID_LEN = 32;

class ZIL_EXPORT_CLASS ZIL_I18N : public UI_ELEMENT
{
	friend class ZIL_EXPORT_CLASS ZIL_I18N_MANAGER;
public:
	const ZIL_ICHAR *className;
	const ZIL_ICHAR *pathName;
	ZIL_ICHAR name[12];
	int useCount;
	int error;

	ZIL_I18N(void);
	virtual ~ZIL_I18N(void);

#if defined(ZIL_LOAD)
	static int Traverse(ZIL_STORAGE_READ_ONLY *storage, const ZIL_ICHAR *_path);
	void Load(ZIL_STORAGE_READ_ONLY *storage, ZIL_STORAGE_OBJECT_READ_ONLY *object);
	virtual void ClassLoadData(ZIL_STORAGE_OBJECT_READ_ONLY *object);
#endif
#if defined(ZIL_STORE)
	static int Traverse(ZIL_STORAGE *storage, const ZIL_ICHAR *_path, int create = FALSE);
	void Store(ZIL_STORAGE *storage, ZIL_STORAGE_OBJECT *object);
	virtual void ClassStoreData(ZIL_STORAGE_OBJECT *object);
#endif
	static ZIL_STORAGE_READ_ONLY *defaultStorage;
	static ZIL_ICHAR *i18nName;
protected:
	ZIL_UINT8 allocated;
	ZIL_UINT8 defaults;

	virtual void AssignData(const ZIL_I18N *data);
	virtual void DeleteData(void);
};

class ZIL_EXPORT_CLASS ZIL_I18N_MANAGER : public UI_LIST
{
public:
	ZIL_ICHAR *defaultName;
	ZIL_ICHAR *defaultOSName;
	ZIL_I18N_MANAGER(void);
	virtual ~ZIL_I18N_MANAGER(void);
	virtual ZIL_I18N *CreateData(void) = 0;
	void FreeI18N(const ZIL_I18N *i18n);
	void LoadDefaultI18N(const ZIL_ICHAR *i18nName);
	ZIL_I18N *UseI18N(const ZIL_I18N *i18n);
	ZIL_I18N *UseI18N(const ZIL_ICHAR *className, const ZIL_ICHAR *i18nName);
protected:
	int initialized;
};

struct ZIL_EXPORT_CLASS ZIL_ERA_TABLE
{
	ZIL_UINT16 direction;
	ZIL_UINT16 offset;
	ZIL_UINT32 startDate;
	ZIL_UINT32 endDate;
	ZIL_ICHAR eraName[6];
	ZIL_ICHAR eraFormat[8];
};

struct ZIL_EXPORT_CLASS ZIL_LOCALE_ELEMENT
{
	ZIL_ICHAR decimalSeparator[4];
	ZIL_ICHAR monDecimalSeparator[4];
	ZIL_ICHAR thousandsSeparator[4];
	ZIL_ICHAR monThousandsSeparator[4];
	ZIL_ICHAR currencySymbol[8];
	char grouping[10];
	char monGrouping[10];
	ZIL_ICHAR intCurrencySymbol[5];
	int posCurrencyPrecedes;
	int negCurrencyPrecedes;
	int fractionDigits;
	int intFractionDigits;
	ZIL_ICHAR positiveSign[4];
	int posSignPrecedes;
	int posSpaceSeparation;
	ZIL_ICHAR negativeSign[4];
	int negSignPrecedes;
	int negSpaceSeparation;

	ZIL_ICHAR *bnumLeftParen;
	ZIL_ICHAR *bnumRightParen;

	ZIL_ICHAR *timeStringFormat;
	ZIL_ICHAR *dateStringFormat;
	ZIL_ICHAR *dateTimeStringFormat;
	ZIL_ICHAR *time12StringFormat;
	ZIL_ICHAR *defDigits;
	ZIL_ICHAR *altDigits;

	ZIL_ICHAR timeSeparator[4];
	ZIL_ICHAR dateSeparator[4];
	int defaultDateFlags;
	int defaultTimeFlags;
	ZIL_UINT32 beginGregorian;
	ZIL_UINT16 skipGregorian;
	ZIL_UINT16 eraTableLen;
	ZIL_ERA_TABLE *eraTable;
};

class ZIL_EXPORT_CLASS ZIL_LOCALE : public ZIL_I18N
{
	friend class ZIL_EXPORT_CLASS ZIL_LOCALE_MANAGER;
public:
	ZIL_LOCALE(void);
	virtual ~ZIL_LOCALE(void);
#if defined(ZIL_LOAD)
	virtual void ClassLoadData(ZIL_STORAGE_OBJECT_READ_ONLY *object);
#endif
#if defined(ZIL_STORE)
	virtual void ClassStoreData(ZIL_STORAGE_OBJECT *object);
#endif
	ZIL_LOCALE_ELEMENT data;
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_CURSES) || defined(ZIL_MACINTOSH)
	static int oemCountryCode;
#endif
protected:
	virtual void AssignData(const ZIL_I18N *data);
	virtual void DeleteData(void);
};

class ZIL_EXPORT_CLASS ZIL_LOCALE_MANAGER;
extern ZIL_LOCALE_MANAGER localeManager;

class ZIL_EXPORT_CLASS ZIL_LOCALE_MANAGER : public ZIL_I18N_MANAGER
{
public:
	ZIL_LOCALE_MANAGER(void);
	virtual ZIL_I18N *CreateData(void) { return new ZIL_LOCALE(); }

	static void FreeLocale(const ZIL_LOCALE *locale) { localeManager.FreeI18N(locale); }
	static void LoadDefaultLocale(const ZIL_ICHAR *localeName) { localeManager.LoadDefaultI18N(localeName); }
	static const ZIL_LOCALE *UseLocale(const ZIL_LOCALE *locale) { localeManager.UseI18N(locale); return locale; }
	static const ZIL_LOCALE *UseLocale(const ZIL_ICHAR *localeName = ZIL_NULLP(ZIL_ICHAR));

	static void SetLocale(const ZIL_ICHAR *localeName,
		ZIL_LOCALE_ELEMENT *defaultLocale);
};

#if defined(ZIL_UNICODE)
	struct ZIL_PRIVATE_LANGUAGE_ELEMENT
	{
		char *text;
		ZIL_NUMBERID numberID;
		char stringID[ZIL_STRINGID_LEN];
	};
#else
#	define ZIL_PRIVATE_LANGUAGE_ELEMENT	ZIL_LANGUAGE_ELEMENT
#endif

struct ZIL_EXPORT_CLASS ZIL_LANGUAGE_ELEMENT
{
//    ZIL_LANGUAGE_ELEMENT();
	ZIL_ICHAR *text;
	ZIL_NUMBERID numberID;
	ZIL_ICHAR stringID[ZIL_STRINGID_LEN];

	void SwapData(ZIL_LANGUAGE_ELEMENT &language);
};

class ZIL_EXPORT_CLASS ZIL_LANGUAGE : public ZIL_I18N
{
	friend class ZIL_EXPORT_CLASS ZIL_LANGUAGE_MANAGER;
public:
	ZIL_LANGUAGE(void);
	virtual ~ZIL_LANGUAGE(void);
#if defined(ZIL_LOAD)
	ZIL_LANGUAGE(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY));
	virtual void Load(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY));
	void Load(ZIL_STORAGE_READ_ONLY *storage, ZIL_STORAGE_OBJECT_READ_ONLY *object)
		{ ZIL_I18N::Load(storage, object); }
	virtual void ClassLoadData(ZIL_STORAGE_OBJECT_READ_ONLY *object);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name,
		ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
		ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT));
	void Store(ZIL_STORAGE *storage, ZIL_STORAGE_OBJECT *object)
		{ ZIL_I18N::Store(storage, object); }
	virtual void ClassStoreData(ZIL_STORAGE_OBJECT *object);
#endif
	int noOfElements;
	ZIL_LANGUAGE_ELEMENT *data;
	ZIL_ICHAR *GetMessage(ZIL_NUMBERID numberID, int useDefault = FALSE) const;
protected:
	virtual void AssignData(const ZIL_I18N *data);
	virtual void DeleteData(void);
};

class ZIL_EXPORT_CLASS ZIL_LANGUAGE_MANAGER;
extern ZIL_LANGUAGE_MANAGER languageManager;

class ZIL_EXPORT_CLASS ZIL_LANGUAGE_MANAGER : public ZIL_I18N_MANAGER
{
public:
	ZIL_LANGUAGE_MANAGER(void);
	virtual ZIL_I18N *CreateData(void) { return new ZIL_LANGUAGE; }

	static void FreeLanguage(const ZIL_LANGUAGE *language) { languageManager.FreeI18N(language); }
	static void LoadDefaultLanguage(const ZIL_ICHAR *languageName) { languageManager.LoadDefaultI18N(languageName); }
	static const ZIL_LANGUAGE *UseLanguage(const ZIL_LANGUAGE *language) { languageManager.UseI18N(language); return language; }
	static const ZIL_LANGUAGE *UseLanguage(const ZIL_ICHAR *className, const ZIL_ICHAR *languageName = ZIL_NULLP(ZIL_ICHAR)) { return (ZIL_LANGUAGE *)languageManager.UseI18N(className, languageName); }

	static void SetLanguage(const ZIL_ICHAR *className,
		ZIL_PRIVATE_LANGUAGE_ELEMENT *defaultMessages);
};

struct ZIL_EXPORT_CLASS ZIL_TEXT_ELEMENT
{
	ZIL_ICHAR *text;
	ZIL_NUMBERID numberID;
	ZIL_ICHAR stringID[ZIL_STRINGID_LEN];
};

struct ZIL_EXPORT_CLASS ZIL_BITMAP_ELEMENT
{
	ZIL_UINT8 *bitmap;
	ZIL_NUMBERID numberID;
	ZIL_ICHAR stringID[ZIL_STRINGID_LEN];
};

class ZIL_EXPORT_CLASS ZIL_DECORATION : public ZIL_I18N
{
	friend class ZIL_EXPORT_CLASS ZIL_DECORATION_MANAGER;
public:
	ZIL_DECORATION(void);
	virtual ~ZIL_DECORATION(void);
#if defined(ZIL_LOAD)
	virtual void ClassLoadData(ZIL_STORAGE_OBJECT_READ_ONLY *object);
#endif
#if defined(ZIL_STORE)
	virtual void ClassStoreData(ZIL_STORAGE_OBJECT *object);
#endif
	int noOfTextElements;
	ZIL_TEXT_ELEMENT *text;
	int noOfBitmapElements;
	ZIL_BITMAP_ELEMENT *bitmap;
	ZIL_ICHAR *GetText(ZIL_NUMBERID numberID, int useDefault = FALSE) const;
	ZIL_UINT8 *GetBitmap(ZIL_NUMBERID numberID, int useDefault = FALSE) const;
protected:
	virtual void AssignData(const ZIL_I18N *data);
	virtual void DeleteData(void);
};

class ZIL_EXPORT_CLASS ZIL_DECORATION_MANAGER;
extern ZIL_DECORATION_MANAGER decorationManager;

class ZIL_EXPORT_CLASS ZIL_DECORATION_MANAGER : public ZIL_I18N_MANAGER
{
public:
	ZIL_DECORATION_MANAGER(void);
	virtual ZIL_I18N *CreateData(void) { return new ZIL_DECORATION; }

	static void FreeDecorations(const ZIL_DECORATION *decorations) { decorationManager.FreeI18N(decorations); }
	static void LoadDefaultDecorations(const ZIL_ICHAR *decorationName) { decorationManager.LoadDefaultI18N(decorationName); }
	static const ZIL_DECORATION *UseDecorations(const ZIL_DECORATION *decorations) { decorationManager.UseI18N(decorations); return decorations; }
	static const ZIL_DECORATION *UseDecorations(const ZIL_ICHAR *className, const ZIL_ICHAR *decorationName = ZIL_NULLP(ZIL_ICHAR)) { return (ZIL_DECORATION *)decorationManager.UseI18N(className, decorationName); }

	static void SetDecorations(const ZIL_ICHAR *className,
		ZIL_TEXT_ELEMENT *defaultText,
		ZIL_BITMAP_ELEMENT *defaultBitmap);
};

// --------------------------------------------------------------------------
// ----- ZIL_MAP_CHARS ------------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_UNICODE)
class ZIL_EXPORT_CLASS ZIL_CHAR_MAP;
#endif

class ZIL_EXPORT_CLASS ZIL_MAP_CHARS
{
private:
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_READ_ONLY;
#if defined(ZIL_UNICODE)
	friend class ZIL_EXPORT_CLASS ZIL_CHAR_MAP;

	typedef struct
	{
		ZIL_UINT8 min, max;		// Range of legal leadin bytes
	} leadinEntry;

	ZIL_INT16 leadinSize;		// Number of leadins valid.
	leadinEntry leadins[20];	// Pairs of lead bytes that mark
					// non-ASCII characters.
	// Major maps.
	ZIL_CHAR_MAP *toHardwareDefault;
	ZIL_CHAR_MAP *toUnicodeDefault;
	// Minor maps.
	ZIL_CHAR_MAP *toHardwareExtra;
	ZIL_CHAR_MAP *toUnicodeExtra;
	ZIL_STORAGE_READ_ONLY *charMap;	// File the char maps are in.
	ZIL_ICHAR *mapUnicodeToLocal;
	ZIL_CHAR_MAP *LoadFontMap(ZIL_STORAGE_READ_ONLY &file, const ZIL_ICHAR *name);
#else
	ZIL_UINT8 minVal, maxVal;
	ZIL_UINT8 *fromStandard;
	ZIL_UINT8 *toStandard;
	ZIL_UINT8 *LoadFontMap(ZIL_STORAGE_READ_ONLY &file, const ZIL_ICHAR *name);
#endif
	int doDelete;
public:
	ZIL_MAP_CHARS(const ZIL_ICHAR *_mapName, const ZIL_ICHAR *extraName);
	ZIL_MAP_CHARS(const ZIL_ICHAR *_mapName, const void *fromStandard, const void *toStandard, int _doDelete);
	virtual ~ZIL_MAP_CHARS();
	char *MapChar(ZIL_ICHAR mapped);
	char *MapText(const ZIL_ICHAR *mapped,
		char *unMapped = ZIL_NULLP(char), int allocate = TRUE);
	ZIL_ICHAR UnMapChar(const char *unMapped);
	ZIL_ICHAR *UnMapText(const char *unMapped,
		ZIL_ICHAR *mapped = ZIL_NULLP(ZIL_ICHAR), int allocate = TRUE);
#if defined(ZIL_UNICODE)
	int mblen(const char *hardware);
	int wcstombs(char *s, const ZIL_ICHAR *pwcs, int n= -1);
	int mbstowcs(ZIL_ICHAR *pwcs, const char *s, int n = -1);
#endif
	ZIL_ICHAR dirSepStr[2];
	int error;		// Returned error of storage
	ZIL_ICHAR name[12];
};

// --------------------------------------------------------------------------
// ----- ZIL_INTERNATIONAL --------------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MACINTOSH)
	const OSType postFileCreator = 'ZDes';
	const OSType postFileType = 'POST';
#elif defined(__IBMCPP__) || defined(__GLOCKENSPIEL__) || (_ALPHA_ == 1 && defined(_MSC_VER))
#	if (_ALPHA_ == 1 && defined(_MSC_VER))
#		define itoa	_itoa
#	endif
#	define chdir		_chdir
#	define close		_close
#	define getcwd		_getcwd
#	define open			_open
#	define lseek		_lseek
#	define read			_read
#	define unlink		_unlink
#	define write		_write
#elif defined(__SC__)
#	define strcmp(x,y)	_inline_strcmp(x,y)
#	define strcpy(x,y)	_inline_strcpy(x,y)
#	define strlen(s)	_inline_strlen(s)
#	if defined unlink
#		undef unlink
#	endif
#endif

typedef char ZIL_FILE_CHAR;

class ZIL_EXPORT_CLASS ZIL_INTERNATIONAL
{
	friend class ZIL_EXPORT_CLASS ZIL_LANGUAGE;
	friend class ZIL_EXPORT_CLASS ZIL_LOCALE;
	friend class ZIL_EXPORT_CLASS ZIL_MAP_CHARS;
public:
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT;

	ZIL_INTERNATIONAL(void);
	virtual ~ZIL_INTERNATIONAL(void);

	// Posix time() support
	static ZIL_INT32 minutesWestGMT;
	void TimeStamp(ZIL_UINT32 *value);

	static void DefaultI18nInitialize(void);
	static void CharMapInitialize(void);
	static void OSI18nInitialize(ZIL_ICHAR *langName, ZIL_ICHAR *locName, int forceInitialization = FALSE);
	static void I18nInitialize(const ZIL_ICHAR *localeName, const ZIL_ICHAR *languageName);

	// Support for international Unicode/ISO helper functions.
	static int chartod(const ZIL_ICHAR value);
	static int IsNonspacing(ZIL_ICHAR value);

	// Support for international Ctype functions.
	static int IsAlnum(ZIL_ICHAR value);
	static int IsAlpha(ZIL_ICHAR value);
	static int IsAscii(ZIL_ICHAR value);
	static int IsCntrl(ZIL_ICHAR value);
	static int IsDigit(ZIL_ICHAR value);
	static int IsGraph(ZIL_ICHAR value);
	static int IsLower(ZIL_ICHAR value);
	static int IsPrint(ZIL_ICHAR value);
	static int IsPunct(ZIL_ICHAR value);
	static int IsSpace(ZIL_ICHAR value);
	static int IsUpper(ZIL_ICHAR value);
	static int IsXDigit(ZIL_ICHAR value);
	static ZIL_ICHAR ToLower(ZIL_ICHAR value);
	static ZIL_ICHAR ToUpper(ZIL_ICHAR value);

	// Support for internationalized ANSI routines.
#if defined(ZIL_NEXTSTEP)
	static double strtod(const ZIL_ICHAR *nptr)
	{ return ::strtod(nptr, ZIL_NULLP(const ZIL_ICHAR *)); }
	static int strtoi(const ZIL_ICHAR *nptr)
	{ return (int)::strtol(nptr, ZIL_NULLP(const ZIL_ICHAR *), 10); }
	static long strtol(const ZIL_ICHAR *nptr) { return ::strtol(nptr, ZIL_NULLP(const ZIL_ICHAR *), 10); }
#else
	static int atoi(const ZIL_ICHAR *nptr) { return (int)strtol(nptr, ZIL_NULLP(ZIL_ICHAR *), 10); }
	static long atol(const ZIL_ICHAR *nptr) { return strtol(nptr, ZIL_NULLP(ZIL_ICHAR *), 10); }
	static double atof(const ZIL_ICHAR *nptr) { return strtod(nptr, ZIL_NULLP(ZIL_ICHAR *)); }
#endif
	static long strtol(const ZIL_ICHAR *nptr, ZIL_ICHAR **endptr, int base);
	static unsigned long strtoul(const ZIL_ICHAR *nptr, ZIL_ICHAR **endptr, int base);
	static double strtod(const ZIL_ICHAR *nptr, ZIL_ICHAR **endptr);

	static ZIL_ICHAR *strcpy(ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	static int strcmp(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	static int strncmp(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n);
	static ZIL_ICHAR *strncpy(ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n);

	static ZIL_ICHAR *strcat(ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	static ZIL_ICHAR *strncat(ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n);

#if !defined(__SC__) || defined(ZIL_MACINTOSH)
	static int strcoll(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
#endif
	static int strxfrm(ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n);
	static ZIL_ICHAR *strchr(const ZIL_ICHAR *s, int c);
	static int strcspn(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	static ZIL_ICHAR *strpbrk(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	static ZIL_ICHAR *strrchr(const ZIL_ICHAR *s, int c);
	static int strspn(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	static ZIL_ICHAR *strstr(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	static ZIL_ICHAR *strtok(ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	// static ZIL_ICHAR *strerror(int errnum);  Not Yet.

	// Zinc (non-ANSI) routines
	static int stricmp(const ZIL_ICHAR *a, const ZIL_ICHAR *b);
	static int strnicmp(const ZIL_ICHAR *a, const ZIL_ICHAR *b, int n);
	static ZIL_ICHAR *strlwr(ZIL_ICHAR *string);
	static ZIL_ICHAR *strupr(ZIL_ICHAR *string);
	static void strstrip(ZIL_ICHAR *string, ZIL_ICHAR c);
	static ZIL_ICHAR *strdup(const ZIL_ICHAR *string);
	static int WildStrcmp(const ZIL_ICHAR *str, const ZIL_ICHAR *pattern);
	static void StripHotMark(ZIL_ICHAR *fillLine);
	static int streq(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2);
	static int strneq(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n);

	// Ansi routines
	static int strlen(const ZIL_ICHAR *string);
	static void itoa(ZIL_INT32 value, ZIL_ICHAR *string, int radix, int pad = 0);
	static void strrepc(ZIL_ICHAR *string, int c, int repc);
	static int sprintf(ZIL_ICHAR *buffer, const ZIL_ICHAR *format, ...);
	static int sscanf(ZIL_ICHAR *buffer, const ZIL_ICHAR *format, ...);

	// File support routines.
	static int chdir(const ZIL_ICHAR *path);
	static ZIL_ICHAR *getcwd(ZIL_ICHAR *buffer, unsigned length);
	static ZIL_ICHAR *getenv(const ZIL_ICHAR *envname);
	static int open(const ZIL_ICHAR *path, int access, unsigned mode = 0);
	static int rename(const ZIL_ICHAR *oldPath, const ZIL_ICHAR *newPath);
	static int stat(const ZIL_ICHAR *path, void *);
	static ZIL_ICHAR *tmpnam(ZIL_ICHAR *path);
	static int unlink(const ZIL_ICHAR *path);

	// Character mapping routines
 	static char *MapText(const ZIL_ICHAR *mapped,
		char *unMapped = ZIL_NULLP(char), int allocate = TRUE)
		{ return (defaultCharMap->MapText(mapped, unMapped, allocate)); }
	static ZIL_ICHAR *UnMapText(const char *unMapped,
		ZIL_ICHAR *mapped = ZIL_NULLP(ZIL_ICHAR), int allocate = TRUE)
		{ return (defaultCharMap->UnMapText(unMapped, mapped, allocate)); }
	static ZIL_ICHAR *ISOtoUNICODE(const char *isoString, ZIL_ICHAR *retValue = ZIL_NULLP(ZIL_ICHAR));

	// File support routines.
#if defined(ZIL_UNICODE)
	static void ConvertFromFilename(ZIL_ICHAR *dst, const ZIL_FILE_CHAR *src);
	static ZIL_FILE_CHAR *ConvertToFilename(const ZIL_ICHAR *src);
#endif
#if defined(ZIL_MACINTOSH)
	static void pstrcat(StringPtr dst, StringPtr src);
	static void pstrinsert(StringPtr dst, StringPtr src);
	static void GetPathNameFromDirID(StringPtr fullPathName, long dirID, short vRefNum);
#endif

	// Character mapping routines
	static ZIL_ICHAR *ISOtoICHAR(const char *isoString,
		ZIL_ICHAR *icharString = ZIL_NULLP(ZIL_ICHAR));
	static int LoadICHARtoHardware(const ZIL_ICHAR *mapName,
		const ZIL_ICHAR *extraName);
	static ZIL_ICHAR UnMapChar(const char *hardware)
		{ return (defaultCharMap->UnMapChar(hardware)); }
	static char *MapChar(ZIL_ICHAR unicode)
		{ return (defaultCharMap->MapChar(unicode)); }

#if defined(ZIL_UNICODE)
	static int mblen(const char *hardware)
		{ return (defaultCharMap->mblen(hardware)); }
	static int wcstombs(char *s, const ZIL_ICHAR *pwcs, int n = -1)
		{ return (defaultCharMap->wcstombs(s, pwcs, n)); }
	static int mbstowcs(ZIL_ICHAR *pwcs, const char *s, int n = -1)
		{ return (defaultCharMap->mbstowcs(pwcs, s, n)); }
	static ZIL_ICHAR *DecomposeCharacter(ZIL_ICHAR val);
	static ZIL_ICHAR *DecomposeString(const ZIL_ICHAR *str, int n = -1);
#endif
private:
	static ZIL_ICHAR zincLangString[];

	static const char *LanguageNameToZincLanguage(const char *osLanguage);
	static const char *ZincLocaleToDefaultLanguage(const char *osLanguage);
	static const ZIL_ICHAR *LanguageToDefaultLocale(const ZIL_ICHAR *language);
	static const char *CountryCodeToDefaultLocale(int countryCode);
	
	// I18N member variables and functions.
public:
	static void ParseLangEnv(ZIL_ICHAR *codeSet, ZIL_ICHAR *_locName, ZIL_ICHAR *_langName);
	static const ZIL_LOCALE *defaultLocale;
	static const ZIL_LOCALE *canonicalLocale;
	static ZIL_MAP_CHARS *defaultCharMap;

	static ZIL_ICHAR _blankString[];
	static ZIL_ICHAR _errorString[];

	static void MachineName(void);
	static ZIL_ICHAR machineName[32];

	// File creator and type.
#if defined(ZIL_MACINTOSH)
	static OSType fileCreator;
	static OSType fileType;
#endif
};

// --------------------------------------------------------------------------
// ----- ZIL_BIGNUM ---------------------------------------------------------
// --------------------------------------------------------------------------

// --- NMF_FLAGS ---
typedef UIF_FLAGS NMF_FLAGS;
const NMF_FLAGS NMF_NO_FLAGS			= 0x0000;
const NMF_FLAGS NMF_CURRENCY 			= 0x0002;
const NMF_FLAGS NMF_CREDIT 				= 0x0004;
const NMF_FLAGS NMF_COMMAS 				= 0x0008;
const NMF_FLAGS NMF_PERCENT 			= 0x0010;
const NMF_FLAGS NMF_SCIENTIFIC 			= 0x0020;
const NMF_FLAGS NMF_DECIMAL_FLAGS		= 0xFF00;

#define	NMF_DECIMAL(decimal)			(((decimal) + 1) << 8)
#define	NMF_DIGITS(flags)				((((int)(flags) >> 8) & 0xFF) - 1)

// --- NMI_RESULT ---
enum NMI_RESULT
{
	NMI_OK = 0,				// Number successfully imported.
	NMI_INVALID,			// Invalid number.
	NMI_OUT_OF_RANGE,		// Number out of range.
	NMI_LESS_THAN_RANGE,	// Number less than positively open-ended range.
	NMI_GREATER_THAN_RANGE	// Number greater than negatively open-ended range.
};

#define ZIL_NUMBER_WHOLE	30
#define ZIL_NUMBER_DECIMAL	8
// Warning, if ZIL_DIGITS is not 4 or 8, you must rewrite "*" and "/" and "%".
#if ZIL_WORD_SIZE > 16
#	define ZIL_DIGITS		8
#else
#	define ZIL_DIGITS		4
#endif
typedef ZIL_INT32   		ZIL_IBIGNUM;
typedef double 				ZIL_RBIGNUM;
#if ZIL_DIGITS == 4
	typedef ZIL_UINT16 ZIL_NUMBER;
#elif ZIL_DIGITS == 8
	typedef ZIL_UINT32 ZIL_NUMBER;
#endif

#if defined(__ghs)
	// Green Hills C++ (__ghs) thinks friend functions have C linkage!
	class ZIL_BIGNUM;
	extern "C++" {
		ZIL_BIGNUM &abs(const ZIL_BIGNUM &number);
		ZIL_BIGNUM &ceil(const ZIL_BIGNUM &number);
		ZIL_BIGNUM &floor(const ZIL_BIGNUM &number);
		ZIL_BIGNUM &round(const ZIL_BIGNUM &number, int places = 0);
		ZIL_BIGNUM &truncate(const ZIL_BIGNUM &number, int places = 0);
	}
#endif

class ZIL_EXPORT_CLASS ZIL_BIGNUM : public ZIL_INTERNATIONAL
{
	friend class ZIL_EXPORT_CLASS UIW_BIGNUM;

	friend ZIL_BIGNUM &abs(const ZIL_BIGNUM &number);
	friend ZIL_BIGNUM &ceil(const ZIL_BIGNUM &number);
	friend ZIL_BIGNUM &floor(const ZIL_BIGNUM &number);
	friend ZIL_BIGNUM &round(const ZIL_BIGNUM &number, int places = 0);
	friend ZIL_BIGNUM &truncate(const ZIL_BIGNUM &number, int places = 0);
public:
	ZIL_BIGNUM(void);
	ZIL_BIGNUM(ZIL_IBIGNUM value);
	ZIL_BIGNUM(ZIL_RBIGNUM value);
	ZIL_BIGNUM(const ZIL_ICHAR *string,
		const ZIL_ICHAR *decimalString = ZIL_NULLP(ZIL_ICHAR),
		const ZIL_ICHAR *signString = ZIL_NULLP(ZIL_ICHAR));
	ZIL_BIGNUM(const ZIL_BIGNUM &number);
	virtual ~ZIL_BIGNUM(void);
	void Export(ZIL_IBIGNUM *value);
	void Export(ZIL_RBIGNUM *value);
	void Export(ZIL_ICHAR *string, NMF_FLAGS nmFlags,
		const ZIL_ICHAR *decimalStr = ZIL_NULLP(ZIL_ICHAR),
		const ZIL_ICHAR *signStr = ZIL_NULLP(ZIL_ICHAR));
	NMI_RESULT Import(ZIL_IBIGNUM value);
	NMI_RESULT Import(ZIL_RBIGNUM value);
	NMI_RESULT Import(const ZIL_BIGNUM &number);
	NMI_RESULT Import(const ZIL_ICHAR *string,
		const ZIL_ICHAR *decimalString = ZIL_NULLP(ZIL_ICHAR),
		const ZIL_ICHAR *signString = ZIL_NULLP(ZIL_ICHAR));

	ZIL_BIGNUM &operator=(const ZIL_BIGNUM &number);
	ZIL_BIGNUM &operator+(const ZIL_BIGNUM &number);
	ZIL_BIGNUM &operator-(const ZIL_BIGNUM &number);
	ZIL_BIGNUM &operator*(const ZIL_BIGNUM &number);
	ZIL_BIGNUM &operator++(void);
	ZIL_BIGNUM &operator--(void);
	ZIL_BIGNUM &operator+=(const ZIL_BIGNUM &number)
		{ return (*this = *this + number); }
	ZIL_BIGNUM &operator-=(const ZIL_BIGNUM &number)
		{ return (*this = *this - number); }
	int operator==(const ZIL_BIGNUM &number)
		{ return (Compare(number, FALSE, FALSE, TRUE)); }
	int operator!=(const ZIL_BIGNUM &number)
		{ return (Compare(number, TRUE, TRUE, FALSE)); }
	int operator>(const ZIL_BIGNUM &number)
		{ return (Compare(number, TRUE, FALSE, FALSE)); }
	int operator>=(const ZIL_BIGNUM &number)
		{ return (Compare(number, TRUE, FALSE, TRUE)); }
	int operator<(const ZIL_BIGNUM &number)
		{ return (Compare(number, FALSE, TRUE, FALSE)); }
	int operator<=(const ZIL_BIGNUM &number)
		{ return (Compare(number, FALSE, TRUE, TRUE)); }
	void SetLocale(const ZIL_ICHAR *localeName)
		{ ZIL_LOCALE_MANAGER::FreeLocale(myLocale); myLocale = ZIL_LOCALE_MANAGER::UseLocale(localeName); }

protected:
	const ZIL_LOCALE *myLocale;
private:
	int sign;
	ZIL_NUMBER num[(ZIL_NUMBER_WHOLE+ZIL_NUMBER_DECIMAL+ZIL_DIGITS-1)/ZIL_DIGITS];

	int DoAddLoop(const ZIL_NUMBER *aw, int carry);
	void NinesComplement(void);
	void Zero(void);
	int AddSameSign(const ZIL_BIGNUM &b, ZIL_BIGNUM &result);
	int AddDiffSign(const ZIL_BIGNUM &b, ZIL_BIGNUM &result);
	ZIL_BIGNUM &FloorCeil(int sign) const;
	int Compare(const ZIL_BIGNUM &b, int gt, int ls, int eq);
	void CheckZero(void);
	void Itob(ZIL_IBIGNUM value);
	void Rtob(ZIL_RBIGNUM value);
	ZIL_ICHAR *Copy(ZIL_ICHAR *to, const ZIL_ICHAR *from);

	ZIL_BIGNUM &abs(void) const;
	ZIL_BIGNUM &Round(int places) const;
	ZIL_BIGNUM &Truncate(int places) const;
	void MakeStack(void);
	void DeleteStack(void);
};

// --------------------------------------------------------------------------
// ----- ZIL_UTIME ----------------------------------------------------------
// --------------------------------------------------------------------------

const ZIL_NUMBERID ZIL_AM = 1;
const ZIL_NUMBERID ZIL_PM = 2;
const ZIL_NUMBERID ZIL_TZ = 3;

const ZIL_NUMBERID ZIL_DAY_1 = 1;
const ZIL_NUMBERID ZIL_DAY_2 = 2;
const ZIL_NUMBERID ZIL_DAY_3 = 3;
const ZIL_NUMBERID ZIL_DAY_4 = 4;
const ZIL_NUMBERID ZIL_DAY_5 = 5;
const ZIL_NUMBERID ZIL_DAY_6 = 6;
const ZIL_NUMBERID ZIL_DAY_7 = 7;
const ZIL_NUMBERID ZIL_SHDAY_1 = ZIL_DAY_7 + 1;
const ZIL_NUMBERID ZIL_SHDAY_2 = ZIL_DAY_7 + 2;
const ZIL_NUMBERID ZIL_SHDAY_3 = ZIL_DAY_7 + 3;
const ZIL_NUMBERID ZIL_SHDAY_4 = ZIL_DAY_7 + 4;
const ZIL_NUMBERID ZIL_SHDAY_5 = ZIL_DAY_7 + 5;
const ZIL_NUMBERID ZIL_SHDAY_6 = ZIL_DAY_7 + 6;
const ZIL_NUMBERID ZIL_SHDAY_7 = ZIL_DAY_7 + 7;
const ZIL_NUMBERID ZIL_ABDAY_1 = ZIL_SHDAY_7 + 1;
const ZIL_NUMBERID ZIL_ABDAY_2 = ZIL_SHDAY_7 + 2;
const ZIL_NUMBERID ZIL_ABDAY_3 = ZIL_SHDAY_7 + 3;
const ZIL_NUMBERID ZIL_ABDAY_4 = ZIL_SHDAY_7 + 4;
const ZIL_NUMBERID ZIL_ABDAY_5 = ZIL_SHDAY_7 + 5;
const ZIL_NUMBERID ZIL_ABDAY_6 = ZIL_SHDAY_7 + 6;
const ZIL_NUMBERID ZIL_ABDAY_7 = ZIL_SHDAY_7 + 7;

const ZIL_NUMBERID ZIL_MON_1 = 1;
const ZIL_NUMBERID ZIL_MON_2 = 2;
const ZIL_NUMBERID ZIL_MON_3 = 3;
const ZIL_NUMBERID ZIL_MON_4 = 4;
const ZIL_NUMBERID ZIL_MON_5 = 5;
const ZIL_NUMBERID ZIL_MON_6 = 6;
const ZIL_NUMBERID ZIL_MON_7 = 7;
const ZIL_NUMBERID ZIL_MON_8 = 8;
const ZIL_NUMBERID ZIL_MON_9 = 9;
const ZIL_NUMBERID ZIL_MON_10 = 10;
const ZIL_NUMBERID ZIL_MON_11 = 11;
const ZIL_NUMBERID ZIL_MON_12 = 12;
const ZIL_NUMBERID ZIL_SHMON_1 = ZIL_MON_12 + 1;
const ZIL_NUMBERID ZIL_SHMON_2 = ZIL_MON_12 + 2;
const ZIL_NUMBERID ZIL_SHMON_3 = ZIL_MON_12 + 3;
const ZIL_NUMBERID ZIL_SHMON_4 = ZIL_MON_12 + 4;
const ZIL_NUMBERID ZIL_SHMON_5 = ZIL_MON_12 + 5;
const ZIL_NUMBERID ZIL_SHMON_6 = ZIL_MON_12 + 6;
const ZIL_NUMBERID ZIL_SHMON_7 = ZIL_MON_12 + 7;
const ZIL_NUMBERID ZIL_SHMON_8 = ZIL_MON_12 + 8;
const ZIL_NUMBERID ZIL_SHMON_9 = ZIL_MON_12 + 9;
const ZIL_NUMBERID ZIL_SHMON_10 = ZIL_MON_12 + 10;
const ZIL_NUMBERID ZIL_SHMON_11 = ZIL_MON_12 + 11;
const ZIL_NUMBERID ZIL_SHMON_12 = ZIL_MON_12 + 12;
const ZIL_NUMBERID ZIL_ABMON_1 = ZIL_SHMON_12 + 1;
const ZIL_NUMBERID ZIL_ABMON_2 = ZIL_SHMON_12 + 2;
const ZIL_NUMBERID ZIL_ABMON_3 = ZIL_SHMON_12 + 3;
const ZIL_NUMBERID ZIL_ABMON_4 = ZIL_SHMON_12 + 4;
const ZIL_NUMBERID ZIL_ABMON_5 = ZIL_SHMON_12 + 5;
const ZIL_NUMBERID ZIL_ABMON_6 = ZIL_SHMON_12 + 6;
const ZIL_NUMBERID ZIL_ABMON_7 = ZIL_SHMON_12 + 7;
const ZIL_NUMBERID ZIL_ABMON_8 = ZIL_SHMON_12 + 8;
const ZIL_NUMBERID ZIL_ABMON_9 = ZIL_SHMON_12 + 9;
const ZIL_NUMBERID ZIL_ABMON_10 = ZIL_SHMON_12 + 10;
const ZIL_NUMBERID ZIL_ABMON_11 = ZIL_SHMON_12 + 11;
const ZIL_NUMBERID ZIL_ABMON_12 = ZIL_SHMON_12 + 12;

class ZIL_EXPORT_CLASS ZIL_UTIME : public ZIL_INTERNATIONAL
{
	friend class ZIL_EXPORT_CLASS UIW_TIME;
	friend class ZIL_EXPORT_CLASS UIW_DATE;
public:
	static ZIL_ICHAR _dayName[];
	static ZIL_ICHAR _monthName[];
	static ZIL_ICHAR _timeName[];
	static int defaultInitialized;

	enum UTMI_RESULT
	{
		UTMI_OK = 0,			// Time successfully imported.
		UTMI_INVALID,			// Invalid time value or format.
		UTMI_VALUE_MISSING,		// Required value not present.
		UTMI_OUT_OF_RANGE,		// Time out of range (used by UIW_TIME)
		UTMI_LESS_THAN_RANGE,	// Time less than positively open-ended range.
		UTMI_GREATER_THAN_RANGE	// Time greater than negatively open-ended range.
	};

	ZIL_UTIME(void);
	ZIL_UTIME(const ZIL_UTIME &utime);
	ZIL_UTIME(const ZIL_ICHAR *string);
	ZIL_UTIME(int year, int month, int day,
		  int hour, int minute, int second, int milliSecond);
	virtual ~ZIL_UTIME(void);
	int DaysInMonth(int month = 0);
	int DaysInYear(void) { return (LeapYear() ? 366 : 365); }
	void Export(int *year, int *month, int *day, int *hour, int *minute, int *second, int *milliSecond);
	int Export(ZIL_ICHAR *string, int maxsize, const ZIL_ICHAR *format);
	int Export(ZIL_ICHAR *string, int maxsize, const ZIL_ICHAR **formats);
	UTMI_RESULT Import(void);
	UTMI_RESULT Import(const ZIL_UTIME &utime);
	const ZIL_ICHAR *Import(const ZIL_ICHAR *string, const ZIL_ICHAR *format);
	int LeapYear(void);

	ZIL_UTIME *operator=(const ZIL_UTIME &utime);
	ZIL_UTIME *operator+(const ZIL_UTIME &utime);
	ZIL_UTIME *operator-(const ZIL_UTIME &utime);
	int operator==(const ZIL_UTIME &utime);
	int operator!=(const ZIL_UTIME &utime);
	int operator>(const ZIL_UTIME &utime);
	int operator>=(const ZIL_UTIME &utime);
	int operator<(const ZIL_UTIME &utime);
	int operator<=(const ZIL_UTIME &utime);

	void SetLanguage(const ZIL_ICHAR *languageName = ZIL_NULLP(ZIL_ICHAR));
	void SetLocale(const ZIL_ICHAR *localeName)
		{ ZIL_LOCALE_MANAGER::FreeLocale(myLocale); myLocale = ZIL_LOCALE_MANAGER::UseLocale(localeName); }

	void Import(int year, int month, int day,
		    int hour, int minute, int second, int millisecond);

	// This is undocumented and private to Zinc Application Framework.
	ZIL_INT32 Difference(const ZIL_UTIME &utime);

protected:
	ZIL_UINT32 jday;
	ZIL_INT32 usec;
	int recurse;
	const ZIL_LOCALE *myLocale;
	const ZIL_LANGUAGE *myDayStrings;
	const ZIL_LANGUAGE *myMonthStrings;
	const ZIL_LANGUAGE *myTimeStrings;

	void ConvertJday(int *pYear, int *pMonth, int *pDay, int *pDayOfWeek);
	void ConvertUsec(int *hour, int *minute, int *second, int *milliSecond);
	int DayOfWeek(void);
	void MakeCanonical(void);
public:
	int basisYear;
	int zoneOffset;
};

// --------------------------------------------------------------------------
// ----- ZIL_DATE -----------------------------------------------------------
// --------------------------------------------------------------------------

// --- DTF_FLAGS ---
typedef UIF_FLAGS DTF_FLAGS;
const DTF_FLAGS DTF_NO_FLAGS			= 0x0000;
const DTF_FLAGS DTF_US_FORMAT		 	= 0x0001;
const DTF_FLAGS DTF_EUROPEAN_FORMAT		= 0x0002;
const DTF_FLAGS DTF_JAPANESE_FORMAT		= 0x0004;
const DTF_FLAGS DTF_ASIAN_FORMAT		= 0x0004;
const DTF_FLAGS DTF_MILITARY_FORMAT		= 0x0008;
const DTF_FLAGS DTF_DASH				= 0x0010;
const DTF_FLAGS DTF_SLASH				= 0x0020;
const DTF_FLAGS DTF_ALPHA_MONTH			= 0x0040;
const DTF_FLAGS DTF_DAY_OF_WEEK			= 0x0080;
const DTF_FLAGS DTF_UPPER_CASE			= 0x0100;
const DTF_FLAGS DTF_SHORT_YEAR			= 0x0200;
const DTF_FLAGS DTF_SHORT_MONTH			= 0x0400;
const DTF_FLAGS DTF_SHORT_DAY			= 0x0800;
const DTF_FLAGS DTF_ZERO_FILL			= 0x1000;
const DTF_FLAGS DTF_SYSTEM				= 0x4000;

// --- DTI_RESULT ---
enum DTI_RESULT
{
	DTI_OK = 0,				// Date successfully imported.
	DTI_INVALID,			// Invalid date value or format.
	DTI_AMBIGUOUS,			// Ambiguous month name.
	DTI_INVALID_NAME,		// Invalid month or day-of-week name.
	DTI_VALUE_MISSING,		// Required date value not present.
	DTI_OUT_OF_RANGE,		// Date out of range.
	DTI_LESS_THAN_RANGE,	// Date less than positively open-ended range.
	DTI_GREATER_THAN_RANGE	// Date greater than negatively open-ended range.
};

class ZIL_EXPORT_CLASS ZIL_DATE : public ZIL_UTIME
{
public:
	ZIL_DATE(void);
	ZIL_DATE(const ZIL_DATE &date);
	ZIL_DATE(int year, int month, int day);
	ZIL_DATE(const ZIL_ICHAR *string, DTF_FLAGS dtFlags = DTF_NO_FLAGS);
	ZIL_DATE(int packedDate);
	int DayOfWeek(void) { return (ZIL_UTIME::DayOfWeek() + 1); }
	int DaysInMonth(int month = 0) { return (ZIL_UTIME::DaysInMonth(month)); }
	int DaysInYear(void) { return (ZIL_UTIME::DaysInYear()); }
	void Export(int *year, int *month, int *day, int *dayOfWeek=ZIL_NULLP(int))
		{ ConvertJday(year, month, day, dayOfWeek); }
	void Export(ZIL_ICHAR *string, DTF_FLAGS dtFlags);
	void Export(int *packedDate);
	DTI_RESULT Import(void) { ZIL_UTIME::Import(); usec = 0; return (DTI_OK); }
	DTI_RESULT Import(const ZIL_DATE &date)
		{ ZIL_UTIME::Import(date); return (DTI_OK); }
	DTI_RESULT Import(int year, int month, int day)
		{ ZIL_UTIME::Import(year, month, day, 0, 0, 0, 0);
		  return (DTI_OK); }
	DTI_RESULT Import(const ZIL_ICHAR *string, DTF_FLAGS dtFlags = DTF_NO_FLAGS);
	DTI_RESULT Import(int packedDate);

	long operator=(long days) { jday = days; return (jday); }
	long operator=(const ZIL_DATE &date)
		{ jday = date.jday; usec = date.usec; return (jday); }
	long operator+(long days) { return (jday + days); }
	long operator+(const ZIL_DATE &date) { return (jday + date.jday); }
	long operator-(long days) { return (jday - days); }
	long operator-(const ZIL_DATE &date) { return (jday - date.jday); }
	long operator++(void) { jday++; return (jday); }
	long operator--(void) { jday--; return (jday); }
	void operator+=(long days) { jday += days; }
	void operator-=(long days) { jday -= days; }
	int operator==(const ZIL_DATE& date)
		{ return (ZIL_UTIME::operator==(date)); }
	int operator!=(const ZIL_DATE& date)
		{ return (ZIL_UTIME::operator!=(date)); }
	int operator>(const ZIL_DATE &date)
		{ return (ZIL_UTIME::operator>(date)); }
	int operator>=(const ZIL_DATE &date)
		{ return (ZIL_UTIME::operator>=(date)); }
	int operator<(const ZIL_DATE &date)
		{ return (ZIL_UTIME::operator<(date)); }
	int operator<=(const ZIL_DATE &date)
		{ return (ZIL_UTIME::operator<=(date)); }

	void SetBasis(int _basisYear) { basisYear = _basisYear; }
	int GetBasis() { return basisYear; }
};

// --------------------------------------------------------------------------
// ----- ZIL_TIME -----------------------------------------------------------
// --------------------------------------------------------------------------

// --- TMF_FLAGS ---
typedef UIF_FLAGS TMF_FLAGS;
const TMF_FLAGS TMF_NO_FLAGS			= 0x0000;
const TMF_FLAGS TMF_SECONDS 			= 0x0001;
const TMF_FLAGS TMF_HUNDREDTHS			= 0x0002;
const TMF_FLAGS TMF_NO_HOURS			= 0x0004;
const TMF_FLAGS TMF_NO_MINUTES			= 0x0008;
const TMF_FLAGS TMF_TWELVE_HOUR			= 0x0010;
const TMF_FLAGS TMF_TWENTY_FOUR_HOUR	= 0x0020;
const TMF_FLAGS TMF_ZERO_FILL			= 0x0040;
const TMF_FLAGS TMF_COLON_SEPARATOR		= 0x0080;
const TMF_FLAGS TMF_NO_SEPARATOR		= 0x0100;
const TMF_FLAGS TMF_UPPER_CASE			= 0x0200;
const TMF_FLAGS TMF_LOWER_CASE			= 0x0400;
const TMF_FLAGS TMF_SYSTEM				= 0x0800;

// --- TMI_RESULT ---
enum TMI_RESULT
{
	TMI_OK = 0,				// Time successfully imported.
	TMI_INVALID,			// Invalid time value or format.
	TMI_VALUE_MISSING,		// Required value not present.
	TMI_OUT_OF_RANGE,		// Time out of range (used by UIW_TIME)
	TMI_LESS_THAN_RANGE,	// Time less than positively open-ended range.
	TMI_GREATER_THAN_RANGE	// Time greater than negatively open-ended range.
};

class ZIL_EXPORT_CLASS ZIL_TIME : public ZIL_UTIME
{
public:
	ZIL_TIME(void);
	ZIL_TIME(const ZIL_TIME &time);
	ZIL_TIME(int hour, int minute, int second = 0, int hundredth = 0);
	ZIL_TIME(const ZIL_ICHAR *string, TMF_FLAGS tmFlags = TMF_NO_FLAGS);
	ZIL_TIME(int packedTime);
 	void Export(int *hour, int *minute, int *second = ZIL_NULLP(int),
		int *hundredth = ZIL_NULLP(int))
		{ ConvertUsec(hour, minute, second, hundredth);
		  if (hundredth) *hundredth /= 10; }
	void Export(ZIL_ICHAR *string, TMF_FLAGS tmFlags);
	void Export(int *packedTime);
	TMI_RESULT Import(void) { ZIL_UTIME::Import(); jday = 1; return (TMI_OK); }
	TMI_RESULT Import(const ZIL_TIME &time)
		{ ZIL_UTIME::Import(time); return (TMI_OK); }
 	TMI_RESULT Import(int hour, int minute, int second = 0, int hundredth = 0);
	TMI_RESULT Import(const ZIL_ICHAR *string, TMF_FLAGS tmFlags);
	TMI_RESULT Import(int packedTime);

	long operator=(long hundredths) { usec = hundredths * 10; jday = 1; return (hundredths); }
	long operator=(const ZIL_TIME &time) { usec = time.usec; jday = time.jday; return (usec / 10); }
	long operator+(long hundredths) { return (usec / 10 + hundredths); }
	long operator+(const ZIL_TIME &time) { return (usec + time.usec) / 10; }
	long operator-(long hundredths) { return (usec / 10 - hundredths); }
	long operator-(const ZIL_TIME &time) { return (usec - time.usec) / 10; }
	long operator++(void) { usec += 10; MakeCanonical(); return (usec / 10); }
	long operator--(void) { usec -= 10; MakeCanonical(); return (usec / 10); }
	void operator+=(long hundredths) { usec += 10 * hundredths; MakeCanonical(); }
	void operator-=(long hundredths) { usec -= 10 * hundredths; MakeCanonical(); }
	int operator==(ZIL_TIME& time) { return (ZIL_UTIME::operator==(time)); }
	int operator!=(ZIL_TIME& time) { return (ZIL_UTIME::operator!=(time)); }
	int operator>(ZIL_TIME& time) { return (ZIL_UTIME::operator>(time)); }
	int operator>=(ZIL_TIME& time) { return (ZIL_UTIME::operator>=(time)); }
	int operator<(ZIL_TIME& time) { return (ZIL_UTIME::operator<(time)); }
	int operator<=(ZIL_TIME& time) { return (ZIL_UTIME::operator<=(time)); }
};

// --------------------------------------------------------------------------
// ----- UI_PATH_ELEMENT & UI_PATH ------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UI_PATH_ELEMENT : public UI_ELEMENT, public ZIL_INTERNATIONAL
{
public:
	ZIL_ICHAR *pathName;

	UI_PATH_ELEMENT(ZIL_ICHAR *pathName, int length = -1);
	virtual ~UI_PATH_ELEMENT(void);

	// List members.
	UI_PATH_ELEMENT *Next(void) { return((UI_PATH_ELEMENT *)next); }
	UI_PATH_ELEMENT *Previous(void) { return((UI_PATH_ELEMENT *)previous); }
};

class ZIL_EXPORT_CLASS UI_PATH : public UI_LIST, public ZIL_INTERNATIONAL
{
public:
	UI_PATH(ZIL_ICHAR *programPath = ZIL_NULLP(ZIL_ICHAR), int rememberCWD = TRUE);
	virtual ~UI_PATH(void);
	const ZIL_ICHAR *FirstPathName(void)
		{ current = first; return (current ? (const ZIL_ICHAR *)Current()->pathName : ZIL_NULLP(const ZIL_ICHAR)); }
	const ZIL_ICHAR *NextPathName(void)
		{ current = current ? Current()->Next() : ZIL_NULLP(UI_ELEMENT);
		  return (current ? Current()->pathName : ZIL_NULLP(const ZIL_ICHAR)); }

	// List members
	UI_PATH_ELEMENT *Current(void) { return((UI_PATH_ELEMENT *)current); }
	UI_PATH_ELEMENT *First(void) { return((UI_PATH_ELEMENT *)first); }
	UI_PATH_ELEMENT *Last(void) { return((UI_PATH_ELEMENT *)last); }

private:
	void LoadPath(const ZIL_ICHAR *envname);

	// I18N member variables and functions.
public:
	static ZIL_ICHAR *_pathString;
	static ZIL_ICHAR *_zincPathString;
};

// --------------------------------------------------------------------------
// ----- ZIL_FILE -----------------------------------------------------------
// --------------------------------------------------------------------------

// --- UIS_FLAGS ---
typedef UIF_FLAGS UIS_FLAGS;
const UIS_FLAGS UIS_READ					= 0x0001;
const UIS_FLAGS UIS_READWRITE				= 0x0002;
const UIS_FLAGS UIS_CREATE					= 0x0004;
const UIS_FLAGS UIS_OPENCREATE				= 0x0008;
const UIS_FLAGS UIS_TEMPORARY				= 0x0010;
const UIS_FLAGS UIS_COPY					= 0x0020;
const UIS_FLAGS UIS_BINARY					= 0x0100;
const UIS_FLAGS UIS_TEXT					= 0x0200;

class ZIL_EXPORT_CLASS ZIL_FILE : public ZIL_INTERNATIONAL
{
public:
	// --- Error definitions ---
	enum Z_ERROR
	{
		ERROR_NONE			= 0,
		ERROR_NAME			= 1,
		ERROR_NULL_STRING	= 2,
		ERROR_ACCESS		= 3
	};

	// --- Seek definitions ---
	enum SEEK
	{
		SEEK_FROM_START		= 0,
		SEEK_FROM_CURRENT	= 1,
		SEEK_FROM_END 		= 2
	};

	// --- General class members ---
	ZIL_FILE(const ZIL_ICHAR *pathName, UIS_FLAGS access = UIS_READ | UIS_BINARY);
	virtual ~ZIL_FILE(void);

	// --- Miscellaneous members ---
	virtual ZIL_FILE::Z_ERROR Close(void);
	long Length(void) const;
	ZIL_FILE::Z_ERROR GetError(void);
	virtual ZIL_FILE::Z_ERROR Open(void);
	Z_ERROR Rename(const ZIL_ICHAR *pathName);
	Z_ERROR Seek(long offset, SEEK location) const;
	ZIL_FILE::Z_ERROR SetError(ZIL_FILE::Z_ERROR error);
	long Tell(void) const;
	Z_ERROR Unlink(void);

	// --- Read functions ---
	virtual int Read(ZIL_INT8 *value) const;
	virtual int Read(ZIL_UINT8 *value) const;
	virtual int Read(ZIL_INT16 *value) const;
	virtual int Read(ZIL_UINT16 *value) const;
	virtual int Read(ZIL_INT32 *value) const;
	virtual int Read(ZIL_UINT32 *value) const;
	virtual int Read(ZIL_ICHAR *text, int length) const;
	virtual int Read(ZIL_ICHAR **text) const;
	virtual int Read(void *buffer, int size, int length) const;
#	if defined(ZIL_UNICODE)
	virtual int Read(char *text, int length) const;
	virtual int Read(char **text) const;
#	endif

	// --- Write functions ---
	virtual int Write(ZIL_INT8 value) const;
	virtual int Write(ZIL_UINT8 value) const;
	virtual int Write(ZIL_INT16 value) const;
	virtual int Write(ZIL_UINT16 value) const;
	virtual int Write(ZIL_INT32 value) const;
	virtual int Write(ZIL_UINT32 value) const;
	virtual int Write(ZIL_ICHAR *text) const;
	virtual int Write(void *buffer, int size, int length) const;
#	if defined(ZIL_UNICODE)
	virtual int Write(char *text) const;
#	endif

	// --- Flag members ---
	int IsBinary(void) { return ((access & UIS_BINARY) ? TRUE : FALSE); }
	int IsCopyMode(void) { return ((access & UIS_COPY) ? TRUE : FALSE); }
	int IsCreateMode(void) { return ((access & UIS_CREATE) ? TRUE : FALSE); }
	int IsOpenCreateMode(void) { return ((access & UIS_OPENCREATE) ? TRUE : FALSE); }
	int IsReadOnly(void) { return ((access & UIS_READ) ? TRUE : FALSE); }
	int IsReadWrite(void) { return ((access & UIS_READWRITE) ? TRUE : FALSE); }
	int IsTemporary(void) { return ((access & UIS_TEMPORARY) ? TRUE : FALSE); }
	int IsText(void) { return ((access & UIS_TEXT) ? TRUE : FALSE); }

protected:
	// --- General class members ---
	ZIL_FILE::Z_ERROR error;
	ZIL_ICHAR *pathName;
	UIS_FLAGS access;
	int osAccess;
	unsigned osMode;
	int handle;
};

// --------------------------------------------------------------------------
// ----- ZIL_STORAGE_OBJECT & ZIL_STORAGE -----------------------------------
// --------------------------------------------------------------------------

typedef ZIL_UINT16 ZIL_DISK_ADDRESS; // Private type declaration.
typedef ZIL_UINT32 ZIL_INODE_NUMBER; // Private type declaration.

struct ZIL_EXPORT_CLASS ZIL_STATS_INFO
{
public:
	ZIL_INT32 size;
	ZIL_INT32 createTime;
	ZIL_INT32 modifyTime;
	ZIL_UINT16 useCount;
	ZIL_UINT16 revision;
	ZIL_UINT16 countryID;
	ZIL_INODE_NUMBER inum;
};

const int ZIL_IREAD 						= 0;
const int ZIL_IWRITE 						= 1;
const ZIL_UINT16 ZIL_UNICODE_FLAG 			= 0x8000;
const ZIL_UINT16 ZIL_DIRECTORY_TAG 			= 0x8000;
const ZIL_ICHAR ZIL_DIRECTORY_SEPARATOR 	= '~';

struct cacheData;
struct inode;
struct openObject;
struct superBlock;
class ZIL_EXPORT_CLASS ZIL_STORAGE_DIRECTORY;

class ZIL_EXPORT_CLASS ZIL_STORAGE_READ_ONLY : public ZIL_INTERNATIONAL
{
	friend struct inode;
	friend struct openObject;
	friend struct superBlock;
	friend struct cacheData;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_SUPPORT;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT_READ_ONLY;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_DIRECTORY;
public:				// Static file system support routines
	void StorageName(ZIL_ICHAR *buff) { AppendFullPath(buff, pname, fname); }
	static void AppendFullPath(ZIL_ICHAR *fullPath,
		const ZIL_ICHAR *pathName = ZIL_NULLP(ZIL_ICHAR),
		const ZIL_ICHAR *fileName = ZIL_NULLP(ZIL_ICHAR),
		const ZIL_ICHAR *extension = ZIL_NULLP(ZIL_ICHAR));
	static void ChangeExtension(ZIL_ICHAR *name, const ZIL_ICHAR *newExtension);
	static void StripFullPath(const ZIL_ICHAR *fullPath,
		ZIL_ICHAR *pathName = ZIL_NULLP(ZIL_ICHAR),
		ZIL_ICHAR *fileName = ZIL_NULLP(ZIL_ICHAR),
		ZIL_ICHAR *objectName = ZIL_NULLP(ZIL_ICHAR),
		ZIL_ICHAR *objectPathName = ZIL_NULLP(ZIL_ICHAR));
	static void TempName(ZIL_ICHAR *tempname);
	static int ValidName(const ZIL_ICHAR *name, int createStorage = FALSE);
protected:
	static void MakeFullPath(ZIL_ICHAR *tmppath);

public:				// Read-Only support
	static int cacheSize;
	static UI_PATH *searchPath;
	int storageError;

	ZIL_STORAGE_READ_ONLY(void);
	ZIL_STORAGE_READ_ONLY(const ZIL_ICHAR *name); // Read Only
	virtual ~ZIL_STORAGE_READ_ONLY(void);
	int ChDir(const ZIL_ICHAR *newName);
	int GetCWD(ZIL_ICHAR *path, int pathLen);
	ZIL_STORAGE_DIRECTORY *OpenDir(const ZIL_ICHAR *name);
	ZIL_STATS_INFO *Stats(void);
	int Version(void);
protected:
	// Treat these as private!!! ZIL_STORAGE needs access, you don't.
	int openLen;
	openObject *openObjects;
	ZIL_STORAGE_OBJECT_READ_ONLY *currentDirectory;
	int fd;
	ZIL_ICHAR pname[ZIL_MAXPATHLEN + 1];
	ZIL_ICHAR fname[ZIL_MAXPATHLEN + 1];
	int modified;				// Unused in RO part
	int flags;
	superBlock *sb;
	int cacheLen;
	cacheData *cd;
	char *cache;

	virtual ZIL_DISK_ADDRESS AllocData(void);
	virtual ZIL_DISK_ADDRESS AppendInode(ZIL_INODE_NUMBER inum);
	void ChDirRoot(void);
	int CheckOpen(ZIL_INODE_NUMBER inum);
	virtual ZIL_STORAGE_OBJECT_READ_ONLY *CreateObject(void);
	long FindInode(ZIL_INODE_NUMBER inum);
	int FindSlot(void);
	int InMemory(ZIL_DISK_ADDRESS blknum);
	int SearchPath(ZIL_ICHAR *tmp);
	int ReadAt(long pos, void *buf, int len);
	void *ReadData(ZIL_DISK_ADDRESS blknum);
	virtual int RWInode(ZIL_INODE_NUMBER inum, inode *ientry, int direction);
	void SetUpLocalData(void);
	void ValidateZincFile(void);
	void WalkPartialPath(const ZIL_ICHAR *pathname,
		ZIL_STORAGE_OBJECT_READ_ONLY **parentdir,
		const ZIL_ICHAR **filename);
	ZIL_STORAGE_OBJECT_READ_ONLY *WalkPath(const ZIL_ICHAR *name,
		int stripLast);
	virtual int WriteAt(long pos, void *buf, int len);
	void WriteData(void *data, int modified);
public:
	ZIL_ICHAR *FindFirstObject(const ZIL_ICHAR *pattern);
	ZIL_ICHAR *FindNextObject(void);
	ZIL_ICHAR *FindFirstID(ZIL_UINT16 id);
	ZIL_ICHAR *FindNextID(void);
};

class ZIL_EXPORT_CLASS ZIL_STORAGE : public ZIL_STORAGE_READ_ONLY
{
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_SUPPORT;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT;
public:				// Read/Write Storage
	ZIL_STORAGE(void);
	ZIL_STORAGE(const ZIL_ICHAR *name, UIS_FLAGS pFlags = UIS_READWRITE);
	virtual ~ZIL_STORAGE(void);
	int DestroyObject(const ZIL_ICHAR *name);
	int Flush(void);
	int Link(const ZIL_ICHAR *path1, const ZIL_ICHAR *path2);
	int MkDir(const ZIL_ICHAR *newName);
	int RenameObject(const ZIL_ICHAR *oldObject, const ZIL_ICHAR *newName);
	int RmDir(const ZIL_ICHAR *name);
	int Save(int revisions = 0);
	int SaveAs(const ZIL_ICHAR *newName, int revisions = 0);
protected:
	// Treat these as private!!! ZIL_DELTA_STORAGE needs access, you don't.
	ZIL_ICHAR tname[ZIL_MAXPATHLEN + 1];
	int firstTime;
	inode *copyInodes;

	ZIL_INODE_NUMBER AllocInode(void);
	int DeallocateSpace(inode *ientry);
	void FreeData(ZIL_DISK_ADDRESS blknum);

	ZIL_STORAGE_OBJECT_READ_ONLY *CreateObject(void);
	int RWInode(ZIL_INODE_NUMBER inum, inode *ientry, int direction);

	ZIL_DISK_ADDRESS AllocData(void);
	ZIL_DISK_ADDRESS AppendInode(ZIL_INODE_NUMBER inum);
	int WriteAt(long pos, void *buf, int len);
};

struct directoryEntry
{
	ZIL_INODE_NUMBER inum;			// Inode number
	ZIL_OBJECTID objectID;
	ZIL_UINT16 revision;
	ZIL_UINT16 country;
	ZIL_ICHAR stringID[ZIL_MAXNAMELEN];
};

class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT_READ_ONLY : public ZIL_INTERNATIONAL
{
	friend struct inode;
	friend struct directoryEntry;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_SUPPORT;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_READ_ONLY;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE;
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_DIRECTORY;
public:				// Read-Only support
	int objectError;
	ZIL_OBJECTID objectID;
	ZIL_ICHAR *stringID;

	ZIL_STORAGE_OBJECT_READ_ONLY(void);
	ZIL_STORAGE_OBJECT_READ_ONLY(ZIL_STORAGE_READ_ONLY &file,
		const ZIL_ICHAR *name, ZIL_OBJECTID nObjectID);
	virtual ~ZIL_STORAGE_OBJECT_READ_ONLY(void);

	long Seek(long _position) { position = _position; return (position); }
	ZIL_STATS_INFO *Stats(void);
	ZIL_STORAGE_READ_ONLY *Storage(void) { return (file); }
	long Tell(void) { return (position); }
#if defined(ZIL_BIGENDIAN)
	virtual int Load(ZIL_INT16 *value);
	virtual int Load(ZIL_UINT16 *value);
	virtual int Load(ZIL_INT32 *value);
	virtual int Load(ZIL_UINT32 *value);
#else
	virtual int Load(ZIL_INT16 *value)
		{ return (RWData(value, sizeof(*value), ZIL_IREAD)); }
	virtual int Load(ZIL_UINT16 *value)
		{ return (RWData(value, sizeof(*value), ZIL_IREAD)); }
	virtual int Load(ZIL_INT32 *value)
		{ return (RWData(value, sizeof(*value), ZIL_IREAD)); }
	virtual int Load(ZIL_UINT32 *value)
		{ return (RWData(value, sizeof(*value), ZIL_IREAD)); }
#endif
	virtual int Load(ZIL_UINT8 *value)
		{ return (RWData(value, sizeof(*value), ZIL_IREAD)); }
	virtual int Load(ZIL_INT8 *value)
		{ return (RWData(value, sizeof(*value), ZIL_IREAD)); }
	virtual int Load(void *buff, int size, int length)
		{ return (RWData(buff, size*length, ZIL_IREAD)); }
	virtual int Load(ZIL_ICHAR *string, int length);
	virtual int Load(ZIL_ICHAR **string);

	virtual int Store(void *buff, int size, int length);
protected:
	// Treat these as private!!! ZIL_STORAGE_OBJECT needs access, you don't.
	ZIL_STORAGE_READ_ONLY *file;
	int inodeIndex;
	long position;
	int flags;
	ZIL_DISK_ADDRESS *cachedBlknum;
	ZIL_DISK_ADDRESS *cachedBlkptr;

	openObject *CheckObject(void);
	void CreateBlkCache(void);
	int FindName(const ZIL_ICHAR *name);
	ZIL_DISK_ADDRESS GetBlockPtr(ZIL_DISK_ADDRESS blknum);
	int Load(directoryEntry *dirent);
	void OpenTheObject(ZIL_STORAGE_READ_ONLY &pfile, directoryEntry *dentry,
		inode *ientry, int truncate);
	int RWData(void *buffer, unsigned N, int direction);
	virtual int Store(const directoryEntry *dirent);
};

class ZIL_EXPORT_CLASS ZIL_STORAGE_OBJECT : public ZIL_STORAGE_OBJECT_READ_ONLY
{
	friend class ZIL_EXPORT_CLASS ZIL_STORAGE_SUPPORT;
public:				// Read/Write support
	ZIL_STORAGE_OBJECT(void);
	ZIL_STORAGE_OBJECT(ZIL_STORAGE &file, const ZIL_ICHAR *name,
		ZIL_OBJECTID nObjectID, UIS_FLAGS pFlags = UIS_READWRITE);
	virtual ~ZIL_STORAGE_OBJECT(void);

	void SetCTime(ZIL_INT32 val);
	void SetMTime(ZIL_INT32 val);
	void Touch(void);
#if defined(ZIL_BIGENDIAN)
	virtual int Store(ZIL_INT16 value);
	virtual int Store(ZIL_UINT16 value);
	virtual int Store(ZIL_INT32 value);
	virtual int Store(ZIL_UINT32 value);
#else
	virtual int Store(ZIL_INT16 value)
		{ return (RWData(&value, sizeof(value), ZIL_IWRITE)); }
	virtual int Store(ZIL_UINT16 value)
		{ return (RWData(&value, sizeof(value), ZIL_IWRITE)); }
	virtual int Store(ZIL_INT32 value)
		{ return (RWData(&value, sizeof(value), ZIL_IWRITE)); }
	virtual int Store(ZIL_UINT32 value)
		{ return (RWData(&value, sizeof(value), ZIL_IWRITE)); }
#endif
	virtual int Store(ZIL_INT8 value)
		{ return (RWData(&value, sizeof(value), ZIL_IWRITE)); }
	virtual int Store(ZIL_UINT8 value)
		{ return (RWData(&value, sizeof(value), ZIL_IWRITE)); }
	virtual int Store(void *buff, int size, int length)
		{ return (RWData(buff, size*length, ZIL_IWRITE)); }
	virtual int Store(const ZIL_ICHAR *string);
protected:
	// Treat these as private!!! ZIL_DELTA_STORAGE needs access, you don't.
	int Store(const directoryEntry *dirent);
};

class ZIL_EXPORT_CLASS ZIL_STORAGE_DIRECTORY
{
	friend class ZIL_STORAGE_READ_ONLY;
public:
	virtual ~ZIL_STORAGE_DIRECTORY(void);
	directoryEntry *ReadDir(void);
	void RewindDir(void);
	void SeekDir(ZIL_UINT16 _position) { position = _position; }
	ZIL_UINT16 TellDir(void) { return (position); }
private:
	ZIL_STORAGE_DIRECTORY(ZIL_STORAGE_OBJECT_READ_ONLY *_directory);
	void PartialConstruct(ZIL_STORAGE_OBJECT_READ_ONLY *_directory);
	directoryEntry current;
	ZIL_UINT8 *directoryBuffer;
	ZIL_UINT16 position;
	ZIL_UINT16 size;
	ZIL_STORAGE_OBJECT_READ_ONLY *directory;
};

// --------------------------------------------------------------------------
// ----- ZIL_DELTA_STORAGE_OBJECT --------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZIL_DELTA_STORAGE_OBJECT_READ_ONLY : public ZIL_STORAGE_OBJECT_READ_ONLY
{
public:				// Read-Only support
	ZIL_DELTA_STORAGE_OBJECT_READ_ONLY(ZIL_STORAGE_OBJECT_READ_ONLY *_object,
		ZIL_STORAGE_READ_ONLY &file, const ZIL_ICHAR *name,
		ZIL_OBJECTID nObjectID);
	virtual ~ZIL_DELTA_STORAGE_OBJECT_READ_ONLY(void);

	int Load(ZIL_INT16 *value);
	int Load(ZIL_UINT16 *value);
	int Load(ZIL_INT32 *value);
	int Load(ZIL_UINT32 *value);
	int Load(ZIL_UINT8 *value);
	int Load(ZIL_INT8 *value);
	int Load(void *buff, int size, int length);
	int Load(ZIL_ICHAR *string, int length);
	int Load(ZIL_ICHAR **string);
private:
	ZIL_STORAGE_OBJECT_READ_ONLY *object;
	ZIL_BIT_VECTOR *bools;
	ZIL_UINT16 current;
};

class ZIL_EXPORT_CLASS ZIL_DELTA_STORAGE_OBJECT : public ZIL_STORAGE_OBJECT
{
public:				// Read/Write support
	ZIL_DELTA_STORAGE_OBJECT(ZIL_STORAGE_OBJECT_READ_ONLY *_object,
		ZIL_STORAGE &file, const ZIL_ICHAR *name,
		ZIL_OBJECTID nObjectID, UIS_FLAGS pFlags = UIS_READWRITE);
	virtual ~ZIL_DELTA_STORAGE_OBJECT(void);

	int Store(ZIL_INT16 value);
	int Store(ZIL_UINT16 value);
	int Store(ZIL_INT32 value);
	int Store(ZIL_UINT32 value);
	int Store(ZIL_INT8 value);
	int Store(ZIL_UINT8 value);
	int Store(void *buff, int size, int length);
	int Store(const ZIL_ICHAR *string);
private:
	ZIL_STORAGE_OBJECT_READ_ONLY *object;
	ZIL_BIT_VECTOR *bools;
	ZIL_UINT16 current;
	int Store(const directoryEntry *dirent);
};

// --- Version 3.6, 3.5, 3.0 compatibility ---
#if defined(ZIL_OLD_DEFS)

#	if !defined(ZIL_MSWINDOWS)
typedef ZIL_OBJECTID OBJECTID;
#	endif

#define BASE_OBJECTID				ZIL_BASE_OBJECTID
#define SIMPLE_OBJECTID				ZIL_SIMPLE_OBJECTID
#define COMPLEX_OBJECTID			ZIL_COMPLEX_OBJECTID
#define COMPOSITE_OBJECTID			ZIL_COMPOSITE_OBJECTID
#define DESIGNER_OBJECTID			ZIL_DESIGNER_OBJECTID
#define USER_OBJECTID				ZIL_USER_OBJECTID

#define COMPARE_FUNCTION			ZIL_COMPARE_FUNCTION
#define FILECHAR					ZIL_FILE_CHAR
#define NUMBER_WHOLE				ZIL_NUMBER_WHOLE
#define NUMBER_DECIMAL				ZIL_NUMBER_DECIMAL
#define DIGITS						ZIL_DIGITS
#define ibignum						ZIL_IBIGNUM
#define rbignum						ZIL_RBIGNUM
#define nm_number					ZIL_NUMBER
#define diskAddr					ZIL_DISK_ADDRESS
#define inum_t						ZIL_INODE_NUMBER
#define UNICODE_FLAG				ZIL_UNICODE_FLAG
#define DIRECTORY_TAG				ZIL_DIRECTORY_TAG
#define DIRECTORY_SEPARATOR			ZIL_DIRECTORY_SEPARATOR
#define MAX_NAME_LENGTH				ZIL_MAXNAMELEN

#	define UI_NUMBER				ZIL_BIGNUM
#	define ui_strdup				ZIL_INTERNATIONAL::strdup
#	define ui_strlen				ZIL_INTERNATIONAL::strlen
#	define ui_strrepc				ZIL_INTERNATIONAL::strrepc
#	define ui_strstrip				ZIL_INTERNATIONAL::strstrip
#	define ui_itoa					ZIL_INTERNATIONAL::itoa
#	define ui_WildStrcmp			ZIL_INTERNATIONAL::WildStrcmp
#	define ui_stricmp				ZIL_INTERNATIONAL::stricmp
#	define ui_strnicmp				ZIL_INTERNATIONAL::strnicmp
#	define ui_strupr				ZIL_INTERNATIONAL::strupr
#	define ui_strlwr				ZIL_INTERNATIONAL::strlwr
#	define UI_STORAGE				ZIL_STORAGE
#	define UI_STORAGE_OBJECT		ZIL_STORAGE_OBJECT
#	define UI_INTERNATIONAL			ZIL_INTERNATIONAL
#	define UI_TIME					ZIL_TIME
#	define UI_DATE					ZIL_DATE
#	define UI_BIGNUM				ZIL_BIGNUM
//#	define localeName				defaultLocale->localeName
//#	define languageName				defaultLanguage->languageName
//#	define currencySymbol			defaultLocale->currencySymbol
//#	define decimalSeparator			defaultLocale->decimalSeparator
//#	define thousandsSeparator		defaultLocale->thousandsSeparator
#endif

#if defined(_SUNOS4)
	void *memmove(void *s1, const void *s2, size_t n);
#endif

#endif

