//	Zinc Interface Library - W_SCROLL.CPP
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

// ----- UIW_WINDOW ---------------------------------------------------------

static WNDPROC _scrollCallback = ZIL_NULLF(WNDPROC);
static WNDPROC _sliderCallback = ZIL_NULLF(WNDPROC);

EVENT_TYPE UIW_SCROLL_BAR::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	/* unused parameters */ (void)event; (void)ccode;

	return (FALSE);
}

EVENT_TYPE UIW_SCROLL_BAR::Event(const UI_EVENT &event)
{
	// Ignore the corner scroll bar if windows is running.
	if (FlagSet(sbFlags, SBF_CORNER) && FlagsSet(woFlags, WOF_SUPPORT_OBJECT))
		return (S_ERROR);

	int processed = FALSE;
	EVENT_TYPE ccode = S_UNKNOWN;

	if (event.type == E_MSWINDOWS)
	{
		UINT message = event.message.message;

		processed = TRUE;

		switch (message)
		{
		case WM_HSCROLL:
		case WM_VSCROLL:
			{
			if ((FlagSet(sbFlags, SBF_VERTICAL) && message != WM_VSCROLL) ||
				(FlagSet(sbFlags, SBF_HORIZONTAL) && message != WM_HSCROLL))
			{
					ccode = S_ERROR;
					break;
			}

			UI_WINDOW_OBJECT::Event(event);
			ccode = message;
			int tCurrent = scroll.current;
			UI_EVENT tEvent;

			WORD notification = LOWORD(event.message.wParam);
			switch (notification)
			{
			case SB_BOTTOM:
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					scroll.current = scroll.minimum;
				else
					scroll.current = scroll.maximum;
				tEvent.type = L_BOTTOM;
				break;

			case SB_TOP:
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					scroll.current = scroll.maximum;
				else
					scroll.current = scroll.minimum;
				tEvent.type = L_TOP;
				break;

			case SB_LINEDOWN:
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					scroll.current -= scroll.delta;
				else
					scroll.current += scroll.delta;
				tEvent.type = L_DOWN;
				break;

			case SB_LINEUP:
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					scroll.current += scroll.delta;
				else
					scroll.current -= scroll.delta;
				tEvent.type = L_UP;
				break;

			case SB_PAGEDOWN:
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					scroll.current -= scroll.showing;
				else
					scroll.current += scroll.showing;
				tEvent.type = L_PGDN;
				break;

			case SB_PAGEUP:
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					scroll.current += scroll.showing;
				else
					scroll.current -= scroll.showing;
				tEvent.type = L_PGUP;
				break;

			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				scroll.current = HIWORD(event.message.wParam) * scroll.delta;
				if (FlagsSet(sbFlags, SBF_SLIDER | SBF_VERTICAL))
					scroll.current = scroll.maximum + scroll.minimum - scroll.current;
				tEvent.type = notification == SB_THUMBTRACK ? L_CONTINUE_SELECT : L_END_SELECT;
				break;
			}

			if (scroll.current < scroll.minimum)
				scroll.current = scroll.minimum;
			else if (scroll.current > scroll.maximum)
				scroll.current = scroll.maximum;
			if (tCurrent != scroll.current)
			{
				if (FlagsSet(sbFlags, SBF_SLIDER))
				{
					if (FlagSet(sbFlags, SBF_VERTICAL))
						SendMessage(screenID, TBM_SETPOS, TRUE,  (scroll.maximum + scroll.minimum - scroll.current) / scroll.delta);
					else
						SendMessage(screenID, TBM_SETPOS, TRUE,  scroll.current / scroll.delta);
				}
				else
					SetScrollPos(screenID, SB_CTL, scroll.current / scroll.delta, TRUE);
			}

			tEvent.scroll = scroll;
			if (!FlagSet(woFlags, WOF_SUPPORT_OBJECT) && userFunction)
				UserFunction(tEvent, L_SELECT);
			}
			break;

		// Ambiguous cases.
		case WM_KEYDOWN:
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
		ccode = LogicalEvent(event);
		switch (ccode)
		{
		case S_INITIALIZE:
			UI_WINDOW_OBJECT::Event(event);
			if (FlagsSet(woFlags, WOF_SUPPORT_OBJECT))
			{
				if (FlagSet(sbFlags, SBF_VERTICAL))
				{
					parent->Information(I_SET_VSCROLL, this);
					dwStyle = WS_VSCROLL;
				}
				else if (FlagSet(sbFlags, SBF_HORIZONTAL))
				{
					parent->Information(I_SET_HSCROLL, this);
					dwStyle = WS_HSCROLL;
				}
				if (parent->parent && parent->parent->Inherited(ID_COMBO_BOX))
				{
					dwStyle |= CBS_DISABLENOSCROLL;
					parent->parent->Information(I_SET_DWSTYLE, &dwStyle);
				}
				else if (parent->Inherited(ID_LIST))
				{
					dwStyle |= LBS_DISABLENOSCROLL;
					parent->Information(I_SET_DWSTYLE, &dwStyle);
				}
				else
					parent->Information(I_SET_DWSTYLE, &dwStyle);
			}
			else
			{
				if (FlagSet(sbFlags, SBF_SLIDER))
				{
					if (FlagSet(sbFlags, SBF_VERTICAL))
						dwStyle |= TBS_VERT;
					else if (FlagSet(sbFlags, SBF_HORIZONTAL))
						dwStyle |= TBS_HORZ;
					dwStyle |= TBS_NOTICKS;
				}
				else if (FlagSet(sbFlags, SBF_VERTICAL))
					dwStyle |= SBS_VERT;
				else if (FlagSet(sbFlags, SBF_HORIZONTAL))
					dwStyle |= SBS_HORZ;
				else
					dwStyle |= SBS_SIZEBOX;
			}
			break;

		case S_CHANGED:
		case S_CREATE:
			{
			if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				break;

			dwStyle &= ~WS_BORDER;

			UI_WINDOW_OBJECT::Event(event);

			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				if (FlagSet(sbFlags, SBF_VERTICAL))
				{
					relative.right =relative.left + 22;
					trueRegion.right = trueRegion.left + relative.Width() - 1;
				}
				else
				{
					relative.bottom =relative.top + 22;
					trueRegion.bottom = trueRegion.top + relative.Height() -1;
				}
			}
			else
			{
				if (FlagSet(sbFlags, SBF_CORNER | SBF_VERTICAL))
					if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					{
						relative.left = relative.right - GetSystemMetrics(SM_CXVSCROLL);
						trueRegion.right = trueRegion.left + relative.Width() - 1;
					}
					else
					{
						relative.right = relative.left + GetSystemMetrics(SM_CXVSCROLL);
						trueRegion.right = trueRegion.left + relative.Width() - 1;
					}
				if (FlagSet(sbFlags, SBF_CORNER | SBF_HORIZONTAL))
					if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
					{
						relative.top = relative.bottom - GetSystemMetrics(SM_CYHSCROLL);
						trueRegion.top = trueRegion.bottom + relative.Height() - 1;
					}
					else
					{
						relative.bottom = relative.top + GetSystemMetrics(SM_CYHSCROLL);
						trueRegion.bottom = trueRegion.top + relative.Height() -1;
					}
			}
			}
			// Fall Through to S_REGISTER_OBJECT.

		case S_REGISTER_OBJECT:
			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				InitCommonControls();
#if defined(ZIL_UNICODE)
				RegisterObject("UIW_SCROLL_BAR(slider)", "msctls_trackbar32", &_sliderCallback);
#else
				RegisterObject("UIW_SCROLL_BAR(slider)", TRACKBAR_CLASS, &_sliderCallback);
#endif
			}
			else
				RegisterObject("UIW_SCROLL_BAR", "SCROLLBAR", &_scrollCallback);
			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				SendMessage(screenID, TBM_SETRANGE, FALSE, MAKELONG(scroll.minimum / scroll.delta, scroll.maximum / scroll.delta));
				if (FlagSet(sbFlags, SBF_VERTICAL))
					SendMessage(screenID, TBM_SETPOS, TRUE,  (scroll.maximum + scroll.minimum - scroll.current) / scroll.delta);
				else
					SendMessage(screenID, TBM_SETPOS, TRUE,  scroll.current / scroll.delta);
			}
			else
			{
				if (scroll.maximum > 0)
				{
					SetScrollRange(screenID, SB_CTL, scroll.minimum / scroll.delta, scroll.maximum / scroll.delta, FALSE);
					SetScrollPos(screenID, SB_CTL, scroll.current / scroll.delta, TRUE);
				}
				else
					SetScrollRange(screenID, SB_CTL, 0, 0, FALSE);
			}
			break;

		case S_VSCROLL:
		case S_HSCROLL:
			{
			scroll.current += event.scroll.delta;

			if (FlagsSet(sbFlags, SBF_SLIDER))
			{
				if (FlagSet(sbFlags, SBF_VERTICAL))
					SendMessage(screenID, TBM_SETPOS, TRUE,  (scroll.maximum + scroll.minimum - scroll.current) / scroll.delta);
				else
					SendMessage(screenID, TBM_SETPOS, TRUE,  scroll.current / scroll.delta);
			}
			else
			{
				ZIL_SCREENID windowID;
				int scrollID;
				if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				{
					windowID = parent->screenID;
					scrollID = FlagSet(sbFlags, SBF_VERTICAL) ? SB_VERT : SB_HORZ;
				}
				else
				{
					windowID = screenID;
					scrollID = SB_CTL;
				}

				SetScrollPos(windowID, scrollID, scroll.current / scroll.delta, TRUE);
			}
			}
			break;

		case S_VSCROLL_SET:
		case S_HSCROLL_SET:
			{
			if ((ccode == S_HSCROLL_SET && !FlagSet(sbFlags, SBF_HORIZONTAL)) ||
				(ccode == S_VSCROLL_SET && !FlagSet(sbFlags, SBF_VERTICAL)))
				return (S_ERROR);
			scroll = event.scroll;

			if (FlagSet(sbFlags, SBF_SLIDER))
			{
				SendMessage(screenID, TBM_SETRANGE, TRUE, MAKELONG(scroll.minimum / scroll.delta, scroll.maximum / scroll.delta));
				if (FlagSet(sbFlags, SBF_VERTICAL))
					SendMessage(screenID, TBM_SETPOS, TRUE,  (scroll.maximum + scroll.minimum - scroll.current) / scroll.delta);
				else
					SendMessage(screenID, TBM_SETPOS, TRUE,  scroll.current / scroll.delta);
			}
			else
			{
				ZIL_SCREENID windowID;
				int scrollID;
				if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
				{
					windowID = parent->screenID;
					scrollID = FlagSet(sbFlags, SBF_VERTICAL) ? SB_VERT : SB_HORZ;
				}
				else
				{
					windowID = screenID;
					scrollID = SB_CTL;
				}

				if (scroll.maximum > 0)
				{
					SetScrollRange(windowID, scrollID, scroll.minimum / scroll.delta, scroll.maximum / scroll.delta, FALSE);
					SetScrollPos(windowID, scrollID, scroll.current / scroll.delta, TRUE);
				}
				else
				{
					ZIL_SCREENID windowID = screenID ? screenID : parent->screenID;
					SetScrollRange(windowID, scrollID, 0, 0, FALSE);
				}
			}
			}
			break;

		case L_PGUP:
		case L_PGDN:
		case L_LEFT:
		case L_RIGHT:
		case L_UP:
		case L_DOWN:
			woStatus |= WOS_WINDOWS_ACTION;
			ccode = UI_WINDOW_OBJECT::Event(event);
			woStatus &= ~WOS_WINDOWS_ACTION;
			break;

		default:
			if (event.type != E_MSWINDOWS)
				ccode = UI_WINDOW_OBJECT::Event(event);
			else
				ccode = S_ERROR;
			break;
		}
	}

	// Return the control code.
	return (ccode);
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_SCROLL_BAR::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	if (!parent)
		return;

	if (FlagSet(woFlags, WOF_SUPPORT_OBJECT))
	{
		parent->Information(I_SET_DWSTYLE, &dwStyle);
		return;
	}

	// See if the field needs to be re-computed.
	if (objectID == ID_SCROLL_BAR && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

