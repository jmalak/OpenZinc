//	Zinc Interface Library - Z_BUTTON.CPP
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
extern ZIL_ICHAR *_bitmapDirectory;

#define ZIL_UIW_BUTTON_DATA
#include "img_def.cpp"

// ----- UIW_BUTTON ---------------------------------------------------------

UIW_BUTTON::UIW_BUTTON(int left, int top, int width, ZIL_ICHAR *_text,
	BTF_FLAGS _btFlags, WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction,
	EVENT_TYPE _value, ZIL_ICHAR *_bitmapName) :
	UI_WINDOW_OBJECT(left, top, width, 1, _woFlags, WOAF_NO_FLAGS),
	text(ZIL_NULLP(ZIL_ICHAR)), btFlags(_btFlags), value(_value), depth(2),
	btStatus(BTS_NO_STATUS), bitmapWidth(0), bitmapHeight(0),
	bitmapArray(ZIL_NULLP(ZIL_UINT8)), colorBitmap(0), monoBitmap(0)
{
	// Initialize the button information.
	bitmapName = strdup(_bitmapName);
	userFunction = _userFunction;
	UIW_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BUTTON::DataSet(_text);
}

UIW_BUTTON::~UIW_BUTTON(void)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_OS2) || (defined(ZIL_MSWINDOWS) && defined(ZIL_MSWINDOWS_CTL3D))
	ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations);
#endif

	if (display)
		display->DestroyBitmapHandle(screenID, &colorBitmap, &monoBitmap);

	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	if (bitmapName)
		delete bitmapName;
	if (bitmapArray && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
		delete bitmapArray;
}

ZIL_ICHAR *UIW_BUTTON::DataGet(int stripText)
{
	// Check for normal DataGet.
	if (!text || !stripText)
		return (text);

	// Strip the button text information.
	static ZIL_ICHAR ZIL_FARDATA sText[128];
	int offset = 0;
	while (text[offset] == ' ')							// Check for beginning spaces.
		offset++;
	strcpy(sText, &text[offset]);
	offset = strlen(sText) - 1;

	while (offset >= 0 && sText[offset] == ' ')			// Check for trailing spaces.
		offset--;
	sText[offset + 1] = '\0';

	ZIL_ICHAR *hotChar = sText;								// Check for underline or '&' character.
	while ((hotChar = strchr(hotChar, '&')) != ZIL_NULLP(ZIL_ICHAR))
	{
		memmove(hotChar, hotChar + 1, strlen(hotChar)*sizeof(ZIL_ICHAR));
		if (*hotChar == '&')
			++hotChar;
	}
	return (sText);
}

void UIW_BUTTON::DataSet(ZIL_ICHAR *_text)
{
	// Reset the string and check for a hot key.
	if (_text)
	{
		if (text && text != _text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			delete text;
		if (text == _text || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			text = _text;
		else
			text = strdup(_text);
		hotKey = HotKey(text);
	}

	// Update the string with the environment.
	UIW_BUTTON::OSDataSet();
}

void *UIW_BUTTON::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_BUTTON;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Check the default initialization.
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_OS2) || (defined(ZIL_MSWINDOWS) && defined(ZIL_MSWINDOWS_CTL3D))
		if (!defaultInitialized)
		{
			ZIL_DECORATION_MANAGER::SetDecorations(_className, TEXT_TABLE, BITMAP_TABLE);
			defaultInitialized = TRUE;
		}
		myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className);
#endif

		// Set the object identification and variables.
		searchID = WindowID(ID_BUTTON);
		hotKey = HotKey(text);

		// See if the bitmap is system specified.
		UIW_BUTTON::OSSystemImage();

		// Read the bitmap from the system file.
#if defined(ZIL_STORAGE_IMAGE) && !defined(ZIL_TEXT_ONLY)
		if (!colorBitmap && bitmapName && !bitmapArray &&
			defaultStorage && !defaultStorage->storageError)
		{
			defaultStorage->ChDir(_bitmapDirectory);
			ZIL_STORAGE_OBJECT_READ_ONLY bFile(*defaultStorage, bitmapName, ID_BITMAP_IMAGE);
			if (!bFile.objectError)
			{
				ZIL_INT16 _value; bFile.Load(&_value); bitmapWidth = _value;
				bFile.Load(&_value); bitmapHeight = _value;
				bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
				bFile.Load(bitmapArray, sizeof(ZIL_UINT8), bitmapWidth * bitmapHeight);
			}
		}
