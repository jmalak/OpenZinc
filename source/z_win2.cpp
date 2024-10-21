//	Zinc Interface Library - Z_WIN2.CPP
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


#define INCL_WINBUTTONS
#define INCL_WINMENUS
#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#	include <stdio.h>
#	include <stdlib.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#include "ui_win.hpp"


extern ZIL_ICHAR *_idFormat;
extern ZIL_ICHAR *_zilExtName;

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

UI_WINDOW_OBJECT::UI_WINDOW_OBJECT(int left, int top, int width, int height,
	WOF_FLAGS _woFlags, WOAF_FLAGS _woAdvancedFlags) : woFlags(_woFlags),
	woAdvancedFlags(_woAdvancedFlags), eventMapTable(ZIL_NULLP(UI_EVENT_MAP)),
	hotKeyMapTable(ZIL_NULLP(UI_EVENT_MAP)), paletteMapTable(ZIL_NULLP(UI_PALETTE_MAP))
{
#ifdef ZIL_EDIT
	designerAdvancedFlags = 0;
#endif
	numberID = 0;
	stringID[0] = '\0';
	userFlags = UIF_NO_FLAGS;
	userStatus = UIS_NO_FLAGS;
	userObject = ZIL_NULLP(void);
	userFunction = ZIL_NULLF(ZIL_USER_FUNCTION);
	userObjectName = ZIL_NULLP(ZIL_ICHAR);
	userFunctionName = ZIL_NULLP(ZIL_ICHAR);
	helpContext = NO_HELP_CONTEXT;
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));

	// Intialize the screen object information.
	trueRegion.left = relative.left = left;
	trueRegion.top = relative.top = top;

	trueRegion.right = relative.right = (left >= 0 && width > 0) ?
		left + width - 1 : width;
	trueRegion.bottom = relative.bottom = (top >= 0 && height > 0) ?
		(height == 1) ? top : top + height - 1 : height;
}

UI_WINDOW_OBJECT::~UI_WINDOW_OBJECT(void)
{
	if (userObjectName)
		delete userObjectName;
	if (userFunctionName)
		delete userFunctionName;
}

ZIL_LOGICAL_FONT UI_WINDOW_OBJECT::Font(ZIL_LOGICAL_FONT _font)
{
	ZIL_LOGICAL_FONT fontFlags = font & FNT_IGNORE_UNDERSCORE;
	if (_font != FNT_NONE)
		font = _font | fontFlags;
	return (font);
}

unsigned UI_WINDOW_OBJECT::HotKey(unsigned _hotKey)
{
	if (_hotKey)
		hotKey = _hotKey;
	return (hotKey);
}

unsigned UI_WINDOW_OBJECT::HotKey(ZIL_ICHAR *text)
{
	hotKey = 0;
	if (text)
	{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
		int length = strlen(text);
		ZIL_ICHAR *hotChar = text;
		for (int i = 0; i < length; i++)
			if ((*hotChar == ZIL_UNDERLINE_CHARACTER && hotChar[1] == ZIL_UNDERLINE_CHARACTER) ||
				(*hotChar == ZIL_OLD_UNDERLINE_CHARACTER && hotChar[1] == ZIL_OLD_UNDERLINE_CHARACTER))
				hotChar++, i++;
			else if (*hotChar == ZIL_UNDERLINE_CHARACTER || *hotChar == ZIL_OLD_UNDERLINE_CHARACTER)
			{
				hotKey = ToLower(hotChar[1]);
				break;
			}
			else hotChar++;
#else
		strrepc(text, ZIL_OLD_UNDERLINE_CHARACTER, ZIL_UNDERLINE_CHARACTER);
		for (ZIL_ICHAR *hotChar = strchr(text, ZIL_UNDERLINE_CHARACTER); !hotKey && hotChar;
			hotChar = strchr(++hotChar, ZIL_UNDERLINE_CHARACTER))
			if (hotChar[1] != ZIL_UNDERLINE_CHARACTER)
			{
#if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MACINTOSH) || defined(ZIL_NEXTSTEP)
				hotKey = (unsigned char)ToLower(hotChar[1]);
#elif defined(ZIL_MOTIF)
				hotKey = hotChar[1];
#endif
				break;
			}
			else
				hotChar++;
#endif
	}
	return (hotKey);
}

int UI_WINDOW_OBJECT::NeedsUpdate(const UI_EVENT &, EVENT_TYPE)
{
	return (TRUE);
}

