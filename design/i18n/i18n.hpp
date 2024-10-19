//	Zinc Interface Library - I18N.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_I18N_HPP)
#define ZAF_I18N_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

// --------------------------------------------------------------------------
// ----- ZAF_I18N_EDITOR ----------------------------------------------------
// --------------------------------------------------------------------------

class ZAF_I18N_EDITOR : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static ZIL_STORAGE_READ_ONLY *_storage;

	static ZIL_ICHAR *_i18nDirectory;
	static ZIL_ICHAR _languageDirectory[];
	static ZIL_ICHAR _localeDirectory[];
	static ZIL_ICHAR *_defaultLanguage;
	static ZIL_ICHAR *_defaultLocale;

#if defined(__DECCXX)
	// Compiler requires a default constructor.
	ZAF_I18N_EDITOR();
#endif
	ZAF_I18N_EDITOR(WOAF_FLAGS woAdvancedFlags, WOS_STATUS woStatus);
	~ZAF_I18N_EDITOR(void);
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	struct PROCESS
	{
		EVENT_TYPE option;

		// This crashes the GNU 2.2.2 compiler, works fine with 2.5.8.
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		EVENT_TYPE (UI_ELEMENT::*Process)(const UI_EVENT &event);
#		else
		EVENT_TYPE (ZAF_I18N_EDITOR::*Process)(const UI_EVENT &event);
#		endif
	};
private:
	static struct PROCESS _processTable[];
	static ZIL_ICHAR _txtExtension[];

	UI_WINDOW_OBJECT *directoryService;
	UI_WINDOW_OBJECT *storageService;
	ZIL_ICHAR originalTitle[ZIL_MAXPATHLEN];

	EVENT_TYPE FileClose(const UI_EVENT &event);
	EVENT_TYPE FileDelete(const UI_EVENT &event);
	EVENT_TYPE FileNew(const UI_EVENT &event);
	EVENT_TYPE FileOpen(const UI_EVENT &event);
	EVENT_TYPE FileRequest(const UI_EVENT &event);
	EVENT_TYPE FileSave(const UI_EVENT &event);
	EVENT_TYPE FileSaveAs(const UI_EVENT &event);

	EVENT_TYPE DefaultsClear(const UI_EVENT &event);
	EVENT_TYPE DefaultsClearAll(const UI_EVENT &event);
	EVENT_TYPE LanguageCreate(const UI_EVENT &event);
	EVENT_TYPE LocaleCreate(const UI_EVENT &event);
	EVENT_TYPE DefaultsDelete(const UI_EVENT &event);
	EVENT_TYPE DefaultsExport(const UI_EVENT &event);
	EVENT_TYPE DefaultsImport(const UI_EVENT &event);
	EVENT_TYPE DefaultsLoad(const UI_EVENT &event);
	EVENT_TYPE DefaultsRequest(const UI_EVENT &event);
	EVENT_TYPE DefaultsStore(const UI_EVENT &event);
	EVENT_TYPE DefaultsStoreAs(const UI_EVENT &event);

	UI_WINDOW_OBJECT *GetDefaultsWindow(void);
	void SetTitle(void);
	void StatusUpdate(UI_WINDOW_OBJECT *editObject = ZIL_NULLP(UI_WINDOW_OBJECT), int clear = FALSE);
	EVENT_TYPE OSImport(ZIL_STORAGE *i18nFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *osFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_STORAGE *datFile, const UI_EVENT &event);

private:
	UIW_VT_LIST *list;
};

const EVENT_TYPE OPT_DEFAULT	= 7000;

typedef UIF_FLAGS I18_FLAGS;
const I18_FLAGS I18_NO_FLAGS				= 0x0000;
const I18_FLAGS I18_LANGUAGE_SET			= 0x1000;
const I18_FLAGS I18_LOCALE_SET				= 0x2000;

