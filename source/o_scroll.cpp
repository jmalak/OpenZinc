//	Zinc Interface Library - O_SCROLL.CPP
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


#define INCL_WINMLE				// OS/2 multi-line messages and flags.
#define INCL_WINSCROLLBARS		// OS/2 scroll messages and flags.
#define INCL_WINSTDSLIDER		// OS/2 slider messages and flags.
#include "ui_win.hpp"

// ----- UIW_SCROLL_BAR -----------------------------------------------------

EVENT_TYPE UIW_SCROLL_BAR::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event; (void)ccode;

	return (FALSE);
}

EVENT_TYPE UIW_SCROLL_BAR::Event(const UI_EVENT &event)
{
	static PFNWP _scrollBarCallback = ZIL_NULLP(FNWP);
	static PFNWP _sliderCallback = ZIL_NULLP(FNWP);
	EVENT_TYPE ccode = event.type;

	// Check for OS/2 specific messages.
	if (ccode == E_OS2)
	{
		ULONG msg = event.message.msg;
		switch (msg)
		{
		case WM_CONTROL:
			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				int notifyCode = HIWORD(event.message.mp1);
				if (notifyCode == SLN_CHANGE || notifyCode == SLN_SLIDERTRACK)
				{
					int oldPosition = scroll.current;
					ULONG info = (ULONG)WinSendMsg(screenID, SLM_QUERYSLIDERINFO,
						MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_RANGEVALUE), (MPARAM)0);
					int position = LOWORD(info);
					int range = HIWORD(info);
					scroll.current = scroll.minimum + (scroll.maximum - scroll.minimum) * position / (range - 1);
					if (scroll.current > scroll.maximum)
						scroll.current = scroll.maximum;
					if (scroll.current != oldPosition)
					{
						UI_EVENT userEvent;
						userEvent.type = (notifyCode == SLN_CHANGE) ? L_END_SELECT : L_CONTINUE_SELECT;
						userEvent.scroll = scroll;
						userEvent.scroll.delta = scroll.current - oldPosition;

						if (userFunction)
							(*userFunction)(this, userEvent, L_SELECT);
					}
				}
			}
			break;

		case WM_VSCROLL:
		case WM_HSCROLL:
			if ((msg == WM_VSCROLL && FlagSet(sbFlags, SBF_VERTICAL)) ||
				(msg == WM_HSCROLL && FlagSet(sbFlags, SBF_HORIZONTAL)))
			{
				UI_EVENT userEvent;
				USHORT command = HIWORD(event.message.mp2);
				int oldPosition = scroll.current;
				int newPosition = scroll.current;
				if (command == SB_LINELEFT)
				{
					newPosition -= scroll.delta;
					userEvent.type = L_UP;
				}
				else if (command == SB_PAGELEFT)
				{
					newPosition -= scroll.showing * scroll.delta;
					userEvent.type = L_PGUP;
				}
				else if (command == SB_LINERIGHT)
				{
					newPosition += scroll.delta;
					userEvent.type = L_DOWN;
				}
				else if (command == SB_PAGERIGHT)
				{
					newPosition += scroll.showing * scroll.delta;
					userEvent.type = L_PGDN;
				}
				else if (command == SB_SLIDERPOSITION || command == SB_SLIDERTRACK)
				{
					newPosition = LOWORD(event.message.mp2) + scroll.minimum;
					userEvent.type = (command == SB_SLIDERPOSITION) ? L_END_SELECT : L_CONTINUE_SELECT;
				}
				else
					return (0);

				if (newPosition > scroll.maximum)
					newPosition = scroll.maximum;
				else if (newPosition < scroll.minimum)
					newPosition = scroll.minimum;

				WinSendMsg(screenID, SBM_SETPOS, (MPARAM)(newPosition - scroll.minimum), (MPARAM)0);
				scroll.current = newPosition;
				userEvent.scroll = scroll;
				userEvent.scroll.delta = newPosition - oldPosition;

				if (userFunction)
					(*userFunction)(this, userEvent, L_SELECT);

				if (FlagSet(woFlags, WOF_SUPPORT_OBJECT) && oldPosition != scroll.current)
				{
					UI_EVENT sEvent((msg == WM_VSCROLL) ? S_VSCROLL : S_HSCROLL);
					sEvent.scroll = scroll;
					sEvent.scroll.delta = scroll.current - oldPosition;
					woStatus |= WOS_INTERNAL_ACTION;
					parent->Event(sEvent);
					woStatus &= ~WOS_INTERNAL_ACTION;
				}
			}
			return (0);

#if defined(ZIL_EDIT)
		case WM_BUTTON1DOWN:
		case WM_BUTTON1DBLCLK:
		case WM_MOUSEMOVE:
			if (FlagSet(parent->woStatus, WOS_EDIT_MODE) &&
				FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			{
				POINTL point;
				point.x = LOWORD(event.message.mp1);
				point.y = HIWORD(event.message.mp1);

				WinMapWindowPoints(screenID, parent->screenID, &point, 1);
				WinPostMsg(parent->screenID, msg, MPFROM2SHORT(point.x, point.y),
					event.message.mp2);
				return (TRUE);
			}
			else
				return (UI_WINDOW_OBJECT::Event(event));
#endif
		}
		// Return the control code.
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
			return DefaultCallback(event);
		else
			return (UI_WINDOW_OBJECT::Event(event));
	}

	// Switch on the event type.
	ccode = LogicalEvent(event, ID_SCROLL_BAR);
	switch (ccode)
	{
	case S_INITIALIZE:
		{
		ccode = UI_WINDOW_OBJECT::Event(event);

		// Get system width and height.
		int width, height;
		if (FlagSet(sbFlags, SBF_SLIDER))
		{
			width = 3 * display->cellWidth;
			height = display->cellHeight;
		}
		else
		{
			width = WinQuerySysValue(HWND_DESKTOP, SV_CXVSCROLL);
			height = WinQuerySysValue(HWND_DESKTOP, SV_CYHSCROLL);
		}

		// Adjust relative width and/or height.
		if (FlagSet(sbFlags, SBF_VERTICAL | SBF_CORNER))
		{
			if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
				relative.left = relative.right - width + 1;
			else
				relative.right = relative.left + width - 1;
		}
		if (FlagSet(sbFlags, SBF_HORIZONTAL | SBF_CORNER))
		{
			if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
				relative.top = relative.bottom - height + 1;
			else
				relative.bottom = relative.top + height - 1;
		}

		// Set Flags.
		if (parent->Inherited(ID_TEXT))
		{
			if (FlagSet(sbFlags, SBF_VERTICAL | SBF_HORIZONTAL))
			{
				flStyle = FlagSet(sbFlags, SBF_VERTICAL) ? MLS_VSCROLL : MLS_HSCROLL;
				parent->Information(I_SET_FLSTYLE, &flStyle);
			}
		}
		else if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		{
			flStyle |= SBS_AUTOTRACK;
			if (FlagSet(sbFlags, SBF_VERTICAL))
			{
				parent->Information(I_SET_VSCROLL, this);
				flFlag = FCF_VERTSCROLL;
				parent->Information(I_SET_FLFLAG, &flFlag);
			}
			else if (FlagSet(sbFlags, SBF_HORIZONTAL))
			{
				parent->Information(I_SET_HSCROLL, this);
				flFlag = FCF_HORZSCROLL;
				parent->Information(I_SET_FLFLAG, &flFlag);
			}
		}
		else
		{
			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				flStyle |= FlagSet(sbFlags, SBF_VERTICAL) ? SLS_VERTICAL : SLS_HORIZONTAL;
				if (FlagSet(woFlags, WOF_NON_SELECTABLE) && !FlagSet(woStatus, WOS_EDIT_MODE))
					flStyle |= WS_DISABLED;
			}
			else
				flStyle |= SBS_AUTOTRACK | (FlagSet(sbFlags, SBF_VERTICAL) ? SBS_VERT : SBS_HORZ);
		}
		}
		break;

	case S_CHANGED:
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
		{
			if (FlagSet(sbFlags, SBF_VERTICAL | SBF_CORNER))
				trueRegion.left = trueRegion.right - relative.Width() + 1;
			if (FlagSet(sbFlags, SBF_HORIZONTAL | SBF_CORNER))
				trueRegion.top = trueRegion.bottom - relative.Height() + 1;
		}
		break;

	case S_REGISTER_OBJECT:
		if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
		{
			if (!FlagSet(sbFlags, SBF_CORNER))
			{
				ZIL_SCREENID frameID;
				parent->Information(I_GET_FRAMEID, &frameID);
				screenID = WinWindowFromID(frameID, FlagSet(sbFlags, SBF_VERTICAL) ? FID_VERTSCROLL : FID_HORZSCROLL);

				defaultCallback = (PFNWP)WinQueryWindowPtr(screenID, QWP_PFNWP);
				WinSetWindowULong(screenID, QWL_USER, (ULONG)this);
				WinSetWindowPtr(screenID, QWP_PFNWP, (PVOID)ObjectJumpProcedure);
				woStatus |= WOS_SYSTEM_OBJECT;
			}
		}
		else
		{
			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				SLDCDATA slcData;
				slcData.cbSize = sizeof(SLDCDATA);
				if (FlagSet(woStatus, WOS_EDIT_MODE))
					slcData.usScale1Increments = 2;
				else
					slcData.usScale1Increments = (FlagSet(sbFlags, SBF_VERTICAL) ?
						trueRegion.Height() : trueRegion.Width()) - 15;
				slcData.usScale1Spacing = 0;
				slcData.usScale2Increments = 0;
				slcData.usScale2Spacing = 0;
				RegisterObject("UIW_SLIDER", WC_SLIDER, &_sliderCallback, ZIL_NULLP(ZIL_ICHAR), &slcData);
				ULONG color;
				if (WinQueryPresParam(parent->screenID, PP_BACKGROUNDCOLORINDEX, 0,
					ZIL_NULLP(ULONG), sizeof(color), &color, 0) != 0)
					WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(color), &color);
			}
			else
				RegisterObject("UIW_SCROLL_BAR", WC_SCROLLBAR, &_scrollBarCallback, ZIL_NULLP(ZIL_ICHAR));

			UI_EVENT scrollEvent(FlagSet(sbFlags, SBF_VERTICAL) ? S_VSCROLL_SET : S_HSCROLL_SET);
			scrollEvent.scroll = scroll;
			Event(scrollEvent);
		}
		break;

	case S_VSCROLL_SET:
	case S_HSCROLL_SET:
		if ((ccode == S_VSCROLL_SET && FlagSet(sbFlags, SBF_VERTICAL)) ||
			(ccode == S_HSCROLL_SET && FlagSet(sbFlags, SBF_HORIZONTAL)))
		{
			scroll = event.scroll;
			int position = scroll.current - scroll.minimum;
			int positions = scroll.maximum - scroll.minimum;
			int total = positions + scroll.showing;

			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				ULONG info = (ULONG)WinSendMsg(screenID, SLM_QUERYSLIDERINFO,
					MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_RANGEVALUE), (MPARAM)0);
				int range = HIWORD(info);
				WinSendMsg(screenID, SLM_SETSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_RANGEVALUE),
					(MPARAM)((range - 1) * position / positions));
			}
			else
			{
				WinSendMsg(screenID, SBM_SETSCROLLBAR, (MPARAM)position, MPFROM2SHORT(0, positions));
				WinSendMsg(screenID, SBM_SETTHUMBSIZE, MPFROM2SHORT(scroll.showing, total), (MPARAM)0);
			}
		}
		break;

	case S_VSCROLL:
	case S_HSCROLL:
		if (!FlagSet(woStatus, WOS_INTERNAL_ACTION) &&
			((ccode == S_VSCROLL && FlagSet(sbFlags, SBF_VERTICAL)) ||
			(ccode == S_HSCROLL && FlagSet(sbFlags, SBF_HORIZONTAL))))
		{
			int newPosition = scroll.current + event.scroll.delta;
			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				ULONG info = (ULONG)WinSendMsg(screenID, SLM_QUERYSLIDERINFO,
					MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_RANGEVALUE), (MPARAM)0);
				int range = HIWORD(info);
				WinSendMsg(screenID, SLM_SETSLIDERINFO, MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_RANGEVALUE),
					(MPARAM)((range - 1) * (newPosition - scroll.minimum) / (scroll.maximum - scroll.minimum)));
			}
			else
			{
				WinSendMsg(screenID, SBM_SETPOS, (MPARAM)(newPosition - scroll.minimum), (MPARAM)0);
				scroll.current = scroll.minimum + (int)WinSendMsg(screenID, SBM_QUERYPOS, (MPARAM)0, (MPARAM)0);
			}
		}
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_SCROLL_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT))
	{
		// See if the field needs to be re-computed.
		if (objectID == ID_SCROLL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
		}
	}
}
