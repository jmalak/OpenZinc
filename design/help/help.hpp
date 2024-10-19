//	Zinc Interface Library - HELP.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_HELP_HPP)
#define ZAF_HELP_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

// --------------------------------------------------------------------------
// ----- ZAF_HELP_EDITOR ----------------------------------------------------
// --------------------------------------------------------------------------

class ZAF_HELP_EDITOR : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static ZIL_STORAGE_READ_ONLY *_storage;

#if defined(__DECCXX)
	// The compiler wants a default constructor.
	ZAF_HELP_EDITOR();
#endif
	ZAF_HELP_EDITOR(WOAF_FLAGS woAdvancedFlags, WOS_STATUS woStatus);
	~ZAF_HELP_EDITOR(void);
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
		EVENT_TYPE (ZAF_HELP_EDITOR::*Process)(const UI_EVENT &event);
#		endif
	};
private:
	static struct PROCESS _processTable[];

	ZIL_LANGUAGE msgTable;
	static ZIL_ICHAR _txtExtension[];

	UI_HELP_CONTEXT helpID;

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

	EVENT_TYPE HelpClear(const UI_EVENT &event);
	EVENT_TYPE HelpClearAll(const UI_EVENT &event);
	EVENT_TYPE HelpCreate(const UI_EVENT &event);
	EVENT_TYPE HelpDelete(const UI_EVENT &event);
	EVENT_TYPE HelpExport(const UI_EVENT &event);
	EVENT_TYPE HelpImport(const UI_EVENT &event);
	EVENT_TYPE HelpLoad(const UI_EVENT &event);
	EVENT_TYPE HelpRequest(const UI_EVENT &event);
	EVENT_TYPE HelpStore(const UI_EVENT &event);
	EVENT_TYPE HelpStoreAs(const UI_EVENT &event);

	UI_WINDOW_OBJECT *GetHelpWindow(void);
	void GenerateHPPEntries(void);
	void SetTitle(void);
	void StatusUpdate(UI_WINDOW_OBJECT *editObject = ZIL_NULLP(UI_WINDOW_OBJECT), int clear = FALSE);
	EVENT_TYPE OSImport(ZIL_STORAGE *srcFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_STORAGE *datFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *txtFile, const UI_EVENT &event);
};

const INFO_REQUEST GET_HELP_IDENTIFICATION	= 0x340;

// --------------------------------------------------------------------------
// ----- UIW_HELP_WINDOW ----------------------------------------------------
// --------------------------------------------------------------------------

class UIW_HELP_WINDOW : public UIW_WINDOW
{
public:
	UI_HELP_CONTEXT helpID;

	UIW_HELP_WINDOW(ZIL_ICHAR *name);
	~UIW_HELP_WINDOW(void);
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
};

#endif

