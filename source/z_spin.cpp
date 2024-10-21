//	Zinc Interface Library - Z_SPIN.CPP
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


#define INCL_WINSTDSPIN         // OS/2 spin button control class
extern "C"
{
#include <stdio.h>
}
#include "ui_win.hpp"

#define ZIL_UIW_SPIN_CONTROL_DATA
#include "img_def.cpp"

// ----- UIW_SPIN_CONTROL ------------------------------------------------------

UIW_SPIN_CONTROL::UIW_SPIN_CONTROL(int left, int top, int width,
	UI_WINDOW_OBJECT *_fieldObject, ZIL_INT32 _delta,
	WNF_FLAGS _wnFlags, WOF_FLAGS _woFlags, ZIL_USER_FUNCTION _userFunction) :
	UIW_WINDOW(left, top, width, 1, _woFlags),
	text(ZIL_NULLP(ZIL_ICHAR)), delta(_delta)
{
	// Initialize the spin control information.
	wnFlags = _wnFlags;
	fieldObject = _fieldObject;
	userFunction = _userFunction;
	UIW_SPIN_CONTROL::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_SPIN_CONTROL::~UIW_SPIN_CONTROL(void)
{
	if (text)
		delete text;
#if defined(ZIL_OS2)
	if (fieldObject)
		delete fieldObject;
#endif
}

void *UIW_SPIN_CONTROL::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_SPIN_CONTROL;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_SPIN_CONTROL);
		font = FNT_DIALOG_FONT;

		// Add the spin fields.
#if defined(ZIL_MSDOS) || defined (ZIL_MOTIF) || defined(ZIL_CURSES) || defined(ZIL_NEXTSTEP)
		upButton = new UIW_SPIN_CONTROL::UIW_SPIN_BUTTON(L_UP);
		downButton = new UIW_SPIN_CONTROL::UIW_SPIN_BUTTON(L_DOWN);
		UIW_WINDOW::Add(upButton);
		UIW_WINDOW::Add(downButton);
		UIW_WINDOW::Add(fieldObject);
#elif defined(ZIL_MSWINDOWS)
#	if !defined(ZIL_WIN32)
		spinButton = new UIW_SPIN_CONTROL::UIW_SPIN_BUTTON();
		UIW_WINDOW::Add(spinButton);
#	endif
		UIW_WINDOW::Add(fieldObject);
#elif defined(ZIL_MACINTOSH)
		littleArrows = ZIL_NULLH(ControlHandle);
		UIW_WINDOW::Add(fieldObject);
#elif defined(ZIL_OS2)
		fieldObject->parent = this;
#endif
		// Continue to I_CHANGED_FLAGS.

	case I_CHANGED_FLAGS:
		// Check the flag settings.
		if (request == I_CHANGED_FLAGS)
			UIW_WINDOW::Information(I_CHANGED_FLAGS, data, ID_SPIN_CONTROL);
		woStatus &= ~WOS_OWNERDRAW;
		woFlags &= ~WOF_BORDER;

		// Make sure fieldObject flags match spin control flags.
		fieldObject->woFlags &= ~(WOF_VIEW_ONLY | WOF_NON_SELECTABLE |
			WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT);
		if (FlagSet(woFlags, WOF_VIEW_ONLY))
			fieldObject->woFlags |= WOF_VIEW_ONLY;
		fieldObject->woAdvancedFlags |= FlagSet(woAdvancedFlags,
			WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP);

#if defined(ZIL_MSDOS) || defined (ZIL_CURSES) || defined (ZIL_MOTIF) || defined(ZIL_NEXTSTEP)
		upButton->woFlags &= ~WOF_NON_SELECTABLE;
		downButton->woFlags &= ~WOF_NON_SELECTABLE;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
		{
			upButton->woFlags |= WOF_NON_SELECTABLE;
			downButton->woFlags |= WOF_NON_SELECTABLE;
			fieldObject->woFlags |= WOF_NON_SELECTABLE;
		}
#elif defined(ZIL_MSWINDOWS)
		dwStyle &= ~WS_BORDER;
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
		{
#	if !defined(ZIL_WIN32)
			spinButton->woFlags |= WOF_NON_SELECTABLE;
#	endif
			fieldObject->woFlags |= WOF_NON_SELECTABLE;
		}
		else
		{
#	if !defined(ZIL_WIN32)
			spinButton->woFlags &= ~WOF_NON_SELECTABLE;
#	endif
			fieldObject->woFlags &= ~WOF_NON_SELECTABLE;
		}
#elif defined(ZIL_MACINTOSH)
		if (FlagSet(woFlags, WOF_NON_SELECTABLE))
		{
			if (littleArrows)
				HiliteControl(littleArrows, 255);
			fieldObject->woFlags |= WOF_NON_SELECTABLE;
		}
		else
		{
			if (littleArrows)
				HiliteControl(littleArrows, 0);
			fieldObject->woFlags &= ~WOF_NON_SELECTABLE;
		}
#endif

		// Check the environment specific flag settings.
#if defined(ZIL_MSDOS) || defined (ZIL_CURSES) || defined (ZIL_MOTIF) || defined(ZIL_NEXTSTEP)
		upButton->Information(request, data);
		downButton->Information(request, data);
		if (request == I_CHANGED_FLAGS)
			fieldObject->Information(request, data);
#elif defined(ZIL_MSWINDOWS)
#	if !defined(ZIL_WIN32)
		spinButton->Information(request, data);