// --------------------------------------------------------------------------
// ----- ZAF_LANGUAGE_WINDOW ------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZAF_LANGUAGE_WINDOW : public UIW_WINDOW
{
public:
	ZAF_LANGUAGE_WINDOW(ZIL_ICHAR *name);
	~ZAF_LANGUAGE_WINDOW(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void Load(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Store(const ZIL_ICHAR *name,
		ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
		ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));

	struct PROCESS
	{
		ZIL_ICHAR *className;

		// This crashes the GNU 2.2.2 compiler, works fine with 2.5.8.
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		void (UI_ELEMENT::*Load)(void);
		void (UI_ELEMENT::*Store)(void);
#		else
		void (ZAF_LANGUAGE_WINDOW::*Load)(void);
		void (ZAF_LANGUAGE_WINDOW::*Store)(void);
#		endif
	};
private:
	static struct PROCESS _processTable[];

	void (ZAF_LANGUAGE_WINDOW::*DataLoad)(void);
	void (ZAF_LANGUAGE_WINDOW::*DataStore)(void);

	int allocate;
	int tableLength;
	ZIL_LANGUAGE_ELEMENT *table;
	UIW_NOTEBOOK *notebook;

	void ZIL_DAY_DataLoad(void);
	void ZIL_DAY_DataStore(void);
	void ZIL_MONTH_DataLoad(void);
	void ZIL_MONTH_DataStore(void);
	void ZIL_TIME_DataLoad(void);
	void ZIL_TIME_DataStore(void);

	void UIW_WINDOW_DataLoad(void);
	void UIW_WINDOW_DataStore(void);
	void UIW_BIGNUM_DataLoad(void);
	void UIW_BIGNUM_DataStore(void);
	void UIW_DATE_DataLoad(void);
	void UIW_DATE_DataStore(void);
	void UIW_INTEGER_DataLoad(void);
	void UIW_INTEGER_DataStore(void);
	void UIW_REAL_DataLoad(void);
	void UIW_REAL_DataStore(void);
	void UIW_TIME_DataLoad(void);
	void UIW_TIME_DataStore(void);
	void UIW_SYSTEM_BUTTON_DataLoad(void);
	void UIW_SYSTEM_BUTTON_DataStore(void);

	void UI_ERROR_SYSTEM_DataLoad(void);
	void UI_ERROR_SYSTEM_DataStore(void);
	void UI_HELP_SYSTEM_DataLoad(void);
	void UI_HELP_SYSTEM_DataStore(void);

	void ZAF_MESSAGE_WINDOW_DataLoad(void);
	void ZAF_MESSAGE_WINDOW_DataStore(void);

	void GetList(ZIL_NUMBERID fieldID, ZIL_NUMBERID *numberID);
	void GetText(ZIL_NUMBERID fieldID, ZIL_NUMBERID numberID);
	void SetList(ZIL_NUMBERID fieldID, ZIL_NUMBERID *numberID);
	void SetText(ZIL_NUMBERID fieldID, ZIL_NUMBERID numberID);
	static EVENT_TYPE StringCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

// --------------------------------------------------------------------------
// ----- ZAF_LOCALE_WINDOW --------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZAF_LOCALE_WINDOW : public UIW_WINDOW
{
public:
	ZAF_LOCALE_WINDOW(ZIL_ICHAR *name);
	~ZAF_LOCALE_WINDOW(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void Load(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Store(const ZIL_ICHAR *name,
		ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
		ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));

	static EVENT_TYPE StringCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE CheckFlagSelection(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);

	struct PROCESS
	{
		ZIL_ICHAR *className;

		// This crashes the GNU 2.2.2 compiler, works fine with 2.5.8.
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		void (UI_ELEMENT::*Load)(void);
		void (UI_ELEMENT::*Store)(void);
#		else
		void (ZAF_LOCALE_WINDOW::*Load)(void);
		void (ZAF_LOCALE_WINDOW::*Store)(void);
#		endif
	};
private:
	static struct PROCESS _processTable[];

	void (ZAF_LOCALE_WINDOW::*DataLoad)(void);
	void (ZAF_LOCALE_WINDOW::*DataStore)(void);

	ZIL_LOCALE_ELEMENT *table;
	UIW_NOTEBOOK *notebook;

	void LOC_DATE_DataLoad(void);
	void LOC_DATE_DataStore(void);
	void LOC_NUMBER_DataLoad(void);
	void LOC_NUMBER_DataStore(void);
	void LOC_TIME_DataLoad(void);
	void LOC_TIME_DataStore(void);
	void LOC_CURRENCY_DataLoad(void);
	void LOC_CURRENCY_DataStore(void);

	void GetFlag(ZIL_NUMBERID fieldID, UIF_FLAGS *currentFlags, UIF_FLAGS flag);
	void GetText(ZIL_NUMBERID fieldID, ZIL_ICHAR *text);
	void GetText(ZIL_NUMBERID fieldID, ZIL_ICHAR **text);
	void GetValue(ZIL_NUMBERID fieldID, int *value);
	void SetFlag(ZIL_NUMBERID fieldID, UIF_FLAGS currentFlags, UIF_FLAGS flag);
	void SetText(ZIL_NUMBERID fieldID, ZIL_ICHAR *text);
	void SetValue(ZIL_NUMBERID fieldID, int value);
};

#endif

