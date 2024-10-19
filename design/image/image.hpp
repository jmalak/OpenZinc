//	Zinc Application Framework - IMAGE.HPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_IMAGE_HPP)

#define ZAF_IMAGE_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

const IMF_FLAGS IMF_STATIC_IMAGE			= 0x0100;
const IMF_FLAGS IMF_MAXFIT					= 0x0200;
const IMF_FLAGS IMF_GRID					= 0x0400;

const IMF_FLAGS IMF_BITMAP_IMAGE			= 0x1000;
const IMF_FLAGS IMF_ICON_IMAGE				= 0x2000;
const IMF_FLAGS IMF_MOUSE_IMAGE				= 0x4000;

const INFO_REQUEST START_INFORMATION 		= 0x400;
const INFO_REQUEST CONTINUE_INFORMATION 	= 0x401;
const INFO_REQUEST END_INFORMATION 			= 0x402;

class ZAF_IMAGE_EDITOR : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static ZIL_STORAGE_READ_ONLY *_storage;

	static ZIL_ICHAR *_imageDirectory;

#if defined(__DECCXX)
	// Compiler requires a default constructor.
	ZAF_IMAGE_EDITOR();
#endif
	ZAF_IMAGE_EDITOR(WOAF_FLAGS woAdvancedFlags, WOS_STATUS woStatus);
	~ZAF_IMAGE_EDITOR(void);
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
		EVENT_TYPE (ZAF_IMAGE_EDITOR::*Process)(const UI_EVENT &event);
#		endif
	};

private:
	static struct PROCESS _processTable[];

	ZIL_LANGUAGE msgTable;
	static ZIL_ICHAR _zncExtension[];
	static ZIL_ICHAR _xpmExtension[];
	static ZIL_ICHAR _bmpExtension[];
	static ZIL_ICHAR _obmpExtension[];
	static ZIL_ICHAR _wbmpExtension[];
	static ZIL_ICHAR _icoExtension[];
	static ZIL_ICHAR _oicoExtension[];
	static ZIL_ICHAR _wicoExtension[];
	static ZIL_ICHAR _curExtension[];
	static ZIL_ICHAR _ptrExtension[];

	UI_WINDOW_OBJECT *directoryService;
	UI_WINDOW_OBJECT *storageService;
	ZIL_ICHAR originalTitle[ZIL_MAXPATHLEN];
	UI_WINDOW_OBJECT *left, *right, *screen;
	ZIL_COLOR leftColor, rightColor, screenColor;

	int rollerSize;
	EVENT_TYPE pattern;

	EVENT_TYPE EditRequest(const UI_EVENT &event);

	EVENT_TYPE FileClose(const UI_EVENT &event);
	EVENT_TYPE FileDelete(const UI_EVENT &event);
	EVENT_TYPE FileNew(const UI_EVENT &event);
	EVENT_TYPE FileOpen(const UI_EVENT &event);
	EVENT_TYPE FileRequest(const UI_EVENT &event);
	EVENT_TYPE FileSave(const UI_EVENT &event);
	EVENT_TYPE FileSaveAs(const UI_EVENT &event);

	EVENT_TYPE BitmapCreate(const UI_EVENT &event);
	EVENT_TYPE IconCreate(const UI_EVENT &event);
	EVENT_TYPE MouseCreate(const UI_EVENT &event);
	EVENT_TYPE ImageClear(const UI_EVENT &event);
	EVENT_TYPE ImageClearAll(const UI_EVENT &event);
	EVENT_TYPE ImageDelete(const UI_EVENT &event);
	EVENT_TYPE ImageExport(const UI_EVENT &event);
	EVENT_TYPE ImageImport(const UI_EVENT &event);
	EVENT_TYPE ImageLoad(const UI_EVENT &event);
	EVENT_TYPE ImageRequest(const UI_EVENT &event);
	EVENT_TYPE ImageStore(const UI_EVENT &event);
	EVENT_TYPE ImageStoreAs(const UI_EVENT &event);

	UI_WINDOW_OBJECT *GetImageWindow(void);
	void SetTitle(void);
	void StatusUpdate(UI_WINDOW_OBJECT *editObject = ZIL_NULLP(UI_WINDOW_OBJECT), int clear = FALSE);
	EVENT_TYPE OSImport(ZIL_STORAGE *srcFile, const UI_EVENT &event);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *osFile, const UI_EVENT &event, ZIL_ICHAR *imageType);
	EVENT_TYPE OSExport(ZIL_STORAGE *srcFile, ZIL_STORAGE *datFile, const UI_EVENT &event);
};

