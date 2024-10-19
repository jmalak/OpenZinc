//	Zinc Interface Library - WINDOW.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_WINDOW_HPP)
#define ZAF_WINDOW_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

// Pinned numbers - do not change.
const ZIL_OBJECTID ID_HZ_SCROLL				= 3000;
const ZIL_OBJECTID ID_VT_SCROLL				= 3001;
const ZIL_OBJECTID ID_DERIVED_OBJECT		= 3009;
const ZIL_OBJECTID ID_EDIT_GROUP			= 3010;
const ZIL_OBJECTID ID_DERIVED_START			= 3500;

const EVENT_TYPE OPT_RESET_PARENT		= 7000;
const EVENT_TYPE OPT_RESET_CHILD		= 7001;
const EVENT_TYPE OPT_RESET_OBJECT		= 7002;
const EVENT_TYPE OPT_EDIT_CHILD			= 7012;
const EVENT_TYPE OPT_OBJECT_CLEAR		= 7013;
const EVENT_TYPE OPT_OBJECT_RESET		= 7014;

const INFO_REQUEST I_GET_GEOMETRY		= 7010;
const INFO_REQUEST I_GET_EDIT_OBJECT	= 7011;
const INFO_REQUEST I_GET_ADDTABLE		= 7012;
const INFO_REQUEST I_CHECK_EDIT_NAMES	= 7013;
const INFO_REQUEST I_CHECK_EDIT_DELETE	= 7014;

// --------------------------------------------------------------------------
// ----- ZAF_WINDOW_EDITOR --------------------------------------------------
// --------------------------------------------------------------------------

class ZAF_WINDOW_EDITOR : public UIW_WINDOW
{
public:
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static UI_ITEM *_duplicateTable;
	static ZIL_STORAGE_READ_ONLY *_storage;
	static ZIL_ICHAR _className[];
	static ZIL_ICHAR _nameTableName[];
	static ZIL_STORAGE *_deltaFile;
	static WOS_STATUS _importStatus;

#if defined(__DECCXX)
	// Compiler requires a default constructor.
	ZAF_WINDOW_EDITOR();
#endif
	ZAF_WINDOW_EDITOR(WOAF_FLAGS woAdvancedFlags, WOS_STATUS woStatus);
	~ZAF_WINDOW_EDITOR(void);
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
		EVENT_TYPE (ZAF_WINDOW_EDITOR::*Process)(const UI_EVENT &event);
#		endif
	};

private:
	static PROCESS _processTable[];
	ZIL_LANGUAGE msgTable;
	ZIL_LANGUAGE nameTable;
	static ZIL_ICHAR _txtExtension[];

	UI_WINDOW_OBJECT *createObject;
	UI_WINDOW_OBJECT *resetObject;
	UI_WINDOW_OBJECT *pasteObject;

	UI_WINDOW_OBJECT *directoryService;
	UI_WINDOW_OBJECT *storageService;
	ZIL_ICHAR originalTitle[ZIL_MAXPATHLEN];

	EVENT_TYPE EditRequest(const UI_EVENT &event);

	EVENT_TYPE FileClose(const UI_EVENT &event);
	EVENT_TYPE FileDelete(const UI_EVENT &event);
	EVENT_TYPE FileNew(const UI_EVENT &event);
	EVENT_TYPE FileOpen(const UI_EVENT &event);
	EVENT_TYPE FilePreferences(const UI_EVENT &event);
	EVENT_TYPE FileRequest(const UI_EVENT &event);
	EVENT_TYPE FileSave(const UI_EVENT &event);
	EVENT_TYPE FileSaveAs(const UI_EVENT &event);
	EVENT_TYPE FileQuerySave(const UI_EVENT &event);

	EVENT_TYPE WindowClear(const UI_EVENT &event);
	EVENT_TYPE WindowClearAll(const UI_EVENT &event);
	EVENT_TYPE WindowCreate(const UI_EVENT &event);
	EVENT_TYPE WindowDelete(const UI_EVENT &event);
	EVENT_TYPE WindowExport(const UI_EVENT &event);
	EVENT_TYPE WindowImport(const UI_EVENT &event);
	EVENT_TYPE WindowLoad(const UI_EVENT &event);
	EVENT_TYPE WindowRequest(const UI_EVENT &event);
	EVENT_TYPE WindowStore(const UI_EVENT &event);
	EVENT_TYPE WindowStoreAs(const UI_EVENT &event);
	EVENT_TYPE WindowTest(const UI_EVENT &event);

	UI_WINDOW_OBJECT *CreateObject(ZIL_ICHAR *name);
	void DestroyObjectTable(UI_ITEM *objectTable);
	UI_ITEM *DuplicateObjectTable(UI_ITEM *objectTable);
	UI_WINDOW_OBJECT *GetEditWindow(void);
	UI_ITEM *ResetDeriveTable(ZIL_STORAGE *editFile, UI_ITEM *objectTable);
	static void ClearNumberIDs(UI_WINDOW_OBJECT *window);
	ZIL_OBJECTID *MergeDeriveTable(UI_ITEM *deriveTable);
	static void ModifySearchID(UI_WINDOW_OBJECT *window, ZIL_OBJECTID oldSearchID, ZIL_OBJECTID newSearchID);
	void SetTitle(void);
	static void ShiftHelpIDs(ZIL_STORAGE *srcFile, ZIL_STORAGE *dstFile, UI_WINDOW_OBJECT *window);
	void ShiftDeriveIDs(ZIL_OBJECTID *mergeTable, UI_WINDOW_OBJECT *window);
	void StatusUpdate(UI_WINDOW_OBJECT *editObject = ZIL_NULLP(UI_WINDOW_OBJECT), int clear = FALSE);

	EVENT_TYPE OSImport(ZIL_STORAGE *windowFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_STORAGE *datFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *txtFile, const UI_EVENT &event);
	void ExportTXT(UI_WINDOW_OBJECT *window, ZIL_FILE *txtFile, ZIL_ICHAR *level);
	void ImportTXT(UI_WINDOW_OBJECT *window, ZIL_FILE *txtFile);
};

