//	Zinc Interface Library - IMAGE.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#define USE_MSG_TABLE
#include "p_image.hpp"

extern ZIL_ICHAR *_bitmapDirectory;
extern ZIL_ICHAR *_iconDirectory;
extern ZIL_ICHAR *_mouseDirectory;

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageImport(const UI_EVENT &event)
{
	static ZIL_STORAGE *imageFile;
	static ZIL_STORAGE *importFile;
	static UI_WINDOW_OBJECT *iObject;

	// Level 1-Select the import file.
	if (event.type == OPT_FILE_IMPORT)
	{
		iObject = event.windowObject;
		// Filter is contained in the stringID.
		if (!storageService->Information(I_GET_STORAGE, &imageFile))
			return (directoryService->Event(S_ERROR));
		// Set up the image directory.
		ZIL_ICHAR *name = iObject->StringID();
		if (!strcmp(&_iconDirectory[1], name))
			_imageDirectory = _iconDirectory;
		else if (!strcmp(&_bitmapDirectory[1], name))
			_imageDirectory = _bitmapDirectory;
		else
			_imageDirectory = _mouseDirectory;
		return (FileRequest(event));
	}

	// Level 2-Open the import file.
	else if (event.type == -OPT_FILE_IMPORT && !event.data)
		return (event.type); // end to service response has no data.
	else if (event.type == -OPT_FILE_IMPORT)
	{
		// Try to import a filter file.
		if (OSImport(imageFile, event) != S_ERROR)
			return (event.type);

		// Import from a zinc file.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		importFile = new ZIL_STORAGE(pathName, UIS_READWRITE);
		if (importFile->storageError)
		{
			delete importFile;
			return (directoryService->Event(S_ERROR));
		}
		storageService->Information(I_SET_STORAGE, importFile);
		eventManager->Put(OPT_RESOURCE_IMPORT); // Wait till provider window is removed from screen.

		// Move to the proper directory.
		extern ZIL_ICHAR *_rootStorageDirectory;
		imageFile->ChDir(_rootStorageDirectory);
		imageFile->ChDir(_imageDirectory);
	}

	// Level 3-Select the import resources.
	else if (event.type == OPT_RESOURCE_IMPORT)
	{
		UI_EVENT iEvent = event;
		iEvent.windowObject = iObject;
		return (ImageRequest(iEvent));
	}

	// Level 4-Save the import resources.
	else if (event.type == -OPT_RESOURCE_IMPORT && event.data)
	{
		// Confirm the import of duplicate objects.
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		if (imageFile->FindFirstObject(pathName) &&
			storageService->Event(event) != event.type)
			return (S_ERROR);

		// Import the resource.
		IMF_FLAGS imFlags;
		if (_imageDirectory == _iconDirectory)
			imFlags = IMF_ICON_IMAGE;
		else if (_imageDirectory == _bitmapDirectory)
			imFlags = IMF_BITMAP_IMAGE;
		else
			imFlags = IMF_MOUSE_IMAGE;
		ZAF_IMAGE_WINDOW imageWindow(pathName, imFlags);
		imageWindow.Store(ZIL_NULLP(ZIL_ICHAR), imageFile);
	}

	// Level 5-Clean up the import services.
	else if (event.type == -OPT_RESOURCE_IMPORT)
	{
		storageService->Information(I_SET_STORAGE, imageFile);
		delete importFile;
	}

	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::OSImport(ZIL_STORAGE *imageFile, const UI_EVENT &event)
{
	// Load the edit image (comparison is lower-case).
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	ZAF_IMAGE editImage(0, 0, 1, 1);

	ZIL_ICHAR compareName[ZIL_MAXPATHLEN];
	strcpy(compareName, pathName);
	strlwr(compareName);
	if (strstr(compareName, _zncExtension))
	{
		_imageDirectory = _bitmapDirectory;
		ZIL_FILE rFile(pathName, UIS_READ | UIS_BINARY);
		editImage.SetImportFilter(F_ZNC);
		editImage.Import(&rFile);
	}
	else if (strstr(compareName, _xpmExtension))
	{
		_imageDirectory = _bitmapDirectory;
		ZIL_FILE rFile(pathName, UIS_READ | UIS_BINARY);
		editImage.SetImportFilter(F_XPM);
		editImage.Import(&rFile);
	}
	else if (strstr(compareName, _curExtension))
	{
		_imageDirectory = _mouseDirectory;
		ZIL_FILE rFile(pathName, UIS_READ | UIS_BINARY);
		editImage.SetImportFilter(F_WICO);
		editImage.Import(&rFile);
	}
	else if (strstr(compareName, _ptrExtension))
	{
		_imageDirectory = _mouseDirectory;
		ZIL_FILE rFile(pathName, UIS_READ | UIS_BINARY);
		editImage.SetImportFilter(F_ODIB);
		editImage.Import(&rFile);
	}
	else if (strstr(compareName, _bmpExtension))
	{
		_imageDirectory = _bitmapDirectory;
		ZIL_FILE rFile(pathName, UIS_READ | UIS_BINARY);
		editImage.SetImportFilter(F_WDIB); // Windows
		if (editImage.Import(&rFile))
		{
			rFile.SetError(ZIL_FILE::ERROR_NONE);
			rFile.Seek(0, ZIL_FILE::SEEK_FROM_START);
			editImage.SetImportFilter(F_ODIB); // OS/2
			editImage.Import(&rFile);
		}
	}
	else if (strstr(compareName, _icoExtension))
	{
		_imageDirectory = _iconDirectory;
		ZIL_FILE rFile(pathName, UIS_READ | UIS_BINARY);
		editImage.SetImportFilter(F_WICO); // Windows
		if (editImage.Import(&rFile))
		{
			rFile.SetError(ZIL_FILE::ERROR_NONE);
			rFile.Seek(0, ZIL_FILE::SEEK_FROM_START);
			editImage.SetImportFilter(F_ODIB); // OS/2
			editImage.Import(&rFile);
		}
	}
	else
		return (S_ERROR);

	// Traverse to the image directory.
	if (imageFile->ChDir(_imageDirectory))
	{
		imageFile->MkDir(_imageDirectory);
		imageFile->ChDir(_imageDirectory);
	}

	ZIL_ICHAR *sTitle = msgTable.GetMessage(ZMSG_IMPORT_TITLE);
	ZIL_ICHAR *sFormat = msgTable.GetMessage(ZMSG_IMPORT_MESSAGE);

	// Create the file object.
	ZIL_INT16 _value; 
	ZIL_ICHAR name[32];
	imageFile->StripFullPath(pathName, ZIL_NULLP(ZIL_ICHAR), name);
	strrepc(name, '.', '\0');
	_serviceManager->StatusUpdate(sTitle, sFormat, name);
	ZIL_STORAGE_OBJECT file(*imageFile, name, 0, UIS_CREATE | UIS_READWRITE);
	// Store the specified bitmap/icon image.
	_value = editImage.bitmapWidth; file.Store(_value);
	_value = editImage.bitmapHeight; file.Store(_value);
	if (FlagSet(editImage.imFlags, IMF_MOUSE_IMAGE))
	{
		_value = editImage.hotspotX; file.Store(_value);
		_value = editImage.hotspotY; file.Store(_value);
	}
	file.Store(editImage.bitmapArray, sizeof(ZIL_UINT8), editImage.bitmapWidth * editImage.bitmapHeight);
	_serviceManager->StatusUpdate();

	return (event.type);
}

