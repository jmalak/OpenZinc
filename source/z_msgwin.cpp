//	Zinc Interface Library - Z_MSGWIN.CPP
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


extern "C"
{
#include <stdio.h>
}
#include "ui_win.hpp"
#include "z_stdarg.hpp"

#define ZIL_ZAF_MESSAGE_WINDOW_DATA
#include "lang_def.cpp"

ZAF_MESSAGE_WINDOW::ZAF_MESSAGE_WINDOW(ZIL_ICHAR *title, ZIL_ICHAR *icon,
	MSG_FLAGS msgFlags, MSG_FLAGS defFlag, ZIL_ICHAR *format, ...) :
	UIW_WINDOW(0, 0, 0, 0, WOF_NO_FLAGS, WOAF_MODAL | WOAF_DIALOG_OBJECT |	WOAF_NO_SIZE | WOAF_NO_DESTROY)
{
	if (!defaultInitialized)
	{
		ZIL_LANGUAGE_MANAGER::SetLanguage(_className, STRINGS);
		defaultInitialized = TRUE;
	}
	myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className);
	searchID = windowID[0] = ID_ZAF_MESSAGE_WINDOW;
	windowID[1] = ID_WINDOW;
	
	// Add border.
	Add(new UIW_BORDER);

	// Add the title and system button (if specified).
	if (title)
	{
		Add(UIW_SYSTEM_BUTTON::Generic());
		Add(new UIW_TITLE(title));
	}

	// Add the icon (if applicable).
	if (display->isText)
		icon = ZIL_NULLP(ZIL_ICHAR);
	if (icon)
		Add(new UIW_ICON(2, 1, icon));

	// Get the formatted text.
	ZIL_ICHAR text[1024];
	va_list arguments;
	va_start(arguments, format);
	ZIL_STDARG::vsprintf(text, format, arguments);
	va_end(arguments);

	// Calculate text size.
	int lineHeight = display->cellHeight
		- display->preSpace
		- display->postSpace;

	// Adjust the environment specific line height with magic numbers.
	// (A better method should be found.)
	if (!display->isText)
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
		lineHeight -= 4;
#elif defined(ZIL_MSWINDOWS)
		lineHeight -= 6;
#elif defined(ZIL_OS2)
		lineHeight -= 6;
#elif defined(ZIL_MOTIF)
		lineHeight -= 8;
#elif defined(ZIL_MACINTOSH)
		lineHeight -= 4;
#elif defined(ZIL_NEXTSTEP)
		lineHeight -= 2;
#endif

	int textWidth = 0;
	int textHeight = 0;
	int index = 0;
	ZIL_ICHAR *linePtr = text;
	do
	{
		if (text[index] == '\n' || text[index] == '\r' || text[index] == '\0')
		{
			int isCR = (text[index] == '\r');
			textHeight += lineHeight;
			ZIL_ICHAR saveChar = text[index];
			text[index] = '\0';
			int tempWidth = display->TextWidth(linePtr);
			text[index] = saveChar;
			if (tempWidth > textWidth)
				textWidth = tempWidth;
			if (isCR && text[index + 1] == '\n')
				index++;
			linePtr = text + index + 1;
		}
	} while (text[index++] != '\0');
	textWidth = (textWidth + 3 * display->cellWidth - 1) / display->cellWidth;
	textHeight = (textHeight + display->cellHeight - 1) / display->cellHeight;

	// Calculate size and number of buttons.
	int buttons = 0;
	int buttonWidth = 10 * display->cellWidth;
	MSG_FLAGS testFlag = 1;
	do
	{
		if (FlagSet(msgFlags, testFlag))
		{
			int tempButtonWidth = display->TextWidth(myLanguage->GetMessage(testFlag));
			if (tempButtonWidth > buttonWidth)
				buttonWidth = tempButtonWidth;
			buttons++;
		}
		testFlag <<= 1;
	} while (testFlag);
	buttonWidth = (buttonWidth + 3 * display->cellWidth - 1) / display->cellWidth;

	// Set window size.
	int windowWidth = textWidth + (icon ? 6 : 0) + 4;
	if (windowWidth < buttons * (buttonWidth + 2) + 5)
		windowWidth = buttons * (buttonWidth + 2) + 5;
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS) || defined(ZIL_MACINTOSH)
	int windowHeight = textHeight + (title ? 5 : 4);
#elif defined(ZIL_MOTIF)
	int windowHeight = textHeight + 5;
#elif defined(ZIL_NEXTSTEP)
	int windowHeight = textHeight + ((icon && textHeight < 2) ? 5 : 4);
#endif
	if (display->isText)
		windowHeight++;
	relative.right = relative.left + windowWidth - 1;
	relative.bottom = relative.top + windowHeight - 1;
	windowManager->Center(this);

	// Create the text field.
	UIW_TEXT *textField;
	textField = new UIW_TEXT((2 + (icon ? 6 : 0)) * display->cellWidth,
		display->cellHeight, textWidth * display->cellWidth,
		textHeight * display->cellHeight + display->cellHeight / 2,
		text, -1, WNF_NO_WRAP, WOF_VIEW_ONLY);
	if (!display->isText)
		textField->woStatus |= WOS_GRAPHICS;
	textField->woAdvancedFlags |= WOAF_NON_CURRENT;

	Add(textField);

	// Add the buttons.
	int left = (windowWidth - buttons * (buttonWidth + 2)) / 2 + 1;
	int top = textHeight + 2;
	if (display->isText)
		top++;
	testFlag = 1;
	index = 0;
	do
	{
		if (FlagSet(msgFlags, testFlag))
		{
			UIW_BUTTON *button = new UIW_BUTTON (left, top, buttonWidth,
				myLanguage->GetMessage(testFlag), BTF_NO_TOGGLE | BTF_AUTO_SIZE | BTF_SEND_MESSAGE |
				(testFlag == defFlag ? BTF_DEFAULT_BUTTON : 0), WOF_JUSTIFY_CENTER,
				ZIL_NULLF(ZIL_USER_FUNCTION), (testFlag == ZIL_MSG_HELP ? L_HELP : DLG_DIALOG_FIRST + index));
			if (testFlag == defFlag)
				button->woStatus |= WOS_CURRENT;
			Add(button);
			left += buttonWidth + 2;
		}
		index++;
		testFlag <<= 1;
	} while (testFlag);
}

ZAF_MESSAGE_WINDOW::~ZAF_MESSAGE_WINDOW(void)
{
	ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage);
}

EVENT_TYPE ZAF_MESSAGE_WINDOW::Control(void)
{
	windowManager->Add(this);
#if defined(ZIL_NEXTSTEP)
	// Let NEXTSTEP handle the Application modal window.
	[NXApp runModalFor:screenID];
#endif
	// Wait for user response.
	EVENT_TYPE returnValue = 0;
	do
	{
		UI_EVENT event;
		eventManager->Get(event);
		if (event.type >= DLG_DIALOG_FIRST && event.type <= DLG_DIALOG_LAST)
			returnValue = event.type;
		else
			windowManager->Event(event);
	} while (!returnValue && screenID);

	windowManager->Subtract(this);
	return returnValue;
}