void *UI_WINDOW_OBJECT::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_WINDOW_OBJECT;
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and initial settings.
		searchID = windowID[0] = windowID[1] = windowID[2] = windowID[3] =
			windowID[4] = ID_WINDOW_OBJECT;
		screenID = 0;
		parent = ZIL_NULLP(UI_WINDOW_OBJECT);
		lastPalette = ZIL_NULLP(UI_PALETTE);

		hotKey = 0;
		woStatus = defaultStatus;
		font = FNT_DIALOG_FONT;
		extern UI_EVENT_MAP *_eventMapTable;
		if (!eventMapTable)
			eventMapTable = _eventMapTable;
		extern UI_EVENT_MAP *_hotKeyMapTable;
		if (!hotKeyMapTable)
			hotKeyMapTable = _hotKeyMapTable;
		extern UI_PALETTE_MAP *_normalPaletteMapTable;
		if (!paletteMapTable)
			paletteMapTable = _normalPaletteMapTable;
		if (FlagSet(woFlags, WOF_INVALID))
			woStatus |= WOS_INVALID;
		if (FlagSet(woFlags, WOF_UNANSWERED))
			woStatus |= WOS_UNANSWERED;

		// Set the environment specific flags.
		UI_WINDOW_OBJECT::OSInitialize();

		// Update the internal flag settings.
		UI_WINDOW_OBJECT::UpdateSettings(objectID);
		break;

	case I_CHANGED_FLAGS:
		// Update the internal flag settings.
		UI_WINDOW_OBJECT::UpdateSettings(objectID);
		break;

	case I_GET_NUMBERID_OBJECT:
		data = (numberID == *(ZIL_NUMBERID *)data) ? this : ZIL_NULLP(void);
		break;

	case I_GET_STRINGID_OBJECT:
		data = stricmp(stringID, (ZIL_ICHAR *)data) == 0 ? this : ZIL_NULLP(void);
		break;

	case I_GET_FLAGS:
	case I_SET_FLAGS:
	case I_CLEAR_FLAGS:
		if (objectID && objectID != ID_WINDOW_OBJECT)
			return (ZIL_NULLP(void));
		else if (request == I_GET_FLAGS && !data)
			data = &woFlags;
		else if (request == I_GET_FLAGS)
			*(WOF_FLAGS *)data = woFlags;
		else if (request == I_SET_FLAGS)
			woFlags |= *(WOF_FLAGS *)data;
		else
			woFlags &= ~(*(WOF_FLAGS *)data);
		break;

	case I_GET_STATUS:
	case I_SET_STATUS:
	case I_CLEAR_STATUS:
		if (objectID && objectID != ID_WINDOW_OBJECT)
			return (ZIL_NULLP(void));
		else if (request == I_GET_STATUS && !data)
			data = &woStatus;
		else if (request == I_GET_STATUS)
			*(WOS_STATUS *)data = woStatus;
		else if (request == I_SET_STATUS)
			woStatus |= *(WOS_STATUS *)data;
		else if (request == I_CLEAR_STATUS)
			woStatus &= ~(*(WOS_STATUS *)data);
		break;

	case I_CHANGED_STATUS:
#if defined(ZIL_MSWINDOWS)
		if (parent && parent->Inherited(ID_LIST))
		{
			SendMessage(screenID, WM_SETREDRAW, FALSE, 0);
			LRESULT tIndex = SendMessage(screenID, LB_GETTOPINDEX, 0, 0);
			SendMessage(screenID, LB_SETSEL, FlagSet(woStatus, WOS_SELECTED), ListIndex());
			SendMessage(screenID, WM_SETREDRAW, TRUE, 0);
			SendMessage(screenID, LB_SETTOPINDEX, tIndex, 0);
		}
#endif
		Event(UI_EVENT(S_REDISPLAY, 0));
		break;

#if defined(ZIL_MSWINDOWS)
	case I_GET_DWSTYLE:		*(DWORD *)data = dwStyle;			break;
	case I_SET_DWSTYLE:		dwStyle |= *(DWORD *)data;			break;
	case I_CLEAR_DWSTYLE:		dwStyle &= ~(*(DWORD *)data);	break;
#elif defined(ZIL_OS2)
	case I_GET_FLSTYLE:		*(ULONG *)data = flStyle;			break;
	case I_SET_FLSTYLE:		flStyle |= *(ULONG *)data;			break;
	case I_CLEAR_FLSTYLE:		flStyle &= ~(*(ULONG *)data);	break;

	case I_GET_FLFLAG:		*(ULONG *)data = flFlag;			break;
	case I_SET_FLFLAG:		flFlag |= *(ULONG *)data;			break;
	case I_CLEAR_FLFLAG:		flFlag &= ~(*(ULONG *)data);	break;
