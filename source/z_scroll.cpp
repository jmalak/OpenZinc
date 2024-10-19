//	Zinc Interface Library - Z_SCROLL.CPP
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

static ZIL_ICHAR _hzStringID[] = { 'N','U','M','I','D','_','H','Z','_','S','C','R','O','L','L', 0 };
static ZIL_ICHAR _vtStringID[] = { 'N','U','M','I','D','_','V','T','_','S','C','R','O','L','L', 0 };
static ZIL_ICHAR _cStringID[] = { 'N','U','M','I','D','_','C','_','S','C','R','O','L','L', 0 };

// ----- UIW_SCROLL_BAR -----------------------------------------------------

UIW_SCROLL_BAR::UIW_SCROLL_BAR(int left, int top, int width, int height,
	SBF_FLAGS _sbFlags, WOF_FLAGS _woFlags) :
	UIW_WINDOW(left, top, width, height, _woFlags), sbFlags(_sbFlags)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	lButton = rButton = mButton = ZIL_NULLP(UIW_SCROLL_BUTTON);
#endif

	// Initialize the slide information.
	UIW_SCROLL_BAR::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	scroll.minimum = scroll.current = 0;
	scroll.maximum = 1;
}

UIW_SCROLL_BAR::UIW_SCROLL_BAR(int left, int top, int width, int height,
	UI_SCROLL_INFORMATION *_scroll, SBF_FLAGS _sbFlags,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags, ZIL_USER_FUNCTION _userFunction) :
	UIW_WINDOW(left, top, width, height, _woFlags, _woAdvancedFlags),
	sbFlags(_sbFlags)
{
	// Initialize the slide information.
	UIW_SCROLL_BAR::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	scroll = *_scroll;
	userFunction = _userFunction;
}

UIW_SCROLL_BAR::~UIW_SCROLL_BAR(void)
{
	// No scroll-bar information needs to be destroyed.
}

void *UIW_SCROLL_BAR::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_SCROLL_BAR;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		searchID = WindowID(ID_SCROLL_BAR);
		scroll.delta = scroll.showing = 1;

		// Update the internal flag settings.
		UIW_SCROLL_BAR::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Check the base class flag settings.
		UIW_WINDOW::Information(I_CHANGED_FLAGS, data, ID_SCROLL_BAR);

		// Update the internal flag settings.
		UIW_SCROLL_BAR::UpdateSettings(objectID);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		// Get, set or clear the flag settings.
		if (objectID && objectID != ID_SCROLL_BAR)
			data = UI_WINDOW_OBJECT::Information(request, data, objectID);
		else if (request == I_GET_FLAGS && !data)
			data = &sbFlags;
		else if (request == I_GET_FLAGS)
			*(SBF_FLAGS *)data = sbFlags;
		else if (request == I_SET_FLAGS)
			sbFlags |= *(SBF_FLAGS *)data;
		else
			sbFlags &= ~(*(SBF_FLAGS *)data);
		break;

	case I_SET_VALUE:
		{
		int delta = *(int *)data - scroll.current;
		UI_EVENT scrollEvent;
		scrollEvent.type = FlagSet(sbFlags, SBF_VERTICAL) ? S_VSCROLL : S_HSCROLL;
		scrollEvent.scroll.delta = delta;
		Event(scrollEvent);
		}
		break;

	case I_GET_VALUE:
		*(int *)data = scroll.current;
		break;

	case I_PRINT_INFORMATION:
	case I_PRINT_USER_FUNCTION:
		UI_WINDOW_OBJECT::Information(request, data, objectID);
		break;

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

void UIW_SCROLL_BAR::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Check the general flag settings.
	if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
	{
		woAdvancedFlags |= WOAF_NON_CURRENT;
		if (FlagSet(sbFlags, SBF_HORIZONTAL))
		{
			numberID = NUMID_HZ_SCROLL;
			StringID(_hzStringID);
		}
		else if (FlagSet(sbFlags, SBF_VERTICAL))
		{
			numberID = NUMID_VT_SCROLL;
			StringID(_vtStringID);
		}
		else
		{
			numberID = NUMID_C_SCROLL;
			StringID(_cStringID);
		}
	}
	if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
		woFlags |= WOF_JUSTIFY_RIGHT;

	// Check the environment specific flag settings.
	UIW_SCROLL_BAR::OSUpdateSettings(objectID);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_SCROLL_BAR::NewFunction(void) { return (UIW_SCROLL_BAR::New); }

UIW_SCROLL_BAR::UIW_SCROLL_BAR(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 2, 6, WOF_NO_FLAGS)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	lButton = rButton = mButton = ZIL_NULLP(UIW_SCROLL_BUTTON);
#endif

	// Initialize the slide information.
	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_SCROLL_BAR::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_SCROLL_BAR::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Load the scroll-bar information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	file->Load(&sbFlags);
	ZIL_INT16 _value; file->Load(&_value); scroll.minimum = _value;
	file->Load(&_value); scroll.maximum = _value;
	file->Load(&_value); scroll.current = _value;
}
#endif

#if defined(ZIL_STORE)
void UIW_SCROLL_BAR::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the scroll-bar information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(sbFlags);
	ZIL_INT16 _value = scroll.minimum; file->Store(_value);
	_value = scroll.maximum; file->Store(_value);
	_value = scroll.current; file->Store(_value);
}
#endif
