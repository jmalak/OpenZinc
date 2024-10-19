//	Zinc Interface Library - W_TBAR.CPP
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

// ----- UIW_TOOL_BAR -------------------------------------------------------

static WNDPROC _toolbarCallback = ZIL_NULLF(WNDPROC);

EVENT_TYPE UIW_TOOL_BAR::Event(const UI_EVENT &event)
{
	UI_WINDOW_OBJECT *object;

	EVENT_TYPE ccode = event.type;
	if (ccode == E_MSWINDOWS)
	{
		switch (event.message.message)
		{
			case WM_NOTIFY:
				return (0);

			case WM_SIZE:
				{
				WNDPROC saveCallback = defaultCallback;
				defaultCallback = DefWindowProc;
				ccode = UIW_WINDOW::Event(event);
				defaultCallback = saveCallback;
				}
				return (ccode);
		}
	}

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_TOOL_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		UIW_WINDOW::Event(event);
		for (object = First(); object; object = object->Next())
		{
			if (object->Inherited(ID_BUTTON))
			{
				object->woStatus |= WOS_OWNERDRAW;
				object->woAdvancedFlags |= WOAF_NON_CURRENT;
			}
		}
		break;

	case S_CHANGED:
		if (!screenID)
			break;
		// Continue to S_CREATE.
	case S_CREATE:
		{
		WOF_FLAGS _woFlags = woFlags;		// Get the remaining window region.
		woFlags |= WOF_NON_FIELD_REGION;
		UI_WINDOW_OBJECT::Event(event);
		woFlags = _woFlags;

		trueRegion.bottom = trueRegion.top + relative.Height();
		++trueRegion;
		if (previous && Previous()->Inherited(ID_TOOL_BAR))
			--trueRegion.top;
		clipList.Destroy();
		UI_REGION region = { 0, 0, trueRegion.Width(), trueRegion.Height() };
		clipList.Add(new UI_REGION_ELEMENT(screenID, region));

		if (!FlagSet(wnFlags, WNF_NO_WRAP) && First())
		{
			int height = 1;
			for (object = First(); object; object = object->Next())
				if (height < object->relative.bottom - object->relative.top)
					height = object->relative.bottom - object->relative.top + 1;
			trueRegion.bottom = trueRegion.top + height + display->preSpace + display->postSpace + 3;
			relative.right = trueRegion.right - trueRegion.left;
			relative.bottom = trueRegion.bottom - trueRegion.top;
			int left = display->preSpace, top = display->preSpace;
			for (object = First(); object; object = object->Next())
			{
				int width = object->relative.right - object->relative.left + 1;
				if (left > 0 && left + width > relative.right -
					display->preSpace - display->postSpace)
				{
					left = display->preSpace;
					top += height + display->preSpace;
					trueRegion.bottom += height + display->preSpace;
				}
				left += width;
			}
		}

		if (ccode == S_CREATE)
		{
			InitCommonControls();
#if defined(ZIL_UNICODE)
			RegisterObject("UIW_TOOL_BAR", "ToolbarWindow32", &_toolbarCallback);
#else
			RegisterObject("UIW_TOOL_BAR", TOOLBARCLASSNAME, &_toolbarCallback);
#endif
			for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
				object->Event(event);
			for (object = First(); object; object = object->Next())
				object->Event(event);
		}

		if (!FlagSet(wnFlags, WNF_NO_WRAP))
		{
			int left = 2, top = 2;
			int height = 1;
			for (object = First(); object; object = object->Next())
				if (height < object->relative.bottom - object->relative.top)
					height = object->relative.bottom - object->relative.top + 1;
			for (object = First(); object; object = object->Next())
			{
				int width = object->relative.right - object->relative.left + 1;
				if (left > 0 && left + width > relative.right - 
					display->preSpace - display->postSpace)
				{
					left = display->preSpace;
					top += height + display->preSpace;
				}
				object->relative.left = left;
				left += width;
				object->relative.right = left - 1;

				if (object->Inherited(ID_COMBO_BOX))
					object->relative.bottom = top + object->relative.Height() - 1;
				else
					object->relative.bottom = top + height - 1;
				object->relative.top = top;

				UI_REGION region = object->trueRegion;
				object->Event(UI_EVENT(S_CHANGED));
				if (object->trueRegion != region)
				{
					HWND objectID;
					object->Information(I_GET_FRAMEID, &objectID);
					MoveWindow(objectID, object->trueRegion.left, object->trueRegion.top,
						object->trueRegion.Width(), object->trueRegion.Height(), TRUE);
				}
			}
		}
		else
		{
			for (object = (UI_WINDOW_OBJECT *)support.First(); object; object = object->Next())
			{
				UI_REGION region = object->trueRegion;
				object->Event(UI_EVENT(S_CHANGED));

				if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
					clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
				if (object->trueRegion != region && FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
				{
					ZIL_SCREENID objectID;
					object->Information(I_GET_FRAMEID, &objectID);
					MoveWindow(objectID, object->trueRegion.left, object->trueRegion.top,
						object->trueRegion.Width(), object->trueRegion.Height(), FALSE);

					// Invalidate the old and new object regions.
					woStatus |= WOS_REDISPLAY;
					RECT winRegion;
					winRegion.left = region.left;
					winRegion.top = region.top;
					winRegion.right = region.right + 1;
					winRegion.bottom = region.bottom + 1;
					InvalidateRect(screenID, &winRegion, TRUE);
					InvalidateRect(objectID, ZIL_NULLP(RECT), TRUE);
				}
			}
			for (object = First(); object; object = object->Next())
			{
				UI_REGION region = object->trueRegion;
				object->Event(UI_EVENT(S_CHANGED));

				if (FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
					clipList.Split(screenID, object->trueRegion, parent ? FALSE : TRUE);
				if (object->trueRegion != region && FlagSet(object->woStatus, WOS_SYSTEM_OBJECT))
				{
					ZIL_SCREENID objectID;
					object->Information(I_GET_FRAMEID, &objectID);
					MoveWindow(objectID, object->trueRegion.left, object->trueRegion.top,
						object->trueRegion.Width(), object->trueRegion.Height(), FALSE);

					// Invalidate the old and new object regions.
					woStatus |= WOS_REDISPLAY;
					RECT winRegion;
					winRegion.left = region.left;
					winRegion.top = region.top;
					winRegion.right = region.right + 1;
					winRegion.bottom = region.bottom + 1;
					InvalidateRect(screenID, &winRegion, TRUE);
					InvalidateRect(objectID, ZIL_NULLP(RECT), TRUE);
				}
			}
		}
		MoveWindow(screenID, trueRegion.left, trueRegion.top, trueRegion.Width(), trueRegion.Height(), TRUE);
		}
		break;

	case S_REGISTER_OBJECT:
		InitCommonControls();
#if defined(ZIL_UNICODE)
		RegisterObject("UIW_TOOL_BAR", "ToolbarWindow32", &_toolbarCallback);
#else
		RegisterObject("UIW_TOOL_BAR", TOOLBARCLASSNAME, &_toolbarCallback);
#endif
		break;

	case S_ADD_OBJECT:
		{
		int newObject = event.data && Index((UI_WINDOW_OBJECT *)event.data) == -1;
		UIW_WINDOW::Event(event);
		if (newObject)
			parent->Event(UI_EVENT(S_CHANGED, 0));
		}
		break;

	case L_PREVIOUS:
	case L_NEXT:
		ccode = S_UNKNOWN;
		break;

	default:
		ccode = UIW_WINDOW::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TOOL_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TOOL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
		parent->Event(UI_EVENT(S_CHANGED));
		Event(UI_EVENT(S_REDISPLAY));
	}
}