#endif

	case I_GET_FRAMEID:
		*(ZIL_SCREENID *)data = screenID;						break;

#if defined(ZIL_EDIT)
	case I_PRINT_INFORMATION:
		if (data && stringID[0] != '\0' && numberID < (ZIL_NUMBERID)0xFF00)
		{
			strupr(stringID);
			ZIL_STORAGE_OBJECT *hppEntry = (ZIL_STORAGE_OBJECT *)data;
			ZIL_ICHAR buffer[128];
			sprintf(buffer, _idFormat, stringID, numberID);
			hppEntry->Store(buffer);
		}
		break;
#endif

#if defined(ZIL_STORE)
	case I_PRINT_USER_FUNCTION:
		if (userFunctionName && *userFunctionName)
		{
			ZIL_STORAGE_OBJECT *cppEntry = (ZIL_STORAGE_OBJECT *)data;
			cppEntry->Store(userFunctionName);
		}
		break;
#endif

	default:
		data = ZIL_NULLP(void);
		break;
	}

	// Return the information.
	return (data);
}

int UI_WINDOW_OBJECT::Inherited(ZIL_OBJECTID matchID)
{
	if (matchID == windowID[0] ||
		matchID == windowID[1] ||
		matchID == windowID[2] ||
		matchID == windowID[3] ||
		matchID == windowID[4])
		return (TRUE);
	return (FALSE);
}

EVENT_TYPE UI_WINDOW_OBJECT::LogicalEvent(const UI_EVENT &event, ZIL_OBJECTID currentID, int nativeType)
{
	// See if there is a matching identification.
	if (!currentID)
		currentID = windowID[0];
#if defined(ZIL_NEXTSTEP)
	// This is because NEXTSTEP uses id.
	ZIL_OBJECTID idArray[5];
	for (int i = 0; i < 5; i++)
		if (currentID == windowID[i])
			break;
	idArray[0] = currentID;
	for (int j = 1; j < 5; j++)
		idArray[j] = (i < 4) ? windowID[++i] : ID_WINDOW_OBJECT;

	EVENT_TYPE value = UI_EVENT_MAP::MapEvent(eventMapTable, event, idArray[0], idArray[1], idArray[2], idArray[3], idArray[4]);
	if (!nativeType && value <= E_DEVICE_LAST)
		return (event.InputType());
	return (value);
#else
	ZIL_OBJECTID id[5];
	int i = 0;
	for (; i < 5; i++)
		if (currentID == windowID[i])
			break;
	id[0] = currentID;
	for (int j = 1; j < 5; j++)
		id[j] = (i < 4) ? windowID[++i] : ID_WINDOW_OBJECT;

	EVENT_TYPE value = UI_EVENT_MAP::MapEvent(eventMapTable, event, id[0], id[1], id[2], id[3], id[4]);
	if (!nativeType && value <= E_DEVICE_LAST)
		return (event.InputType());
	return (value);
#endif
}

UI_PALETTE *UI_WINDOW_OBJECT::LogicalPalette(ZIL_LOGICAL_EVENT logicalEvent, ZIL_OBJECTID currentID)
{
// Temporary method for supporting Windows 95 dialog colors.
#if defined(ZIL_WIN32)
	if (FlagSet(woAdvancedFlags, WOAF_DIALOG_OBJECT))
	{
		static UI_PALETTE dialogPalette = 
			{ ' ', attrib(BLACK, LIGHTGRAY), attrib(MONO_NORMAL, MONO_BLACK),
				PTN_SYSTEM_COLOR, COLOR_WINDOWTEXT, COLOR_BTNFACE, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE };
		return (&dialogPalette);
	}
#endif
	// Determine the logical palette.
	ZIL_LOGICAL_PALETTE logicalPalette = PM_ANY;
	if (FlagSet(woFlags, WOF_NON_SELECTABLE))
		logicalPalette = PM_NON_SELECTABLE;
	else if (logicalEvent == S_CURRENT && FlagSet(woStatus, WOS_CURRENT))
		logicalPalette = PM_CURRENT;
	else if (logicalEvent == S_CURRENT || logicalEvent == S_DISPLAY_ACTIVE ||
		logicalEvent == S_NON_CURRENT ||
		(parent && FlagSet(parent->woStatus, WOS_CURRENT)))
		logicalPalette = PM_ACTIVE;
	if (FlagSet(woStatus, WOS_SELECTED) && !FlagSet(woFlags, WOF_NON_SELECTABLE))
		logicalPalette |= PM_SELECTED;
	if (currentID == ID_HOT_KEY)
	{
		logicalPalette |= PM_HOT_KEY;
		currentID = 0;
	}

	// See if there is a matching identification.
	if (!currentID)
		currentID = windowID[0];

#if defined(ZIL_NEXTSTEP)
	// This is because NEXTSTEP uses id.
	ZIL_OBJECTID idArray[5];
	for (int i = 0; i < 5; i++)
		if (currentID == windowID[i])
			break;
	idArray[0] = currentID;
	for (int j = 1; j < 5; j++)
		idArray[j] = (i < 4) ? windowID[++i] : ID_WINDOW_OBJECT;

	return (UI_PALETTE_MAP::MapPalette(paletteMapTable, logicalPalette, idArray[0], idArray[1], idArray[2], idArray[3], idArray[4]));
#else
	ZIL_OBJECTID id[5];
	int i = 0;
	for (; i < 5; i++)
		if (currentID == windowID[i])
			break;
	id[0] = currentID;
	for (int j = 1; j < 5; j++)
		id[j] = (i < 4) ? windowID[++i] : ID_WINDOW_OBJECT;

	return (UI_PALETTE_MAP::MapPalette(paletteMapTable, logicalPalette, id[0], id[1], id[2], id[3], id[4]));
#endif
}