// --------------------------------------------------------------------------
// ---- ZAF_IMAGE -----------------------------------------------------------
// --------------------------------------------------------------------------

static const int F_NONE = 0;
static const int F_ODIB = 1;
static const int F_WDIB = 2;
static const int F_OICO = 3;
static const int F_WICO = 4;
static const int F_XPM = 5;
static const int F_ZNC = 6;

class ZAF_IMAGE : public UI_WINDOW_OBJECT
{
	friend class ZAF_IMAGE_WINDOW;
	friend class ZAF_IMAGE_EDITOR;
public:
	// ----- exception, attribute & image definitions -----------------------
	enum Z_ERROR
	{
		ERROR_NONE						= 0,
		ERROR_FILE						= 1,
		ERROR_FILTER					= 2,
		ERROR_FORMAT					= 3,

		ERROR_RGB_QUAD					= 10,
		ERROR_RGB_TRIPLE				= 11,
		ERROR_FILE_HEADER				= 12,
		ERROR_INFO_HEADER				= 13,
		ERROR_BITMAP_TOO_LARGE			= 14,
		ERROR_BITMAP_COMPRESSED			= 15,

		ERROR_INVALID_COORDINATE		= 20
	};

	ZAF_IMAGE(int left, int top, int width, int height, IMF_FLAGS imFlags = IMF_NO_FLAGS, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	~ZAF_IMAGE(void);
	void DataGet(ZIL_UINT8 **bitmapArray, int *bitmapWidth, int *bitmapHeight,
		int *hotspotX = ZIL_NULLP(int), int *hotspotY = ZIL_NULLP(int));
	void DataSet(ZIL_UINT8 *bitmapArray, int bitmapWidth, int bitmapHeight,
		int hotspotX = -1, int hotspotY = -1);
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE Event(const UI_EVENT &event);

	struct PROCESS
	{
		EVENT_TYPE option;

		// This crashes the GNU 2.2.2 compiler, works fine with 2.5.8.
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		EVENT_TYPE (UI_ELEMENT::*Process)(const UI_EVENT &event);
#		else
		EVENT_TYPE (ZAF_IMAGE::*Process)(const UI_EVENT &event);
#		endif
	};
	typedef ZAF_IMAGE::Z_ERROR (ZAF_IMAGE::*IMPORT_FILTER)(ZIL_FILE *file);
	typedef ZAF_IMAGE::Z_ERROR (ZAF_IMAGE::*EXPORT_FILTER)(ZIL_FILE *file);
	struct IMPORT_FILTER_STRUCT
	{
		int tag;
		IMPORT_FILTER filter;
	};
	struct EXPORT_FILTER_STRUCT
	{
		int tag;
		EXPORT_FILTER filter;
	};
private:
	static struct PROCESS _processTable[];
	static struct IMPORT_FILTER_STRUCT _importTable[];
	static struct EXPORT_FILTER_STRUCT _exportTable[];

	ZAF_IMAGE::Z_ERROR error;
	IMF_FLAGS imFlags;
	int grid;

	int zoomRatio, pixelWidth, pixelHeight;

	// Image information.
	int bitmapWidth, bitmapHeight;
	int hotspotX, hotspotY;
	ZIL_UINT8 *bitmapArray;

	// Attribute information.
	int fill;
	ZIL_COLOR color;
	UI_REGION start, end;

	// Support information.
	int updateDisplay;
	UI_WINDOW_OBJECT *imageEditor;
	static UI_WINDOW_OBJECT *pasteObject;
	static UI_REGION pasteRegion;
	static ZIL_UINT8 *pasteArray;

	int Clear(void);
	int Ellipse(int left, int top, int right, int bottom);
	int Fill(int column, int line);
	int Line(int column1, int line1, int column2, int line2);
	int Pixel(int column, int line);
	int Rectangle(int left, int top, int right, int bottom);

	EVENT_TYPE DrawFill(const UI_EVENT &event, EVENT_TYPE ccode, EVENT_TYPE pattern);
	EVENT_TYPE DrawLine(const UI_EVENT &event, EVENT_TYPE ccode, EVENT_TYPE pattern);
	EVENT_TYPE DrawPixel(const UI_EVENT &event, EVENT_TYPE ccode, EVENT_TYPE pattern);
	EVENT_TYPE DrawRectangle(const UI_EVENT &event, EVENT_TYPE ccode, EVENT_TYPE pattern);

	EVENT_TYPE EditCopy(const UI_EVENT &event);
	EVENT_TYPE EditCut(const UI_EVENT &event);
	EVENT_TYPE EditDelete(const UI_EVENT &event);
	EVENT_TYPE EditGroupDefine(const UI_EVENT &event);
	EVENT_TYPE EditGroupUndefine(const UI_EVENT &event);
	EVENT_TYPE EditMove(const UI_EVENT &event);
	EVENT_TYPE EditPaste(const UI_EVENT &event);

	ZAF_IMAGE::Z_ERROR GetError(void);
	ZAF_IMAGE::Z_ERROR SetError(ZAF_IMAGE::Z_ERROR _error);

	// --- Import Filter definitions ---
	ZAF_IMAGE::Z_ERROR SetImportFilter(int FilterImport);

	ZAF_IMAGE::Z_ERROR Import(ZIL_FILE *file);
	ZAF_IMAGE::Z_ERROR ImportODIB(ZIL_FILE *file); // ODIB (OS/2 device independent bitmap).
	ZAF_IMAGE::Z_ERROR ImportWDIB(ZIL_FILE *file); // WDIB (Windows device independent bitmap).
	ZAF_IMAGE::Z_ERROR ImportOICO(ZIL_FILE *file); // OICO (OS/2 device independent icon).
	ZAF_IMAGE::Z_ERROR ImportWICO(ZIL_FILE *file); // WICO (Windows device independent icon).
	ZAF_IMAGE::Z_ERROR ImportXPM(ZIL_FILE *file); // XPM (X PixMap).
	ZAF_IMAGE::Z_ERROR ImportZNC(ZIL_FILE *file); // Zinc.

	// --- Filter definitions ---
	ZAF_IMAGE::Z_ERROR SetExportFilter(int FilterExport);
	ZAF_IMAGE::Z_ERROR Export(ZIL_FILE *file);

	ZAF_IMAGE::Z_ERROR ExportODIB(ZIL_FILE *file); // ODIB (OS/2 device independent bitmap).
	ZAF_IMAGE::Z_ERROR ExportWDIB(ZIL_FILE *file); // WDIB (Windows device independent bitmap).
	ZAF_IMAGE::Z_ERROR ExportOICO(ZIL_FILE *file); // OICO (OS/2 device independent icon).
	ZAF_IMAGE::Z_ERROR ExportWICO(ZIL_FILE *file); // WICO (Windows device independent icon).
	ZAF_IMAGE::Z_ERROR ExportXPM(ZIL_FILE *file); // XPM (X PixMap).
	ZAF_IMAGE::Z_ERROR ExportZNC(ZIL_FILE *file); // Zinc.

	IMPORT_FILTER FilterImport;
	EXPORT_FILTER FilterExport;
};

class ZAF_IMAGE_WINDOW : public UIW_WINDOW
{
public:
	ZAF_IMAGE_WINDOW(ZIL_ICHAR *name, IMF_FLAGS imFlags);
	~ZAF_IMAGE_WINDOW(void);
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

