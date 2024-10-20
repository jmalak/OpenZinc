//	Zinc Interface Library - Z_FMTSTR.CPP
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

#define MAXLEN	64

// ----- UIW_FORMATTED_STRING -----------------------------------------------

UIW_FORMATTED_STRING::UIW_FORMATTED_STRING(int left, int top, int width,
	ZIL_ICHAR *_compressedText, ZIL_ICHAR *_editMask, ZIL_ICHAR *_deleteText,
	WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction) :
	UIW_STRING(left, top, width, ZIL_NULLP(ZIL_ICHAR), MAXLEN, STF_NO_FLAGS, _woFlags, _userFunction)
{
	// Initialize the formatted string information.
	// Set the compressed text.
	if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
	{
		text = new ZIL_ICHAR[MAXLEN+1];
		compressedText = _compressedText;
	}
	else
	{
		compressedText = new ZIL_ICHAR[MAXLEN+1];
		strcpy(compressedText, _compressedText ? _compressedText : _blankString);
	}
	// Set the edit mask.
	editMask = new ZIL_ICHAR[MAXLEN+1];
	strcpy(editMask, _editMask ? _editMask : _blankString);
	// Set the delete text.
	deleteText = new ZIL_ICHAR[MAXLEN+1];
	deleteText[0] = 0;
	if (_deleteText)
		strcpy(deleteText, _deleteText);
	int length = strlen(editMask);
	for (int i = strlen(deleteText); i < length; i++)
		deleteText[i] = ' ';
	deleteText[length] = 0;
	UIW_FORMATTED_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_FORMATTED_STRING::~UIW_FORMATTED_STRING(void)
{
	// Restore the formatted string information.
	if (FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		delete text;
	else
		delete compressedText;
	delete editMask;
	delete deleteText;
}

ZIL_ICHAR *UIW_FORMATTED_STRING::DataGet(int _compressedText)
{
	// Update the text buffers.
	Import(UIW_STRING::DataGet());

	// Return the formatted string.
	return (_compressedText ? compressedText : text);
}

void UIW_FORMATTED_STRING::DataSet(ZIL_ICHAR *_text)
{
	// Reset the formatted string.
	if (_text == compressedText || FlagSet(woFlags, WOF_NO_ALLOCATE_DATA))
		compressedText = _text;
	if (_text)
		Import(_text);
	UIW_STRING::DataSet(text);
}

void UIW_FORMATTED_STRING::Expand(ZIL_ICHAR *destination)
{
	ZIL_ICHAR *source = compressedText;
	for (int i = 0; editMask[i]; i++)
		if (editMask[i] != 'L' && *source)
			*destination++ = *source++;
		else
			*destination++ = deleteText[i];
	*destination = '\0';
}

void UIW_FORMATTED_STRING::Export(ZIL_ICHAR *destination, int expanded)
{
	// Update the text buffers if text has been entered previously.
	ZIL_ICHAR *tText = UIW_STRING::DataGet();
	if (tText && tText[0] != '\0')
		Import(tText);

	// Export the formatted string value.
	if (expanded)
		Expand(destination);
	else
		strcpy(destination, compressedText);
}

FMI_RESULT UIW_FORMATTED_STRING::Import(ZIL_ICHAR *source)
{
	// Import the formatted string value.
	ZIL_ICHAR *destination = compressedText;
	ZIL_ICHAR *tdeleteText = deleteText;
	for (ZIL_ICHAR *mask = editMask; *mask && *source; mask++, tdeleteText++)
		if (*mask != 'L')
		{
			for (int processed = FALSE; !processed && *source; source++)
			{
				int editValue = *mask;
				int textValue = *source;
				int isAlpha = (textValue == ' ' || IsAlpha(textValue));
				int isDigit = IsDigit(textValue);
				if (IsUpper(editValue))
					textValue = ToUpper(textValue);
				editValue = ToUpper(editValue);
				if ((editValue == 'X' && textValue >= ' ' && textValue <= '~') ||
					(editValue == 'N' && isDigit) ||
					(editValue == 'A' && isAlpha) ||
					(editValue == 'C' && (isDigit || isAlpha)) ||
					(textValue == *tdeleteText))
				{
					*destination++ = *source = textValue;
					processed = TRUE;
				}
			}
		}
		else if (*tdeleteText == *source)
			source++;

	*destination = '\0';

	// Expand the displayed text to include formatting.
	Expand(text);

	// Return success.
	return (FMI_OK);
}

void *UIW_FORMATTED_STRING::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_FORMATTED_STRING;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_FORMATTED_STRING);

		// Update the internal flag settings.
		UIW_FORMATTED_STRING::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the object and base class flag settings.
		UIW_STRING::Information(I_CHANGED_FLAGS, data, ID_FORMATTED_STRING);

		// Update the internal flag settings.
		UIW_FORMATTED_STRING::UpdateSettings(objectID);
		break;

	case I_SET_TEXT:
		Export(text, TRUE);
		UIW_FORMATTED_STRING::DataSet((ZIL_ICHAR *)data);
		break;

	default:
		data = UIW_STRING::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

int UIW_FORMATTED_STRING::LegalChar(ZIL_ICHAR &textValue, int pos)
{
	int result;
	ZIL_ICHAR editValue = editMask[pos];
	int isAlpha = (textValue == ' ' || IsAlpha(textValue));
	int isDigit = IsDigit(textValue);
	if (IsUpper(editValue))
		textValue = ToUpper(textValue);
	editValue = ToUpper(editValue);
	if ((editValue == 'X' && textValue >= ' ' && textValue <= '~') ||
		(editValue == 'N' && isDigit) ||
		(editValue == 'A' && isAlpha) ||
		(editValue == 'C' && (isDigit || isAlpha)) ||
		(editValue == 'L' && (textValue == deleteText[pos])))
			result = TRUE;
	else
		result = FALSE;

	return (result);
}

void UIW_FORMATTED_STRING::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the environment specific flag settings.
	UIW_FORMATTED_STRING::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_FORMATTED_STRING::NewFunction(void) { return (UIW_FORMATTED_STRING::New); }

UIW_FORMATTED_STRING::UIW_FORMATTED_STRING(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_STRING(0, 0, 10, ZIL_NULLP(ZIL_ICHAR), MAXLEN, STF_NO_FLAGS, WOF_NO_FLAGS)
{
	// Initialize the formatted string information.
	compressedText = new ZIL_ICHAR[MAXLEN+1];
	editMask = new ZIL_ICHAR[MAXLEN+1];
	deleteText = new ZIL_ICHAR[MAXLEN+1];
	UIW_FORMATTED_STRING::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_FORMATTED_STRING::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_FORMATTED_STRING::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the formatted string information.
	UIW_STRING::Load(name, directory, file, objectTable, userTable);
	file->Load(compressedText, MAXLEN);
	file->Load(editMask, MAXLEN);
	file->Load(deleteText, MAXLEN);
}
#endif

#if defined(ZIL_STORE)
void UIW_FORMATTED_STRING::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Make sure information is current.
	DataGet();

	// Store the formatted string information.
	UIW_STRING::Store(name, directory, file, objectTable, userTable);
	file->Store(compressedText);
	file->Store(editMask);
	file->Store(deleteText);
}
#endif
