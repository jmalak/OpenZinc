//	Zinc Interface Library - Z_WIN.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "window.hpp"
#define USE_UIW_WINDOW
#include "p_window.hpp"

static ZIL_NUMBERID RenumberObject(UI_WINDOW_OBJECT *window, ZIL_NUMBERID numberID)
{
	// Renumber the objects.
	UI_WINDOW_OBJECT *object = ZIL_NULLP(UI_WINDOW_OBJECT);
	window->Information(I_GET_SUPPORT_FIRST, &object);
	if (!object)
		window->Information(I_GET_FIRST, &object);
	while (object)
	{
		// Number this item.
		if (object->NumberID() < (ZIL_NUMBERID)0xFF00)
			object->NumberID(++numberID);
		// Check for special sub-types.
		UI_WINDOW_OBJECT *subWindow = ZIL_NULLP(UI_WINDOW_OBJECT);
		object->Information(I_GET_SUBWINDOW, &subWindow);
		if (subWindow && subWindow->NumberID() < (ZIL_NUMBERID)0xFF00)
			subWindow->NumberID(++numberID);
		// Set the child numbers.
		if (object->NumberID() != NUMID_GEOMETRY)
			numberID = RenumberObject(object, numberID);
		if (!object->Next() && FlagSet(object->woFlags, WOF_SUPPORT_OBJECT))
			window->Information(I_GET_FIRST, &object);
		else
			object = object->Next();
	}
	return (numberID);
}

// ----- UIW_WINDOW ---------------------------------------------------------

void ZAF_OBJECT_EDITOR::UIW_WINDOW_DataLoad(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_WINDOW *object = (UIW_WINDOW *)data;
	// Set the allowable advanced flags.
	dsFlagsAllowed = DSF_POSITION | DSF_GEOMETRY | DSF_CALLBACK;
	woFlagsAllowed =
		WOF_BORDER |
		WOF_PIXEL | WOF_MINICELL |
		WOF_NON_SELECTABLE;
	woAdvancedFlagsAllowed =
		WOAF_TEMPORARY | WOAF_NO_DESTROY |
		WOAF_NORMAL_HOT_KEYS |
		WOAF_NO_SIZE | WOAF_NO_MOVE |
		WOAF_MODAL | WOAF_LOCKED |
		WOAF_MOVE_DRAG_OBJECT | WOAF_COPY_DRAG_OBJECT | WOAF_ACCEPTS_DROP;
	if (object->parent)
		woFlagsAllowed |= WOF_NON_FIELD_REGION;

	// Load the object data.
	ZIL_ICHAR *title = ZIL_NULLP(ZIL_ICHAR);
	if (object->parent && object->parent->Inherited(ID_NOTEBOOK))
	{
		int searchPage = object->ListIndex(); // set the search page.
		object->parent->Information(I_SET_SEARCH_PAGE, &searchPage);
		object->parent->Information(I_GET_TEXT, &title);
	}
	else
		object->Information(I_GET_TEXT, &title);
	SetText(FIELD_TITLE, title);
	// Minimize icon
	UI_WINDOW_OBJECT *field = NULL;
	for (field = (UI_WINDOW_OBJECT *)object->support.First(); field; field = field->Next())
		if (field->Inherited(ID_ICON))
		{
			UIW_ICON *iconField = (UIW_ICON *)field; // typecast workaround.
			SetImage(FIELD_MINICON, iconField->iconName, ID_ICON_IMAGE);
			break;
		}
	if (!field)
		SetImage(FIELD_MINICON, ZIL_NULLP(ZIL_ICHAR), ID_ICON_IMAGE);
	SetText(FIELD_STRINGID, object->stringID);
	SetHelp(FIELD_HELP_CONTEXT, object->helpContext);
	// Determine the support objects.
	SetSupport(FLAG_TITLE, object, NUMID_TITLE);
	SetSupport(FLAG_BORDER, object, NUMID_BORDER);
	SetSupport(FLAG_MAX_BUTTON, object, NUMID_MAXIMIZE);
	SetSupport(FLAG_MIN_BUTTON, object, NUMID_MINIMIZE);
	SetSupport(FLAG_SYS_BUTTON, object, NUMID_SYSTEM);
	SetSupport(FLAG_VT_SCROLL, object, NUMID_VT_SCROLL);
	SetSupport(FLAG_HZ_SCROLL, object, NUMID_HZ_SCROLL);
	SetSupport(FLAG_GEOMETRY_MANAGER, object, NUMID_GEOMETRY);
	// Load the object flags.
	// object type
	WOAF_FLAGS advancedFlags = FlagSet(object->woStatus, WOS_EDIT_MODE) ? object->designerAdvancedFlags : object->woAdvancedFlags;
	SetFlag(FLAG_DEFAULT_OBJECT,
		FlagSet(advancedFlags, WOAF_MDI_OBJECT | WOAF_DIALOG_OBJECT), 0);
	SetFlag(FLAG_MDI_OBJECT, advancedFlags, WOAF_MDI_OBJECT);
	SetFlag(FLAG_DIALOG_OBJECT, advancedFlags, WOAF_DIALOG_OBJECT);
	// interaction
	SetFlag(FLAG_AUTO_SORT, object->wnFlags, WNF_AUTO_SORT);
	SetFlag(FLAG_SELECT_MULTIPLE, object->wnFlags, WNF_SELECT_MULTIPLE);
	SetFlag(FLAG_CONTINUE_SELECT, object->wnFlags, WNF_CONTINUE_SELECT);
	SetFlag(FLAG_AUTO_SELECT, object->wnFlags, WNF_AUTO_SELECT);
	// presentation
	SetFlag(FLAG_TEMPORARY, advancedFlags, WOAF_TEMPORARY);
	SetFlag(FLAG_NO_DESTROY, advancedFlags, WOAF_NO_DESTROY);
	SetFlag(FLAG_NORMAL_HOTKEYS, advancedFlags, WOAF_NORMAL_HOT_KEYS);
	SetFlag(FLAG_NO_SIZE, advancedFlags, WOAF_NO_SIZE);
	SetFlag(FLAG_NO_MOVE, advancedFlags, WOAF_NO_MOVE);
	SetFlag(FLAG_MODAL, advancedFlags, WOAF_MODAL);
	SetFlag(FLAG_LOCKED, advancedFlags, WOAF_LOCKED);
}

