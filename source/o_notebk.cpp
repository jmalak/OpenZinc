//	Zinc Interface Library - O_NOTEBK.CPP
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


#include <stdio.h>
#define INCL_WINSTDBOOK
#define INCL_WINDIALOGS
#include "ui_win.hpp"

UIW_NOTEBOOK::UIW_NOTEBOOK(void) :
	UIW_WINDOW(0, 0, 0, 0, WOF_NON_FIELD_REGION),
	totalPages(0), currentPage(0), searchPage(-1), tabWidth(0)
{
	UIW_NOTEBOOK::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_NOTEBOOK::~UIW_NOTEBOOK(void)
{
}

UI_WINDOW_OBJECT *UIW_NOTEBOOK::Add(UI_WINDOW_OBJECT *object)
{
	if (Index(object) == -1)
		totalPages++;
	return (UIW_WINDOW::Add(object));
}

EVENT_TYPE UIW_NOTEBOOK::DrawItem(const UI_EVENT &, EVENT_TYPE)
{
	return (TRUE);
}

EVENT_TYPE UIW_NOTEBOOK::Event(const UI_EVENT &event)
{
	static PFNWP _notebookCallback = ZIL_NULLP(FNWP);

	// Check the event type.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case E_OS2:
		if (event.message.msg == WM_CONTROL)
		{
			USHORT notification = HIWORD(event.message.mp1);
			if (notification == BKN_PAGESELECTED)
			{
				PAGESELECTNOTIFY *pageSelected;
				pageSelected = (PAGESELECTNOTIFY *)event.message.mp2;
				if (pageSelected->ulPageIdCur != pageSelected->ulPageIdNew &&
					pageSelected->ulPageIdCur != 0)
				{
					int pageIndex = 0;
					ULONG pageID = (ULONG)WinSendMsg(screenID, BKM_QUERYPAGEID,
						(MPARAM)NULL, MPFROM2SHORT(BKA_FIRST, BKA_FIRST));
					while(pageID != pageSelected->ulPageIdNew)
					{
						pageIndex++;
						pageID = (ULONG)WinSendMsg(screenID, BKM_QUERYPAGEID,
						(MPARAM)pageID, MPFROM2SHORT(BKA_NEXT, BKA_NEXT));
					}

					if (currentPage == pageIndex)
						break;
					currentPage = pageIndex;
					EVENT_TYPE nCode = S_CHANGE_PAGE - pageIndex;
					Event(nCode);
				}
			}
		}

		if (event.message.msg == WM_BUTTON1DBLCLK || event.message.msg == WM_CHAR)
			return (UI_WINDOW_OBJECT::Event(event));
		else
			return (DefaultCallback(event));

	case S_INITIALIZE:
		parent->Information(I_SET_DEFAULT_OBJECT, this);
		if (!FlagSet(woStatus, WOS_EDIT_MODE))
			Root()->NumberID(0x1000);
		if (FlagSet(parent->woFlags, WOF_BORDER))
		{
			parent->woFlags &= ~WOF_BORDER;
			ULONG flFlag = FCF_BORDER;
			parent->Information(I_CLEAR_FLFLAG, &flFlag);
		}
		UIW_WINDOW::Event(event);
		break;

	case S_CREATE:
	case S_CHANGED:
		{
		UI_WINDOW_OBJECT::Event(event);

		int tabWidth = 0;
		if (ccode == S_CREATE)
		{
			for (UI_WINDOW_OBJECT *object = First(); object; object = object->Next())
			{
				object->Event(event);

				ZIL_ICHAR *tabText;
				UI_WINDOW_OBJECT *title = object->Get(NUMID_TITLE);
				title->Information(I_GET_TEXT, &tabText);
				ULONG pageID = (ULONG)WinSendMsg(screenID, BKM_INSERTPAGE,
					(MPARAM)NULL, MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_LAST));
#if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
				char *TEXT_ = MapText(tabText, ZIL_NULLP(char), FALSE);
#else
#				define TEXT_ tabText
#endif
				WinSendMsg(screenID, BKM_SETTABTEXT, MPFROMLONG(pageID), (MPARAM)TEXT_);
				ZIL_SCREENID pageFrameID;
				object->Information(I_GET_FRAMEID, &pageFrameID);
				WinSendMsg(screenID, BKM_SETPAGEWINDOWHWND, MPFROMLONG(pageID),
					MPFROMHWND(pageFrameID));
				tabWidth = MaxValue(tabWidth, display->TextWidth(tabText, screenID));
			}

			// Set tab size.
			tabWidth += 2 * display->cellWidth;
			WinSendMsg(screenID, BKM_SETDIMENSIONS, MPFROM2SHORT(tabWidth, tabHeight), (MPARAM)BKA_MAJORTAB);
		}
		}
		break;

	case S_REGISTER_OBJECT:
		{
		flStyle |= BKS_SPIRALBIND | BKS_BACKPAGESTR | BKS_MAJORTABTOP |
			BKS_ROUNDEDTABS | BKS_TABTEXTCENTER;
		RegisterObject("UIW_NOTEBOOK", WC_NOTEBOOK, &_notebookCallback, ZIL_NULLP(ZIL_ICHAR));
		ULONG color;
		if (WinQueryPresParam(parent->screenID, PP_BACKGROUNDCOLORINDEX, 0,
			ZIL_NULLP(ULONG), sizeof(color), &color, 0) != 0)
			WinSetPresParam(screenID, PP_BACKGROUNDCOLORINDEX, sizeof(color), &color);
		color = SYSCLR_DIALOGBACKGROUND;
		WinSendMsg(screenID, BKM_SETNOTEBOOKCOLORS, (MPARAM)color,
			(MPARAM)BKA_BACKGROUNDPAGECOLORINDEX);
		WinSendMsg(screenID, BKM_SETNOTEBOOKCOLORS, (MPARAM)color,
			(MPARAM)BKA_BACKGROUNDMAJORCOLORINDEX);
		}
		break;

	case S_ADD_OBJECT:
		if (event.windowObject->SearchID() == ID_WINDOW)
			Add(event.windowObject);
		else if (currentPage)
			((UI_WINDOW_OBJECT *)UI_LIST::Get(searchPage))->Event(event);
		break;

    case S_SUBTRACT_OBJECT:
		if (Index(event.windowObject) != -1 &&
			event.windowObject->SearchID() == ID_WINDOW)
			Subtract(event.windowObject);
		else
			ccode = S_ERROR; // cannot subtract tabs.
		break;

	case L_SELECT:
		ccode = Current()->Event(event);
		break;

	default:
		if (ccode <= S_CHANGE_PAGE)
		{
			int newPage = -event.type + S_CHANGE_PAGE;
			if (currentPage == newPage)
				break;
			currentPage = newPage;
			UIW_WINDOW *window = (UIW_WINDOW *)UI_LIST::Get(currentPage);

			if (window)
			{
				ULONG pageID = (ULONG)WinSendMsg(screenID, BKM_QUERYPAGEID,
					(MPARAM)0, MPFROM2SHORT(BKA_FIRST, 0));
				for (int i = 0; i < currentPage; i++)
					pageID = (ULONG)WinSendMsg(screenID, BKM_QUERYPAGEID,
						(MPARAM)pageID, MPFROM2SHORT(BKA_NEXT, 0));
				WinSendMsg(screenID, BKM_TURNTOPAGE, (MPARAM)pageID, (MPARAM)0);
			}
		}
		else
			ccode = UIW_WINDOW::Event(event);
	}

	return (ccode);
}

