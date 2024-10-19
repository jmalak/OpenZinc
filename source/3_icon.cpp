//	Zinc Interface Library - W_ICON.CPP
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

// ----- UIW_ICON -----------------------------------------------------------

static WNDPROC _iconCallback = ZIL_NULLF(WNDPROC);

EVENT_TYPE UIW_ICON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif

	int iconOffset = parent->Inherited(ID_LIST) ? 4 : 0;

	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);
	lastPalette = LogicalPalette(ccode);

	// Draw the object's fill region.
	UI_REGION region = trueRegion;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);

	if (parent)
	{
		UI_WINDOW_OBJECT *bObject = NULL;
		for (bObject = parent; bObject &&
			(bObject->LogicalPalette(S_DISPLAY_ACTIVE)->colorBackground == BACKGROUND ||
			bObject->LogicalPalette(S_DISPLAY_ACTIVE)->fillPattern == PTN_RGB_COLOR);
			bObject = bObject->parent)
			;
		UI_PALETTE *bPalette = bObject->LogicalPalette(ccode);
		
		display->Rectangle(screenID, region, bPalette, 0, TRUE);
	}

	if (!FlagSet(woStatus, WOS_SYSTEM_OBJECT) || Inherited(ID_STATUS_ITEM))
	{
		display->Rectangle(screenID, region, lastPalette, 0, TRUE);
		if (FlagSet(woStatus, WOS_CURRENT))
			display->Rectangle(screenID, trueRegion, lastPalette, 1, FALSE, TRUE);
	}

	// Compute the draw region.
	region.left += iconOffset;
	if (parent->Inherited(ID_VT_LIST))
		region.left += relative.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT | WOF_JUSTIFY_CENTER))
		region.right -= iconOffset;
	int fieldWidth = region.Width();
	int fieldHeight = region.Height();
	int left = region.left;
	if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		left = region.right - iconWidth + 1;
	else if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		left = region.left + (fieldWidth - iconWidth) / 2;
	int top = (FlagSet(woFlags, WOF_JUSTIFY_CENTER) && title) ?
		region.top + 2 : region.top + (fieldHeight - iconHeight) / 2;

	// Draw the icon.
	if (icon && !FlagSet(woStatus, WOS_SYSTEM_OBJECT))
		DrawIcon(UI_MSWINDOWS_DISPLAY::hDC, left, top, icon);
	else if (icon)
		DrawIcon(UI_MSWINDOWS_DISPLAY::hDC, left - trueRegion.left, top - trueRegion.top, icon);
	else if (iconArray)
		display->Bitmap(screenID, left, top, iconWidth, iconHeight, iconArray);

	// Draw the title.
	if (title)
	{
		WOF_FLAGS flags = woFlags;
		if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			region.top = region.bottom - display->cellHeight + 4 + display->preSpace + display->postSpace;
		else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
		{
			region.right -= iconWidth - iconOffset;
			woFlags &= ~(WOF_JUSTIFY_RIGHT);
		}
		else
			region.left += iconWidth + iconOffset;
		region.bottom--;
		if (!FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		else if (FlagSet(woStatus, WOS_CURRENT))
		{
			display->Rectangle(screenID, region, lastPalette, 0, TRUE);
			DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		}
		else
			DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		woFlags = flags;
	}

	// Restore the display.
	display->VirtualPut(screenID);

	// Return the control code.
	return (TRUE);
}

