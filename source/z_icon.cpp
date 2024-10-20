//	Zinc Interface Library - Z_ICON.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

/*       This file is a part of OpenZinc

          OpenZinc is free software; you can redistribute it and/or modify it under
          the terms of the GNU Lessor General Public License as published by
          the Free Software Foundation, either version 3 of the License, or (at
          your option) any later version

	OpenZinc is distributed in the hope that it will be useful, but WITHOUT
          ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
          or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser 
          General Public License for more details.

          You should have received a copy of the GNU Lessor General Public License
	 along with OpenZinc. If not, see <http://www.gnu.org/licenses/>                          */


#include "ui_win.hpp"

extern ZIL_ICHAR *_iconDirectory;

// ----- UIW_ICON -----------------------------------------------------------

UIW_ICON::UIW_ICON(int left, int top, ZIL_ICHAR *_iconName, ZIL_ICHAR *_title,
	ICF_FLAGS _icFlags, WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction) :
	UI_WINDOW_OBJECT(left, top, 0, 0, _woFlags, WOAF_NO_FLAGS),
	icFlags(_icFlags), iconWidth(32), iconHeight(32),
	iconArray(ZIL_NULLP(ZIL_UINT8)), title(ZIL_NULLP(ZIL_ICHAR))
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	icon.colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	icon.monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
#elif defined(ZIL_MACINTOSH)
	icon.iconHandle = ZIL_NULLH(Handle);
	icon.monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	icon.colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
#else
	icon = 0;
#endif

	// Initialize the icon information.
	DataSet(_title);
	iconName = strdup(_iconName);
	userFunction = _userFunction;
	UIW_ICON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_ICON::~UIW_ICON(void)
{
	if (display)
		display->DestroyIconHandle(screenID, &icon);

	if (title)
		delete title;
	if (iconName)
		delete iconName;
	if (iconArray && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))
		delete iconArray;
}

ZIL_ICHAR *UIW_ICON::DataGet(void)
{
 	return (title);
}