#if defined(ZIL_MOTIF) || defined(ZIL_NEXTSTEP)
void UI_WINDOW_OBJECT::NormalizePosition(UI_WINDOW_OBJECT *object, const UI_EVENT &event, UI_POSITION &destination)
{
	if (event.InputType() == E_MOUSE)
	{
		if (object->parent)
		{
			destination.line += object->trueRegion.top;
			destination.column += object->trueRegion.left;
		}
	}
}
#elif defined(ZIL_MACINTOSH)
void UI_WINDOW_OBJECT::NormalizePosition(UI_WINDOW_OBJECT *object, const UI_EVENT &event, UI_POSITION &destination)
{
	if (event.InputType() == E_MOUSE)
	{
		// Mouse coordinates are global, and need to be converted to local coordinates.
		GrafPtr savePort;
		GetPort(&savePort);

		for (; object; object = object->parent)
			if (FlagsSet(object->woStatus, WOS_MACINTOSH_WINDOW | WOS_SYSTEM_OBJECT))
			{
				SetPort(object->windowScreenID);
				break;
			}

		ZIL_DEFINE_POINT(mPoint, destination.line, destination.column);
		GlobalToLocal(&mPoint);
		destination.Assign(mPoint);
		SetPort(savePort);
	}
}
#else
void UI_WINDOW_OBJECT::NormalizePosition(UI_WINDOW_OBJECT *, const UI_EVENT &, UI_POSITION &)
{
}
#endif

void UI_WINDOW_OBJECT::NormalizeString(ZIL_ICHAR *destination, const char *source)
{
#	if defined(ZIL_UNICODE)
	ZIL_INTERNATIONAL::ISOtoUNICODE(source, destination);
#	else
	strcpy(destination, source);
#	endif
}

ZIL_NUMBERID UI_WINDOW_OBJECT::NumberID(ZIL_NUMBERID _numberID)
{
	if (_numberID)
		numberID = _numberID;
	return (numberID);
}

EVENT_TYPE UI_WINDOW_OBJECT::RedisplayType(void)
{
	UI_WINDOW_OBJECT *root = Root();
	EVENT_TYPE ccode;

	if (FlagSet(woStatus, WOS_CURRENT) && FlagSet(root->woStatus, WOS_CURRENT))
		ccode = S_CURRENT;
	else if (FlagSet(root->woStatus, WOS_CURRENT))
		ccode = S_DISPLAY_ACTIVE;
	else
		ccode = S_DISPLAY_INACTIVE;

	return ccode;
}