void *UIW_NOTEBOOK::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		data = UIW_WINDOW::Information(request, data, objectID);
		searchID = windowID[0] = ID_NOTEBOOK;
		windowID[1] = ID_WINDOW;
		tabHeight = display->cellHeight;
		break;

	case I_CHANGED_FLAGS:
		if (FlagSet(woFlags, WOF_NON_FIELD_REGION))
			parent->Information(I_CHANGED_FLAGS, data, objectID);
		else
			UIW_WINDOW::Information(I_CHANGED_FLAGS, data, objectID);
		break;

	case I_SET_SEARCH_PAGE:
		searchPage = *(int *)data;
		break;

	case I_GET_NUMBERID_OBJECT:
	case I_GET_STRINGID_OBJECT:
		if (searchPage == -1)
			data = UIW_WINDOW::Information(request, data, objectID);
		else
		{
			UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)UI_LIST::Get(searchPage);
			data = object->Information(request, data, objectID);
		}
		break;

    case I_GET_TEXT:
	case I_SET_TEXT:
		{
		if (request == I_SET_TEXT)
		{
			ULONG pageID = (ULONG)WinSendMsg(screenID, BKM_QUERYPAGEID,
				(MPARAM)0, MPFROM2SHORT(BKA_FIRST, 0));
			for (int i = (searchPage == -1 ? currentPage : searchPage); i > 0; i--)
				pageID = (ULONG)WinSendMsg(screenID, BKM_QUERYPAGEID,
					(MPARAM)pageID, MPFROM2SHORT(BKA_NEXT, 0));
#	if defined(ZIL_UNICODE) || defined(ZIL_ISO8859_1)
			char *TEXT_ = MapText((ZIL_ICHAR *)data, ZIL_NULLP(char), FALSE);
#	else
#			define TEXT_ (ZIL_ICHAR *)data
#	endif
 			WinSendMsg(screenID, BKM_SETTABTEXT, MPFROMLONG(pageID), (MPARAM)TEXT_);
 		}
 		UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)UI_LIST::Get((searchPage == -1) ? currentPage : searchPage);
 		return (object ? object->Information(request, data, objectID) : ZIL_NULLP(void));
		}

	default:
		data = UIW_WINDOW::Information(request, data, objectID);
		break;
	}

	// Return the information.
	return (data);
}

