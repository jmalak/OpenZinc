//	Zinc Interface Library - IMAGE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#define USE_MSG_TABLE
#include "p_image.hpp"

// --- Definitions taken from p_image.hpp ---
const ZIL_NUMBERID EDIT_GRID 		= 0x0022;
const ZIL_NUMBERID ROLLER_1X1		= 0x0025;
const ZIL_NUMBERID PATTERN_PENCIL	= 0x003B;

const ZIL_NUMBERID MNU_PENCIL                   	= 0x003B;
const ZIL_NUMBERID MNU_ROLLER                   	= 0x003C;
const ZIL_NUMBERID MNU_LINE                     	= 0x0043;
const ZIL_NUMBERID MNU_RECTANGLE                	= 0x003D;
const ZIL_NUMBERID MNU_FILL_RECTANGLE              	= 0x003E;
const ZIL_NUMBERID MNU_ELLIPSE                  	= 0x003F;
const ZIL_NUMBERID MNU_FILL_ELLIPSE             	= 0x0040;
const ZIL_NUMBERID MNU_FILL                     	= 0x0041;
const ZIL_NUMBERID BTN_PENCIL                       = 0x0033;
const ZIL_NUMBERID BTN_ROLLER                       = 0x0034;
const ZIL_NUMBERID BTN_LINE                         = 0x0035;
const ZIL_NUMBERID BTN_RECTANGLE                    = 0x0036;
const ZIL_NUMBERID BTN_FILL_RECTANGLE               = 0x0037;
const ZIL_NUMBERID BTN_ELLIPSE                      = 0x0038;
const ZIL_NUMBERID BTN_FILL_ELLIPSE                 = 0x0039;
const ZIL_NUMBERID BTN_FILL                         = 0x003A;
static struct IMG_PATTERN
{
	EVENT_TYPE option;
	ZIL_NUMBERID btnField;
	ZIL_NUMBERID mnuField;
} _patternTable[] =
{
	{ OPT_PATTERN_PENCIL,			BTN_PENCIL, 		MNU_PENCIL },
	{ OPT_PATTERN_ROLLER,			BTN_ROLLER, 		MNU_ROLLER },
	{ OPT_PATTERN_LINE,				BTN_LINE,			MNU_LINE },
	{ OPT_PATTERN_RECTANGLE,		BTN_RECTANGLE, 		MNU_RECTANGLE },
	{ OPT_PATTERN_FILL_RECTANGLE,	BTN_FILL_RECTANGLE, MNU_FILL_RECTANGLE },
	{ OPT_PATTERN_ELLIPSE,			BTN_ELLIPSE, 		MNU_ELLIPSE },
	{ OPT_PATTERN_FILL_ELLIPSE,		BTN_FILL_ELLIPSE, 	MNU_FILL_ELLIPSE },
	{ OPT_PATTERN_FILL,				BTN_FILL, 			MNU_FILL },
	{ 0, 0, 0 }
};

extern ZIL_ICHAR *_bitmapDirectory;
extern ZIL_ICHAR *_iconDirectory;
extern ZIL_ICHAR *_mouseDirectory;
ZIL_ICHAR *ZAF_IMAGE_EDITOR::_imageDirectory = ZIL_NULLP(ZIL_ICHAR);

// ----- ZAF_IMAGE_EDITOR ---------------------------------------------------

#if defined(__DECCXX)
ZAF_IMAGE_EDITOR::ZAF_IMAGE_EDITOR() : UIW_WINDOW(0, 0, 0, 0) {}
#endif

