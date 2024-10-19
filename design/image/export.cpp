//	Zinc Interface Library - EXPORT.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"

extern ZIL_ICHAR *_mouseDirectory;

EVENT_TYPE ZAF_IMAGE_EDITOR::ImageExport(const UI_EVENT &event)
{
	static ZIL_STORAGE *imageFile;
	static UI_WINDOW_OBJECT *iObject;
	static ZIL_STORAGE *datFile = ZIL_NULLP(ZIL_STORAGE);
	static ZIL_FILE *osFile = ZIL_NULLP(ZIL_FILE);
	static ZIL_ICHAR *imageType = ZIL_NULLP(ZIL_ICHAR);

	// Level 1-Select the export file.
	if (event.type == OPT_FILE_EXPORT)
	{
		iObject = event.windowObject;
		// Filter is contained in the stringID.
		if (!storageService->Information(I_GET_STORAGE, &imageFile))
			return (directoryService->Event(S_ERROR));
		return (FileRequest(event));
	}

	// Level 2-Open the export file.
	else if (event.type == -OPT_FILE_EXPORT && !event.data)
		return (event.type); // end to service response has no data.
	else if (event.type == -OPT_FILE_EXPORT)
	{
		// Open the export file (comparison is lower-case).
		ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
		static ZIL_ICHAR compareName[ZIL_MAXPATHLEN];
		strcpy(compareName, pathName);
		strlwr(compareName);
		imageType = compareName;
		if (strstr(compareName, _zncExtension) ||
			strstr(compareName, _xpmExtension) ||
			strstr(compareName, _bmpExtension) ||
			strstr(compareName, _icoExtension) ||
			strstr(compareName, _curExtension) ||
			strstr(compareName, _ptrExtension))
			osFile = new ZIL_FILE(pathName, UIS_CREATE | UIS_READWRITE | UIS_BINARY);
		else if (strstr(compareName, _obmpExtension) ||
			strstr(compareName, _wbmpExtension))
		{
			ZIL_ICHAR temp[256]; strcpy(temp, pathName);
			ZIL_STORAGE::ChangeExtension(temp, _bmpExtension);
			osFile = new ZIL_FILE(temp, UIS_CREATE | UIS_READWRITE | UIS_BINARY);
		}
		else if (strstr(compareName, _oicoExtension) ||
			strstr(compareName, _wicoExtension))
		{
			ZIL_ICHAR temp[256]; strcpy(temp, pathName);
			ZIL_STORAGE::ChangeExtension(temp, _icoExtension);
			osFile = new ZIL_FILE(temp, UIS_CREATE | UIS_READWRITE | UIS_BINARY);
		}
		else
			datFile = new ZIL_STORAGE(pathName, UIS_OPENCREATE | UIS_READWRITE);
		eventManager->Put(OPT_RESOURCE_EXPORT); // Wait till provider window is removed from screen.
	}

	// Level 3-Select the export resources.
	else if (event.type == OPT_RESOURCE_EXPORT)
	{
		UI_EVENT iEvent = event;
		iEvent.windowObject = iObject;
		return (ImageRequest(iEvent));
	}

	// Level 4-Save the export resources.
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && osFile)
		OSExport(imageFile, osFile, event, imageType);
	else if (event.type == -OPT_RESOURCE_EXPORT && event.data && datFile)
		OSExport(imageFile, datFile, event);

	// Level 5-Clean up the export services.
	else if (event.type == -OPT_RESOURCE_EXPORT && datFile)
	{
		datFile->Save();
		delete datFile;
		datFile = ZIL_NULLP(ZIL_STORAGE);
	}
	else if (event.type == -OPT_RESOURCE_EXPORT && osFile)
	{
		delete osFile;
		osFile = ZIL_NULLP(ZIL_FILE);
	}

	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::OSExport(ZIL_STORAGE *srcFile, ZIL_FILE *osFile, const UI_EVENT &event, ZIL_ICHAR *imageType)
{
	if (srcFile->ChDir(_imageDirectory))
		return (S_ERROR);

	// Load the export image.
	ZIL_ICHAR *text = (ZIL_ICHAR *)event.data;
	ZIL_STORAGE_OBJECT_READ_ONLY srcImage(*srcFile, text, 0);

	ZIL_INT16 _value; 
	srcImage.Load(&_value); int bitmapWidth = _value;
	srcImage.Load(&_value); int bitmapHeight = _value;
	int hotspotX = -1, hotspotY = -1;
	if (_imageDirectory == _mouseDirectory)
	{
		srcImage.Load(&_value); hotspotX = _value;
		srcImage.Load(&_value); hotspotY = _value;
	}
	ZIL_UINT8 *bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
	srcImage.Load(bitmapArray, bitmapWidth * bitmapHeight, sizeof(ZIL_UINT8));
	ZAF_IMAGE editImage(0, 0, 1, 1);
	editImage.imFlags = IMF_BITMAP_IMAGE;
	editImage.DataSet(bitmapArray, bitmapWidth, bitmapHeight, hotspotX, hotspotY);

	// Store out the filter image.
	if (strstr(imageType, _zncExtension))
		editImage.SetExportFilter(F_ZNC);
	else if (strstr(imageType, _xpmExtension))
		editImage.SetExportFilter(F_XPM);
	else if (strstr(imageType, _curExtension)) // Windows
	{
		editImage.imFlags = IMF_MOUSE_IMAGE;
		editImage.SetExportFilter(F_WICO);
	}
	else if (strstr(imageType, _ptrExtension)) // OS/2
	{
		editImage.imFlags = IMF_MOUSE_IMAGE;
		editImage.SetExportFilter(F_ODIB);
	}
	else if (strstr(imageType, _bmpExtension)) // Default
	{
#		if defined(ZIL_OS2)
		editImage.SetExportFilter(F_ODIB);
#		else
		editImage.SetExportFilter(F_WDIB);
#		endif
	}
	else if (strstr(imageType, _obmpExtension)) // OS/2
		editImage.SetExportFilter(F_ODIB);
	else if (strstr(imageType, _wbmpExtension)) // Windows
		editImage.SetExportFilter(F_WDIB);
	else if (strstr(imageType, _icoExtension)) // Default
	{
		editImage.imFlags = IMF_ICON_IMAGE;
#		if defined(ZIL_OS2)
		editImage.SetExportFilter(F_ODIB);
#		else
		editImage.SetExportFilter(F_WICO);
#		endif
	}
	else if (strstr(imageType, _oicoExtension)) // OS2
	{
		editImage.imFlags = IMF_ICON_IMAGE;
		editImage.SetExportFilter(F_ODIB);
	}
	else if (strstr(imageType, _wicoExtension)) // Windows
	{
		editImage.imFlags = IMF_ICON_IMAGE;
		editImage.SetExportFilter(F_WICO);
	}
	else
		return (S_ERROR);
	editImage.Export(osFile);
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE_EDITOR::OSExport(ZIL_STORAGE *srcFile, ZIL_STORAGE *datFile, const UI_EVENT &event)
{
	// Traverse to the image directory.
	if (srcFile->ChDir(_imageDirectory))
		return (FALSE);
	if (datFile->ChDir(_imageDirectory))
	{
		datFile->MkDir(_imageDirectory);
		datFile->ChDir(_imageDirectory);
	}

	// Confirm the export of duplicate objects.
	ZIL_ICHAR *pathName = (ZIL_ICHAR *)event.data;
	if (datFile->FindFirstObject(pathName) &&
		storageService->Event(event) != event.type)
		return (FALSE);

	// Store the image information.
	int length;
	char buffer[1024];
	ZIL_STORAGE_OBJECT_READ_ONLY srcObject(*srcFile, pathName, 0);
	ZIL_STORAGE_OBJECT dstObject(*datFile, pathName, 0, UIS_OPENCREATE | UIS_READWRITE);
	while ((length = srcObject.Load(buffer, 1024, 1)) > 0)
		dstObject.Store(buffer, length, 1);

	return (event.type);
}

