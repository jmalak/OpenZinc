//	Zinc Interface Library - Z_IMAGE.CPP
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

// --------------------------------------------------------------------------
// ----- UIW_IMAGE ----------------------------------------------------------
// --------------------------------------------------------------------------

UIW_IMAGE::UIW_IMAGE(int left, int top, int width, int height,
	ZIL_ICHAR *_pathName, int _pathID, IMF_FLAGS _imFlags,
	WOF_FLAGS _woFlags) :
	UI_WINDOW_OBJECT(left, top, width, height, _woFlags, WOAF_NO_FLAGS),
	image(0), pathID(_pathID)
{
#if defined(ZIL_MSWINDOWS)
	hPalette = 0;
#endif

	// Initialize the default data.
	searchID = WindowID(ID_IMAGE);
	woStatus |= WOS_OWNERDRAW;

	// Set the image information.
	imFlags = _imFlags;
	pathName = strdup(_pathName);

	// Level 1 - Load the image from the application resources.
	if (!image)
		LoadImageFromApplication();

	// Level 2 - Load the image from the native file.
	if (!image)
		LoadImageFromFile();
}

UIW_IMAGE::~UIW_IMAGE(void)
{
	// Destroy the image and pathName.
	if (image && !HasStaticHandle())
		DestroyImageHandle();
	if (pathName)
		delete pathName;
}

EVENT_TYPE UIW_IMAGE::Event(const UI_EVENT &event)
{
	// Check for system events.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_CREATE:
	case S_SIZE:
	case S_CHANGED:
		// Determine the size, then register the object.
		if (IsBackground())
		{
			parent->Information(I_GET_CLIPREGION, &relative);
			relative.right -= relative.left;
			relative.bottom -= relative.top;
			relative.left = relative.top = 0;
		}
		else if (AutoSizes())
		{
			relative.right = relative.left + imageWidth - 1;
			relative.bottom = relative.top + imageHeight - 1;
		}
		UI_WINDOW_OBJECT::Event(event);
		if (ccode == S_CREATE)
			UI_WINDOW_OBJECT::Event(S_REGISTER_OBJECT);
		return (ccode);
	}

	// Default to the base class.
	return (UI_WINDOW_OBJECT::Event(event));
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_IMAGE::NewFunction(void) { return (UIW_IMAGE::New); }

UIW_IMAGE::UIW_IMAGE(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 10, 2, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	image(0)
{
#if defined(ZIL_MSWINDOWS)
	hPalette = 0;
#endif

	// Initialize the button information.
	UIW_IMAGE::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Initialize the default data.
	searchID = WindowID(ID_IMAGE);
	woStatus |= WOS_OWNERDRAW;
}

void UIW_IMAGE::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the button information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	file->Load(&imFlags);
	file->Load(&pathName);
	ZIL_INT16 _pathID; file->Load(&_pathID); pathID = _pathID;

	// Level 1 - Load the image from the application resources.
	if (!image)
		LoadImageFromApplication();

	// Level 2 - Load the image from the native file.
	if (!image)
		LoadImageFromFile();
}
#endif

#if defined(ZIL_STORE)
void UIW_IMAGE::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the button information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(imFlags);
	file->Store(pathName);
	ZIL_INT16 _pathID = (ZIL_INT16)pathID; file->Store(_pathID);
}
#endif