void UI_WINDOW_OBJECT::RegionConvert(UI_REGION &region, int absolute)
{
	// Check for a convertable region.
	if (FlagSet(woFlags, WOF_PIXEL))
		woStatus |= WOS_GRAPHICS;
	if ((display->isText && !FlagSet(woFlags, WOF_MINICELL)) ||
		FlagSet(woStatus, WOS_GRAPHICS))
	{
		if (relative.right < relative.left && relative.right < 0)
			region.right += display->columns;
		if (relative.bottom < relative.top && relative.bottom < 0)
			region.bottom += display->lines;
		if (FlagSet(woStatus, WOS_GRAPHICS) && region.top == region.bottom)
		{
			if (Inherited(ID_BUTTON))
				region.top = region.bottom - display->cellHeight + display->preSpace + display->postSpace + 1;
			else
				region.bottom = region.top + display->cellHeight - display->preSpace - display->postSpace - 1;
		}
		if (display->isText && FlagSet(woStatus, WOS_GRAPHICS))
		{
			// Do estimated conversion if graphics status set in text mode.
			region.left /= 7;
			region.right /= 7;
			region.top /= 21;
			region.bottom /= 21;
			woStatus &= ~WOS_GRAPHICS;
		}
		return;
	}

	// Convert the coordinates.
	int cellWidth = display->cellWidth, cellHeight = display->cellHeight;
	if (FlagSet(woFlags, WOF_MINICELL))
	{
		long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
		long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;
		if (display->isText)
		{
			int width = (int)((miniNX * region.Width() + miniDX / 2) / miniDX) - 1;
			if (width < 0)
				width = 0;
			int height = (int)((miniNY * region.Height() + miniDY / 2) / miniDY) - 1;
			if (height < 0)
				height = 0;
			region.left = (int)((miniNX * region.left + miniDX / 2) / miniDX);
			region.top = (int)((miniNY * region.top + miniDY / 2) / miniDY);
			region.right = region.left + width;
			region.bottom = region.top + height;
			woFlags &= ~WOF_MINICELL;
		}
#if !defined(ZIL_TEXT_ONLY)
		else
		{
			int width = (int)((miniNX * region.Width() * cellWidth + miniDX / 2) / miniDX);
			int height = (int)((miniNY * region.Height() * cellHeight + miniDY / 2) / miniDY);
			region.left = (int)((miniNX * region.left * cellWidth + miniDX / 2) / miniDX);
			region.top = (int)((miniNY * region.top * cellHeight + miniDY / 2) / miniDY);
			if (width < 0)
				region.right = (int)((miniNX * region.right * cellWidth + miniDX / 2) / miniDX);
			else
				region.right = region.left + width - 1;
			if (height < 0)
				region.bottom = (int)((miniNY * region.bottom * cellHeight + miniDY / 2) / miniDY);
			else
				region.bottom = region.top + height - 1;
		}
#endif
	}
	else
	{
		region.left = region.left * cellWidth;
		region.top = region.top * cellHeight;
		region.right = region.right * cellWidth + cellWidth - 1;
		region.bottom = region.bottom * cellHeight + cellHeight - 1;
	}

	// Check for absolute coordinates.
	if (!absolute && !FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL))
	{
		region.top += display->preSpace;
		region.bottom -= display->postSpace;
	}

#if !defined(ZIL_TEXT_ONLY)
	if (!display->isText)
	{
		// Check for negative values.
		if (region.left < 0)
			region.left += display->columns;
		if (region.top < 0)
			region.top += display->lines;
		if (region.right < 0)
			region.right += display->columns;
		if (region.bottom < 0)
			region.bottom += display->lines;

		// Update the object status.
		woStatus |= WOS_GRAPHICS;
	}
#endif
}

void UI_WINDOW_OBJECT::RegionMax(UI_WINDOW_OBJECT *object)
{
	object->trueRegion = object->clip = relative;
}

void UI_WINDOW_OBJECT::RegisterObject(char *className)
{
	// Call the object's event for actual registration.
	UI_EVENT event(S_REGISTER_OBJECT);
	event.data = className;
	Event(event);
}

UI_WINDOW_OBJECT *UI_WINDOW_OBJECT::Root(int mdiChild)
{
	UI_WINDOW_OBJECT *object = this;
	for (; object->parent && (!mdiChild || !FlagSet(object->woAdvancedFlags, WOAF_MDI_OBJECT));
		object = object->parent)
			;
	return (object);
}

ZIL_ICHAR *UI_WINDOW_OBJECT::StringID(const ZIL_ICHAR *_stringID)
{
	if (_stringID)
		strcpy(stringID, _stringID);
	return (stringID);
}

