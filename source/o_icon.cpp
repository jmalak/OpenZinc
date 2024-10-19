//	Zinc Interface Library - O_ICON.CPP
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


#define INCL_WINSTATICS			// OS/2 static messages and flags.
#define INCL_WINLISTBOXES		// OS/2 list messages and flags.
#define INCL_WINPOINTERS		// OS/2 WinQuerySysPointer() function.
#define INCL_WINBUTTONS			// OS/2 button flags
#include "ui_win.hpp"

// ----- UIW_ICON -----------------------------------------------------------

EVENT_TYPE UIW_ICON::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
#if defined(ZIL_OPTIMIZE)
	ZIL_SCREENID screenID = this->screenID;
	UI_DISPLAY *display = this->display;
#endif

	// OS/2 automatically displays text only information.
	if (!iconArray && !icon)
		return (FALSE);

	int listItem = parent->Inherited(ID_LIST);
	int iconOffset = listItem ? 4 : 0;

	// Virtualize the display.
	display->VirtualGet(screenID, trueRegion);
	lastPalette = LogicalPalette(ccode);

	// Draw the object shadow and fill it's region.
	UI_REGION region = trueRegion;
	if (FlagSet(woFlags, WOF_BORDER))
		DrawBorder(screenID, region, FALSE, ccode);
	if (listItem)
	{
		display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE);
		if (FlagSet(woStatus, WOS_CURRENT))
			DrawFocus(screenID, region, ccode);
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
	if (iconArray)
	{
		display->IconArrayToHandle(screenID, iconWidth, iconHeight,
			iconArray, ZIL_NULLP(UI_PALETTE), &icon);
		if (iconArray && icon && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))
		{
			delete iconArray;
			iconArray = ZIL_NULLP(ZIL_UINT8);
		}
	}
	if (icon)
	{
		SWP swp;
		WinQueryWindowPos(screenID, &swp);
		if (FlagSet(woStatus, WOS_SYSTEM_OBJECT))
			top -= region.top;
		WinDrawPointer(UI_OS2_DISPLAY::hps, left - trueRegion.left,
			swp.cy - top - iconHeight, icon, DP_NORMAL);
	}
	else if (iconArray)
		display->Bitmap(screenID, left, top, iconWidth, iconHeight, iconArray, ZIL_NULLP(UI_PALETTE));

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
		if (listItem)
			DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		else if (FlagSet(woStatus, WOS_CURRENT))
		{
			display->Rectangle(screenID, region, lastPalette, 0, TRUE, FALSE);
			DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		}
		else
		{
			display->Rectangle(screenID, region, parent->LogicalPalette(ccode), 0, TRUE, FALSE);
			DrawText(screenID, region, title, lastPalette, FALSE, ccode);
		}
		woFlags = flags;
	}

	// Restore the display.
	display->VirtualPut(screenID);
	return (TRUE);
}

