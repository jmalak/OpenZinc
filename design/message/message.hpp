//	Zinc Interface Library - MESSAGE.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_MESSAGE_HPP)
#define ZAF_MESSAGE_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

// --------------------------------------------------------------------------
// ----- ZAF_MESSAGE_EDITOR -------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZAF_MESSAGE_EDITOR : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static ZIL_STORAGE_READ_ONLY *_storage;

#if defined(__DECCXX)
	// Compiler requires a default constructor.
	ZAF_MESSAGE_EDITOR();
#endif

	ZAF_MESSAGE_EDITOR(WOAF_FLAGS woAdvancedFlags, WOS_STATUS woStatus);
	~ZAF_MESSAGE_EDITOR(void);
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
		EVENT_TYPE (ZAF_MESSAGE_EDITOR::*Process)(const UI_EVENT &event);
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
	EVENT_TYPE FilePreferences(const UI_EVENT &event);
	EVENT_TYPE FileRequest(const UI_EVENT &event);
	EVENT_TYPE FileSave(const UI_EVENT &event);
	EVENT_TYPE FileSaveAs(const UI_EVENT &event);

	EVENT_TYPE MessageClear(const UI_EVENT &event);
	EVENT_TYPE MessageClearAll(const UI_EVENT &event);
	EVENT_TYPE MessageCreate(const UI_EVENT &event);
	EVENT_TYPE MessageDelete(const UI_EVENT &event);
	EVENT_TYPE MessageExport(const UI_EVENT &event);
	EVENT_TYPE MessageImport(const UI_EVENT &event);
	EVENT_TYPE MessageLoad(const UI_EVENT &event);
	EVENT_TYPE MessageRequest(const UI_EVENT &event);
	EVENT_TYPE MessageStore(const UI_EVENT &event);
	EVENT_TYPE MessageStoreAs(const UI_EVENT &event);

	UI_WINDOW_OBJECT *GetMessageWindow(void);
	void SetTitle(void);
	void StatusUpdate(UI_WINDOW_OBJECT *editObject = ZIL_NULLP(UI_WINDOW_OBJECT), int clear = FALSE);
	EVENT_TYPE OSImport(ZIL_STORAGE *srcFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_STORAGE *datFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *txtFile, const UI_EVENT &event);
	void ExportTXT(ZIL_LANGUAGE *table, ZIL_FILE *txtFile, ZIL_ICHAR *level);
	void ImportTXT(ZIL_LANGUAGE *table, ZIL_FILE *txtFile);
};

// --------------------------------------------------------------------------
// ----- ZAF_MESSAGE_PREFERENCES --------------------------------------------
// --------------------------------------------------------------------------

class ZAF_MESSAGE_PREFERENCES : public UIW_WINDOW
{
public:
	ZAF_MESSAGE_PREFERENCES(void);
	~ZAF_MESSAGE_PREFERENCES(void);
	EVENT_TYPE Event(const UI_EVENT &event);

private:
	void LoadOptions(void);
	void StoreOptions(void);
};

// --------------------------------------------------------------------------
// ----- UIW_MESSAGE_WINDOW -------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_MESSAGE_WINDOW : public UIW_WINDOW
{
public:
	UIW_MESSAGE_WINDOW(ZIL_ICHAR *name);
	~UIW_MESSAGE_WINDOW(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
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
private:
	ZIL_LANGUAGE messageList;
	UIW_VT_LIST *list;
};

#endif

