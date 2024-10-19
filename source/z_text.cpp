//	Zinc Interface Library - Z_TEXT.CPP
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

// ----- UIW_TEXT -----------------------------------------------------------

UIW_TEXT::UIW_TEXT(int left, int top, int width, int height, ZIL_ICHAR *_text,
	int _maxLength, WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UIW_WINDOW(left, top, width, height, _woFlags, WOAF_NO_FLAGS),
	maxLength(_maxLength), text(ZIL_NULLP(ZIL_ICHAR))
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	editBuffer = ZIL_NULLP(ZIL_ICHAR);
#endif

	// Initialize the text information.
	wnFlags |= _wnFlags;
	if (maxLength == -1)
		maxLength = _text ? strlen(_text) : width * height;
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		text = new ZIL_ICHAR[maxLength+1];
	userFunction = _userFunction;

	UIW_TEXT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TEXT::DataSet(_text);
}

UIW_TEXT::~UIW_TEXT(void)
{
#	if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	delete editBuffer;
#	endif

	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
}

ZIL_ICHAR *UIW_TEXT::DataGet(void)
{
	// Update the string with the environment.
	UIW_TEXT::OSDataGet();

	// Return the text information.
	return (text);
}

void UIW_TEXT::DataSet(ZIL_ICHAR *_text, int _maxLength)
{
	// Reset the length.
	if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		maxLength = (_maxLength != -1) ? _maxLength : maxLength;
	else if (_maxLength != -1)
	{
		if (_maxLength > maxLength)
		{
			ZIL_ICHAR *tText = new ZIL_ICHAR[_maxLength+1];
			strncpy(tText, text ? text : _blankString, maxLength);
			tText[maxLength] = '\0';
			if (_text == text)
				_text = tText;
			if (text)
				delete text;
			text = tText;
		}
		maxLength = _maxLength;
		text[maxLength] = '\0';
	}

	// Reset the text and compute the length.
	if (_text)
	{
		if (text == _text || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
			text = _text;
		else
			strncpy(text, _text, maxLength);
		text[maxLength] = '\0';
		if (FlagSet(woStatus, WOS_UNANSWERED))
			strcpy(text, _blankString);
	}

	// Update the string with the environment.
	UIW_TEXT::OSDataSet();
}

void *UIW_TEXT::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_TEXT;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and default variables.
		WindowID(ID_STRING);
		searchID = WindowID(ID_TEXT);
		insertMode = TRUE;
		wnFlags |= WNF_NO_SCROLL | WNF_CONTINUE_SELECT;
#if defined(ZIL_MACINTOSH)
		font = FNT_SMALL_FONT | FNT_IGNORE_UNDERSCORE;
#else
		font = FNT_DIALOG_FONT | FNT_IGNORE_UNDERSCORE;
#endif

		// Update the internal flag settings.
		UIW_TEXT::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_WINDOW::Information(I_CHANGED_FLAGS, data, ID_TEXT);

		// Update the internal flag settings.
		UIW_TEXT::UpdateSettings(objectID);
		break;

	case I_GET_TEXT:
		// Get the text.
		if (!data)
			return (DataGet());
		*(ZIL_ICHAR **)data = DataGet();
		break;

	case I_COPY_TEXT:
		data = strcpy((ZIL_ICHAR *)data, DataGet());
		break;

	case I_SET_TEXT:
		// Set the text.
		DataSet((ZIL_ICHAR *)data);
		break;

	case I_GET_MAXLENGTH:
		// Get the text length.
		if (!data)
			return (&maxLength);
		*(int *)data = maxLength;
		break;

	case I_SET_MAXLENGTH:
		// Set the text length.
		DataSet(text, *(int *)data);
		break;

	default:
		// Default to the base class.
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_TEXT::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_TEXT::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_TEXT::NewFunction(void) { return (UIW_TEXT::New); }

UIW_TEXT::UIW_TEXT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 6, WOF_NO_FLAGS), text(ZIL_NULLP(ZIL_ICHAR))
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	editBuffer = ZIL_NULLP(ZIL_ICHAR);
#endif

	// Initialize the text information.
	UIW_TEXT::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TEXT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_TEXT::DataSet(text);
}

void UIW_TEXT::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the text information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	ZIL_INT16 _value; file->Load(&_value); maxLength = _value;
	text = new ZIL_ICHAR[maxLength+1]; file->Load(text, maxLength);
	ZIL_INT16 noOfObjects;
	file->Load(&noOfObjects);
	for (int i = 0; i < noOfObjects; i++)
		Add(UI_WINDOW_OBJECT::New(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable));
	file->Load(&wnFlags);
}
#endif

#if defined(ZIL_STORE)
void UIW_TEXT::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the text information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	ZIL_INT16 _value = maxLength; file->Store(_value);
	file->Store(text);
	ZIL_INT16 noOfObjects = support.Count();
	file->Store(noOfObjects);
	for (UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
	{
		_value = object->SearchID();
		file->Store(_value);
		object->Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
	}
	file->Store(wnFlags);
}
#endif