ZAF_IMAGE_EDITOR::ZAF_IMAGE_EDITOR(WOAF_FLAGS _woAdvancedFlags,
	WOS_STATUS _woStatus) :
	// base initialization
	UIW_WINDOW(_className, _serviceManager->Storage(&_storage, "p_image"),
	ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY), _objectTable, _userTable),
	// member initialization
	rollerSize(1), pattern(OPT_PATTERN_PENCIL),
	msgTable(_serviceManager->ZMSG_msgTableName(), _serviceManager->Storage(&_storage, "p_image"))
{
	// Set the services.
	searchID = ID_IMAGE_EDITOR;
	woStatus |= _woStatus;
	woAdvancedFlags |= _woAdvancedFlags;
	directoryService = _serviceManager->Get(ID_DIRECTORY_SERVICE);
	storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
	Information(I_COPY_TEXT, originalTitle);

	// Set default options (TEMPORARY setting of NUMBERIDs).
	Get(EDIT_GRID)->woStatus |= WOS_SELECTED;
	Get(ROLLER_1X1)->woStatus |= WOS_SELECTED;
	Get(PATTERN_PENCIL)->woStatus |= WOS_SELECTED;

	// Add the color palette.
#	if defined(ZIL_MACINTOSH)
	UIW_TOOL_BAR *toolBar = new UIW_TOOL_BAR(0, 0, 100, 2, WNF_NO_WRAP,
		WOF_BORDER | WOF_NON_FIELD_REGION);
	*toolBar
		+ new UIW_PROMPT(1, 0, 7, msgTable.GetMessage(ZMSG_MOUSE_CLICK), WOF_JUSTIFY_RIGHT)
		+ (left = new UIW_COLOR_BAR(8, 0, 40, 1, CLF_LEFT_ACTIVATE))
		+ new UIW_PROMPT(1, 1, 7, msgTable.GetMessage(ZMSG_SCREEN), WOF_JUSTIFY_RIGHT)
		+ (screen = new UIW_COLOR_BAR(8, 1, 40, 1, CLF_LEFT_ACTIVATE | CLF_RIGHT_ACTIVATE));
	left->Information(I_GET_COLOR, &leftColor);
	right = ZIL_NULLP(UI_WINDOW_OBJECT);
	screen->Information(I_GET_COLOR, &screenColor);
	relative.bottom--;
	Add(toolBar);
#	else
	if (!display->isText)
	{
		UIW_TOOL_BAR *toolBar = new UIW_TOOL_BAR(0, 0, 100, 3, WNF_NO_WRAP,
			WOF_BORDER | WOF_NON_FIELD_REGION);
		*toolBar
			+ new UIW_PROMPT(1, 0, 7, msgTable.GetMessage(ZMSG_LEFT), WOF_JUSTIFY_RIGHT)
			+ (left = new UIW_COLOR_BAR(8, 0, 40, 1, CLF_LEFT_ACTIVATE))
			+ new UIW_PROMPT(1, 1, 7, msgTable.GetMessage(ZMSG_RIGHT), WOF_JUSTIFY_RIGHT)
			+ (right = new UIW_COLOR_BAR(8, 1, 40, 1, CLF_RIGHT_ACTIVATE))
			+ new UIW_PROMPT(1, 2, 7, msgTable.GetMessage(ZMSG_SCREEN), WOF_JUSTIFY_RIGHT)
			+ (screen = new UIW_COLOR_BAR(8, 2, 40, 1, CLF_LEFT_ACTIVATE | CLF_RIGHT_ACTIVATE));
		left->Information(I_GET_COLOR, &leftColor);
		right->Information(I_GET_COLOR, &rightColor);
		screen->Information(I_GET_COLOR, &screenColor);
		Add(toolBar);
	}
	else
		left = right = screen = ZIL_NULLP(UI_WINDOW_OBJECT);
#	endif
	StatusUpdate();

	// Center the window on the screen.
	windowManager->Center(this);
	relative.bottom = relative.Height() - 1;
	relative.top = 0;

	// Remove the File option if this is a child-service.
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
	{
		UI_WINDOW_OBJECT *menuItem = Get(_serviceManager->ZMSG_fileOption());
		menuItem->woFlags |= WOF_NON_SELECTABLE;
	}
	SetTitle();
}

ZAF_IMAGE_EDITOR::~ZAF_IMAGE_EDITOR(void)
{
	// Remove the resource storage.
	if (_storage)
		delete _storage;
}

EVENT_TYPE ZAF_IMAGE_EDITOR::Event(const UI_EVENT &event)
{
	// Check for user controls.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	if ((ccode >= D_DESIGNER_FIRST && ccode <= D_DESIGNER_LAST) ||
		(ccode <= -D_DESIGNER_FIRST && ccode >= -D_DESIGNER_LAST))
	{
		for (int i = 0; _processTable[i].option; i++)
			if (_processTable[i].option == ccode)
				return ((this->*(_processTable[i].Process))(event));
	}

	// Switch on event type.
	switch (ccode)
	{
	case S_SIZE:
	case S_RESTORE:
		ccode = UIW_WINDOW::Event(event);
		SetTitle();
		break;

	case OPT_HELP:
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(_storage, FALSE);
		break;

	case OPT_ACTIVATE_SERVICE:
		ccode = _serviceManager->Event(event);
		break;

	default:
		if (ccode >= OPT_PATTERN_PENCIL && ccode <= OPT_PATTERN_FILL)
		{
			pattern = ccode;
			StatusUpdate();
		}
		else if (ccode >= OPT_ROLLER_SIZE && ccode <= OPT_ROLLER_SIZE + 9)
			rollerSize = (int)(ccode - OPT_ROLLER_SIZE);
		else if (ccode == OPT_GRID)
		{
			for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
				if (window->SearchID() == ID_IMAGE_WINDOW)
					window->Event(event);
		}
		else if (ccode > OPT_GRID)
		{
			UI_WINDOW_OBJECT *imageWindow = GetImageWindow();
			ccode = imageWindow ? imageWindow->Event(event) : S_ERROR;
		}
		else
			ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return an unknown event.
	return (ccode);
}

