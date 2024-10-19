//	Zinc Interface Library - FILE.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_FILE_HPP)
#define ZAF_FILE_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

// --------------------------------------------------------------------------
// ----- ZAF_FILE_EDITOR ----------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZAF_FILE_EDITOR : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static ZIL_STORAGE_READ_ONLY *_storage;

	ZAF_FILE_EDITOR(WOAF_FLAGS woAdvancedFlags, WOS_STATUS woStatus);
	~ZAF_FILE_EDITOR(void);
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	struct PROCESS
	{
		EVENT_TYPE option;
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		EVENT_TYPE (UI_ELEMENT::*Process)(const UI_EVENT &event);
#		else
		EVENT_TYPE (ZAF_FILE_EDITOR::*Process)(const UI_EVENT &event);
#		endif
	};
private:
	static struct PROCESS _processTable[];

	UI_WINDOW_OBJECT *directoryService;

	int unicode;
	int codePage;
	ZIL_ICHAR codeSet[ZIL_MAXPATHLEN];
	ZIL_ICHAR originalTitle[ZIL_MAXPATHLEN];

	UIW_TEXT *textField;
	ZIL_ICHAR currentFile[ZIL_MAXPATHLEN];

	EVENT_TYPE FileClose(const UI_EVENT &event);
	EVENT_TYPE FileDelete(const UI_EVENT &event);
	EVENT_TYPE FileNew(const UI_EVENT &event);
	EVENT_TYPE FileOpen(const UI_EVENT &event);
	EVENT_TYPE FileRequest(const UI_EVENT &event);
	EVENT_TYPE FileSave(const UI_EVENT &event);
	EVENT_TYPE FileSaveAs(const UI_EVENT &event);

	EVENT_TYPE Format(const UI_EVENT &event);
	void SetTitle(void);
};

#endif