typedef UI_WINDOW_OBJECT *(*ZAF_CREATE_FUNCTION)(void);

// --------------------------------------------------------------------------
// ----- ZAF_WINDOW_PREFERENCES ---------------------------------------------
// --------------------------------------------------------------------------

class ZAF_WINDOW_PREFERENCES : public UIW_WINDOW
{
public:
	static void LoadOptions(void);
	static void StoreOptions(void);

	ZAF_WINDOW_PREFERENCES(void);
	~ZAF_WINDOW_PREFERENCES(void);
	EVENT_TYPE Event(const UI_EVENT &event);

private:
	void GetOptions(void);
	void SetOptions(void);
};

// --------------------------------------------------------------------------
// ----- ZAF_GEOMETRY_MANAGER -----------------------------------------------
// --------------------------------------------------------------------------

class ZAF_GEOMETRY_MANAGER : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];
	enum FIELD_ID
	{
		// constraint fields.
		FLD_SIDE_LEFT		= 0x00,
		FLD_SIDE_TOP		= 0x01,
		FLD_SIDE_RIGHT		= 0x02,
		FLD_SIDE_BOTTOM		= 0x03,
		FLD_WIDTH			= 0x04,
		FLD_HEIGHT			= 0x05,
		// type fields.
		FLD_TYPE_NONE		= 0x06,
		FLD_TYPE_ABSOLUTE	= 0x07,
		FLD_TYPE_RELATIVE	= 0x08,
		FLD_TYPE_DIMENSION	= 0x09
	};
	struct CONSTRAINT
	{
		UI_CONSTRAINT *origin;
		int type;
		UI_WINDOW_OBJECT *anchor;
		int offset;
		int stretch;
		int opposite;
		int vtCenter;
		int hzCenter;
		int min;
		int max;
	};
	UI_WINDOW_OBJECT *editObject;
	UI_WINDOW_OBJECT *field[0x0A], *lastSide;
	CONSTRAINT constraint[0x06];
	UI_WINDOW_OBJECT *anchorField, *offsetField, *stretchField,
		*oppositeField, *vtCenterField, *hzCenterField;
	UI_WINDOW_OBJECT *widthField, *heightField,
		*minXField, *maxXField, *minYField, *maxYField;

	ZAF_GEOMETRY_MANAGER(void);
	~ZAF_GEOMETRY_MANAGER(void);
	void DataLoad(UI_WINDOW_OBJECT *object);
	int DataStore(UI_WINDOW_OBJECT *object);
	void UpdateCurrent(UI_WINDOW_OBJECT *requestor);
	void UpdateNonCurrent(UI_WINDOW_OBJECT *requestor);
	static EVENT_TYPE UpdateSettings(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
};

// --------------------------------------------------------------------------
// ----- ZAF_OBJECT_EDITOR --------------------------------------------------
// --------------------------------------------------------------------------