	static EVENT_TYPE CheckImage(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);

private:
	ZAF_IMAGE *editImage;
	ZAF_IMAGE *actualImage;
	IMF_FLAGS imFlags;

	void CheckVariables(void);
};

typedef UIF_FLAGS CLF_FLAGS;
const CLF_FLAGS CLF_LEFT_ACTIVATE			= 0x0001;
const CLF_FLAGS CLF_RIGHT_ACTIVATE			= 0x0002;

class UIW_COLOR_BAR : public UI_WINDOW_OBJECT
{
public:
	UIW_COLOR_BAR(int left, int top, int right, int bottom, CLF_FLAGS clFlags, WOF_FLAGS woFlags = WOF_BORDER);
	~UIW_COLOR_BAR(void);
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

private:
	ZIL_COLOR color;
	int cellWidth;
	int maxColors;
	CLF_FLAGS clFlags;
};

const INFO_REQUEST I_GET_COLOR				= 6500;
const INFO_REQUEST I_GET_LEFT_COLOR			= 6501;
const INFO_REQUEST I_GET_RIGHT_COLOR		= 6502;
const INFO_REQUEST I_GET_SCREEN_COLOR		= 6503;
const INFO_REQUEST I_SET_LEFT_COLOR			= 6504;
const INFO_REQUEST I_SET_RIGHT_COLOR		= 6505;
const INFO_REQUEST I_SET_SCREEN_COLOR		= 6506;
const INFO_REQUEST I_GET_PATTERN			= 6507;
const INFO_REQUEST I_GET_ROLLER_SIZE		= 6508;

// category 1
const EVENT_TYPE OPT_BITMAP_CREATE			= 6320;
const EVENT_TYPE OPT_ICON_CREATE			= 6321;
const EVENT_TYPE OPT_MOUSE_CREATE			= 6322;
// category 2
const EVENT_TYPE OPT_PATTERN				= 6500;
const EVENT_TYPE OPT_PATTERN_PENCIL			= 6501;
const EVENT_TYPE OPT_PATTERN_ROLLER			= 6502;
const EVENT_TYPE OPT_PATTERN_LINE			= 6503;
const EVENT_TYPE OPT_PATTERN_RECTANGLE		= 6504;
const EVENT_TYPE OPT_PATTERN_FILL_RECTANGLE	= 6505;
const EVENT_TYPE OPT_PATTERN_ELLIPSE		= 6506;
const EVENT_TYPE OPT_PATTERN_FILL_ELLIPSE	= 6507;
const EVENT_TYPE OPT_PATTERN_FILL			= 6508;
// category 3
const EVENT_TYPE OPT_GRID					= 6509;
const EVENT_TYPE OPT_CHECK_VARIABLES		= 6510;
const EVENT_TYPE OPT_REFRESH_BACKGROUND		= 6511;
const EVENT_TYPE OPT_REFRESH_PASTE			= 6512;
const EVENT_TYPE OPT_ERASE					= 6513;
// category 4
const EVENT_TYPE OPT_ROLLER_SIZE			= 6550;
// category 5

class ZAF_IMAGE_INFORMATION : public UIW_WINDOW
{
public:
	ZAF_IMAGE_INFORMATION(ZIL_ICHAR *name, UI_WINDOW_OBJECT *object);
	~ZAF_IMAGE_INFORMATION(void);
	EVENT_TYPE Event(const UI_EVENT &event);	

