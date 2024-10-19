//	Zinc Interface Library - STORAGE.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_STORAGE_HPP)
#define ZAF_STORAGE_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

// Service options.
const EVENT_TYPE NEG_OPT_FILE_OPEN				= -6101;
const EVENT_TYPE NEG_OPT_RESOURCE_IMPORT		= -6305;
const EVENT_TYPE NEG_OPT_RESOURCE_EXPORT		= -6310;

// --------------------------------------------------------------------------
// ----- ZAF_STORAGE_SERVICE ------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZAF_STORAGE_SERVICE : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];
	static UI_ITEM _userTable[];
	static UI_ITEM _objectTable[];
	static ZIL_STORAGE_READ_ONLY *_storage;

	ZAF_STORAGE_SERVICE(int argc = 0, ZIL_ICHAR **argv = ZIL_NULLP(ZIL_ICHAR *));
	~ZAF_STORAGE_SERVICE(void);
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	static int RRmDir(ZIL_STORAGE *storage, const ZIL_ICHAR *name);

private:
	ZIL_ICHAR currentPath[ZIL_MAXPATHLEN];
	UIW_VT_LIST *dirList;
	UIW_VT_LIST *objList;

	int revisions;
	int traverseLock;
	int writeCPP;
	int writeHPP;
	int writeDelta;
	ZIL_ICHAR defaultExtension[ZIL_MAXNAMELEN];

	EVENT_TYPE response;
	ZIL_STORAGE *requestFile;
	UI_WINDOW_OBJECT *requestor;
	ZIL_LANGUAGE msgTable;

	static int DirectoryCompare(void *object1, void *object2);
	int GenerateCPP(void);
	int GenerateHPP(void);
	void ListUpdate(void);
	static EVENT_TYPE ObjectNameCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

#endif