typedef UIF_FLAGS DSF_FLAGS;
const DSF_FLAGS DSF_NO_FLAGS			= 0x0000;
const DSF_FLAGS DSF_POSITION			= 0x0001;
const DSF_FLAGS DSF_GEOMETRY			= 0x0002;
const DSF_FLAGS DSF_CALLBACK			= 0x0004;

class ZAF_OBJECT_EDITOR : public UIW_WINDOW
{
	friend class ZAF_WINDOW_EDITOR;
public:
	static ZIL_ICHAR _className[];

	ZAF_OBJECT_EDITOR(ZIL_ICHAR *name, UI_WINDOW_OBJECT *object);
	~ZAF_OBJECT_EDITOR(void);
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static EVENT_TYPE CheckCoordinates(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static EVENT_TYPE CheckFlagSelection(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	static int LeftTopCompare(void *object1, void *object2);
	static EVENT_TYPE ObjectDrag(UI_WINDOW_OBJECT *object, ZIL_OBJECTID *idTable, const UI_EVENT &event);
	static EVENT_TYPE ObjectDrop(UI_WINDOW_OBJECT *object, ZIL_OBJECTID *idTable, const UI_EVENT &event);
	static EVENT_TYPE ObjectSet(UI_WINDOW_OBJECT *object, ZIL_OBJECTID *idTable, const UI_EVENT &event);

	struct PROCESS
	{
		EVENT_TYPE option;

		// This crashes the GNU 2.2.2 compiler, works fine with 2.5.8.
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		EVENT_TYPE (UI_ELEMENT::*Process)(const UI_EVENT &event);
#		else
		EVENT_TYPE (ZAF_OBJECT_EDITOR::*Process)(const UI_EVENT &event);
#		endif
	};

	struct TABLE
	{
		ZIL_OBJECTID objectID;
		ZIL_ICHAR *className;
		int showObject;
		ZIL_OBJECTID subObjectID;
		UI_WINDOW_OBJECT *(*New)(const ZIL_ICHAR *name,
			ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
			UI_ITEM *objectTable, UI_ITEM *userTable);

		// This crashes the GNU 2.2.2 compiler, works fine with 2.5.8.
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		void (UI_ELEMENT::*Load)(UI_WINDOW_OBJECT *object);
		int (UI_ELEMENT::*Store)(UI_WINDOW_OBJECT *object);
#		else
		void (ZAF_OBJECT_EDITOR::*Load)(UI_WINDOW_OBJECT *object);
		int (ZAF_OBJECT_EDITOR::*Store)(UI_WINDOW_OBJECT *object);
#		endif
	};
private:
	static int _editLevel;
	static PROCESS _processTable[];
	static struct TABLE _classTable[];
	ZIL_LANGUAGE nameTable;
	ZIL_LANGUAGE msgTable;

	DSF_FLAGS dsFlagsAllowed;
	WOF_FLAGS woFlagsAllowed;
	WOAF_FLAGS woAdvancedFlagsAllowed;

	// --- Notebook information.
	UIW_NOTEBOOK *notebook;
	UI_WINDOW_OBJECT *dirObject, *editObject;

	ZIL_OBJECTID subObjectID;
	void (ZAF_OBJECT_EDITOR::*DataLoad)(UI_WINDOW_OBJECT *object);
	int (ZAF_OBJECT_EDITOR::*DataStore)(UI_WINDOW_OBJECT *object);

	// --- Level 1: Object specific information.
	UIW_WINDOW *genWindow;

	// --- Level 2: Sub-object information.
	static UIW_WINDOW *_subWindow[2];
	static UIW_VT_LIST *_dirList[2];
	static UIW_VT_LIST *_objList[2];
	static UIW_COMBO_BOX *_subList[2];
	UIW_WINDOW *subWindow;
	UIW_VT_LIST *dirList;
	UIW_VT_LIST *objList;
	UIW_COMBO_BOX *subList;

	// --- Level 3: Position information.
	static UIW_WINDOW *_posWindow[2];
	UIW_WINDOW *posWindow;

	// --- Level 4: Geometry management.
	static ZAF_GEOMETRY_MANAGER *_geoWindow[2];
	ZAF_GEOMETRY_MANAGER *geoWindow;

	// --- Level 5: Advanced information.
	static UIW_WINDOW *_advWindow[2];
	UIW_WINDOW *advWindow;

	// ----- Support members -----
	void OBJ_ADVANCED_DataLoad(UI_WINDOW_OBJECT *object);
	int OBJ_ADVANCED_DataStore(UI_WINDOW_OBJECT *object);
	void OBJ_GEOMETRY_DataLoad(UI_WINDOW_OBJECT *object);
	int OBJ_GEOMETRY_DataStore(UI_WINDOW_OBJECT *object);
	void OBJ_POSITION_DataLoad(UI_WINDOW_OBJECT *object);
	int OBJ_POSITION_DataStore(UI_WINDOW_OBJECT *object);
	void OBJ_SUBOBJECT_DataLoad(UI_WINDOW_OBJECT *object);
	int OBJ_SUBOBJECT_DataStore(UI_WINDOW_OBJECT *object);

	// --- UI_GEOMETRY_MANAGER
	void UI_GEOMETRY_MANAGER_DataLoad(UI_WINDOW_OBJECT *object);
	int UI_GEOMETRY_MANAGER_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_BIGNUM
	void UIW_BIGNUM_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_BIGNUM_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_BORDER
	void UIW_BORDER_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_BORDER_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_BUTTON
	void UIW_BUTTON_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_BUTTON_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_COMBO_BOX
	void UIW_COMBO_BOX_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_COMBO_BOX_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_DATE
	void UIW_DATE_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_DATE_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_FORMATTED_STRING
	void UIW_FORMATTED_STRING_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_FORMATTED_STRING_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_GROUP
	void UIW_GROUP_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_GROUP_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_HZ_LIST
	void UIW_HZ_LIST_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_HZ_LIST_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_ICON
	void UIW_ICON_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_ICON_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_INTEGER
	void UIW_INTEGER_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_INTEGER_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_MAXIMIZE_BUTTON
	void UIW_MAXIMIZE_BUTTON_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_MAXIMIZE_BUTTON_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_MINIMIZE_BUTTON
	void UIW_MINIMIZE_BUTTON_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_MINIMIZE_BUTTON_DataStore(UI_WINDOW_OBJECT *object);

	// --- UIW_NOTEBOOK
	void UIW_NOTEBOOK_DataLoad(UI_WINDOW_OBJECT *object);
	int UIW_NOTEBOOK_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_POP_UP_ITEM -----
	void UIW_POP_UP_ITEM_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_POP_UP_ITEM_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_POP_UP_MENU -----
	void UIW_POP_UP_MENU_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_POP_UP_MENU_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_PROMPT -----
	void UIW_PROMPT_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_PROMPT_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_PULL_DOWN_ITEM -----
	void UIW_PULL_DOWN_ITEM_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_PULL_DOWN_ITEM_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_PULL_DOWN_MENU -----
	void UIW_PULL_DOWN_MENU_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_PULL_DOWN_MENU_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_REAL -----
	void UIW_REAL_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_REAL_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_SCROLL_BAR -----
	void UIW_SCROLL_BAR_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_SCROLL_BAR_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_SPIN_CONTROL -----
	void UIW_SPIN_CONTROL_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_SPIN_CONTROL_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_STATUS_BAR -----
	void UIW_STATUS_BAR_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_STATUS_BAR_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_STRING -----
	void UIW_STRING_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_STRING_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_SYSTEM_BUTTON -----
	void UIW_SYSTEM_BUTTON_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_SYSTEM_BUTTON_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_TABLE -----
	void UIW_TABLE_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_TABLE_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_TABLE_HEADER -----
	void UIW_TABLE_HEADER_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_TABLE_HEADER_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_TABLE_RECORD -----
	void UIW_TABLE_RECORD_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_TABLE_RECORD_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_TEXT -----
	void UIW_TEXT_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_TEXT_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_TIME -----
	void UIW_TIME_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_TIME_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_TITLE -----
	void UIW_TITLE_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_TITLE_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_TOOL_BAR -----
	void UIW_TOOL_BAR_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_TOOL_BAR_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_VT_LIST -----
	void UIW_VT_LIST_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_VT_LIST_DataStore(UI_WINDOW_OBJECT *object);

	// ----- UIW_WINDOW -----
	void UIW_WINDOW_DataLoad(UI_WINDOW_OBJECT *object);
	int  UIW_WINDOW_DataStore(UI_WINDOW_OBJECT *object);

	// ----- Other Objects -----
	void ZAF_DERIVED_OBJECT_DataLoad(UI_WINDOW_OBJECT *object);
	int ZAF_DERIVED_OBJECT_DataStore(UI_WINDOW_OBJECT *object);

	void ZAF_EDIT_GROUP_DataLoad(UI_WINDOW_OBJECT *object);
	int ZAF_EDIT_GROUP_DataStore(UI_WINDOW_OBJECT *object);

	// ----- Get Routines -----
	int GetFlag(ZIL_NUMBERID fieldID, UIF_FLAGS *currentFlags, UIF_FLAGS flag);
	int GetHelp(ZIL_NUMBERID fieldID, UI_HELP_CONTEXT *helpContext);
	int GetImage(ZIL_NUMBERID fieldID, ZIL_ICHAR **name, ZIL_OBJECTID imageType);
	int GetScroll(ZIL_NUMBERID fieldID, UIW_WINDOW *window, SBF_FLAGS sbFlags);
	int GetSupport(ZIL_NUMBERID fieldID, UIW_WINDOW *window, NUMBERID numberID);
	int GetText(ZIL_NUMBERID fieldID, ZIL_ICHAR *text);
	int GetText(ZIL_NUMBERID fieldID, ZIL_ICHAR **text);
	int GetValue(ZIL_NUMBERID fieldID, ZIL_UINT16 *value);
	int GetValue(ZIL_NUMBERID fieldID, int *value);
	int GetValue(ZIL_NUMBERID fieldID, double *value);
	int GetValue(ZIL_NUMBERID fieldID, ZIL_BIGNUM *value);
	int GetValue(ZIL_NUMBERID fieldID, ZIL_DATE *value);
	int GetValue(ZIL_NUMBERID fieldID, ZIL_TIME *value);

	// ----- Set Routines -----
	void SetFlag(ZIL_NUMBERID fieldID, UIF_FLAGS currentFlags, UIF_FLAGS flag);
	void SetHelp(ZIL_NUMBERID fieldID, UI_HELP_CONTEXT helpContext);
	void SetImage(ZIL_NUMBERID fieldID, ZIL_ICHAR *name, ZIL_OBJECTID imageType);
	void SetScroll(ZIL_NUMBERID fieldID, UIW_WINDOW *window, SBF_FLAGS sbFlags);
	void SetSupport(ZIL_NUMBERID fieldID, UIW_WINDOW *window, NUMBERID numberID);
	void SetText(ZIL_NUMBERID fieldID, ZIL_ICHAR *text);
	void SetValue(ZIL_NUMBERID fieldID, int value);
	void SetValue(ZIL_NUMBERID fieldID, ZIL_UINT16 value);
	void SetValue(ZIL_NUMBERID fieldID, double value);
	void SetValue(ZIL_NUMBERID fieldID, ZIL_BIGNUM *value);
	void SetValue(ZIL_NUMBERID fieldID, ZIL_DATE *value);
	void SetValue(ZIL_NUMBERID fieldID, ZIL_TIME *value);

	void ListUpdate(void);
	ZIL_ICHAR *BitmapName(UI_WINDOW_OBJECT *);
	static int InitializeStaticMembers(void);
	static void NaturalCoordinate(UI_WINDOW_OBJECT *object, UI_REGION &region, WOF_FLAGS newFlags);
	EVENT_TYPE ObjectAdd(const UI_EVENT &event);
	EVENT_TYPE ObjectDelete(const UI_EVENT &event);
	EVENT_TYPE ObjectMoveUp(const UI_EVENT &event);
	EVENT_TYPE ObjectMoveDown(const UI_EVENT &event);
	static int RestoreStaticMembers(void);
};

// ----- Class Objects ------------------------------------------------------

class ZAF_BIGNUM : public UIW_BIGNUM
{
public:
	ZAF_BIGNUM(int left, int top, int width, ZIL_BIGNUM *value,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_BIGNUM(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_BIGNUM::New); }
};

class ZAF_BORDER : public UIW_BORDER
{
public:
	ZAF_BORDER(BDF_FLAGS bdFlags = BDF_NO_FLAGS);
	ZAF_BORDER(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_BIGNUM::New); }
};

class ZAF_BUTTON : public UIW_BUTTON
{
public:
	ZAF_BUTTON(int left, int top, int width, ZIL_ICHAR *text,
		BTF_FLAGS btFlags = BTF_NO_TOGGLE | BTF_AUTO_SIZE,
		WOF_FLAGS woFlags = WOF_JUSTIFY_CENTER,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION),
		EVENT_TYPE value = 0, ZIL_ICHAR *bitmapName = ZIL_NULLP(ZIL_ICHAR));
	ZAF_BUTTON(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	static UI_WINDOW_OBJECT *CheckBoxNew(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	static UI_WINDOW_OBJECT *RadioButtonNew(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_BUTTON::New); }
};

class ZAF_COMBO_BOX : public UIW_COMBO_BOX
{
public:
	ZAF_COMBO_BOX(int left, int top, int width, int height,
		ZIL_COMPARE_FUNCTION compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION),
		WNF_FLAGS wnFlags = WNF_NO_WRAP, WOF_FLAGS woFlags = WOF_NO_FLAGS,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	ZAF_COMBO_BOX(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_COMBO_BOX::New); }
};

