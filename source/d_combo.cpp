//	Zinc Interface Library - D_COMBO.CPP
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


#define USE_RAW_KEYS
#include "ui_win.hpp"

#define CELL_RATIO	3 / 4

#define ZIL_UIW_COMBO_BUTTON_DATA
#include "img_def.cpp"

// ----- UIW_COMBO_BOX ------------------------------------------------------

UIW_COMBO_BOX::UIW_COMBO_BUTTON::UIW_COMBO_BUTTON(void) :
	UIW_BUTTON(0, 0, 0, ZIL_NULLP(ZIL_ICHAR), BTF_NO_TOGGLE, WOF_JUSTIFY_CENTER | WOF_BORDER,
	ZIL_NULLF(ZIL_USER_FUNCTION), 0, ZIL_NULLP(ZIL_ICHAR))
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	if (!defaultInitialized)
	{
		ZIL_DECORATION_MANAGER::SetDecorations(UIW_COMBO_BOX::_className, TEXT_TABLE, BITMAP_TABLE);
		defaultInitialized = TRUE;
	}
	myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(UIW_COMBO_BOX::_className);
#endif

	woAdvancedFlags |= WOAF_NON_CURRENT;
	depth = 1;
}

UIW_COMBO_BOX::UIW_COMBO_BUTTON::~UIW_COMBO_BUTTON(void)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations);
#endif

	text = ZIL_NULLP(ZIL_ICHAR);
	bitmapArray = ZIL_NULLP(ZIL_UINT8);
}