EVENT_TYPE UIW_ICON::Event(const UI_EVENT &event)
{
	// Initialize the standard OS/2 icons.
	struct ICON_ITEM
	{
		ZIL_ICHAR *name;
		LONG identification;
	} _iconTable[] = {
		{ _applicationIconName, SPTR_APPICON },
		{ _asteriskIconName, 	SPTR_ICONINFORMATION },
		{ _exclamationIconName, SPTR_ICONERROR },
		{ _questionIconName, 	SPTR_ICONQUESTION },
		{ 0, 0 } };

	// Check for OS/2 specific messages.
	EVENT_TYPE ccode = event.type;
	if (ccode == E_OS2)
	{
		static int discardUpClick = TRUE;
		switch (event.message.msg)
		{
		case WM_BUTTON1DOWN:
			WinSetFocus(HWND_DESKTOP, screenID);
			discardUpClick = FALSE;
			return (UI_WINDOW_OBJECT::Event(event));

		case WM_BUTTON1DBLCLK:
			if (FlagSet(icFlags, ICF_DOUBLE_CLICK))
				UserFunction(event, L_DOUBLE_CLICK);
			else
				discardUpClick = FALSE;
			return (UI_WINDOW_OBJECT::Event(event));

		case WM_BUTTON1UP:
			if (!discardUpClick)
			{
				UserFunction(event, L_SELECT);
				discardUpClick = TRUE;
			}
			return (DefaultCallback(event));

		case WM_CHAR:
			break;

		default:
			return (UI_WINDOW_OBJECT::Event(event));
		}
	}

	// Check for zinc specific messages.
	ccode = LogicalEvent(event, ID_ICON);
	switch (ccode)
	{
	case S_INITIALIZE:
		woStatus |= WOS_OWNERDRAW;

		// See if the icon is system specified.
		if (!icon && iconName)
		{
			for (int i = 0; _iconTable[i].name && !icon; i++)
				if (stricmp(iconName, _iconTable[i].name) == 0)
					icon = WinQuerySysPointer(HWND_DESKTOP, _iconTable[i].identification, FALSE);
			if (!icon)
				icon = WinLoadFileIcon((PSZ)iconName, FALSE);
			if (iconArray && icon && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))
			{
				delete iconArray;
				iconArray = ZIL_NULLP(ZIL_UINT8);
			}
			// If it is a system icon find size from the system resolution
			if (icon)
			{
				if (display->columns > 800 && display->lines > 600)
				{
					iconWidth = 40;
					iconHeight = 40;
				}
				else
				{
					iconWidth = 32;
					iconHeight = 32;
				}
			}
		}
		else
		{
			iconWidth = 32;
			iconHeight = 32;
		}


		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case S_CHANGED:
		trueRegion.right = trueRegion.left + iconWidth + 2;
		trueRegion.bottom = trueRegion.top + iconHeight + 2;
		if (title && FlagSet(woFlags, WOF_JUSTIFY_CENTER))
		{
			trueRegion.bottom += (display->cellHeight - display->preSpace - display->postSpace);
			if (trueRegion.Width() < display->TextWidth(title, screenID, font))
				trueRegion.right = trueRegion.left + display->TextWidth(title, screenID, font);
		}
		break;

	case S_REGISTER_OBJECT:
		if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		{
			UI_WINDOW_OBJECT::Event(event);
			Event(UI_EVENT(S_SIZE, 0, relative));
		}
		else
		{
			parent->Information(I_GET_FRAMEID, &screenID);
			if (!icon && iconArray)
				display->IconArrayToHandle(screenID, iconWidth, iconHeight,
					iconArray, ZIL_NULLP(UI_PALETTE), &icon);
			if (icon)
				WinSendMsg(screenID, WM_SETICON, (MPARAM)icon, (MPARAM)0);
			if (iconArray && icon && !FlagSet(icFlags, ICF_STATIC_ICONARRAY))
			{
				delete iconArray;
				iconArray = ZIL_NULLP(ZIL_UINT8);
			}
		}
		break;

	case L_SELECT:
		ccode = UserFunction(event, L_SELECT);
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		if (!FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	case L_LEFT:
	case L_RIGHT:
	case L_UP:
	case L_DOWN:
		parent->Event(UI_EVENT(ccode));
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_ICON::OSDataSet(void)
{
	if (screenID)
	{
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
		char *TEXT_ = MapText(title, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ title
#endif
		WinSetWindowText(screenID, (PSZ)TEXT_);
	}
}

void UIW_ICON::OSImageInitialize(void)
{
}

void UIW_ICON::OSSystemImage(void)
{
}

void UIW_ICON::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	woStatus |= WOS_OWNERDRAW;
	icStatus = 0;

	// See if the field needs to be re-computed.
	if (parent)
	{
		if (FlagSet(icFlags, ICF_MINIMIZE_OBJECT))
			parent->Information(I_SET_ICON, &icon);
		else if (objectID == ID_ICON && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
	}
}