class ZAF_DATE : public UIW_DATE
{
public:
	ZAF_DATE(int left, int top, int width, ZIL_DATE *date,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		DTF_FLAGS dtFlags = DTF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_DATE(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_DATE::New); }
};

class ZAF_DERIVED_OBJECT : public UI_WINDOW_OBJECT
{
public:
	static ZIL_ICHAR _className[];

	ZAF_DERIVED_OBJECT(void);
	~ZAF_DERIVED_OBJECT(void);
	ZIL_ICHAR *ClassName(void) { return (_className); }
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE Event(const UI_EVENT &event);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	ZAF_DERIVED_OBJECT(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_DERIVED_OBJECT::New); }
private:
	ZIL_OBJECTID baseSearchID;
};

class ZAF_EDIT_GROUP : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];

	ZAF_EDIT_GROUP(void);
	~ZAF_EDIT_GROUP(void);
	ZIL_ICHAR *ClassName(void) { return (_className); }
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE Event(const UI_EVENT &event);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);

	ZAF_EDIT_GROUP(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_EDIT_GROUP::New); }
	void Store(const ZIL_ICHAR *name,
		ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
		ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
};

class ZAF_FORMATTED_STRING : public UIW_FORMATTED_STRING
{
public:
	ZAF_FORMATTED_STRING(int left, int top, int width, ZIL_ICHAR *compressedText,
		ZIL_ICHAR *editMask, ZIL_ICHAR *deleteText,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_FORMATTED_STRING(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_FORMATTED_STRING::New); }
};