EVENT_TYPE UIW_COMBO_BOX::UIW_COMBO_BUTTON::Event(const UI_EVENT &event)
{
 	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
	case S_INITIALIZE:
		if (display->isText)
		{
			text = myDecorations->GetText(ZIL_ComboDownArrow);
			bitmapArray = ZIL_NULLP(ZIL_UINT8);
			bitmapWidth = bitmapHeight = 0;
		}
		else
		{
#if !defined(ZIL_TEXT_ONLY)
			text = ZIL_NULLP(ZIL_ICHAR);
			ZIL_UINT8 *bitmap = myDecorations->GetBitmap(ZIL_ComboDownArrow);
			btFlags |= BTF_STATIC_BITMAPARRAY;
			bitmapWidth = bitmap[0];
			bitmapHeight = bitmap[1];
			bitmapArray = &bitmap[2];
#endif
		}
		ccode = UIW_BUTTON::Event(event);
		windowID[0] = ID_BUTTON;
		windowID[1] = ID_WINDOW_OBJECT;
		break;

	case L_CONTINUE_SELECT:
		return ccode;

	case L_BEGIN_SELECT:
		if (FlagSet(windowManager->First()->woAdvancedFlags, WOAF_TEMPORARY) &&
			windowManager->First()->parent == parent)
			windowManager->Event(UI_EVENT(S_CLOSE_TEMPORARY));
		else
			parent->Event(UI_EVENT(L_SELECT));
		break;

	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		userFlags = FALSE;
		// Continue to default.
	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// --- UIW_COMBO_STRING ---

UIW_COMBO_BOX::UIW_COMBO_STRING::UIW_COMBO_STRING(void) :
	UIW_STRING(0, 0, 0, _blankString, 128, STF_NO_FLAGS, WOF_BORDER | WOF_AUTO_CLEAR)
{
}

EVENT_TYPE UIW_COMBO_BOX::UIW_COMBO_STRING::Event(const UI_EVENT &event)
{
	WNF_FLAGS comboFlags = 0;
	parent->Information(I_GET_FLAGS, &comboFlags, ID_WINDOW);

 	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_STRING);
	switch (ccode)
	{
	case S_CREATE:
	case S_CHANGED:
		woFlags = parent->woFlags;
		woFlags |= (WOF_BORDER | WOF_AUTO_CLEAR);
		woFlags &= ~(WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT);
		ccode = UIW_STRING::Event(event);
		break;

	case L_LEFT:
	case L_RIGHT:
	case L_BOL:
	case L_EOL:
	case L_WORD_LEFT:
	case L_WORD_RIGHT:
	case L_MARK_LEFT:
	case L_MARK_RIGHT:
	case L_MARK_BOL:
	case L_MARK_EOL:
	case L_MARK_WORD_LEFT:
	case L_MARK_WORD_RIGHT:
		if (!FlagSet(parent->woFlags, WOF_VIEW_ONLY | WOF_NON_SELECTABLE) &&
			!FlagSet(comboFlags, WNF_BITMAP_CHILDREN))
			ccode = UIW_STRING::Event(event);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		if (userObject)
			UI_WINDOW_OBJECT::Event(event);
		else
			UIW_STRING::Event(event);
		break;

	case E_KEY:
		ccode = UIW_STRING::Event(event);
		parent->Information(I_MATCH_INPUT, 0);

		if (FlagSet(woStatus, WOS_CHANGED))
			parent->woStatus |= WOS_CHANGED;
		break;

	default:
		ccode = UIW_STRING::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

EVENT_TYPE UIW_COMBO_BOX::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;
	UI_WINDOW_OBJECT *oldCurrent = list.Current();

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_COMBO_BOX);
	switch (ccode)
	{
	case S_INITIALIZE:
		UI_WINDOW_OBJECT::Event(event);
		list.Event(event);
		if (FlagSet(woFlags, WOF_UNANSWERED))
			list.current = ZIL_NULLP(UI_ELEMENT);
		if (!display->isText && FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		{
#if !defined(ZIL_TEXT_ONLY)
			int maxHeight = display->cellHeight - display->preSpace - display->postSpace;
			for (UI_WINDOW_OBJECT *object = list.First(); object; object = object->Next())
				if (maxHeight <= object->relative.Height())
					maxHeight = object->relative.Height();
			relative.bottom = relative.top + maxHeight;
#	if defined(ZIL_OS2_STYLE)
			relative.bottom += 2;
#	elif defined(ZIL_MOTIF_STYLE)
			relative.bottom += 4;
#	endif
#endif
		}
		else if (!display->isText)
		{
#if !defined(ZIL_TEXT_ONLY)
			relative.bottom = relative.top + display->cellHeight - display->preSpace - display->postSpace;
#endif
		}
		else
			relative.bottom = relative.top;
		UIW_WINDOW::Event(event);
		wnFlags |= WNF_CONTINUE_SELECT;
		if (FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP))
			list.woAdvancedFlags |= WOAF_ACCEPTS_DROP;
		if (FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT))
			list.woAdvancedFlags |= WOAF_MOVE_DRAG_OBJECT;
		if (FlagSet(woAdvancedFlags, WOAF_COPY_DRAG_OBJECT))
			list.woAdvancedFlags |= WOAF_COPY_DRAG_OBJECT;

		break;

	case S_CHANGED:
	case S_CREATE:
		list.Event(event);
		if (!display->isText && list.First() && FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
			relative.bottom = relative.top + list.First()->trueRegion.Height()+1;
		UIW_WINDOW::Event(event);
		button->trueRegion = trueRegion;
		button->trueRegion.left = display->isText ? trueRegion.right - 2 :
#if defined(ZIL_OS2_STYLE)
			trueRegion.right - 14;
#else
			trueRegion.right - display->cellHeight * CELL_RATIO;
#endif
		if (string)
		{
			string->trueRegion = trueRegion;
			string->Font(font);
#if defined(ZIL_OS2_STYLE)
			string->trueRegion.right = button->trueRegion.left;
			if (display->isText)
				string->trueRegion.right--;
#else
			string->trueRegion.right = (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY)) ?
				button->trueRegion.left : button->trueRegion.left - display->cellWidth;
#endif
			if (display->isText && (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY)))
				string->trueRegion.right--;
		}

		// Set minimum height for text mode list.
		if (display->isText && list.relative.Height() < 5)
			list.relative.bottom = list.relative.top + 4;

		if (text)
		{
			string->Information(I_SET_TEXT, text);
			delete text;
			text = ZIL_NULLP(ZIL_ICHAR);
		}
		break;

	case S_RESET_DISPLAY:
		UIW_WINDOW::Event(event);
		list.Event(event);
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	case L_SELECT:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		{

		// Don't drop the list if parent has a default button.
		UI_WINDOW_OBJECT *defaultButton = ZIL_NULLP(UI_WINDOW_OBJECT);
		for (UI_WINDOW_OBJECT *pObject = parent; pObject && !defaultButton; pObject = pObject->parent)
			pObject->Information(I_GET_DEFAULT_OBJECT, &defaultButton);

		if (defaultButton && event.type != L_SELECT)
			break;
		list.RegionConvert(list.relative, parent ? FALSE : TRUE);
		list.wnFlags = wnFlags | WNF_AUTO_SELECT;
		int height = list.relative.bottom - list.relative.top;
		int width = relative.right - relative.left;
		if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY))
			width -= display->cellWidth;
		list.relative.left = trueRegion.right - width;
		list.relative.top = trueRegion.bottom;
		if (display->isText)
			list.relative.top++;
		list.relative.right = trueRegion.right;
		list.relative.bottom = list.relative.top + height;
		list.lastPalette = UI_PALETTE_MAP::MapPalette(paletteMapTable, PM_ANY, ID_LIST_ITEM);
		list.woStatus &=  ~WOS_SELECTED;
		list.Event(UI_EVENT(S_CREATE));
		(void *)&(*windowManager + &list);
		ZIL_ICHAR *comboString, *currentString;
		if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY) &&
			Current() && Current()->Inherited(ID_STRING))
		{
			comboString = string->DataGet();
			Current()->Information(I_GET_TEXT, &currentString);
			if (stricmp(comboString, currentString))
				Information(I_RESET_SELECTION, ZIL_NULLP(void));
			else
				string->Event(UI_EVENT(S_CURRENT, 0, string->trueRegion));
		}
		woStatus |= WOS_CHANGED;
		}
		break;

	case L_FIRST:
	case L_LAST:
		if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY))
			return (string->Event(event));
		// Continue to L_UP.
	case L_UP:
	case L_DOWN:
	case L_PGUP:
	case L_PGDN:
		{
		list.Event(event);
		// Make sure the user function (if any) is called for the items.
		if (oldCurrent != list.Current())
		{
			oldCurrent->UI_WINDOW_OBJECT::Event(UI_EVENT(S_NON_CURRENT));
			list.Current()->UI_WINDOW_OBJECT::Event(UI_EVENT(S_CURRENT));
			list.Current()->UI_WINDOW_OBJECT::Event(UI_EVENT(L_SELECT));
			woStatus |= WOS_CHANGED;
			if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY))
			{
				string->woStatus &= ~WOS_CURRENT;
				return string->Event(UI_EVENT(S_CURRENT, 0, string->trueRegion));
			}
		}
		}
		break;

	case S_DRAG_DEFAULT:
		ccode = S_DRAG_MOVE_OBJECT;
	case S_DRAG_MOVE_OBJECT:
	case S_DRAG_COPY_OBJECT:
		if (windowManager->dragObject == this ||
			FlagSet(woAdvancedFlags, WOAF_ACCEPTS_DROP))
			return list.Event(event);
		else
			return UI_WINDOW_OBJECT::Event(event);

	case S_DROP_DEFAULT:
		ccode = S_DROP_MOVE_OBJECT;
	case S_DROP_MOVE_OBJECT:
	case S_DROP_COPY_OBJECT:
		ccode = list.Event(event);
		Event(S_REDISPLAY);
		break;

	case S_ADD_OBJECT:
		object = event.windowObject;
		if (list.Index(object) == -1)
			list.Event(event);
		else if (object != list.current)
		{
			if (list.current)
				list.Current()->woStatus &= ~WOS_CURRENT;
			list.current = object;
			list.Current()->woStatus |= WOS_CURRENT;
		}
		break;		// Stop at the combo-box level.

	case S_SUBTRACT_OBJECT:
		object = event.windowObject;
		if (list.Index(object) != -1)
			list.Event(event);
		break;		// Stop at the combo-box level.

	case S_CURRENT:
	case S_NON_CURRENT:
	case S_DISPLAY_ACTIVE:
	case S_DISPLAY_INACTIVE:
		current = string;
		if (ccode == S_NON_CURRENT)
			string->Event(event);
		UIW_WINDOW::Event(event);
		if (FlagSet(wnFlags, WNF_BITMAP_CHILDREN) || FlagSet(woFlags, WOF_VIEW_ONLY))
			oldCurrent = ZIL_NULLP(UI_WINDOW_OBJECT);
		break;

	case S_REDISPLAY:
		ccode = UIW_WINDOW::Event(event);
		if (!list.current && list.first)
			list.current = list.first;
		if (Current() && Current()->Inherited(ID_STRING) &&
			!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) &&
			!FlagSet(woFlags, WOF_VIEW_ONLY))
		{
	 		ZIL_ICHAR *currentText;
			list.Current()->Information(I_GET_TEXT, &currentText);
			if (strcmp(currentText, string->DataGet()))
				Information(I_RESET_SELECTION, ZIL_NULLP(void));
		}
		if (!list.current)
		{
			string->userObject = ZIL_NULLP(void);
			string->DataSet(_blankString);
		}
		break;

	case L_VIEW:
	case L_BEGIN_SELECT:
	case L_CONTINUE_SELECT:
	case L_END_SELECT:
	case L_CONTINUE_COPY_DRAG:
	case L_END_COPY_DRAG:
		if (FlagSet(woStatus, WOS_EDIT_MODE))
			return (UI_WINDOW_OBJECT::Event(event));
		if (windowManager->dragObject)
			return (UI_WINDOW_OBJECT::Event(event));

		if (ccode == L_BEGIN_SELECT &&
			FlagSet(woAdvancedFlags, WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT) &&
			!button->trueRegion.Overlap(event.position) && list.current)
		{
			list.Current()->woStatus |= WOS_SELECTED;
			windowManager->dragObject = this;
			break;
		}
		ccode = UIW_WINDOW::Event(event);

		break;

	case E_KEY:
		if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY))
			return (string->Event(event));

		// Send E_KEY event to list for first character recognition.
		if (windowManager->First() != &list &&
			(FlagSet(woFlags, WOF_VIEW_ONLY) || FlagSet(wnFlags, WNF_BITMAP_CHILDREN)))
			return list.Event(event);

		ccode = UIW_WINDOW::Event(event);
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Redraw the current combo-item.
	if (oldCurrent != list.Current() && ccode != E_KEY && ccode != L_LEFT &&
		ccode != L_RIGHT && event.rawCode != BACKSPACE)
		UIW_COMBO_BOX::Information(I_RESET_SELECTION, ZIL_NULLP(void));

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_COMBO_BOX::OSDataGet(void)
{
	string->Information(I_COPY_TEXT, text);
}
void UIW_COMBO_BOX::OSDataSet(void)
{
	// Set the string information.
	string->Information(I_SET_TEXT, text);
}

