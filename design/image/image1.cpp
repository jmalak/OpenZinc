//	Zinc Interface Library - IMAGE1.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#define USE_ZAF_IMAGE_WINDOW
#define USE_UI_MOUSE_WINDOW
#include "p_image.hpp"

extern ZIL_ICHAR *_bitmapDirectory;
extern ZIL_ICHAR *_iconDirectory;
extern ZIL_ICHAR *_mouseDirectory;
static ZIL_ICHAR _iconWindow[] = { 'U','I','_','I','C','O','N','_','W','I','N','D','O','W',0 };
static ZIL_ICHAR _bitmapWindow[] = { 'U','I','_','B','I','T','M','A','P','_','W','I','N','D','O','W',0 };
static ZIL_ICHAR _mouseWindow[] = { 'U','I','_','M','O','U','S','E','_','W','I','N','D','O','W',0 };

// ----- ZAF_IMAGE_WINDOW --------------------------------------------------

ZAF_IMAGE_WINDOW::ZAF_IMAGE_WINDOW(ZIL_ICHAR *name, IMF_FLAGS _imFlags) :
	UIW_WINDOW(FlagSet(_imFlags, IMF_BITMAP_IMAGE) ? _bitmapWindow : (FlagSet(_imFlags, IMF_ICON_IMAGE) ? _iconWindow : _mouseWindow),
	ZAF_IMAGE_EDITOR::_storage, ZIL_NULLP(ZIL_STORAGE_OBJECT),
	ZAF_IMAGE_EDITOR::_objectTable, ZAF_IMAGE_EDITOR::_userTable),
	imFlags(_imFlags)
{
	// Set the help context title.
	searchID = ID_IMAGE_WINDOW;
	if (!name)
	{
		StringID(_serviceManager->ZMSG_tempResourceName());
		Information(I_SET_TEXT, StringID());
	}
	else
		Information(I_SET_TEXT, StringID(name));

	// Center the window on the screen.
	windowManager->Center(this);

	// Create the default bitmap/icon image.
	int bitmapWidth, bitmapHeight, hotspotX, hotspotY;
	if (FlagSet(imFlags, IMF_ICON_IMAGE))
		bitmapWidth = bitmapHeight = 32;
	else
		bitmapWidth = bitmapHeight = 16;
	ZIL_UINT8 *bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	memset(bitmapArray, (char)WHITE, bitmapWidth * bitmapHeight);

	// compute the optimal edit-image position.
	editImage = new ZAF_IMAGE(2, 0, 40, 10, FlagSet(imFlags, IMF_GRID) | IMF_MAXFIT, WOF_BORDER);
	editImage->DataSet(bitmapArray, bitmapWidth, bitmapHeight);
	editImage->RegionConvert(editImage->relative, FALSE);
	Add(editImage);
	// compute the optimal actual-image position.
	actualImage = new ZAF_IMAGE(52, 0, 20, 11, IMF_STATIC_IMAGE, WOF_BORDER | WOF_JUSTIFY_CENTER | WOF_NON_SELECTABLE);
	actualImage->DataSet(bitmapArray, bitmapWidth, bitmapHeight);
	actualImage->RegionConvert(actualImage->relative, FALSE);
	actualImage->relative.top = editImage->relative.top;
	actualImage->relative.bottom = actualImage->relative.top + actualImage->relative.Width() - 1;
	Add(actualImage);
	// Load the image.
	if (name)
	{
		UI_WINDOW_OBJECT *storageService = _serviceManager->Get(ID_STORAGE_SERVICE);
		ZIL_STORAGE *imageFile = ZIL_NULLP(ZIL_STORAGE);
		if (storageService->Information(I_GET_STORAGE, &imageFile))
			Load(name, imageFile);
	}

	// Set the field information.
	editImage->DataGet(&bitmapArray, &bitmapWidth, &bitmapHeight, &hotspotX, &hotspotY);
	Get(FIELD_WIDTH)->Information(I_SET_VALUE, &bitmapWidth);
	Get(FIELD_HEIGHT)->Information(I_SET_VALUE, &bitmapHeight);
	if (FlagSet(imFlags, IMF_MOUSE_IMAGE))
	{
		Get(FIELD_HOT_COLUMN)->Information(I_SET_VALUE, &hotspotX);
		Get(FIELD_HOT_LINE)->Information(I_SET_VALUE, &hotspotY);
	}
}

ZAF_IMAGE_WINDOW::~ZAF_IMAGE_WINDOW(void)
{
}