class ZAF_GROUP : public UIW_GROUP
{
public:
	ZAF_GROUP(int left, int top, int width, int height, ZIL_ICHAR *text,
		WNF_FLAGS wnFlags = WNF_AUTO_SELECT, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	ZAF_GROUP(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_GROUP::New); }
};

class ZAF_HZ_LIST : public UIW_HZ_LIST
{
public:
	ZAF_HZ_LIST(int left, int top, int width, int height,
		int cellWidth, int cellHeight,
		ZIL_COMPARE_FUNCTION compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION),
		WNF_FLAGS wnFlags = WNF_NO_WRAP | WNF_CONTINUE_SELECT, WOF_FLAGS woFlags = WOF_BORDER,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	ZAF_HZ_LIST(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_HZ_LIST::New); }
};

class ZAF_ICON : public UIW_ICON
{
public:
	ZAF_ICON(int left, int top, ZIL_ICHAR *iconName,
		ZIL_ICHAR *title = ZIL_NULLP(ZIL_ICHAR), ICF_FLAGS icFlags = ICF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_JUSTIFY_CENTER | WOF_NON_SELECTABLE,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_ICON(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_ICON::New); }
};

class ZAF_INTEGER : public UIW_INTEGER
{
public:
	ZAF_INTEGER(int left, int top, int width, int *value,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_INTEGER(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_INTEGER::New); }
};