void UIW_COMBO_BOX::OSDestroy(void)
{
	// No code necessary.
}

void UIW_COMBO_BOX::OSMatchInput(void)
{
	if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN) && !FlagSet(woFlags, WOF_VIEW_ONLY))
	{
		UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
		ZIL_ICHAR *comboText = string->DataGet();
		if (!comboText || !First())
			return;

		if (!*comboText && First() != Current())
			object = First();
		else
			for (object = First(); object; object = object->Next())
				if (*comboText && !strnicmp(comboText,	(ZIL_ICHAR *)object->Information(I_GET_TEXT, NULL),
					strlen(comboText)))
					break;

		if (!object)
			object = First();
		if (object && object != Current())
		{
			object->woStatus |= WOS_INTERNAL_ACTION;
			Add(object);
			if (windowManager->First() == &list)
				list.Event(UI_EVENT(S_CREATE));
			object->woStatus &= ~WOS_INTERNAL_ACTION;
		}
	}
}

void UIW_COMBO_BOX::OSResetSelection(UI_WINDOW_OBJECT *)
{
	string->Font(font);
	if (!FlagSet(woStatus, WOS_CURRENT))
		string->woStatus &= ~(WOS_SELECTED | WOS_CURRENT);
	if (FlagSet(list.woStatus, WOS_INTERNAL_ACTION))
		return;

	if (!list.Current())
		string->DataSet(_blankString);
	else if (!FlagSet(wnFlags, WNF_BITMAP_CHILDREN))
		string->DataSet((ZIL_ICHAR *)list.Current()->Information(I_GET_TEXT, ZIL_NULLP(void)));
	else
	{
#if !defined(ZIL_TEXT_ONLY)
		UI_WINDOW_OBJECT *object = list.Current();
		string->userObject = object;
		UI_REGION _clip = object->clip;
		UI_REGION _trueRegion = object->trueRegion;
		ZIL_SCREENID _screenID = object->screenID;
		WOF_FLAGS _woFlags = object->woFlags;
		WOS_STATUS _woStatus = object->woStatus;
		UI_WINDOW_OBJECT *_parent = object->parent;
		object->clip = string->clip;
		object->trueRegion = string->trueRegion;
		object->screenID = string->screenID;
		object->woFlags = (string->woFlags & ~(WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT)) |
			(_woFlags & (WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT));
		object->woStatus = string->woStatus;
		if (FlagSet(woStatus, WOS_CURRENT))
		object->parent = this;
#	if defined (ZIL_OS2_STYLE) || defined(ZIL_MOTIF_STYLE)
		DrawBorder(screenID, object->trueRegion, FALSE, S_DISPLAY_ACTIVE);
#	endif
		object->Event(UI_EVENT(S_REDISPLAY));
		object->clip = _clip;
		object->trueRegion = _trueRegion;
		object->screenID = _screenID;
		object->woFlags = _woFlags;
		object->woStatus = _woStatus;
		object->parent = _parent;
#endif
	}
}

void UIW_COMBO_BOX::OSSort(void)
{
	Event(UI_EVENT(S_REDISPLAY));
}

void UIW_COMBO_BOX::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	woFlags &= ~WOF_BORDER;

	// See if the field needs to be re-computed.
	if (objectID == ID_COMBO_BOX && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}