void *ZAF_IMAGE_EDITOR::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	switch (request)
	{
	// ----- General service requests -----
	case I_ACTIVATE_SERVICE:
		// Activate the storage service.
		*windowManager + this;
		if (FlagSet(woStatus, WOS_MINIMIZED))
		{
			// Restore the minimized window.
			UI_EVENT restoreEvent(L_RESTORE);
			restoreEvent.windowObject = this;
			Event(restoreEvent);
		}
		break;

	case I_GET_ORIGINAL_TITLE:
		if (!data)
			return (originalTitle);
		*(ZIL_ICHAR **)data = originalTitle;
		break;

	// ----- Image service requests -----
	case I_GET_LEFT_COLOR:
		*(ZIL_COLOR *)data = leftColor;
		break;
	case I_SET_LEFT_COLOR:
		leftColor = *(ZIL_COLOR *)data;
		if (right)
			right->Information(I_GET_COLOR, &rightColor); // clear possible screen color.
		break;

	case I_GET_RIGHT_COLOR:
		*(ZIL_COLOR *)data = rightColor;
		break;
	case I_SET_RIGHT_COLOR:
		rightColor = *(ZIL_COLOR *)data;
		left->Information(I_GET_COLOR, &leftColor); // clear possible screen color.
		break;

	case I_GET_SCREEN_COLOR:
		*(ZIL_COLOR *)data = screenColor;
		break;
	case I_SET_SCREEN_COLOR:
		{
		screenColor = *(ZIL_COLOR *)data;
		// Update all the edit windows.
		for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
			if (window->SearchID() == ID_IMAGE_WINDOW)
				window->Event(OPT_REFRESH_BACKGROUND);
		}
		break;

	case I_GET_PATTERN:
		*(EVENT_TYPE *)data = pattern;
		break;

	case I_GET_ROLLER_SIZE:
		*(int *)data = rollerSize;
		break;

	case I_STATUS_UPDATE:
	case I_STATUS_CLEAR:
		StatusUpdate((UI_WINDOW_OBJECT *)data, (request == I_STATUS_CLEAR));
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	return (data);
}

UI_WINDOW_OBJECT *ZAF_IMAGE_EDITOR::GetImageWindow(void)
{
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_IMAGE_WINDOW)
			return (window);
	return (ZIL_NULLP(UI_WINDOW_OBJECT));
}

void ZAF_IMAGE_EDITOR::SetTitle(void)
{
	static ZIL_ICHAR _format[] = { '%','s',' ','-',' ','%','s',' ','-',' ','%','s',0 };
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService)
		storageService->Information(I_GET_STORAGE, &imageFile);

	ZIL_ICHAR fullTitle[ZIL_MAXPATHLEN];
	if (FlagSet(woAdvancedFlags, WOAF_LOCKED))
		strcpy(fullTitle, originalTitle);
	else if (imageFile)
	{
		ZIL_ICHAR pathName[ZIL_MAXPATHLEN], fileName[ZIL_MAXPATHLEN];
		imageFile->StorageName(pathName);
		imageFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), fileName);
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, fileName);
	}
	else
		sprintf(fullTitle, _format, _serviceManager->ZMSG_mode(), originalTitle, _serviceManager->ZMSG_noFile());
	Information(I_SET_TEXT, fullTitle);
}