int ZAF_OBJECT_EDITOR::UIW_WINDOW_DataStore(UI_WINDOW_OBJECT *data)
{
	// Get the window object.
	UIW_WINDOW *object = (UIW_WINDOW *)data;

	// Store the object data.

	// Window title.
	int changed = FALSE;
	ZIL_ICHAR title[64];
	if (GetText(FIELD_TITLE, title))
	{
		changed |= TRUE;
		if (object->parent && object->parent->Inherited(ID_NOTEBOOK))
		{
			int searchPage = object->ListIndex(); // set the search page.
			object->parent->Information(I_SET_SEARCH_PAGE, &searchPage);
			object->parent->Information(I_SET_TEXT, title);
		}
		else
			object->Information(I_SET_TEXT, title);
	}

	// Minimize icon.
	UIW_ICON *iconField = ZIL_NULLP(UIW_ICON);
	for (UI_WINDOW_OBJECT *field = (UI_WINDOW_OBJECT *)object->support.First(); field && !iconField; field = field->Next())
		if (field->Inherited(ID_ICON))
			iconField = (UIW_ICON *)field; // typecast workaround.
	if (!iconField)
		iconField = new UIW_ICON(0, 0, ZIL_NULLP(ZIL_ICHAR),
			ZIL_NULLP(ZIL_ICHAR), ICF_MINIMIZE_OBJECT,
			WOF_JUSTIFY_CENTER | WOF_SUPPORT_OBJECT);
	UI_WINDOW_OBJECT *temp = editObject;
	editObject = iconField;
	GetImage(FIELD_MINICON, &iconField->iconName, ID_ICON_IMAGE);
	editObject = temp;
	if (iconField->iconName && iconField->iconName[0])
	{
		changed |= TRUE;
		*object + iconField;
	}
	else
	{
		changed |= TRUE;
		*object - iconField;
	}

	// StringID.
	if (GetText(FIELD_STRINGID, object->stringID))
	{
		changed |= TRUE;
		// Warn the other editors.
		for (UI_WINDOW_OBJECT *window = Next(); window; window = window->Next())
			if (window->SearchID() == ID_OBJECT_EDITOR)
				window->Information(I_CHECK_EDIT_NAMES, object);
	}

	// Help context.
	changed |= GetHelp(FIELD_HELP_CONTEXT, &object->helpContext);

	// Determine the support objects.
	int options = FALSE;
	options |= GetSupport(FLAG_TITLE, object, NUMID_TITLE);
	options |= GetSupport(FLAG_BORDER, object, NUMID_BORDER);
	options |= GetSupport(FLAG_MAX_BUTTON, object, NUMID_MAXIMIZE);
	options |= GetSupport(FLAG_MIN_BUTTON, object, NUMID_MINIMIZE);
	options |= GetSupport(FLAG_SYS_BUTTON, object, NUMID_SYSTEM);
	options |= GetSupport(FLAG_VT_SCROLL, object, NUMID_VT_SCROLL);
	options |= GetSupport(FLAG_HZ_SCROLL, object, NUMID_HZ_SCROLL);
	options |= GetSupport(FLAG_GEOMETRY_MANAGER, object, NUMID_GEOMETRY);
	if (options)
	{
		// Subtract, then Add support objects to ensure proper ordering.
		changed |= TRUE;
		UI_WINDOW_OBJECT *border = object->Get(NUMID_BORDER);
		UI_WINDOW_OBJECT *max = object->Get(NUMID_MAXIMIZE);
		UI_WINDOW_OBJECT *min = object->Get(NUMID_MINIMIZE);
		UI_WINDOW_OBJECT *sys = object->Get(NUMID_SYSTEM);
		UI_WINDOW_OBJECT *title = object->Get(NUMID_TITLE);
		UI_WINDOW_OBJECT *cscroll = object->Get(NUMID_C_SCROLL);
		UI_WINDOW_OBJECT *vscroll = object->Get(NUMID_VT_SCROLL);
		UI_WINDOW_OBJECT *hscroll = object->Get(NUMID_HZ_SCROLL);
		UI_WINDOW_OBJECT *gmgr = object->Get(NUMID_GEOMETRY);
		*object - border - max - min - sys - title - cscroll - vscroll - hscroll - gmgr;
		UI_LIST supportList;
		while (object->support.First())
		{
			UI_ELEMENT *supportObject = object->support.First();
			object->support - supportObject;
			supportList + supportObject;
		}
		if (cscroll) // Match up the center scroll.
			delete cscroll;
		cscroll = ZIL_NULLP(UI_WINDOW_OBJECT);
		if (vscroll && hscroll && !cscroll)
			cscroll = new UIW_SCROLL_BAR(0, 0, 0, 0, SBF_CORNER);

		// Add back the support objects (order is important).
		*object + border + max + min + sys + title;
		while (supportList.First())
		{
			UI_ELEMENT *supportObject = supportList.First();
			supportList - supportObject;
			object->support + supportObject;
		}
		*object + cscroll + vscroll + hscroll + gmgr;
		object->Event(S_REDISPLAY);
	}
	// Store the object flags.
	// object type
	WOAF_FLAGS *advancedFlags = FlagSet(object->woStatus, WOS_EDIT_MODE) ? &object->designerAdvancedFlags : &object->woAdvancedFlags;
	changed |= GetFlag(FLAG_MDI_OBJECT, advancedFlags, WOAF_MDI_OBJECT);
	changed |= GetFlag(FLAG_DIALOG_OBJECT, advancedFlags, WOAF_DIALOG_OBJECT);
	// interaction
	if (GetFlag(FLAG_AUTO_SORT, &object->wnFlags, WNF_AUTO_SORT))
	{
		changed |= TRUE;
		if (FlagSet(object->wnFlags, WNF_AUTO_SORT))
		{
			object->compareFunction = UIW_WINDOW::StringCompare;
			object->Sort();
		}
		else
			object->compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION);
	}
	changed |= GetFlag(FLAG_SELECT_MULTIPLE, &object->wnFlags, WNF_SELECT_MULTIPLE);
	changed |= GetFlag(FLAG_CONTINUE_SELECT, &object->wnFlags, WNF_CONTINUE_SELECT);
	changed |= GetFlag(FLAG_AUTO_SELECT, &object->wnFlags, WNF_AUTO_SELECT);
	// presentation
	changed |= GetFlag(FLAG_TEMPORARY, advancedFlags, WOAF_TEMPORARY);
	changed |= GetFlag(FLAG_NO_DESTROY, advancedFlags, WOAF_NO_DESTROY);
	changed |= GetFlag(FLAG_NORMAL_HOTKEYS, advancedFlags, WOAF_NORMAL_HOT_KEYS);
	changed |= GetFlag(FLAG_NO_SIZE, advancedFlags, WOAF_NO_SIZE);
	changed |= GetFlag(FLAG_NO_MOVE, advancedFlags, WOAF_NO_MOVE);
	changed |= GetFlag(FLAG_MODAL, advancedFlags, WOAF_MODAL);
	changed |= GetFlag(FLAG_LOCKED, advancedFlags, WOAF_LOCKED);

	// Check special options.
	if (FlagSet(notebook->Get(FLAG_RENUMBER_OBJECTS)->woStatus, WOS_SELECTED))
	{
		changed |= TRUE;
		ZIL_NUMBERID numberID = 0;
		object->NumberID(RenumberObject(object, numberID) + 1);
	}

	// Return the status.
	return (changed);
}

