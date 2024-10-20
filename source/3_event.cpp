//	Zinc Interface Library - W_EVENT.CPP
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


#include "ui_evt.hpp"

UI_EVENT::UI_EVENT(EVENT_TYPE _type, HWND hWnd, UINT wMsg, WPARAM wParam,
	LPARAM lParam) : type(_type)
{
	// Set up the Windows event message.
	rawCode = modifiers = 0;
	message.hwnd = hWnd;
	message.message = wMsg;
	message.wParam = wParam,
	message.lParam = lParam;

	// Check for mouse or keyboard information.
	if ((wMsg >= WM_MOUSEFIRST && wMsg <= WM_MOUSELAST) ||
		(wMsg >= WM_NCMOUSEMOVE && wMsg <= WM_NCMBUTTONDBLCLK))
	{
		if (wMsg == WM_LBUTTONDOWN || wMsg == WM_LBUTTONDBLCLK ||
			wMsg == WM_NCLBUTTONDOWN || wMsg == WM_NCLBUTTONDBLCLK)
				rawCode = M_LEFT | M_LEFT_CHANGE;
		else if (wMsg == WM_LBUTTONUP || wMsg == WM_NCLBUTTONUP)
			rawCode = M_LEFT_CHANGE;
		else if (wMsg == WM_RBUTTONDOWN || wMsg == WM_RBUTTONDBLCLK ||
			wMsg == WM_NCRBUTTONDOWN || wMsg == WM_NCRBUTTONDBLCLK)
				rawCode = M_RIGHT | M_RIGHT_CHANGE;
		else if (wMsg == WM_RBUTTONUP || wMsg == WM_NCRBUTTONUP)
			rawCode = M_RIGHT_CHANGE;
		else if (wMsg == WM_MBUTTONDOWN || wMsg == WM_MBUTTONDBLCLK ||
			wMsg == WM_NCMBUTTONDOWN || wMsg == WM_NCMBUTTONDBLCLK)
				rawCode = M_MIDDLE | M_MIDDLE_CHANGE;
		else if (wMsg == WM_MBUTTONUP || wMsg == WM_NCMBUTTONUP)
			rawCode = M_MIDDLE_CHANGE;

		if (FlagSet(wParam, MK_LBUTTON))
			rawCode |= M_LEFT;
		if (FlagSet(wParam, MK_RBUTTON))
			rawCode |= M_RIGHT;
		if (FlagSet(wParam, MK_MBUTTON))
			rawCode |= M_MIDDLE;

 		if (FlagSet(wParam, MK_SHIFT))
			rawCode |= S_SHIFT;
		if (FlagSet(wParam, MK_CONTROL))
			rawCode |= S_CTRL;

		POINTS point = MAKEPOINTS(lParam);
		position.column = point.x;
		position.line = point.y;
		if (wMsg < WM_MOUSEFIRST)
			SendMessage(hWnd, ZM_CONVERTNCPOINT, 0, (LPARAM)&position);
		else
			SendMessage(hWnd, ZM_CONVERTPOINT, 0, (LPARAM)&position);
	}
	else if (wMsg >= WM_KEYFIRST && wMsg <= WM_KEYLAST)
	{
		key.shiftState = 0;
		rawCode = key.value = wParam;
		static BYTE ZIL_FARDATA keyState[256];
		GetKeyboardState(keyState);
		if (keyState[VK_SHIFT] & 0x80)
			key.shiftState |= S_SHIFT;
		if (keyState[VK_CONTROL] & 0x80)
			key.shiftState |= S_CTRL;
		if (keyState[VK_MENU] & 0x80)
			key.shiftState |= S_ALT;
		if (keyState[VK_NUMLOCK] & 0x80)
			key.shiftState |= S_NUM_LOCK;
		if (keyState[VK_CAPITAL] & 0x80)
			key.shiftState |= S_CAPS_LOCK;
		if (keyState[VK_INSERT] & 0x80)
			key.shiftState |= S_INSERT;
		key.value |= key.shiftState << 8;
	}
}

EVENT_TYPE UI_EVENT::InputType(void) const
{
	// Check for mouse or keyboard information.
	if (type == E_MSWINDOWS)
	{
		if (message.message == WM_CHAR ||
			message.message == WM_SYSCHAR ||
			(message.message == WM_KEYDOWN && FlagSet(key.shiftState, S_CTRL)))
			return (E_KEY);
		else if (message.message >= WM_MOUSEFIRST && message.message <= WM_MOUSELAST)
			return (E_MOUSE);
	}
	return (type);
}