void UI_WINDOW_OBJECT::TruetoRelative(UI_WINDOW_OBJECT *object, const UI_POSITION &trueRegion, UI_POSITION &relative)
{
		UI_REGION clipRegion;
		object->Information(I_GET_CLIPREGION, &clipRegion);
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
		relative.column = trueRegion.column - clipRegion.left;
		relative.line = trueRegion.line - clipRegion.top;
#elif defined(ZIL_MOTIF) || defined(ZIL_NEXTSTEP)
		relative.column = trueRegion.column;
		relative.line = trueRegion.line;
#elif defined(ZIL_MACINTOSH)
		if (FlagsSet(object->woStatus, WOS_MACINTOSH_WINDOW | WOS_SYSTEM_OBJECT))
		{
			relative.column = trueRegion.column - clipRegion.left - object->trueRegion.left;
			relative.line = trueRegion.line - clipRegion.top - object->trueRegion.top;
		}
		else
		{
			GrafPtr savePort;
			GetPort(&savePort);
			for (UI_WINDOW_OBJECT *pObject = object; pObject; pObject = pObject->parent)
				if (FlagSet(pObject->woStatus, WOS_MACINTOSH_WINDOW))
				{
					SetPort(pObject->windowScreenID);
					break;
				}

			ZIL_DEFINE_POINT(localPoint, trueRegion.line, trueRegion.column);
			GlobalToLocal(&localPoint);
			SetPort(savePort);

			relative.column = localPoint.h - clipRegion.left;
			relative.line = localPoint.v - clipRegion.top;
		}
#else
		relative.column = trueRegion.column - clipRegion.left - object->trueRegion.left;
		relative.line = trueRegion.line - clipRegion.top - object->trueRegion.top;
#endif
}

void UI_WINDOW_OBJECT::UpdateSettings(ZIL_OBJECTID objectID)
{
	// Set the environment specific flags.
	woStatus &= ~WOS_REDISPLAY;

	// Update the environment specific flag settings.
	UI_WINDOW_OBJECT::OSUpdateSettings(objectID);
}

EVENT_TYPE UI_WINDOW_OBJECT::UserFunction(const UI_EVENT &event, EVENT_TYPE action)
{
	if (FlagSet(woStatus, WOS_INTERNAL_ACTION | WOS_EDIT_MODE))
		return (0);

	woStatus |= WOS_INTERNAL_ACTION;	// Prevent recursive calls.
	EVENT_TYPE ccode = action;
	if (ccode == L_SELECT && !FlagSet(woFlags, WOF_NON_SELECTABLE))
	{
		BTF_FLAGS btFlags = BTF_NO_FLAGS;
		Information(I_GET_FLAGS, &btFlags, ID_BUTTON);
		if (parent && !FlagSet(btFlags, BTF_NO_TOGGLE))
		{
			WNF_FLAGS wnFlags = WNF_NO_FLAGS;
			parent->Information(I_GET_FLAGS, &wnFlags, ID_WINDOW);
			if (FlagSet(wnFlags, WNF_SELECT_MULTIPLE))
				woStatus ^= WOS_SELECTED;
			else
				woStatus |= WOS_SELECTED;
		}
		if (parent)
			parent->Information(I_CHECK_SELECTION, this);
	}

	if (userFunction && !FlagSet(woFlags, WOF_NON_SELECTABLE))
	{
		UI_EVENT uEvent = event;
		ccode = (*userFunction)(this, uEvent, ccode);
	}
	else if (ccode == S_NON_CURRENT || ccode == L_SELECT)
		ccode = (Validate(TRUE) == 0) ? 0 : -1;
	woStatus &= ~WOS_INTERNAL_ACTION;
	return (ccode);
}

ZIL_OBJECTID UI_WINDOW_OBJECT::WindowID(ZIL_OBJECTID _windowID)
{
	// Check the searchID.
	if (!_windowID)
		return (windowID[0]);

	// Shift the object's windowID.
	if (_windowID != windowID[0] &&
		_windowID != windowID[1] &&
		_windowID != windowID[2] &&
		_windowID != windowID[3] &&
		_windowID != windowID[4])
	{
		windowID[4] = windowID[3];
		windowID[3] = windowID[2];
		windowID[2] = windowID[1];
		windowID[1] = windowID[0];
		windowID[0] = _windowID;
	}
	return (_windowID);
}