EVENT_TYPE UIW_ICON::Event(const UI_EVENT &event)
{
	int processed = FALSE;
	EVENT_TYPE ccode = S_UNKNOWN;

	if (event.type == E_MSWINDOWS)
	{
		WORD message = event.message.message;
		WPARAM wParam = event.message.wParam;
		LPARAM lParam = event.message.lParam;

/* START BLOCK COMMENT
**			if (FlagSet(GetVersion(), 0x80000000) && message >= WM_USER) 		// Win32s Bug
**				switch (message)
**				{
**				case WM_USER+0:
**					message = BM_GETCHECK;
**					break;
**				case WM_USER+1:
**					message = BM_SETCHECK;
**					break;
**				case WM_USER+2:
**					message = BM_GETSTATE;
**					break;
**				case WM_USER+3:
**					message = BM_SETSTATE;
**					break;
**				case WM_USER+4:
**					message = BM_SETSTYLE;
**					break;
**				}
END BLOCK COMMENT */

		processed = TRUE;

		switch (message)
		{
		case BM_SETSTATE:
			if (wParam)
				icStatus |= BTS_DEPRESSED;
			else
				icStatus &= ~BTS_DEPRESSED;
			break;

		case WM_SYSCHAR:
			{
			ZIL_RAW_CODE hotCharKey = wParam;
			if (hotCharKey >= 'A' && hotCharKey <= 'Z')
				hotCharKey -= (ZIL_RAW_CODE)('A' - 'a');
			hotCharKey += (ZIL_RAW_CODE)((lParam & 0x20000000L) >> 18);	// Add the ALT key

			ZIL_LOGICAL_EVENT hotChar = 0;
			if (hotCharKey & 0x800)
				for (int i = 0; hotKeyMapTable[i].objectID != ID_END; i++)
				{
					if (hotKeyMapTable[i].rawCode == hotCharKey)
						hotChar = hotKeyMapTable[i].logicalValue;
				}
			else if (FlagSet(woAdvancedFlags, WOAF_NORMAL_HOT_KEYS))				
				hotChar = hotCharKey;
			if (hotChar == (ZIL_LOGICAL_EVENT)hotKey)
				ccode = UserFunction(UI_EVENT(E_KEY), L_SELECT);
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			}
			break;

		case WM_LBUTTONUP:
			if (!FlagSet(woStatus, WOS_EDIT_MODE))
			{
				if (FlagSet(icStatus, BTS_DEPRESSED))
				{
					ccode = UI_WINDOW_OBJECT::Event(event);
					UIW_ICON::Event(UI_EVENT(L_END_SELECT));
				}
				else
					ccode = UI_WINDOW_OBJECT::Event(event);
				icStatus = 0;
			}
			else
				processed = FALSE;
			break;

		case WM_LBUTTONDBLCLK:
			if (!FlagSet(woStatus, WOS_EDIT_MODE) &&
				FlagSet(icFlags, ICF_DOUBLE_CLICK) && userFunction)
			{
				ccode = UI_WINDOW_OBJECT::Event(event);
				UI_EVENT uEvent = event;
				ccode = (*userFunction)(this, uEvent, L_DOUBLE_CLICK);
			}
			else
				ccode = UI_WINDOW_OBJECT::Event(event);
			break;

		// Ambiguous cases
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
			processed = FALSE;
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}
	}

	if (!processed)
	{
		// Switch on the event type.
		ccode = LogicalEvent(event, ID_ICON);
		switch (ccode)
		{
		case S_INITIALIZE:
			// Initialize the icon information.
			if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				icFlags |= ICF_MINIMIZE_OBJECT;
			UI_WINDOW_OBJECT::Event(event);

			// Do calculations here for lists and combo boxes.
			relative.right = relative.left + iconWidth + 2;
			relative.bottom = relative.top + iconHeight + 2;
			if (title && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
			{
				relative.bottom += (display->cellHeight - display->preSpace - display->postSpace);
				if (relative.Width() < display->TextWidth(title, screenID, font))
					relative.right = relative.left + display->TextWidth(title, screenID, font);
			}
			trueRegion = relative;
			break;

		case S_CREATE:
			ccode = UI_WINDOW_OBJECT::Event(event);

			// Convert the icon array to a handle if possible.
			if (iconArray && !icon)
			{
				display->IconArrayToHandle(screenID, iconWidth, iconHeight,
					iconArray, ZIL_NULLP(UI_PALETTE), &icon);
				if (icon && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))
				{
					delete iconArray;
					iconArray = ZIL_NULLP(ZIL_UINT8);
				}
			}

			// Attach it to parent as a minimize object if it is a support object.
			if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			{
				parent->Information(I_SET_ICON, &icon);
				break;
			}
			else
				dwStyle |= BS_OWNERDRAW;

			// else fall through to S_REGISTER_OBJECT

		case S_REGISTER_OBJECT:
			RegisterObject("UIW_ICON", "BUTTON", &_iconCallback, title);
			break;

		case L_END_SELECT:
		case L_SELECT:
			ccode = UserFunction(UI_EVENT(ccode), L_SELECT);
			parent->Information(I_CHECK_SELECTION, ZIL_NULLP(void));
			break;

		default:
			ccode = UI_WINDOW_OBJECT::Event(event);
			break;
		}
	}

	// Return the control code.
	return (ccode);
}


// ----- OS Specific Functions ----------------------------------------------

void UIW_ICON::OSDataSet(void)
{
	// Redisplay the string.
	if (screenID)
	{
		InvalidateRect(screenID, ZIL_NULLP(RECT), TRUE);
#	if defined(ZIL_UNICODE)
		char *TEXT_ = MapText(title, ZIL_NULLP(char), FALSE);
#	else
#		define TEXT_ title
#	endif
		SendMessage(screenID, WM_SETTEXT, 0, (LONG)TEXT_);
	}
}

void UIW_ICON::OSImageInitialize(void)
{
	if (iconArray && !icon)
	{
		display->IconArrayToHandle(screenID, iconWidth, iconHeight,
			iconArray, ZIL_NULLP(UI_PALETTE), &icon);
		if (icon)
		{
			delete iconArray;
			iconArray = ZIL_NULLP(ZIL_UINT8);
		}
	}
}

void UIW_ICON::OSSystemImage(void)
{
	// Initialize the standard windows icons.
	struct ICON_ITEM
	{
		ZIL_ICHAR *name;
		void *identification;
	} _iconTable[] = {
		{ _applicationIconName, 	(void *)IDI_APPLICATION },
		{ _asteriskIconName, 		(void *)IDI_ASTERISK },
		{ _exclamationIconName, 	(void *)IDI_EXCLAMATION },
		{ _handIconName, 			(void *)IDI_HAND },
		{ _questionIconName, 		(void *)IDI_QUESTION },
		{ 0, 0 } };

	if (!icon && iconName)
	{
		for (int i = 0; _iconTable[i].name && !icon; i++)
			if (stricmp(iconName, _iconTable[i].name) == 0)
				icon = LoadIcon(0, (ZIL_ICHAR *)_iconTable[i].identification);
		if (!icon)
		{
#			undef TEXT_
#			define TEXT_ iconName
			icon = LoadIcon(display->hInstance, TEXT_);
		}
	}
}

void UIW_ICON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the icon is system specified.
	UIW_ICON::OSSystemImage();

	// Check the environment specific flag settings.
	woStatus |= WOS_OWNERDRAW;
	icStatus = 0;

	// See if the field needs to be re-computed.
	if (parent && FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
		parent->Information(I_SET_ICON, &icon);
	else if (objectID == ID_ICON && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}


