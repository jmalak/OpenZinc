//	Zinc Interface Library - Z_NOTEBK.CPP
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


#if defined(ZIL_NEXTSTEP)
#include "zincapp.h"
#endif

static UI_PALETTE_MAP ZIL_FARDATA _paletteMapTable[] =
{
	// Notebook palette.
	{ ID_WINDOW_OBJECT, PM_NON_SELECTABLE,
		{ ' ', attrib(DARKGRAY, LIGHTGRAY), attrib(MONO_BLACK, MONO_NORMAL),
		PTN_SOLID_FILL, DARKGRAY, LIGHTGRAY, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE } },
	{ ID_WINDOW_OBJECT, PM_ANY,
		{ ' ', attrib(BLACK, WHITE), attrib(MONO_BLACK, MONO_NORMAL),
		PTN_SOLID_FILL, BLACK, LIGHTGRAY, BW_BLACK, BW_WHITE, GS_BLACK, GS_WHITE } },

	// Special palettes.
	{ ID_OUTLINE, PM_ANY,
		{ ' ', attrib(WHITE, LIGHTGRAY), attrib(MONO_NORMAL, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK } },
	{ ID_WHITE_SHADOW, PM_ANY,
		{ ' ', attrib(WHITE, WHITE), attrib(MONO_HIGH, MONO_HIGH),
		PTN_SOLID_FILL, WHITE, WHITE, BW_WHITE, BW_WHITE, GS_WHITE, GS_WHITE } },
	{ ID_LIGHT_SHADOW, PM_ANY,
		{ ' ', attrib(LIGHTGRAY, LIGHTGRAY), attrib(MONO_NORMAL, MONO_NORMAL),
		PTN_SOLID_FILL, LIGHTGRAY, LIGHTGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK } },
	{ ID_DARK_SHADOW, PM_ANY,
		{ ' ', attrib(DARKGRAY, LIGHTGRAY), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, DARKGRAY, DARKGRAY, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK } },
	{ ID_BLACK_SHADOW, PM_ANY,
		{ ' ', attrib(BLACK, BLACK), attrib(MONO_BLACK, MONO_BLACK),
		PTN_SOLID_FILL, BLACK, BLACK, BW_BLACK, BW_BLACK, GS_BLACK, GS_BLACK } },

	// End of array.
	{ ID_END, 0, { 0, 0, 0, 0, 0, 0 } }
};

// ----- NOTEBOOK_TAB -------------------------------------------------------

class NOTEBOOK_TAB : public UIW_BUTTON
{
public:
	NOTEBOOK_TAB(ZIL_ICHAR *text, WOF_FLAGS parentFlags, EVENT_TYPE value);
	EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE Event(const UI_EVENT &event);
};

NOTEBOOK_TAB::NOTEBOOK_TAB(ZIL_ICHAR *text, WOF_FLAGS parentFlags, EVENT_TYPE _value) :
	UIW_BUTTON(0, 0, 0, text, BTF_SEND_MESSAGE | BTF_NO_TOGGLE,
	WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT | (parentFlags & (WOF_VIEW_ONLY | WOF_NON_SELECTABLE)),
	ZIL_NULLF(ZIL_USER_FUNCTION), S_CHANGE_PAGE - _value)
{
	woStatus |= WOS_OWNERDRAW;
	woAdvancedFlags |= WOAF_NON_CURRENT;
	depth = 0;
}

#if defined(ZIL_WIN32)
EVENT_TYPE NOTEBOOK_TAB::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	const int chisel = 2;

	display->VirtualGet(screenID, trueRegion);

	// Get Drawing palettes.
	UI_PALETTE *palette = &display->colorMap[LIGHTGRAY];
	UI_PALETTE *whiteShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
	UI_PALETTE *lightShadow = LogicalPalette(ccode, ID_LIGHT_SHADOW);
	UI_PALETTE *darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
	UI_PALETTE *blackShadow = LogicalPalette(ccode, ID_OUTLINE);

	// Determine selection status.
	int selected = FALSE, selectedOnLeft = FALSE, selectedOnRight = FALSE;
	if (FlagSet(woStatus, WOS_SELECTED))
		selected = TRUE;
	if (previous && FlagSet(Previous()->woStatus, WOS_SELECTED))
		selectedOnLeft = TRUE;
	else if (next && FlagSet(Next()->woStatus, WOS_SELECTED))
		selectedOnRight = TRUE;

	// Keep track of selection changes.
	if (selected)
		userFlags |= WOS_SELECTED;
	else if (FlagSet(userFlags, WOS_SELECTED))
	{
		// Tab has been unselected, so overlapped regions of adjacent
		// tabs must be redrawn.

		// Redraw the button to the left.
		if (previous && !selectedOnLeft)
			Previous()->Event(UI_EVENT(S_REDISPLAY));
		// Redraw the button to the right.
		if (next && !selectedOnRight)
			Next()->Event(UI_EVENT(S_REDISPLAY));
		userFlags &= ~WOS_SELECTED;
	}

	// Clear the background.
	UI_REGION region = trueRegion;
	region.top -= 2;
	if (selected)
	{
		region.left -= 2;
		region.right += 2;
	}
	else
	{
		if (!next)
			region.right += 2;
		else if (selectedOnRight)
			region.right -= 2;
		if (selectedOnLeft)
			region.left += 2;
		else if (!previous)
			region.left -= 2;
	}
	display->Rectangle(screenID, region, palette, 0, TRUE, FALSE, &clip);

	// Draw the tab shadowing.
	region = trueRegion;
	if (selected)
	{
		region.top -= 2;
		region.left -= 2;
		region.right += 2;
	}
	if (selectedOnLeft)
		region.left += 2;
	else if (selectedOnRight)
		region.right -= 2;

	if (!selectedOnLeft)
	{
		// Draw left shadow.
		int point[6];
		point[0] = region.left + chisel; point[1] = region.top;
		point[2] = region.left; point[3] = region.top + chisel;
		point[4] = region.left; point[5] = region.bottom + 1;
		display->Polygon(screenID, 3, point, whiteShadow, FALSE, FALSE, &clip);
		display->Line(screenID, region.left + 1, region.top + chisel,
			region.left + 1, region.bottom,	lightShadow, 1, FALSE, &clip);
		region.left += 2;
	}

	if (!selectedOnRight)
	{
		// Draw right shadow.
		int point[6];
		point[0] = region.right - chisel + 1; point[1] = region.top + 1;
		point[2] = region.right; point[3] = region.top + chisel;
		point[4] = region.right; point[5] = region.bottom + 1;
		display->Polygon(screenID, 3, point, blackShadow, FALSE, FALSE, &clip);
		display->Line(screenID, region.right - 1, region.top + chisel,
			region.right - 1, region.bottom, darkShadow, 1, FALSE, &clip);
		region.right -= 2;
	}

	// Draw the top shadow area.
	display->Line(screenID, region.left, region.top,
		region.right, region.top, whiteShadow, 1, FALSE, &clip);
	display->Line(screenID, region.left, region.top + 1,
		region.right, region.top + 1, lightShadow, 1, FALSE, &clip);
	region.top += 2;

	if (!selected)
	{
		// Draw the lower shadow area.
		int left = selectedOnLeft ? trueRegion.left : trueRegion.left - 2;
		int right = selectedOnRight ? trueRegion.right : trueRegion.right + 2;
		display->Line(screenID, left, region.bottom - 1, right, region.bottom - 1, whiteShadow, 1, FALSE, &clip);
		display->Line(screenID, left, region.bottom, right, region.bottom, lightShadow, 1, FALSE, &clip);
	}

	// Draw the tab text.
	if (text)
	{
		font = FNT_SMALL_FONT;
		blackShadow = LogicalPalette(ccode, FlagSet(woFlags, WOF_NON_SELECTABLE) ? ID_DARK_SHADOW : ID_BLACK_SHADOW);
		DrawText(screenID, region, text, blackShadow, FALSE, ccode);
	}

	display->VirtualPut(screenID);
	return (TRUE);
}
#else
EVENT_TYPE NOTEBOOK_TAB::DrawItem(const UI_EVENT &, EVENT_TYPE ccode)
{
	int chisel = display->cellWidth / 2;

	int point[20];
	UI_REGION region = trueRegion;
	UI_PALETTE *outline = LogicalPalette(ccode, ID_OUTLINE);
	UI_PALETTE *lightShadow = outline, *darkShadow = outline;

	display->VirtualGet(screenID, trueRegion);

	// Draw the remaining shadow features.
	if (FlagSet(woStatus, WOS_SELECTED))
	{
		lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
		darkShadow = LogicalPalette(ccode, ID_DARK_SHADOW);
		int offset = display->isText ? 0 : 3;

#	if defined(ZIL_MACINTOSH)
		UI_PALETTE *palette = display->isMono ? &display->colorMap[WHITE] : &display->colorMap[LIGHTGRAY];
#	else
		UI_PALETTE *palette = display->isText ? &display->colorMap[WHITE] : &display->colorMap[LIGHTGRAY];
#	endif
		display->Rectangle(screenID, region.left + offset, region.top + offset,
			region.right - offset, region.bottom, palette, 0, TRUE, FALSE, &clip);

		if (display->isText)
			outline = LogicalPalette(ccode, ID_BLACK_SHADOW);
		else
		{
			// Draw the left, top shadow area.
			point[0] = region.left + 1; point[1] = region.bottom;
			point[2] = region.left + 1; point[3] = region.top + chisel;
			point[4] = region.left + chisel; point[5] = region.top + 1;
			point[6] = region.right - chisel - 1; point[7] = region.top + 1;
			point[8] = region.right - chisel - 2; point[9] = region.top + 2;
			point[10] = region.left + chisel + 2; point[11] = region.top + 2;
			point[12] = region.left + 2; point[13] = region.top + chisel;
			point[14] = region.left + 2; point[15] = region.bottom;
			point[16] = point[0]; point[17] = point[1];
			display->Polygon(screenID, 9, point, lightShadow, TRUE, FALSE, &clip);
		
			// Draw the right shadow area.
			point[0] = region.right - 2; point[1] = region.bottom;
			point[2] = region.right - 2; point[3] = region.top + chisel;
			point[4] = region.right - chisel - 1; point[5] = region.top + 2;
			point[6] = region.right - chisel; point[7] = region.top + 1;
			point[8] = region.right - 1; point[9] = region.top + chisel;
			point[10] = region.right - 1; point[11] = region.bottom - 1;
			point[12] = point[0]; point[13] = point[1];
			display->Polygon(screenID, 7, point, darkShadow, TRUE, FALSE, &clip);
		}
	}
	else
	{
		// Draw the lower shadow area.
		int offset = display->isText ? 0 : 3;

#	if defined(ZIL_MACINTOSH)
		UI_PALETTE *palette = display->isMono ? &display->colorMap[WHITE] : &display->colorMap[LIGHTGRAY];
#	else
		UI_PALETTE *palette = &display->colorMap[LIGHTGRAY];
#	endif
		display->Rectangle(screenID, region.left, region.top, region.right, region.bottom - offset, palette, 0, TRUE, FALSE, &clip);
		if (!display->isText)
			for (int count = 0; count < UIW_NOTEBOOK::shadowWidth; count++)
			{
				int line = region.bottom - UIW_NOTEBOOK::shadowWidth + count + 1;
				display->Line(screenID, region.left, line, region.right, line, lightShadow, 1, FALSE, &clip);
				if (!count)
					lightShadow = LogicalPalette(ccode, ID_WHITE_SHADOW);
				if (!next)
					region.right--;
				if (!previous)
					region.left++;
			}
		outline = LogicalPalette(ccode, ID_DARK_SHADOW);
	}

	// Draw the tab outline.
	if (!display->isText)
	{
		region = trueRegion;
		point[0] = region.left; point[1] = region.bottom - UIW_NOTEBOOK::shadowWidth;
		point[2] = region.left; point[3] = region.top + chisel;
		point[4] = region.left + chisel; point[5] = region.top;
		point[6] = region.right - chisel; point[7] = region.top;
		point[8] = region.right; point[9] = region.top + chisel;
		point[10] = region.right; point[11] = region.bottom - UIW_NOTEBOOK::shadowWidth;
		display->Polygon(screenID, 6, point, outline, FALSE, FALSE, &clip);

		region -= 3;
	}
	outline = LogicalPalette(ccode, display->isText ? ID_BLACK_SHADOW : ID_OUTLINE);

	// Draw the tab text.
	if (text)
	{
		font = FlagSet(woStatus, WOS_SELECTED) ? FNT_DIALOG_FONT : FNT_SMALL_FONT;
		outline = LogicalPalette(ccode, FlagSet(woFlags, WOF_NON_SELECTABLE) ? ID_DARK_SHADOW : ID_BLACK_SHADOW);
		DrawText(screenID, region, text, outline, FALSE, ccode);
	}

	display->VirtualPut(screenID);
	return (TRUE);
}
#endif