#endif

		// Update the internal flag settings.
		UIW_BUTTON::UpdateSettings(objectID);
		UIW_BUTTON::OSImageInitialize();
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UI_WINDOW_OBJECT::Information(I_CHANGED_FLAGS, data, ID_BUTTON);

		// Update the internal flag settings.
		UIW_BUTTON::UpdateSettings(objectID);
		break;

	case I_GET_TEXT:
		// Get the button text.
		if (!data)
			return (DataGet());
		*(ZIL_ICHAR **)data = DataGet();
		break;

	case I_COPY_TEXT:
		// Copy the button text.
		{
            ZIL_ICHAR *tmp = DataGet();
            ((ZIL_ICHAR *)data)[0] = 0;
            if (tmp)
                strcpy((ZIL_ICHAR *)data, tmp);
        }
		break;

	case I_SET_TEXT:
		// Set the button text.
		DataSet((ZIL_ICHAR *)data);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the button flag settings.
		if (objectID && objectID != ID_BUTTON)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &btFlags;
		else if (request == I_GET_FLAGS)
			*(BTF_FLAGS *)data = btFlags;
		else if (request == I_SET_FLAGS)
			btFlags |= *(BTF_FLAGS *)data;
		else
			btFlags &= ~(*(BTF_FLAGS *)data);
		break;

	case I_GET_STATUS:
	case I_SET_STATUS:
	case I_CLEAR_STATUS:
		// Get, set or clear the button status settings.
		if (objectID && objectID != ID_BUTTON)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == I_GET_STATUS && !data)
			data = &btStatus;
		else if (request == I_GET_STATUS)
			*(BTS_STATUS *)data = btStatus;
		else if (request == I_SET_STATUS)
			btStatus |= *(BTS_STATUS *)data;
		else if (request == I_CLEAR_STATUS)
			btStatus &= ~(*(BTS_STATUS *)data);
		break;

	case I_CHANGED_STATUS:
		DataSet(ZIL_NULLP(ZIL_ICHAR));	// Redraw the item contents.
		UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;

	case I_GET_VALUE:
		// Get the button value.
		*(EVENT_TYPE *)data = value;
		break;

	case I_SET_VALUE:
		// Reset the button value.
		value = *(EVENT_TYPE *)data;
		break;

	case I_GET_BITMAP_WIDTH:
		// Get the bitmap width.
		*(int *)data = bitmapWidth;
		break;

	case I_SET_BITMAP_WIDTH:
		// Set the bitmap width.
		bitmapWidth = *(int *)data;
		break;

	case I_GET_BITMAP_HEIGHT:
		// Get the bitmap height.
		*(int *)data = bitmapHeight;
		break;

	case I_SET_BITMAP_HEIGHT:
		// Set the bitmap height.
		bitmapHeight = *(int *)data;
		break;

	case I_GET_BITMAP_ARRAY:
		// See if the array needs to be converted.
		if (!bitmapArray && display)
			display->BitmapHandleToArray(screenID, colorBitmap, monoBitmap,
				&bitmapWidth, &bitmapHeight, &bitmapArray);

		// Get the bitmap array.
		if (!data)
			return (bitmapArray);
		*(ZIL_UINT8 **)data = bitmapArray;
		break;

	case I_SET_BITMAP_ARRAY:
		// Set the bitmap array.
		if (bitmapArray && !FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
			delete bitmapArray;
		if (data)
		{
			if (FlagSet(btFlags, BTF_STATIC_BITMAPARRAY))
				bitmapArray = (ZIL_UINT8 *)data;
			else
			{
				bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
				memcpy(bitmapArray, (ZIL_UINT8 *)data, bitmapWidth * bitmapHeight);
			}
		}
		else
			bitmapArray = ZIL_NULLP(ZIL_UINT8);

		// Convert the bitmap array.
		display->DestroyBitmapHandle(screenID, &colorBitmap, &monoBitmap);
		break;

	default:
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

EVENT_TYPE UIW_BUTTON::Message(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode)
{
	// Check for valid button.
	if (ccode == S_DRAG_COPY_OBJECT || ccode == S_DRAG_MOVE_OBJECT || ccode == S_DRAG_DEFAULT ||
		ccode == S_DROP_COPY_OBJECT || ccode == S_DROP_MOVE_OBJECT || ccode == S_DROP_DEFAULT)
			return (S_UNKNOWN);
	else if (FlagSet(object->woStatus, WOS_EDIT_MODE) ||
		(ccode != L_SELECT && ccode != L_DOUBLE_CLICK))
			return (0);

	// Check for a double-click message.
	BTF_FLAGS btFlags;
	object->Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
	if (FlagSet(btFlags, BTF_DOUBLE_CLICK) && ccode == L_SELECT &&
		object->LogicalEvent(event, object->SearchID()) != L_SELECT)
		return (ccode);

	// Process the button value as a system message.
	EVENT_TYPE command;
	object->Information(I_GET_VALUE, &command);
	event.type = command;
	event.rawCode = 0;
	event.windowObject = object;
	if (command == L_RESTORE || command == L_MOVE || command == L_SIZE ||
		command == L_MINIMIZE || command == L_MAXIMIZE || command == S_CLOSE)
	{
		for (UI_WINDOW_OBJECT *tObject = object; tObject; tObject = tObject->parent)
			if (tObject->Inherited(ID_WINDOW) && !tObject->Inherited(ID_MENU))
			{
				event.data = tObject;
				break;
			}
	}
	object->eventManager->Put(event);

	// Return the control code.
	return (ccode);
}

void UIW_BUTTON::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the general flag settings.
	if (FlagSet(btFlags, BTF_SEND_MESSAGE) && !userFunction)
		userFunction = UIW_BUTTON::Message;
	if (FlagSet(btFlags, BTF_CHECK_BOX | BTF_RADIO_BUTTON))
	{
		woFlags &= ~WOF_JUSTIFY_CENTER;
		btFlags &= ~(BTF_AUTO_SIZE | BTF_NO_TOGGLE);
		btFlags |= BTF_NO_3D;
	}
	depth = FlagSet(btFlags, BTF_NO_3D) ? 0 : 2;

	// Check for the check-box or radio-button setting.
	if (FlagSet(btFlags, BTF_CHECK_BOX))
		WindowID(ID_CHECK_BOX);
	else if (FlagSet(btFlags, BTF_RADIO_BUTTON))
		WindowID(ID_RADIO_BUTTON);
	else
	{
		// Remove the radio-button and check-box settings.
		ZIL_SIMPLE_OBJECTID theId = ID_BUTTON;
		for (int i = 0; i < 5; i++)
			if (windowID[i] == ID_RADIO_BUTTON ||
				windowID[i] == ID_CHECK_BOX ||
				windowID[i] == ID_BUTTON)
			{
				windowID[i] = theId;
				theId = ID_WINDOW_OBJECT;
			}
	}

	// Check the environment specific flag settings.
	UIW_BUTTON::OSImageInitialize();
	UIW_BUTTON::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_BUTTON::NewFunction(void) { return (UIW_BUTTON::New); }

UIW_BUTTON::UIW_BUTTON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 15, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS),
	btStatus(BTS_NO_STATUS), bitmapWidth(0), bitmapHeight(0),
	bitmapArray(ZIL_NULLP(ZIL_UINT8)), colorBitmap(0), monoBitmap(0)
{
	// Initialize the button information.
	UIW_BUTTON::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BUTTON::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_BUTTON::DataSet(text);
}

