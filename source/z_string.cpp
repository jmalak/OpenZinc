//	Zinc Interface Library - Z_STRING.CPP
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

// ----- UIW_STRING ---------------------------------------------------------

UIW_STRING::UIW_STRING(int left, int top, int width, ZIL_ICHAR *_text,
	int _maxLength, STF_FLAGS _stFlags, WOF_FLAGS _woFlags,
	ZIL_USER_FUNCTION _userFunction) :
	UI_WINDOW_OBJECT(left, top, width, 1, _woFlags, WOAF_NO_FLAGS),
	text(ZIL_NULLP(ZIL_ICHAR)), maxLength(_maxLength), stFlags(_stFlags)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	scrollCount = 5;
	leftClip = cursor = lineWidth = 0;
	beginMark = endMark = -1;
#endif

	// Initialize the string information.
	if (maxLength == -1)
		maxLength = _text ? strlen(_text) : width;
	if (!FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
	{
		text = new ZIL_ICHAR[maxLength + 1];
		text[0] = '\0';
	}
	userFunction = _userFunction;

	UIW_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::DataSet(_text);
}

UIW_STRING::~UIW_STRING(void)
{
	// Destroy the string information.
	if (text && !FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
}

ZIL_ICHAR *UIW_STRING::DataGet(void)
{
	// Update the string with the environment.
	UIW_STRING::OSDataGet();

	// Return the text information.
	return (text);
}

void UIW_STRING::DataSet(ZIL_ICHAR *_text, int _maxLength)
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
		if (HasVariableName())
		{
			ZIL_ICHAR *temp = text;
			for(;temp && *temp; temp++)
				if (*temp == ' ')
					*temp = '_';
		}
	}

	// Update the string with the environment.
	UIW_STRING::OSDataSet();
}

void *UIW_STRING::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_STRING;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_STRING);
		insertMode = TRUE;
		font |= FNT_IGNORE_UNDERSCORE;

		// Update the internal flag settings.
		UIW_STRING::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UI_WINDOW_OBJECT::Information(I_CHANGED_FLAGS, data, ID_STRING);

		// Update the internal flag settings.
		UIW_STRING::UpdateSettings(objectID);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the button status settings.
		if (objectID && objectID != ID_STRING)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &stFlags;
		else if (request == I_GET_FLAGS)
			*(STF_FLAGS *)data = stFlags;
		else if (request == I_SET_FLAGS)
			stFlags |= *(STF_FLAGS *)data;
		else
			stFlags &= ~(*(STF_FLAGS *)data);
		break;

	case I_GET_TEXT:
		// Get the string.
		if (!data)
			return (DataGet());
		*(ZIL_ICHAR **)data = DataGet();
		break;

	case I_COPY_TEXT:
		// Copy the string.
		data = strcpy((ZIL_ICHAR *)data, DataGet());
		break;

	case I_SET_TEXT:
		// Set the string.
		DataSet((ZIL_ICHAR *)data);
		break;

	case I_GET_MAXLENGTH:
		// Get the string length.
		if (!data)
			return (&maxLength);
		*(int *)data = maxLength;
		break;

	case I_SET_MAXLENGTH:
		// Set the string length.
		DataSet(text, *(int *)data);
		break;

	default:
		// Default to the base class.
		data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

ZIL_ICHAR *UIW_STRING::ParseRange(ZIL_ICHAR *buffer, ZIL_ICHAR *minValue, ZIL_ICHAR *maxValue)
{
	// Parse the minimum value.
	int position = 0, offset = 0;
	while (buffer[offset] != '\0' &&
		(buffer[offset] != '.' || buffer[offset+1] != '.') &&
		buffer[offset] != '/')
		minValue[position++] = buffer[offset++];
	minValue[position] = '\0';

	// See if it is a standalone value.
	if (buffer[offset] == '/' || buffer[offset] == '\0')
	{
		strcpy(maxValue, minValue);
		return (buffer[offset] ? &buffer[++offset] : ZIL_NULLP(ZIL_ICHAR));
	}

	// Increment the offset.
	while (buffer[offset] == '.')
		offset++;

	// Parse the maximum value.
	position = 0;
	while (buffer[offset] != '\0' && buffer[offset] != '/')
		maxValue[position++] = buffer[offset++];
	maxValue[position] = '\0';

	// Return the offset position.
	return (buffer[offset] ? &buffer[++offset] : ZIL_NULLP(ZIL_ICHAR));
}

void UIW_STRING::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_STRING::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_STRING::NewFunction(void) { return (UIW_STRING::New); }

UIW_STRING::UIW_STRING(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UI_WINDOW_OBJECT(0, 0, 15, 1, WOF_NO_FLAGS, WOAF_NO_FLAGS), text(ZIL_NULLP(ZIL_ICHAR))
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	scrollCount = 5;
	leftClip = cursor = lineWidth = 0;
	beginMark = endMark = -1;
#endif

	// Initialize the string information.
	UIW_STRING::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::DataSet(text);
}

void UIW_STRING::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the string information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	file->Load(&stFlags);
	ZIL_INT16 _value; file->Load(&_value); maxLength = _value;
	if (text)
		delete text;
	text = new ZIL_ICHAR[maxLength+1]; file->Load(text, maxLength);
}
#endif

#if defined(ZIL_STORE)
void UIW_STRING::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the string information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(stFlags);
	ZIL_INT16 _value = maxLength; file->Store(_value);
	DataGet();
	file->Store(text);
}
#endif