void ZAF_IMAGE_EDITOR::StatusUpdate(UI_WINDOW_OBJECT *editObject, int clear)
{
	// Check the buttons and menu items.
	for (int i = 0; _patternTable[i].option; i++)
	{
		UI_WINDOW_OBJECT *button = Get(_patternTable[i].btnField);
		UI_WINDOW_OBJECT *item = Get(_patternTable[i].mnuField);
		if (pattern == _patternTable[i].option &&
			(!FlagSet(button->woStatus, WOS_SELECTED) ||
			 !FlagSet(item->woStatus, WOS_SELECTED)))
		{
			button->woStatus |= WOS_SELECTED;
			button->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_BUTTON);
			item->woStatus |= WOS_SELECTED;
			item->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_BUTTON);
		}
		else if (pattern != _patternTable[i].option &&
			(FlagSet(button->woStatus, WOS_SELECTED) ||
			 FlagSet(item->woStatus, WOS_SELECTED)))
		{
			button->woStatus &= ~WOS_SELECTED;
			button->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_BUTTON);
			item->woStatus &= ~WOS_SELECTED;
			item->Information(I_CHANGED_STATUS, ZIL_NULLP(void), ID_BUTTON);
		}
	}

	// ----- Modify the menu items. -----
	UI_WINDOW_OBJECT *field;
	WOF_FLAGS flag = WOF_NON_SELECTABLE;

	const ZIL_NUMBERID EDIT_ERASE = 0x008C;
	field = Get(EDIT_ERASE);
	if (clear)
		editObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	else if (!editObject)
		editObject = GetImageWindow();
	if ((editObject && FlagSet(field->woFlags, flag)) || (!editObject && !FlagSet(field->woFlags, flag)))
	{
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID EDIT_CUT = 0x006E;
		field = Get(EDIT_CUT);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID EDIT_COPY = 0x006F;
		field = Get(EDIT_COPY);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID EDIT_DELETE = 0x0046;
		field = Get(EDIT_DELETE);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID EDIT_GROUP_DEFINE = 0x0073;
		field = Get(EDIT_GROUP_DEFINE);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID EDIT_GROUP_UNDEFINE = 0x0074;
		field = Get(EDIT_GROUP_UNDEFINE);
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);
	}

	// Check the paste option.
	const ZIL_NUMBERID EDIT_PASTE = 0x0070;
	field = Get(EDIT_PASTE);
	if ((ZAF_IMAGE::pasteArray && editObject && FlagSet(field->woFlags, flag)) ||
		((!ZAF_IMAGE::pasteArray || !editObject) && !FlagSet(field->woFlags, flag)))
	{
		field->Information(editObject ? I_CLEAR_FLAGS : I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);
	}

	// Get the image type.
	IMF_FLAGS imFlags = IMF_NO_FLAGS;
	if (editObject)
		editObject->Information(I_GET_FLAGS, &imFlags);

	const ZIL_NUMBERID FLD_BMP_STORE = 0x004D;
	field = Get(FLD_BMP_STORE);
	if ((imFlags == IMF_BITMAP_IMAGE && FlagSet(field->woFlags, flag)) || (imFlags != IMF_BITMAP_IMAGE && !FlagSet(field->woFlags, flag)))
	{
		INFO_REQUEST request = (imFlags == IMF_BITMAP_IMAGE) ? I_CLEAR_FLAGS : I_SET_FLAGS;
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_BMP_STOREAS = 0x004E;
		field = Get(FLD_BMP_STOREAS);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_BMP_CLEAR = 0x0068;
		field = Get(FLD_BMP_CLEAR);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_BMP_CLEARALL = 0x0069;
		field = Get(FLD_BMP_CLEARALL);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);
	}

	const ZIL_NUMBERID FLD_ICO_STORE = 0x0054;
	field = Get(FLD_ICO_STORE);
	if ((imFlags == IMF_ICON_IMAGE && FlagSet(field->woFlags, flag)) || (imFlags != IMF_ICON_IMAGE && !FlagSet(field->woFlags, flag)))
	{
		INFO_REQUEST request = (imFlags == IMF_ICON_IMAGE) ? I_CLEAR_FLAGS : I_SET_FLAGS;
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_ICO_STOREAS = 0x0055;
		field = Get(FLD_ICO_STOREAS);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_ICO_CLEAR = 0x006A;
		field = Get(FLD_ICO_CLEAR);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_ICO_CLEARALL = 0x006B;
		field = Get(FLD_ICO_CLEARALL);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);
	}

	const ZIL_NUMBERID FLD_MOU_STORE = 0x0061;
	field = Get(FLD_MOU_STORE);
	if ((imFlags == IMF_MOUSE_IMAGE && FlagSet(field->woFlags, flag)) || (imFlags != IMF_MOUSE_IMAGE && !FlagSet(field->woFlags, flag)))
	{
		INFO_REQUEST request = (imFlags == IMF_MOUSE_IMAGE) ? I_CLEAR_FLAGS : I_SET_FLAGS;
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_MOU_STOREAS = 0x0062;
		field = Get(FLD_MOU_STOREAS);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_MOU_CLEAR = 0x006C;
		field = Get(FLD_MOU_CLEAR);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_MOU_CLEARALL = 0x006D;
		field = Get(FLD_MOU_CLEARALL);
		field->Information(request, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);
	}

	// Check for text mode.
	static int initialized = FALSE;
	if (!initialized && display->isText)
	{
		const ZIL_NUMBERID FLD_EDIT = 0x0003;
		field = Get(FLD_EDIT);
		field->Information(I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_PULL_DOWN_ITEM);

		const ZIL_NUMBERID FLD_BMP_CREATE = 0x004A;
		field = Get(FLD_BMP_CREATE);
		field->Information(I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_BMP_LOAD = 0x004B;
		field = Get(FLD_BMP_LOAD);
		field->Information(I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_ICO_CREATE = 0x0066;
		field = Get(FLD_ICO_CREATE);
		field->Information(I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_ICO_LOAD = 0x0053;
		field = Get(FLD_ICO_LOAD);
		field->Information(I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_MOU_CREATE = 0x0067;
		field = Get(FLD_MOU_CREATE);
		field->Information(I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);

		const ZIL_NUMBERID FLD_MOU_LOAD = 0x0060;
		field = Get(FLD_MOU_LOAD);
		field->Information(I_SET_FLAGS, &flag, ID_WINDOW_OBJECT);
		field->Information(I_CHANGED_FLAGS, ZIL_NULLP(void), ID_POP_UP_ITEM);
	}
	initialized = TRUE;
}

// ----- Edit Options -------------------------------------------------------

EVENT_TYPE ZAF_IMAGE_EDITOR::EditRequest(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *imageWindow = GetImageWindow();
	if (!imageWindow)
		return (storageService->Event(S_ERROR));
	return (imageWindow->Event(event));
}

// ----- File Options -------------------------------------------------------

EVENT_TYPE ZAF_IMAGE_EDITOR::FileClose(const UI_EVENT &event)
{
	// Make sure there is an image file.
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &imageFile))
		return (directoryService->Event(S_ERROR));

	// Close the image file.
	delete imageFile;
	storageService->Information(I_SET_STORAGE, ZIL_NULLP(ZIL_STORAGE));
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::FileDelete(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Delete the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	if (unlink(pathName))
		return (directoryService->Event(S_ERROR));
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::FileNew(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Try to create the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_STORAGE *newFile = new ZIL_STORAGE(pathName, UIS_CREATE | UIS_READWRITE);
	if (newFile->storageError)
	{
		delete newFile;
		return (directoryService->Event(S_ERROR));
	}

	// See if there is a pre-existing image file.
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &imageFile))
		delete imageFile;
	imageFile = newFile;
	storageService->Information(I_SET_STORAGE, imageFile);
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::FileOpen(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Try to open the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZIL_STORAGE *openFile = new ZIL_STORAGE(pathName, UIS_READWRITE);
	if (openFile->storageError)
	{
		delete openFile;
		return (directoryService->Event(S_ERROR));
	}

	// See if there is a pre-existing image file.
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &imageFile))
		delete imageFile;
	imageFile = openFile;
	storageService->Information(I_SET_STORAGE, imageFile);
	SetTitle();
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::FileRequest(const UI_EVENT &event)
{
	// Make sure there is a directory service.
	if (!directoryService)
		return (S_UNKNOWN);

	// Make a request to the directory service.
	directoryService->Information(I_SET_FILTER, _serviceManager->ZMSG_datFilter());
	directoryService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	directoryService->Information(I_SET_REQUEST, &ccode);
	directoryService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::FileSave(const UI_EVENT &event)
{
	// Make sure there is a image file.
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &imageFile))
		return (directoryService->Event(S_ERROR));

	// Make sure the image file has a valid name.
	ZIL_ICHAR name[ZIL_MAXPATHLEN], pathName[ZIL_MAXPATHLEN];
	imageFile->StorageName(pathName);
	imageFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), name);
	if (!stricmp(name, _serviceManager->ZMSG_tempFileName()))
		return (Event(OPT_FILE_SAVEAS));

	// Save the current image file.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_IMAGE_WINDOW)
			window->Store(ZIL_NULLP(ZIL_ICHAR), imageFile);
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	imageFile->Save(revisions);
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::FileSaveAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data && event.type == -OPT_FILE_SAVEAS)
		return (event.type);

	// See if there is a pre-existing image file.
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &imageFile))
		return (directoryService->Event(S_ERROR));
	else if (event.type == OPT_FILE_SAVEAS)
		return (FileRequest(event));

	// Save the specified file.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
		if (window->SearchID() == ID_IMAGE_WINDOW)
			window->Store(ZIL_NULLP(ZIL_ICHAR), imageFile);
	int revisions = 0;
	storageService->Information(I_GET_REVISIONS, &revisions);
	if (imageFile->SaveAs(pathName, revisions))
	{
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
		return (directoryService->Event(S_ERROR));
	}
	SetTitle();
	ZAF_SERVICE_MANAGER::_changedData = FALSE;
	if (ZAF_SERVICE_MANAGER::_queuedEvent)
		eventManager->Put(ZAF_SERVICE_MANAGER::_queuedEvent);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Image Options ------------------------------------------------------

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageClear(const UI_EVENT &event)
{
	// Check for an image window.
	UI_WINDOW_OBJECT *imageWindow = GetImageWindow();
	if (!imageWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of existing window.
	UI_EVENT cEvent(OPT_RESOURCE_CLEAR);
	cEvent.data = imageWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the first edit window.
	{
		ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &imageFile);
		imageWindow->Store(imageWindow->StringID(), imageFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear the first image window.
	*windowManager - imageWindow;
	delete imageWindow;
	StatusUpdate(GetImageWindow(), TRUE);
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageClearAll(const UI_EVENT &event)
{
	// Check for a message window.
	UI_WINDOW_OBJECT *imageWindow = GetImageWindow();
	if (!imageWindow)
		return (storageService->Event(S_ERROR));

	// Confirm writing of edit windows.
	UI_EVENT cEvent(OPT_RESOURCE_CLEARALL);
	cEvent.data = imageWindow->StringID();
	cEvent.type = storageService->Event(cEvent);
	if (cEvent.type == ZIL_DLG_CANCEL) // cancel the clear-all operation.
		return (S_ERROR);
	else if (cEvent.type == ZIL_DLG_YES) // save the visible edit windows.
	{
		ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		storageService->Information(I_GET_STORAGE, &imageFile);
		for (UI_WINDOW_OBJECT *window = windowManager->First(); window; window = window->Next())
			if (window->SearchID() == ID_IMAGE_WINDOW)
				window->Store(window->StringID(), imageFile, ZIL_NULLP(ZIL_STORAGE_OBJECT));
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}

	// Clear all the image windows.
	for (UI_WINDOW_OBJECT *window = windowManager->First(); window; )
		if (window->SearchID() == ID_IMAGE_WINDOW)
		{
			UI_WINDOW_OBJECT *imageWindow = window->Next();
			*windowManager - window;
			delete window;
			window = imageWindow;
		}
		else
			window = window->Next();
	StatusUpdate(GetImageWindow(), TRUE);
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::BitmapCreate(const UI_EVENT &event)
{
	IMF_FLAGS imFlags = IMF_BITMAP_IMAGE;
	if (FlagSet(Get(EDIT_GRID)->woStatus, WOS_SELECTED))
		imFlags |= IMF_GRID;

	*windowManager + new ZAF_IMAGE_WINDOW(ZIL_NULLP(ZIL_ICHAR), imFlags);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::IconCreate(const UI_EVENT &event)
{
	IMF_FLAGS imFlags = IMF_ICON_IMAGE;
	if (FlagSet(Get(EDIT_GRID)->woStatus, WOS_SELECTED))
		imFlags |= IMF_GRID;

	*windowManager + new ZAF_IMAGE_WINDOW(ZIL_NULLP(ZIL_ICHAR), imFlags);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::MouseCreate(const UI_EVENT &event)
{
	IMF_FLAGS imFlags = IMF_MOUSE_IMAGE;
	if (FlagSet(Get(EDIT_GRID)->woStatus, WOS_SELECTED))
		imFlags |= IMF_GRID;

	*windowManager + new ZAF_IMAGE_WINDOW(ZIL_NULLP(ZIL_ICHAR), imFlags);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageDelete(const UI_EVENT &event)
{
	// Delete the specified image.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &imageFile))
		return (directoryService->Event(S_ERROR));

	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	imageFile->ChDir(_imageDirectory);	// remove the image.
	if (imageFile->DestroyObject(pathName))
		return (storageService->Event(S_ERROR));
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageLoad(const UI_EVENT &event)
{
	// Make sure there is an image file.
	if (!event.data) // end to service response has no data.
		return (event.type);
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &imageFile))
		return (directoryService->Event(S_ERROR));

	static ZAF_IMAGE_WINDOW *imageWindow = ZIL_NULLP(ZAF_IMAGE_WINDOW);
	if (!imageWindow)
	{
		IMF_FLAGS imFlags;
		if (_imageDirectory == _iconDirectory)
			imFlags = IMF_ICON_IMAGE;
		else if (_imageDirectory == _bitmapDirectory)
			imFlags = IMF_BITMAP_IMAGE;
		else
			imFlags = IMF_MOUSE_IMAGE;
		if (FlagSet(Get(EDIT_GRID)->woStatus, WOS_SELECTED))
			imFlags |= IMF_GRID;

		eventManager->DeviceState(E_MOUSE, DM_WAIT);
		ZIL_ICHAR *name = (ZIL_ICHAR *)event.data;
		imageWindow = new ZAF_IMAGE_WINDOW(name, imFlags);
		eventManager->Put(event); // Wait till provider window is removed from screen.
		eventManager->DeviceState(E_MOUSE, DM_VIEW);
	}
	else
	{
		*windowManager + imageWindow;
		ZAF_SERVICE_MANAGER::_changedData = TRUE;
		imageWindow = ZIL_NULLP(ZAF_IMAGE_WINDOW);
	}
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageRequest(const UI_EVENT &event)
{
	// Make sure there is an image file.
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &imageFile))
		return (directoryService->Event(S_ERROR));

	// Make a request to the resource service.
	ZIL_ICHAR *name = event.windowObject ? event.windowObject->StringID() : &_imageDirectory[1];
	if (!strcmp(&_iconDirectory[1], name))
		_imageDirectory = _iconDirectory;
	else if (!strcmp(&_bitmapDirectory[1], name))
		_imageDirectory = _bitmapDirectory;
	else
		_imageDirectory = _mouseDirectory;
	if (imageFile->ChDir(_imageDirectory))
	{
		imageFile->MkDir(_imageDirectory);
		imageFile->ChDir(_imageDirectory);
	}
	int traverseLock = TRUE;
	storageService->Information(I_SET_TRAVERSE_LOCK, &traverseLock);
	storageService->Information(I_SET_REQUESTOR, this);
	EVENT_TYPE ccode = event.type; // Set up the response.
	storageService->Information(I_SET_REQUEST, &ccode);
	storageService->Information(I_ACTIVATE_SERVICE, &ccode);
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageStore(const UI_EVENT &)
{
	// Make sure there is an image window.
	UI_WINDOW_OBJECT *imageWindow = GetImageWindow();
	if (!imageWindow)
		return (storageService->Event(S_ERROR));

	// Store the image.
	UI_EVENT storeEvent(OPT_RESOURCE_STOREAS);
	storeEvent.data = imageWindow->StringID();
	return (ImageStoreAs(storeEvent));
}

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageStoreAs(const UI_EVENT &event)
{
	// End to service response has no data.
	if (!event.data)
		return (event.type);

	// Make sure there is a message file.
	ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
	if (!storageService->Information(I_GET_STORAGE, &imageFile))
		return (directoryService->Event(S_ERROR));

	// Make sure there is an image window.
	UI_WINDOW_OBJECT *imageWindow = GetImageWindow();
	if (!imageWindow)
		return (storageService->Event(S_ERROR));

	// Store the image.
	eventManager->DeviceState(E_MOUSE, DM_WAIT);
	imageWindow->Store((ZIL_ICHAR *)event.data, imageFile);
	ZAF_SERVICE_MANAGER::_changedData = TRUE;
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	return (event.type);
}

// ----- Static tables ------------------------------------------------------

ZIL_ICHAR ZAF_IMAGE_EDITOR::_className[] = { 'Z','A','F','_','I','M','A','G','E','_','E','D','I','T','O','R',0 };

ZIL_STORAGE_READ_ONLY *ZAF_IMAGE_EDITOR::_storage = ZIL_NULLP(ZIL_STORAGE_READ_ONLY);
ZIL_ICHAR ZAF_IMAGE_EDITOR::_zncExtension[] = { '.','z','n','c',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_xpmExtension[] = { '.','x','p','m',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_bmpExtension[] = { '.','b','m','p',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_obmpExtension[] = { '.','o','b','m','p',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_wbmpExtension[] = { '.','w','b','m','p',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_icoExtension[] = { '.','i','c','o',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_oicoExtension[] = { '.','o','i','c','o',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_wicoExtension[] = { '.','w','i','c','o',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_curExtension[] = { '.','c','u','r',0 };
ZIL_ICHAR ZAF_IMAGE_EDITOR::_ptrExtension[] = { '.','p','t','r',0 };

static UI_ITEM ZIL_FARDATA __userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_IMAGE_EDITOR::_userTable = __userTable;

static UI_ITEM ZIL_FARDATA __objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
	{ ID_ICON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_ICON::New), UIW_ICON::_className, 0 },
	{ ID_INTEGER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_INTEGER::New), UIW_INTEGER::_className, 0 },
	{ ID_MAXIMIZE_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MAXIMIZE_BUTTON::New), UIW_MAXIMIZE_BUTTON::_className, 0 },
	{ ID_MINIMIZE_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_MINIMIZE_BUTTON::New), UIW_MINIMIZE_BUTTON::_className, 0 },
	{ ID_POP_UP_ITEM, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_POP_UP_ITEM::New), UIW_POP_UP_ITEM::_className, 0 },
	{ ID_POP_UP_MENU, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_POP_UP_MENU::New), UIW_POP_UP_MENU::_className, 0 },
	{ ID_PROMPT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PROMPT::New), UIW_PROMPT::_className, 0 },
	{ ID_PULL_DOWN_ITEM, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PULL_DOWN_ITEM::New), UIW_PULL_DOWN_ITEM::_className, 0 },
	{ ID_PULL_DOWN_MENU, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PULL_DOWN_MENU::New), UIW_PULL_DOWN_MENU::_className, 0 },
	{ ID_STRING, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_TOOL_BAR, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TOOL_BAR::New), UIW_TOOL_BAR::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *ZAF_IMAGE_EDITOR::_objectTable = __objectTable;

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux__) || (_MSC_VER > 1500)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_IMAGE_EDITOR::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

ZAF_IMAGE_EDITOR::PROCESS ZAF_IMAGE_EDITOR::_processTable[] =
{
	{ OPT_FILE_CLOSE,				ZIL_PROCESS_REFERENCE(FileClose) },		// File requests.
	{ OPT_FILE_DELETE, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_NEW, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_OPEN, 				ZIL_PROCESS_REFERENCE(FileRequest) },
	{ OPT_FILE_SAVE, 				ZIL_PROCESS_REFERENCE(FileSave) },
	{ OPT_FILE_SAVEAS,				ZIL_PROCESS_REFERENCE(FileSaveAs) },
	{ -OPT_FILE_DELETE, 			ZIL_PROCESS_REFERENCE(FileDelete) },		// File responses.
	{ -OPT_FILE_NEW, 				ZIL_PROCESS_REFERENCE(FileNew) },
	{ -OPT_FILE_OPEN, 				ZIL_PROCESS_REFERENCE(FileOpen) },
	{ -OPT_FILE_SAVEAS,				ZIL_PROCESS_REFERENCE(FileSaveAs) },

	{ OPT_RESOURCE_CLEAR,			ZIL_PROCESS_REFERENCE(ImageClear) },		// Image requests.
	{ OPT_RESOURCE_CLEARALL,		ZIL_PROCESS_REFERENCE(ImageClearAll) },
	{ OPT_BITMAP_CREATE,			ZIL_PROCESS_REFERENCE(BitmapCreate) },
	{ OPT_ICON_CREATE,				ZIL_PROCESS_REFERENCE(IconCreate) },
	{ OPT_MOUSE_CREATE,				ZIL_PROCESS_REFERENCE(MouseCreate) },
	{ OPT_RESOURCE_DELETE,			ZIL_PROCESS_REFERENCE(ImageRequest) },
	{ OPT_RESOURCE_LOAD,			ZIL_PROCESS_REFERENCE(ImageRequest) },
	{ OPT_RESOURCE_STORE,			ZIL_PROCESS_REFERENCE(ImageStore) },
	{ OPT_RESOURCE_STOREAS,			ZIL_PROCESS_REFERENCE(ImageRequest) },
	{ -OPT_RESOURCE_DELETE,			ZIL_PROCESS_REFERENCE(ImageDelete) },		// Image responses.
	{ -OPT_RESOURCE_LOAD,			ZIL_PROCESS_REFERENCE(ImageLoad) },
	{ -OPT_RESOURCE_STOREAS,		ZIL_PROCESS_REFERENCE(ImageStoreAs) },

	{ OPT_EDIT_COPY, 				ZIL_PROCESS_REFERENCE(EditRequest) },		// edit requests
	{ OPT_EDIT_CUT, 				ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_DELETE, 				ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_GROUP_DEFINE, 		ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_GROUP_UNDEFINE, 		ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_MOVE, 				ZIL_PROCESS_REFERENCE(EditRequest) },
	{ OPT_EDIT_PASTE, 				ZIL_PROCESS_REFERENCE(EditRequest) },

	{ OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(ImageExport) },
	{ -OPT_FILE_EXPORT,				ZIL_PROCESS_REFERENCE(ImageExport) },
	{ OPT_RESOURCE_EXPORT,			ZIL_PROCESS_REFERENCE(ImageExport) },
	{ -OPT_RESOURCE_EXPORT,			ZIL_PROCESS_REFERENCE(ImageExport) },
	{ OPT_FILE_IMPORT,				ZIL_PROCESS_REFERENCE(ImageImport) },
	{ -OPT_FILE_IMPORT,				ZIL_PROCESS_REFERENCE(ImageImport) },
	{ OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(ImageImport) },
	{ -OPT_RESOURCE_IMPORT,			ZIL_PROCESS_REFERENCE(ImageImport) },

	{ 0, 0 }
};
