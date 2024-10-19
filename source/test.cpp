//	HELLO1.CPP (HELLO) - Simple "Hello, World!" in a window.
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

//  May be freely copied, used and distributed.

#include <ui_win.hpp>

//static ZIL_ICHAR ZIL_FARDATA _i18nName[] = ZIL_TEXT("xxx.dat");
//ZIL_ICHAR *ZIL_I18N::i18nName = _i18nName;
//static ZIL_ICHAR title[] = ZIL_TEXT("Hello World Window");
//static ZIL_ICHAR text[] = ZIL_TEXT("Hello, World!");
static ZIL_ICHAR title[] = { 'H','e','l','l','o',' ','W','o','r','l','d',' ','W','i','n','d','o','w',0 };
static ZIL_ICHAR text[] = { 'H','e','l','l','o',',',' ','W','o','r','l','d','!',' ','E','s','p','e','c','i','a','l','l','y',' ','J','a','c','k',0 };

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	// Create the hello world window.
	UIW_WINDOW *window = new UIW_WINDOW(5, 5, 40, 6);

	// Add the window objects to the window.
	*window
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE(title)
		+ new UIW_TEXT(0, 0, 0, 0, text, 256, WNF_NO_FLAGS, WOF_NON_FIELD_REGION);

	// Add the window to the window manager.
	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	return (0);
}