EVENT_TYPE NOTEBOOK_TAB::Event(const UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
	case S_ADD_OBJECT:
		ccode = parent->Event(event);
		break;

	case L_BEGIN_SELECT:
	case L_SELECT:
		{
		// Make sure the notebook becomes current.
		UI_EVENT tEvent(S_ADD_OBJECT);
		for (UI_WINDOW_OBJECT *pWindow = parent; pWindow->parent; pWindow = pWindow->parent)
		{
			if (!FlagSet(pWindow->woStatus, WOS_CURRENT))
			{
				tEvent.windowObject = pWindow;
				pWindow->parent->Event(tEvent);
			}
		}
		ccode = UIW_BUTTON::Event(event);
		}
		break;

	default:
		ccode = UIW_BUTTON::Event(event);
		break;
	}
	return (ccode);
}

// ----- UIW_NOTEBOOK -------------------------------------------------------

UIW_NOTEBOOK::UIW_NOTEBOOK(void) :
	UIW_WINDOW(0, 0, 10, 3, WOF_NON_FIELD_REGION),
	totalPages(0), currentPage(0), searchPage(-1), tabWidth(0)
{
	UIW_NOTEBOOK::Information(I_INITIALIZE_CLASS, ZIL_NULLP(void));
}

UIW_NOTEBOOK::~UIW_NOTEBOOK(void)
{
}