void UIW_ICON::DataSet(ZIL_ICHAR *_title)
{
	// Reset the title and check for a hotkey.
	if (title && title != _title && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete title;
	if (title == _title || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		title = _title;
	else
		title = strdup(_title);
	hotKey = HotKey(title);

	// Update the string with the environment.
	UIW_ICON::OSDataSet();
}

void *UIW_ICON::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_ICON;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_ICON);
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			woFlags |= WOF_SUPPORT_OBJECT;
		font = FNT_SMALL_FONT;
		hotKey = HotKey(title);

		// See if the icon is system specified.
		UIW_ICON::OSSystemImage();

		// Read the bitmap from the system file.
#if defined(ZIL_STORAGE_IMAGE) && !defined(ZIL_TEXT_ONLY)
		if (iconName && !iconArray &&
#	if defined(ZIL_MSWINDOWS)
			!icon &&
#	elif defined(ZIL_MACINTOSH)
			!icon.iconHandle && !FlagSet(icFlags, ICF_MINIMIZE_OBJECT) &&
#	endif
			defaultStorage && !defaultStorage->storageError)
		{
			defaultStorage->ChDir(_iconDirectory);
			ZIL_STORAGE_OBJECT_READ_ONLY iFile(*defaultStorage, iconName, ID_ICON_IMAGE);
			if (!iFile.objectError)
			{
				ZIL_INT16 _value; iFile.Load(&_value); iconWidth = _value;
				iFile.Load(&_value); iconHeight = _value;
				iconArray = new ZIL_UINT8[iconWidth * iconHeight];
				iFile.Load(iconArray, sizeof(ZIL_UINT8), iconWidth * iconHeight);
			}
		}
#endif

		// Update the internal flag settings.
		UIW_ICON::UpdateSettings(objectID);
		UIW_ICON::OSImageInitialize();
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		if (!FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			UI_WINDOW_OBJECT::Information(I_CHANGED_FLAGS, data, ID_ICON);

		// Update the internal flag settings.
		UIW_ICON::UpdateSettings(objectID);
		break;

	case I_GET_TEXT:
		// Get the icon text.
		if (!data)
			return (title);
		*(ZIL_ICHAR **)data = title;
		break;

	case I_COPY_TEXT:
		// Set the icon text.
		data = strcpy((ZIL_ICHAR *)data, title ? title : ZIL_TEXT(""));
		break;

	case I_SET_TEXT:
		// Set the icon text.
		DataSet((ZIL_ICHAR *)data);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the icon flag settings.
		if (objectID && objectID != ID_ICON)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &icFlags;
		else if (request == I_GET_FLAGS)
			*(ICF_FLAGS *)data = icFlags;
		else if (request == I_SET_FLAGS)
			icFlags |= *(ICF_FLAGS *)data;
		else
			icFlags &= ~(*(ICF_FLAGS *)data);
		break;

	case I_GET_ICON_ARRAY:
		// See if the array needs to be converted.
		if (!iconArray)
			display->IconHandleToArray(screenID, icon, &iconWidth,
				&iconHeight, &iconArray);

		// Get the bitmap array.
		if (!data)
			return (iconArray);
		*(ZIL_UINT8 **)data = iconArray;
		break;

	case I_SET_ICON_ARRAY:
		// Set the bitmap array.
		if (iconArray && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))
			delete iconArray;
		if (data)
		{
			if (FlagSet(icFlags, ICF_STATIC_ICONARRAY))
				iconArray = (ZIL_UINT8 *)data;
			else
			{
				iconArray = new ZIL_UINT8[iconWidth * iconHeight];
				memcpy(iconArray, (ZIL_UINT8 *)data, iconWidth * iconHeight);
			}
		}
		else
			iconArray = ZIL_NULLP(ZIL_UINT8);

		// Convert the bitmap array.
		display->DestroyIconHandle(screenID, &icon);
		break;

	case I_GET_ICON_WIDTH:
		// Get the icon width.
		*(int *)data = iconWidth;
		break;

	case I_GET_ICON_HEIGHT:
		// Get the icon height.
		*(int *)data = iconHeight;
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_ICON::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the general flag settings.
	if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		woFlags |= WOF_SUPPORT_OBJECT;

	// Check the environment specific flag settings.
	UIW_ICON::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_ICON::NewFunction(void) { return (UIW_ICON::New); }

UIW_ICON::UIW_ICON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 15, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	iconWidth(32), iconHeight(32), iconArray(ZIL_NULLP(ZIL_UINT8))
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	icon.colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	icon.monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
#elif defined(ZIL_MACINTOSH)
	icon.iconHandle = ZIL_NULLH(Handle);
	icon.monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
	icon.colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
#else
	icon = 0;
#endif

	// Initialize the icon information.
	UIW_ICON::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_ICON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_ICON::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the icon information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	file->Load(&icFlags);
	file->Load(&title);
	file->Load(&iconName);

	// Read the bitmap information.
	if (iconName)
	{
		directory->ChDir(_iconDirectory);
		ZIL_STORAGE_OBJECT_READ_ONLY iFile(*directory, iconName, ID_ICON_IMAGE);
		if (!iFile.objectError)
		{
			ZIL_INT16 _value; iFile.Load(&_value); iconWidth = _value;
			iFile.Load(&_value); iconHeight = _value;
			iconArray = new ZIL_UINT8[iconWidth * iconHeight];
			iFile.Load(iconArray, sizeof(ZIL_UINT8), iconWidth * iconHeight);

			// Convert the icon array to a handle if possible.
			OSImageInitialize();
		}
	}
}
#endif

#if defined(ZIL_STORE)
void UIW_ICON::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the icon information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(icFlags);
	file->Store(title);
	file->Store(iconName);

	// See if the bitmap needs to be converted.
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_MACINTOSH)
	if (iconName && icon.colorBitmap && !iconArray)
		display->IconHandleToArray(screenID, icon, &iconWidth, &iconHeight,
			&iconArray);
#else
	if (iconName && icon && !iconArray)
		display->IconHandleToArray(screenID, icon, &iconWidth, &iconHeight,
			&iconArray);
#endif

	// Store the bitmap information.
	if (iconName && iconArray)
	{
		if (directory->ChDir(_iconDirectory))
		{
			directory->MkDir(_iconDirectory);
			directory->ChDir(_iconDirectory);
		}
		ZIL_STORAGE_OBJECT iFile(*directory, iconName, ID_ICON_IMAGE, UIS_CREATE | UIS_READWRITE);
		if (!iFile.objectError)
		{
			ZIL_INT16 _value = iconWidth; iFile.Store(_value);
			_value = iconHeight; iFile.Store(_value);
			iFile.Store(iconArray, sizeof(ZIL_UINT8), iconWidth * iconHeight);
		}
	}
}
#endif