int UI_WINDOW_OBJECT::Validate(int)
{
	return (0);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UI_WINDOW_OBJECT::NewFunction(void) { return (UI_WINDOW_OBJECT::New); }

UI_WINDOW_OBJECT::UI_WINDOW_OBJECT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	eventMapTable(ZIL_NULLP(UI_EVENT_MAP)), hotKeyMapTable(ZIL_NULLP(UI_EVENT_MAP)),
	paletteMapTable(ZIL_NULLP(UI_PALETTE_MAP))
{
	// Intialize the window object information.
	userFlags = UIF_NO_FLAGS;
	userStatus = UIS_NO_FLAGS;
	userObject = ZIL_NULLP(void);
	userFunction = ZIL_NULLF(ZIL_USER_FUNCTION);
	helpContext = NO_HELP_CONTEXT;
	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

#if defined(__HIGHC__)
void UI_WINDOW_OBJECT::Load(const ZIL_ICHAR *arg1, ZIL_STORAGE_READ_ONLY *arg2,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *, UI_ITEM *userTable)
#else
void UI_WINDOW_OBJECT::Load(const ZIL_ICHAR *, ZIL_STORAGE_READ_ONLY *,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *, UI_ITEM *userTable)
#endif
{
	if (!userTable)
		userTable = UI_WINDOW_OBJECT::userTable;

	ZIL_INT16 value;
#if defined(CONVERT_V200)
	struct ZIL_EXPORT_CLASS UI_SEARCH_INFO
	{
		// Fields described in UI_SEARCH_ELEMENT reference chapter.
		ZIL_UINT16 type;
		ZIL_UINT16 numberID;
		ZIL_INT8 stringID[32];
		long offset;
		ZIL_UINT16 size;
	} search;

	file->Load(&search, sizeof(search));
	numberID = search.numberID;
	strcpy(string, search.stringID);
#else
	file->Load(&numberID);
	file->Load(stringID, 32);
#endif
	file->Load(&woFlags);
#if defined (ZIL_EDIT)
	if (FlagSet(defaultStatus, WOS_EDIT_MODE))
		file->Load(&designerAdvancedFlags);
	else
		file->Load(&woAdvancedFlags);
#else
	file->Load(&woAdvancedFlags);
#endif
	file->Load(&value); relative.left = value;
	file->Load(&value); relative.top = value;
	file->Load(&value); relative.right = value;
	file->Load(&value); relative.bottom = value;
	trueRegion = relative;
	file->Load(&value); helpContext = value;
	file->Load(&userFlags);
	file->Load(&userStatus);
	file->Load(&userObjectName);
	if (userObjectName && userTable)
	{
		for (int i = 0; !userObject && userTable[i].value != ID_END; i++)
			if (stricmp(userTable[i].text, userObjectName) == 0)
				userObject = userTable[i].data;
	}
	file->Load(&userFunctionName);
	if (userFunctionName && userTable)
	{
		for (int i = 0; !userFunction && userTable[i].value != ID_END; i++)
			if (stricmp(userTable[i].text, userFunctionName) == 0)
				userFunction = (ZIL_USER_FUNCTION)userTable[i].data;
	}
}

UI_WINDOW_OBJECT *UI_WINDOW_OBJECT::New(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	if (!objectTable)
		objectTable = UI_WINDOW_OBJECT::objectTable;
	if (!userTable)
		userTable = UI_WINDOW_OBJECT::userTable;
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);

	// Check for a valid directory and file.
	int tempDirectory = FALSE;
	if (name && !file)
	{
		ZIL_ICHAR pathName[128], fileName[32], objectName[32], objectPathName[128];
		ZIL_STORAGE::StripFullPath(name, pathName, fileName, objectName, objectPathName);
		if (!directory)
		{
			ZIL_STORAGE::AppendFullPath(pathName, pathName, fileName);
			ZIL_STORAGE::ChangeExtension(pathName, _zilExtName);
			directory = new ZIL_STORAGE_READ_ONLY(pathName);
			if (directory->storageError)
			{
				delete directory;
				return (ZIL_NULLP(UI_WINDOW_OBJECT));
			}
			tempDirectory = TRUE;
		}
		if (!file)
		{
			object = new UIW_WINDOW(name, directory, ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
				objectTable, userTable);
			if (directory->storageError)
			{
				delete object;
				object = ZIL_NULLP(UI_WINDOW_OBJECT);
			}
			if (tempDirectory)
				delete directory;
			return (object);
		}
	}

	// Read the object based on its identification.
	ZIL_INT16 searchID;
	file->Load(&searchID);
	for (int i = 0; objectTable[i].value != ID_END; i++)
		if (objectTable[i].value == searchID)
		{
#if defined(ZIL_EDIT)
			if (FlagsSet(defaultStatus, WOS_EDIT_MODE | WOS_INTERNAL_ACTION) &&
				objectTable[i].flags > 1)
				i = objectTable[i].flags;
//			objectTable[i].flags = TRUE;
#endif
			ZIL_NEW_FUNCTION newFunction = (ZIL_NEW_FUNCTION)objectTable[i].data;
			object = newFunction(ZIL_NULLP(ZIL_ICHAR), directory, file,
				objectTable, userTable);
			object->searchID = (ZIL_OBJECTID)objectTable[i].value;
			break;
		}

	// Clean up the file and storage then return the object.
	if (tempDirectory)
		delete directory;
	if (object)
		return (object);

	// Error report for unknown object types.
	return (ZIL_NULLP(UI_WINDOW_OBJECT));
}
#endif

#if defined(ZIL_STORE)
#if defined(__HIGHC__)
void UI_WINDOW_OBJECT::Store(const ZIL_ICHAR *arg1, ZIL_STORAGE *arg2,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
#else
void UI_WINDOW_OBJECT::Store(const ZIL_ICHAR *, ZIL_STORAGE *,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
#endif
{
	if (!file)
		return;

	if (!objectTable)
		objectTable = UI_WINDOW_OBJECT::objectTable;
	if (!userTable)
		userTable = UI_WINDOW_OBJECT::userTable;

	// Mark the object as used.
	if (objectTable)
	{
		for (int i = 0; objectTable[i].value != ID_END; i++)
			if (objectTable[i].value == searchID)
			{
				objectTable[i].flags = searchID;
				break;
			}
	}

	// Store the object information.
	file->Store(numberID);
	file->Store(stringID);
	file->Store(woFlags);
#if defined (ZIL_EDIT)
	if (FlagSet(woStatus, WOS_EDIT_MODE))
		file->Store(designerAdvancedFlags);
	else
		file->Store(woAdvancedFlags);
#else
	file->Store(woAdvancedFlags);
#endif

	// Compute the actual store region.
	UI_REGION region;
#if defined(ZIL_MOTIF) || defined(ZIL_NEXTSTEP)
	region = relative;
#else
	if (parent)
	{
		region = relative;
		if (FlagSet(woStatus, WOS_GRAPHICS) &&
			!FlagSet(woFlags, WOF_PIXEL) &&
			!FlagSet(woFlags, WOF_NON_FIELD_REGION) &&
			!FlagSet(woFlags, WOF_MINICELL))
		{
			region.top -= display->preSpace;
			region.bottom += display->postSpace;
		}
	}
	else
#		if defined(ZIL_MACINTOSH)
		region = relative;
#		else
		region = trueRegion;
#		endif
#endif

	// Convert according to the type of coordinate system.
	int cellWidth = display->cellWidth;
	int cellHeight = display->cellHeight;
	if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS) && FlagSet(woFlags, WOF_MINICELL))
	{
#if !defined(ZIL_TEXT_ONLY)
		long miniNX = display->miniNumeratorX, miniDX = display->miniDenominatorX;
		long miniNY = display->miniNumeratorY, miniDY = display->miniDenominatorY;

		int width = (int)(((miniDX * region.Width()) / cellWidth + miniNX / 2) / miniNX);
		region.left = (int)(((miniDX * region.left) / cellWidth + miniNX / 2) / miniNX);
		region.right = region.left + width - 1;
		int height = (int)(((miniDY * region.Height()) / cellHeight + miniNY / 2) / miniNY);
		region.bottom = (int)(((miniDY * region.bottom) / cellHeight + miniNY / 2) / miniNY);
		region.top = region.bottom - height + 1;
#endif
	}
	else if (!display->isText && FlagSet(woStatus, WOS_GRAPHICS) && !FlagSet(woFlags, WOF_PIXEL))
	{
#if !defined(ZIL_TEXT_ONLY)
#	if defined(ZIL_OS2)
		int width = MaxValue(region.Width() / cellWidth, 1);
		int height = MaxValue(region.Height() / cellHeight, 1);
#	else
		int width = region.Width() / cellWidth;
		int height = region.Height() / cellHeight;
#	endif
		region.left /= cellWidth;
		region.bottom++;
		region.bottom /= cellHeight;
		region.bottom--;
		region.right = region.left + width - 1;
		region.top = region.bottom - height + 1;
#endif
	}

	// Store the coordinates.
	ZIL_INT16 value = region.left; file->Store(value);
	value = region.top; file->Store(value);
	value = region.right; file->Store(value);
	value = region.bottom; file->Store(value);
	value = helpContext; file->Store(value);
	file->Store(userFlags);
	file->Store(userStatus);
	if (userObject && !userObjectName && userTable)
	{
		for (int i = 0; !userObjectName && userTable[i].value != ID_END; i++)
			if (userTable[i].data == userObject)
				userObjectName = strdup(userTable[i].text);
	}
	file->Store(userObjectName);
	if (userFunction && !userFunctionName && userTable)
	{
		for (int i = 0; !userFunctionName && userTable[i].value != ID_END; i++)
			if (userTable[i].data == (void *)userFunction)
				userFunctionName = strdup(userTable[i].text);
	}
	file->Store(userFunctionName);
}
#endif