class ZAF_MAXIMIZE_BUTTON : public UIW_MAXIMIZE_BUTTON
{
public:
	ZAF_MAXIMIZE_BUTTON(void);
	ZAF_MAXIMIZE_BUTTON(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_MAXIMIZE_BUTTON::New); }
};

class ZAF_MINIMIZE_BUTTON : public UIW_MINIMIZE_BUTTON
{
public:
	ZAF_MINIMIZE_BUTTON(void);
	ZAF_MINIMIZE_BUTTON(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_MINIMIZE_BUTTON::New); }
};

class ZAF_NOTEBOOK : public UIW_NOTEBOOK
{
public:
	ZAF_NOTEBOOK(void);
	ZAF_NOTEBOOK(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_NOTEBOOK::New); }
};

class ZAF_POP_UP_ITEM : public UIW_POP_UP_ITEM
{
public:
	ZAF_POP_UP_ITEM(ZIL_ICHAR *text, MNIF_FLAGS mniFlags = MNIF_NO_FLAGS,
		BTF_FLAGS btFlags = BTF_NO_3D, WOF_FLAGS woFlags = WOF_NO_FLAGS,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION), EVENT_TYPE value = 0);
	ZAF_POP_UP_ITEM(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_POP_UP_ITEM::New); }
};