EVENT_TYPE ZAF_IMAGE_WINDOW::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	switch (ccode)
	{
	case OPT_STORE:
		// Let the image editor handle the store request.
		if (FlagSet(imFlags, IMF_BITMAP_IMAGE))
			ZAF_IMAGE_EDITOR::_imageDirectory = _bitmapDirectory;
		else if (FlagSet(imFlags, IMF_ICON_IMAGE))
			ZAF_IMAGE_EDITOR::_imageDirectory = _iconDirectory;
		else
			ZAF_IMAGE_EDITOR::_imageDirectory = _mouseDirectory;
		_serviceManager->Get(ID_IMAGE_EDITOR)->Event(OPT_RESOURCE_STORE);
		break;

	case OPT_STORE_AS:
		// Let the image editor handle the store-as request.
		if (FlagSet(imFlags, IMF_BITMAP_IMAGE))
			ZAF_IMAGE_EDITOR::_imageDirectory = _bitmapDirectory;
		else if (FlagSet(imFlags, IMF_ICON_IMAGE))
			ZAF_IMAGE_EDITOR::_imageDirectory = _iconDirectory;
		else
			ZAF_IMAGE_EDITOR::_imageDirectory = _mouseDirectory;
		_serviceManager->Get(ID_IMAGE_EDITOR)->Event(OPT_RESOURCE_STOREAS);
		break;

	case OPT_CLOSE:
		// Close the window.
		eventManager->Put(S_CLOSE);
		break;

	case OPT_HELP:
		// Call the help system.
		helpSystem->DisplayHelp(windowManager, event.windowObject->helpContext);
		break;

	case OPT_ERASE:
		{
		ZIL_UINT8 *bitmapArray;
		int bitmapWidth, bitmapHeight;
		// Get the image and clear the array.
		editImage->DataGet(&bitmapArray, &bitmapWidth, &bitmapHeight);
		memset(bitmapArray, (char)WHITE, bitmapWidth * bitmapHeight);
		editImage->DataSet(bitmapArray, bitmapWidth, bitmapHeight);
		// reset the actual image (uses the same bitmap).
		actualImage->DataSet(ZIL_NULLP(ZIL_UINT8), -1, -1);
		}
		break;

	case OPT_CHECK_VARIABLES:
		CheckVariables();
		break;

	case OPT_REFRESH_BACKGROUND:
		{
		UI_EVENT event(S_REDISPLAY);
		event.region = editImage->trueRegion;
		editImage->Event(event);
		event.region = actualImage->trueRegion;
		actualImage->Event(event);
		}
		break;

	case OPT_GRID:
		{
		if (FlagSet(imFlags, IMF_GRID))
		{
			editImage->imFlags &= ~IMF_GRID;
			imFlags &= ~IMF_GRID;
		}
		else
		{
			editImage->imFlags |= IMF_GRID;
			imFlags |= IMF_GRID;
		}
		UI_EVENT event(S_REDISPLAY);
		event.region = editImage->trueRegion;
		editImage->Event(event);
		}
		break;
		
#	if defined(ZIL_NEXTSTEP)
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
		editImage->Event(event);
#	endif

	case S_DEINITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		if (!ZAF_SERVICE_MANAGER::_queuedEvent)
			_serviceManager->Get(ID_IMAGE_EDITOR)->Information(I_STATUS_CLEAR, ZIL_NULLP(void));
		break;

	case S_CURRENT:
		ccode = UIW_WINDOW::Event(event);
		helpSystem->ResetStorage(ZAF_IMAGE_EDITOR::_storage, FALSE);
		_serviceManager->Get(ID_IMAGE_EDITOR)->Information(I_STATUS_UPDATE, this);
		break;

	default:
		if (ccode >= OPT_EDIT_OBJECT && ccode <= OPT_EDIT_GROUP_UNDEFINE)
			editImage->Event(event);
		else
			ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

void *ZAF_IMAGE_WINDOW::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the image flag settings.
		if (request == I_GET_FLAGS && !data)
			data = &imFlags;
		else if (request == I_GET_FLAGS)
			*(IMF_FLAGS *)data = imFlags;
		else if (request == I_SET_FLAGS)
			imFlags |= *(IMF_FLAGS *)data;
		else
			imFlags &= ~(*(IMF_FLAGS *)data);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void ZAF_IMAGE_WINDOW::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *, UI_ITEM *)
{
	// Move to the appropriate directory.
	if (FlagSet(imFlags, IMF_BITMAP_IMAGE))
		directory->ChDir(_bitmapDirectory);
	else if (FlagSet(imFlags, IMF_ICON_IMAGE))
		directory->ChDir(_iconDirectory);
	else
		directory->ChDir(_mouseDirectory);

	// Create the file object.
	ZIL_ICHAR *text;
	if (name)
		file = new ZIL_STORAGE_OBJECT_READ_ONLY(*directory, name, 0);
	else
	{
		Information(I_GET_TEXT, &text);
		file = new ZIL_STORAGE_OBJECT_READ_ONLY(*directory, text, 0);
	}
	// Load the specified bitmap/icon image.
	int bitmapWidth, bitmapHeight, hotspotX, hotspotY;
	ZIL_UINT8 *bitmapArray;
	ZIL_INT16 _value;
	file->Load(&_value); bitmapWidth = _value;
	file->Load(&_value); bitmapHeight = _value;
	if (FlagSet(imFlags, IMF_MOUSE_IMAGE))
	{
		file->Load(&_value); hotspotX = _value;
		file->Load(&_value); hotspotY = _value;
	}
	bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	file->Load(bitmapArray, bitmapWidth * bitmapHeight, sizeof(ZIL_UINT8));
	editImage->DataSet(bitmapArray, bitmapWidth, bitmapHeight, hotspotX, hotspotY);
	actualImage->DataSet(bitmapArray, bitmapWidth, bitmapHeight);
	// Cleanup.
	delete file;
}