UI_WINDOW_OBJECT *UIW_NOTEBOOK::Add(UI_WINDOW_OBJECT *object)
{
	// Check the type of object to be added;
	if (!object->Inherited(ID_WINDOW))
		return (ZIL_NULLP(UI_WINDOW_OBJECT));
	else if (Index(object) != -1)
		return (UIW_WINDOW::Add(object));
	UIW_WINDOW *window = (UIW_WINDOW *)object;

	// Modify the window for notebook use.
	window->woStatus |= WOS_OWNERDRAW;
	window->HotKey(HOT_KEY_SUB_WINDOW);

	// Clean up the child windows for presentation as pages.
	ZIL_ICHAR title[256];
	window->Information(I_COPY_TEXT, title);
	NOTEBOOK_TAB *tab = new NOTEBOOK_TAB(title, window->woFlags, totalPages++);
	UIW_WINDOW::Add(tab);

	// Remove invalid support objects.
	for (object = (UI_WINDOW_OBJECT *)window->support.First(); object; )
		// Remove NUMID_BORDER, NUMID_MAXIMIZE, NUMID_MINIMIZE, NUMID_SYSTEM, and NUMID_TITLE objects.
		if (object->NumberID() >= (ZIL_NUMBERID)0xFFFB)
		{
			UI_WINDOW_OBJECT *tobject = object->Next();
			window->Subtract(object);
			if (!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
				delete object;
			object = tobject;
		}
		else
			object = object->Next();

	// Modify the object palettes.
	window->paletteMapTable = _paletteMapTable;
	for (object = window->First(); object; object = object->Next())
		if (object->Inherited(ID_GROUP) ||
			(object->Inherited(ID_WINDOW) &&
			!object->Inherited(ID_LIST) &&
			!object->Inherited(ID_TEXT) &&
			!object->Inherited(ID_SCROLL_BAR)))
				object->paletteMapTable = _paletteMapTable;
	UIW_WINDOW::Add(window);
	return (window);
}

void *UIW_NOTEBOOK::Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	switch (request)
	{
	case I_INITIALIZE_CLASS:
		// Set the object identification and variables.
		data = UIW_WINDOW::Information(request, data, objectID);
		searchID = WindowID(ID_NOTEBOOK);
		woStatus |= WOS_OWNERDRAW;
		HotKey(HOT_KEY_SUB_WINDOW);
		tabHeight = display->isText ? 1 : display->cellHeight - display->preSpace - display->postSpace - 1;
		break;

	case I_CHANGED_FLAGS:
		data = UIW_WINDOW::Information(request, data, objectID);

		// See if the field needs to be re-computed.
		if (objectID == ID_NOTEBOOK && FlagSet(woStatus, WOS_REDISPLAY))
		{
			UI_EVENT event(S_INITIALIZE, 0);
			Event(event);
			event.type = S_CREATE;
			Event(event);
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
			event.type = S_REDISPLAY;
			Event(event);
#endif
		}
		break;

	case I_SET_SEARCH_PAGE:
		searchPage = *(int *)data;
		break;

	case I_GET_NUMBERID_OBJECT:
	case I_GET_STRINGID_OBJECT:
		if (searchPage == -1)
		{
			void *match = UIW_WINDOW::Information(request, data, objectID);
			for (UI_WINDOW_OBJECT *object = First(); object && !match; object = object->Next())
				match = object->Information(request, data, objectID);
			data = match;
		}
		else
		{
			UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)UI_LIST::Get(searchPage);
			data = object->Information(request, data, objectID);
		}
		break;

	case I_GET_TEXT:
	case I_SET_TEXT:
		{
		UI_WINDOW_OBJECT *object = (UI_WINDOW_OBJECT *)support.Get((searchPage == -1) ? currentPage : searchPage);
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
	// Check the type of object to be added;
	if (!object || !object->Inherited(ID_WINDOW) ||
		FlagSet(woStatus, WOS_INTERNAL_ACTION)) // Windows Patch to prevent re-entrance.
		return (ZIL_NULLP(UI_WINDOW_OBJECT));
	UIW_WINDOW *window = (UIW_WINDOW *)object;

	// Check for a valid page.
	int index = Index(window);
	if (index < 0)
		return (ZIL_NULLP(UIW_WINDOW));

	// Remove the notebook tab and restore the window title.
	totalPages--;
	EVENT_TYPE value = S_CHANGE_PAGE;
	for (object = (UI_WINDOW_OBJECT *)support.First(); object; )
		if (object->Inherited(ID_BUTTON))
		{
			UI_WINDOW_OBJECT *temp = object->Next();
			if (index-- == 0)
			{
				ZIL_ICHAR *text;
				object->Information(I_GET_TEXT, &text);
				*window + new UIW_TITLE(text); // restore the title.
				UIW_WINDOW::Subtract(object);
				delete object;
			}
			else
			{
				object->Information(I_SET_VALUE, &value);
				value--;
			}
			object = temp;
		}
		else
			object = object->Next();

	// Subtract page and update internal members.
	object = UIW_WINDOW::Subtract(window);
	searchPage = currentPage = current ? Index(current) : 0;
	return (object);
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
		UIW_NOTEBOOK::Add(object);
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
	UI_WINDOW_OBJECT *tab = (UI_WINDOW_OBJECT *)support.First();
	UIW_TITLE title(_blankString);
	UIW_WINDOW *window = (UIW_WINDOW *)First();
	for ( ; window; window = (UIW_WINDOW *)window->Next(), tab = tab->Next())
	{
		ZIL_OBJECTID searchID = window->SearchID();
		file->Store(searchID);
		window->support.Add(&title);
		title.Information(I_SET_TEXT, tab->Information(I_GET_TEXT, ZIL_NULLP(void)));
		window->woFlags |= (tab->woFlags & (WOF_VIEW_ONLY | WOF_NON_SELECTABLE));
		window->Store(ZIL_NULLP(ZIL_ICHAR), directory, file, objectTable, userTable);
		window->support.Subtract(&title);
	}
}
#endif