class ZAF_POP_UP_MENU : public UIW_POP_UP_MENU
{
public:
	ZAF_POP_UP_MENU(int left, int top, WNF_FLAGS wnFlags,
		WOF_FLAGS woFlags = WOF_BORDER, WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	ZAF_POP_UP_MENU(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_POP_UP_MENU::New); }
};

class ZAF_PROMPT : public UIW_PROMPT
{
public:
	ZAF_PROMPT(int left, int top, ZIL_ICHAR *text, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	ZAF_PROMPT(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_PROMPT::New); }
};

class ZAF_PULL_DOWN_ITEM : public UIW_PULL_DOWN_ITEM
{
public:
	ZAF_PULL_DOWN_ITEM(ZIL_ICHAR *text, WNF_FLAGS wnFlags = WNF_NO_FLAGS,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION),
		EVENT_TYPE value = 0);
	ZAF_PULL_DOWN_ITEM(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_PULL_DOWN_ITEM::New); }
};

class ZAF_PULL_DOWN_MENU : public UIW_PULL_DOWN_MENU
{
public:
	ZAF_PULL_DOWN_MENU(int indentation = 0,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	ZAF_PULL_DOWN_MENU(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_PULL_DOWN_MENU::New); }
};

class ZAF_REAL : public UIW_REAL
{
public:
	ZAF_REAL(int left, int top, int width, double *value,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_REAL(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_REAL::New); }
};

class ZAF_SCROLL_BAR : public UIW_SCROLL_BAR
{
public:
	ZAF_SCROLL_BAR(int left, int top, int width, int height,
		SBF_FLAGS sbFlags = SBF_VERTICAL,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION);
	ZAF_SCROLL_BAR(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	static UI_WINDOW_OBJECT *HzSlideNew(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	static UI_WINDOW_OBJECT *VtSlideNew(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_SCROLL_BAR::New); }
};

class ZAF_SPIN_CONTROL : public UIW_SPIN_CONTROL
{
public:
	ZAF_SPIN_CONTROL(int left, int top, int width, UI_WINDOW_OBJECT *fieldObject,
		ZIL_INT32 _delta = 1, WNF_FLAGS wnFlags = WNF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_NO_FLAGS,
		ZIL_USER_FUNCTION _userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_SPIN_CONTROL(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_SPIN_CONTROL::New); }
};

class ZAF_STATUS_BAR : public UIW_STATUS_BAR
{
public:
	ZAF_STATUS_BAR(int _height = 1, WOF_FLAGS _woFlags = WOF_BORDER | WOF_SUPPORT_OBJECT);
	ZAF_STATUS_BAR(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_STATUS_BAR::New); }
};

class ZAF_STRING : public UIW_STRING
{
public:
	ZAF_STRING(int left, int top, int width, ZIL_ICHAR *text, int maxLength = -1,
		STF_FLAGS stFlags = STF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_STRING(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_STRING::New); }
};

class ZAF_SYSTEM_BUTTON : public UIW_SYSTEM_BUTTON
{
public:
	ZAF_SYSTEM_BUTTON(SYF_FLAGS syFlags = SYF_NO_FLAGS);
	ZAF_SYSTEM_BUTTON(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_SYSTEM_BUTTON::New); }
};

class ZAF_TABLE : public UIW_TABLE
{
public:
	ZAF_TABLE(int left, int top, int width, int height, int columns = 1,
		int recordSize = 0, int maxRecords = -1, void *data = ZIL_NULLP(void),
		int records = 0, TBLF_FLAGS tblFlags = TBLF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER);
	ZAF_TABLE(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_TABLE::New); }
};

class ZAF_TABLE_HEADER : public UIW_TABLE_HEADER
{
public:
	ZAF_TABLE_HEADER(THF_FLAGS thFlags, int recordSize = 0, int maxRecords = -1,
		void *data = ZIL_NULLP(void), WOF_FLAGS woFlags = WOF_BORDER);
	ZAF_TABLE_HEADER(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_TABLE_HEADER::New); }
};

class ZAF_TABLE_RECORD : public UIW_TABLE_RECORD
{
public:
	ZAF_TABLE_RECORD(int width, int height,	ZIL_USER_FUNCTION userFunction =
		ZIL_NULLF(ZIL_USER_FUNCTION), WOF_FLAGS woFlags = WOF_NO_FLAGS);
	ZAF_TABLE_RECORD(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_TABLE_RECORD::New); }
};