void ZAF_IMAGE_WINDOW::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *, UI_ITEM *)
{
	// Check for an invalid file name.
	if (name)
		Information(I_SET_TEXT, StringID(name));
	else
		name = StringID();

	// Move to the appropriate directory.
	if (FlagSet(imFlags, IMF_BITMAP_IMAGE))
		ZAF_IMAGE_EDITOR::_imageDirectory = _bitmapDirectory;
	else if (FlagSet(imFlags, IMF_ICON_IMAGE))
		ZAF_IMAGE_EDITOR::_imageDirectory = _iconDirectory;
	else
		ZAF_IMAGE_EDITOR::_imageDirectory = _mouseDirectory;
	if (directory->ChDir(ZAF_IMAGE_EDITOR::_imageDirectory))
	{
		directory->MkDir(ZAF_IMAGE_EDITOR::_imageDirectory);
		directory->ChDir(ZAF_IMAGE_EDITOR::_imageDirectory);
	}

	// Create the file object.
	file = new ZIL_STORAGE_OBJECT(*directory, name, 0, UIS_CREATE | UIS_READWRITE);
	// Store the specified bitmap/icon image.
	ZIL_INT16 _value;
	int bitmapWidth, bitmapHeight, hotspotX, hotspotY;
	ZIL_UINT8 *bitmapArray;
	editImage->DataGet(&bitmapArray, &bitmapWidth, &bitmapHeight, &hotspotX, &hotspotY);
	_value = bitmapWidth; file->Store(_value);
	_value = bitmapHeight; file->Store(_value);
	if (FlagSet(imFlags, IMF_MOUSE_IMAGE))
	{
		_value = hotspotX; file->Store(_value);
		_value = hotspotY; file->Store(_value);
	}
	file->Store(bitmapArray, bitmapWidth * bitmapHeight, sizeof(ZIL_UINT8));
	// Cleanup.
	delete file;
}

const int maxBitmapSize = 128;
const int minBitmapSize = 0;

void ZAF_IMAGE_WINDOW::CheckVariables(void)
{
	// Get the current information.
	ZIL_UINT8 *oldArray;
	int oldWidth, oldHeight;
	editImage->DataGet(&oldArray, &oldWidth, &oldHeight);

	// Get the new information.
	ZIL_UINT8 *newArray;
	int newWidth, newHeight;
	Get(FIELD_WIDTH)->Information(I_GET_VALUE, &newWidth);
	Get(FIELD_HEIGHT)->Information(I_GET_VALUE, &newHeight);

	// Reset the image information.
	if (oldWidth != newWidth || oldHeight != newHeight)
	{
		if (newWidth > maxBitmapSize)
		{
			newWidth = maxBitmapSize;
			Get(FIELD_WIDTH)->Information(I_SET_VALUE, &newWidth);
		}
		else if (newWidth < minBitmapSize)
		{
			newWidth = minBitmapSize; 
			Get(FIELD_WIDTH)->Information(I_SET_VALUE, &newWidth);
		}
		if (newHeight > maxBitmapSize)
		{
			newHeight = maxBitmapSize;
			Get(FIELD_HEIGHT)->Information(I_SET_VALUE, &newHeight);
		}
		else if (newHeight < minBitmapSize)
		{
			newHeight = minBitmapSize;
			Get(FIELD_HEIGHT)->Information(I_SET_VALUE, &newHeight);
		}
		editImage->DataSet(ZIL_NULLP(ZIL_UINT8), newWidth, newHeight);
		editImage->DataGet(&newArray, &newWidth, &newHeight);
		actualImage->DataSet(newArray, newWidth, newHeight);
	}
}