	struct PROCESS
	{
		ZIL_OBJECTID objectID;

		// This crashes the GNU 2.2.2 compiler, works fine with 2.5.8.
#		if defined(ZIL_NEXTSTEP) && defined(__NX_COMPILER_RELEASE_3_1)
		void (UI_ELEMENT::*Load)(UI_WINDOW_OBJECT *data, int page);
		int (UI_ELEMENT::*Store)(UI_WINDOW_OBJECT *data, int page);
#		else
		void (ZAF_IMAGE_INFORMATION::*Load)(UI_WINDOW_OBJECT *data, int page);
		int (ZAF_IMAGE_INFORMATION::*Store)(UI_WINDOW_OBJECT *data, int page);
#		endif
	};
private:
	static struct PROCESS _processTable[];

	void (ZAF_IMAGE_INFORMATION::*DataLoad)(UI_WINDOW_OBJECT *data, int page);
	int (ZAF_IMAGE_INFORMATION::*DataStore)(UI_WINDOW_OBJECT *data, int page);
	void UI_BITMAP_DataLoad(UI_WINDOW_OBJECT *data, int page);
	int UI_BITMAP_DataStore(UI_WINDOW_OBJECT *data, int page);
	void UI_ICON_DataLoad(UI_WINDOW_OBJECT *data, int page);
	int UI_ICON_DataStore(UI_WINDOW_OBJECT *data, int page);
	void UI_MOUSE_DataLoad(UI_WINDOW_OBJECT *data, int page);
	int UI_MOUSE_DataStore(UI_WINDOW_OBJECT *data, int page);
};

#endif