#	endif
		if (request == I_CHANGED_FLAGS)
			fieldObject->Information(request, data);
#elif defined(ZIL_MACINTOSH)
		if (request == I_CHANGED_FLAGS)
			fieldObject->Information(request, data);
#elif defined(ZIL_OS2)
#endif

		// See if the field needs to be re-computed.
		if (objectID == ID_SPIN_CONTROL && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_MACINTOSH)
			event.type = S_REDISPLAY;
			Event(event);
#endif
		}
		break;

	case I_GET_VALUE:
	case I_GET_TEXT:
	case I_COPY_TEXT:
		data = fieldObject->Information(request, data);
		break;

	case I_SET_VALUE:
	case I_SET_TEXT:
	case I_INCREMENT_VALUE:
	case I_DECREMENT_VALUE:
		{
		data = fieldObject->Information(request, data);
#if defined(ZIL_OS2)
		ZIL_ICHAR *fieldText;
		fieldObject->Information(I_GET_TEXT, &fieldText);
#	if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(fieldText, ZIL_NULLP(char), FALSE);
#	else
#		define TEXT_ fieldText
#	endif
		WinSendMsg(screenID, SPBM_SETARRAY, (MPARAM)&TEXT_, (MPARAM)1);
#endif
		}
		break;

	case I_PRINT_INFORMATION:
	case I_PRINT_USER_FUNCTION:
		UI_WINDOW_OBJECT::Information(request, data, objectID);
		if (fieldObject)
			fieldObject->Information(request, data, objectID);
		break;

	case I_GET_FIRST:
	case I_GET_LAST:
	case I_GET_CURRENT:
		if (!data)
			return (fieldObject);
		*(UI_WINDOW_OBJECT **)data = fieldObject;
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_MOTIF) || (defined(ZIL_MSWINDOWS) && !defined(ZIL_WIN32)) || defined(ZIL_NEXTSTEP)
void *UIW_SPIN_CONTROL::UIW_SPIN_BUTTON::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
#	if !defined(ZIL_MSWINDOWS)
		if (!defaultInitialized)
		{
			ZIL_DECORATION_MANAGER::SetDecorations(UIW_SPIN_CONTROL::_className, TEXT_TABLE, BITMAP_TABLE);
			defaultInitialized = TRUE;
		}
		myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(UIW_SPIN_CONTROL::_className);
#	endif
		// Fall onto I_CHANGED_FLAGS
	case I_CHANGED_FLAGS:
		if (!display->isText)
		{
#	if !defined(ZIL_TEXT_ONLY)
			if (FlagSet(woFlags, WOF_NON_SELECTABLE))
			{
#		if defined(ZIL_MSDOS)
				if (colorBitmap)
				{
					delete colorBitmap;
					colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
				}
				if (monoBitmap)
				{
					delete monoBitmap;
					monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
				}
#		endif
#		if !defined(ZIL_MSWINDOWS)
				bitmapArray = myDecorations->GetBitmap(direction == L_UP ?
					ZIL_SpinUpArrowDisabled : ZIL_SpinDownArrowDisabled);
#		endif
			}
#		if !defined(ZIL_MSWINDOWS)
			else
			{
#			if defined(ZIL_MSDOS)
				if (colorBitmap)
				{
					delete colorBitmap;
					colorBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
				}
				if (monoBitmap)
				{
					delete monoBitmap;
					monoBitmap = ZIL_NULLH(ZIL_BITMAP_HANDLE);
				}
#			endif
				bitmapArray = myDecorations->GetBitmap(direction == L_UP ?
					ZIL_SpinUpArrow : ZIL_SpinDownArrow);
			}
			btFlags |= BTF_STATIC_BITMAPARRAY;
			bitmapWidth = bitmapArray[0];
			bitmapHeight = bitmapArray[1];
			bitmapArray = &bitmapArray[2];
#		endif
#	endif
		}
#	if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
		else
		{
			text = myDecorations->GetText(direction == L_UP ?
				ZIL_SpinUpArrow : ZIL_SpinDownArrow);
			bitmapArray = ZIL_NULLP(ZIL_UINT8);
		}
#	endif
		break;

	default:
		data = UIW_BUTTON::Information(request, data, objectID);
		break;
	}

	return (data);
}
#endif

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_SPIN_CONTROL::NewFunction(void) { return (UIW_SPIN_CONTROL::New); }

UIW_SPIN_CONTROL::UIW_SPIN_CONTROL(const ZIL_ICHAR *name,
	ZIL_STORAGE_READ_ONLY *directory, ZIL_STORAGE_OBJECT_READ_ONLY *file,
	UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 20, 1, WOF_NO_FLAGS),
	text(ZIL_NULLP(ZIL_ICHAR))
{
	// Initialize the text information.
	UIW_SPIN_CONTROL::Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_SPIN_CONTROL::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_SPIN_CONTROL::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the spin control information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	fieldObject = UI_WINDOW_OBJECT::New(ZIL_NULLP(ZIL_ICHAR),	directory, file,
		objectTable, userTable);
	file->Load(&wnFlags);
	file->Load(&delta);
}
#endif

#if defined(ZIL_STORE)
void UIW_SPIN_CONTROL::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the spin control information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(fieldObject->SearchID());
	fieldObject->Store(name, directory, file, objectTable, userTable);
	file->Store(wnFlags);
	file->Store(delta);
}
#endif