void UIW_BUTTON::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the button information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	file->Load(&btFlags);
	ZIL_INT16 _value; file->Load(&_value); value = _value;
	file->Load(&_value); depth = _value;
	file->Load(&text);
	file->Load(&bitmapName);

	// Load the bitmap information.
	if (bitmapName)
	{
		directory->ChDir(_bitmapDirectory);
		ZIL_STORAGE_OBJECT_READ_ONLY bFile(*directory, bitmapName, ID_BITMAP_IMAGE);
		if (!bFile.objectError)
		{
			ZIL_INT16 _value; bFile.Load(&_value); bitmapWidth = _value;
			bFile.Load(&_value); bitmapHeight = _value;
			bitmapArray = new ZIL_UINT8[bitmapWidth * bitmapHeight];
			bFile.Load(bitmapArray, sizeof(ZIL_UINT8), bitmapWidth * bitmapHeight);

			// Convert the bitmap array to a handle if possible.
			UIW_BUTTON::OSImageInitialize();
		}
	}
}
#endif

#if defined(ZIL_STORE)
void UIW_BUTTON::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the button information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(btFlags);
	ZIL_INT16 _value = (ZIL_INT16)value; file->Store(_value);
	_value = depth; file->Store(_value);
	file->Store(text);
	file->Store(bitmapName);

	// See if the bitmap needs to be converted.
	if (bitmapName && colorBitmap && !bitmapArray)
		display->BitmapHandleToArray(screenID, colorBitmap, monoBitmap,
			&bitmapWidth, &bitmapHeight, &bitmapArray);

	// Store the bitmap information.
	if (bitmapName && bitmapArray)
	{
		if (directory->ChDir(_bitmapDirectory))
		{
			directory->MkDir(_bitmapDirectory);
			directory->ChDir(_bitmapDirectory);
		}
		ZIL_STORAGE_OBJECT bFile(*directory, bitmapName, ID_BITMAP_IMAGE, UIS_CREATE | UIS_READWRITE);
		if (!bFile.objectError)
		{
			_value = bitmapWidth; bFile.Store(_value);
			_value = bitmapHeight; bFile.Store(_value);
			bFile.Store(bitmapArray, sizeof(ZIL_UINT8), bitmapWidth * bitmapHeight);
		}
	}
}
#endif