UI_WINDOW_OBJECT *UIW_NOTEBOOK::Subtract(UI_WINDOW_OBJECT *object)
{
	// Check for a valid notebook page.
	int pageIndex = Index(object);
	if (pageIndex == -1)
		return (ZIL_NULLP(UI_WINDOW_OBJECT));

	// Remove the page.
	UI_WINDOW_OBJECT *nextObject = UIW_WINDOW::Subtract(object);
	totalPages--;
	searchPage = currentPage = current ? Index(current) : 0;
	if (screenID)
	{
		ULONG pageID = (ULONG)WinSendMsg(screenID, BKM_QUERYPAGEID,
			(MPARAM)0, MPFROM2SHORT(BKA_FIRST, 0));
		for (int i = 0; i < pageIndex; i++)
			pageID = (ULONG)WinSendMsg(screenID, BKM_QUERYPAGEID,
				(MPARAM)pageID, MPFROM2SHORT(BKA_NEXT, 0));
		WinSendMsg(screenID, BKM_DELETEPAGE, (MPARAM)pageID, (MPARAM)BKA_SINGLE);
	}

	return (nextObject);
}

// ----- ZIL_PERSISTENCE ----------------------------------------------------

#if defined(ZIL_LOAD)
ZIL_NEW_FUNCTION UIW_NOTEBOOK::NewFunction(void) { return (UIW_NOTEBOOK::New); }

UIW_NOTEBOOK::UIW_NOTEBOOK(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable) :
	UIW_WINDOW(0, 0, 0, 0, WOF_NON_FIELD_REGION),
	totalPages(0), currentPage(0), searchPage(-1), tabWidth(0)
{
	Load(name, directory, file, objectTable, userTable);
	UI_WINDOW_OBJECT::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_WINDOW::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
	UIW_NOTEBOOK::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

void UIW_NOTEBOOK::Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *directory,
	ZIL_STORAGE_OBJECT_READ_ONLY *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the notebook information.
	UI_WINDOW_OBJECT::Load(name, directory, file, objectTable, userTable);
	file->Load(&wnFlags);
	file->Load(&compareFunctionName);

	// Store the page information.
	ZIL_INT16 noOfObjects; file->Load(&noOfObjects);
	for (int i = 0; i < noOfObjects; i++)
	{
		UI_WINDOW_OBJECT *object = UI_WINDOW_OBJECT::New(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
		Add((UIW_WINDOW *)object);
	}
}
#endif

#if defined(ZIL_STORE)
void UIW_NOTEBOOK::Store(const ZIL_ICHAR *name, ZIL_STORAGE *directory,
	ZIL_STORAGE_OBJECT *file, UI_ITEM *objectTable, UI_ITEM *userTable)
{
	// Store the notebook information.
	UI_WINDOW_OBJECT::Store(name, directory, file, objectTable, userTable);
	file->Store(wnFlags);
	file->Store(compareFunctionName);

	// Store the page information.
	ZIL_INT16 noOfObjects = UI_LIST::Count();
	file->Store(noOfObjects);
	UIW_WINDOW *window = (UIW_WINDOW *)First();
	for ( ; window; window = (UIW_WINDOW *)window->Next())
	{
		ZIL_OBJECTID searchID = window->SearchID();
		file->Store(searchID);
		window->Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
	}
}
#endif