class ZAF_TEXT : public UIW_TEXT
{
public:
	ZAF_TEXT(int left, int top, int width, int height, ZIL_ICHAR *text,
		int maxLength = -1, WNF_FLAGS wnFlags = WNF_NO_WRAP,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_TEXT(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_TEXT::New); }
};

class ZAF_TIME : public UIW_TIME
{
public:
	ZAF_TIME(int left, int top, int width, ZIL_TIME *time,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		TMF_FLAGS tmFlags = TMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	ZAF_TIME(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_TIME::New); }
};

class ZAF_TITLE : public UIW_TITLE
{
public:
	ZAF_TITLE(ZIL_ICHAR *text, WOF_FLAGS woFlags = WOF_BORDER | WOF_JUSTIFY_CENTER);
	ZAF_TITLE(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_TITLE::New); }
};

class ZAF_TOOL_BAR : public UIW_TOOL_BAR
{
public:
	ZAF_TOOL_BAR(int left, int top, int width, int height,
		WNF_FLAGS wnFlags = WNF_NO_FLAGS, WOF_FLAGS woFlags = WOF_BORDER | WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	ZAF_TOOL_BAR(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_TOOL_BAR::New); }
};

class ZAF_VT_LIST : public UIW_VT_LIST
{
public:
	ZAF_VT_LIST(int left, int top, int width, int height,
		ZIL_COMPARE_FUNCTION compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION),
		WNF_FLAGS wnFlags = WNF_NO_WRAP | WNF_CONTINUE_SELECT, WOF_FLAGS woFlags = WOF_BORDER,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	ZAF_VT_LIST(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_VT_LIST::New); }
};

const INFO_REQUEST I_GET_CREATE_OBJECT		= 0x310;
const INFO_REQUEST I_RESET_CREATE_OBJECT	= 0x311;
const INFO_REQUEST I_GET_DUPLICATE_OBJECT	= 0x312;
const INFO_REQUEST I_SET_PASTE_OBJECT		= 0x314;
const INFO_REQUEST I_GET_PASTE_OBJECT		= 0x315;

class ZAF_WINDOW : public UIW_WINDOW
{
public:
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static UI_ITEM *_deriveTable;

	ZAF_WINDOW(int left, int top, int width, int height,
		WOF_FLAGS woFlags = WOF_NO_FLAGS,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS,
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT,
		UI_WINDOW_OBJECT *minObject = ZIL_NULLP(UI_WINDOW_OBJECT));
	ZAF_WINDOW(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	~ZAF_WINDOW(void);
	EVENT_TYPE Event(const UI_EVENT &);
	void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable, UI_ITEM *userTable);
	ZIL_NEW_FUNCTION NewFunction(void) { return (ZAF_WINDOW::New); }
	void Store(const ZIL_ICHAR *name,
		ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
		ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));

	struct PROCESS
	{
		EVENT_TYPE option;

 		// This crashes the GNU 2.2.2 compiler, works fine with 2.5.8.
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		EVENT_TYPE (UI_ELEMENT::*Process)(const UI_EVENT &event);
#		else
		EVENT_TYPE (ZAF_WINDOW::*Process)(const UI_EVENT &event);
#		endif
	};

private:
	static PROCESS _processTable[];
	ZIL_LANGUAGE msgTable;

	UI_WINDOW_OBJECT *editObject;
	UI_WINDOW_OBJECT *windowEditor;
	ZAF_EDIT_GROUP *editGroup;
	UI_WINDOW_OBJECT *storageService;

	EVENT_TYPE EditCopy(const UI_EVENT &event);
	EVENT_TYPE EditCut(const UI_EVENT &event);
	EVENT_TYPE EditDelete(const UI_EVENT &event);
	EVENT_TYPE EditGroupDefine(const UI_EVENT &event);
	EVENT_TYPE EditGroupUndefine(const UI_EVENT &event);
	EVENT_TYPE EditMove(const UI_EVENT &event);
	EVENT_TYPE EditObject(const UI_EVENT &event);
	EVENT_TYPE EditPaste(const UI_EVENT &event);
	EVENT_TYPE EditSize(const UI_EVENT &event);
};

// --- Message definitions ---
const ZIL_NUMBERID ZMSG_FILE_NONE 		= 0;
const ZIL_NUMBERID ZMSG_FILE_NOT_FOUND 	= 1;

#endif
