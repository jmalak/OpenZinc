#include <ui_win.hpp>
#define USE_DERIVED_OBJECTS
#include "p_store.hpp"
void z_jump_dummy(void) { }   // Bug fix for broken linkers.

extern EVENT_TYPE ObjectNameCallback(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
extern int DirectoryCompare(void *, void *);

static ZIL_ICHAR _ObjectNameCallback[] = { 'O','b','j','e','c','t','N','a','m','e','C','a','l','l','b','a','c','k',0 };
static ZIL_ICHAR _DirectoryCompare[] = { 'D','i','r','e','c','t','o','r','y','C','o','m','p','a','r','e',0 };

static UI_ITEM _userTable[] =
{
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(ObjectNameCallback), _ObjectNameCallback, 0 },
	{ 0, (EVENT_TYPE*)ZIL_VOIDF(DirectoryCompare), _DirectoryCompare, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::userTable = _userTable;

static UI_ITEM _objectTable[] =
{
	{ ID_BORDER, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BORDER::New), UIW_BORDER::_className, 0 },
	{ ID_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_BUTTON::New), UIW_BUTTON::_className, 0 },
	{ ID_PROMPT, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_PROMPT::New), UIW_PROMPT::_className, 0 },
	{ ID_SCROLL_BAR, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SCROLL_BAR::New), UIW_SCROLL_BAR::_className, 0 },
	{ ID_STRING, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_STRING::New), UIW_STRING::_className, 0 },
	{ ID_SYSTEM_BUTTON, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), UIW_SYSTEM_BUTTON::_className, 0 },
	{ ID_TITLE, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_TITLE::New), UIW_TITLE::_className, 0 },
	{ ID_VT_LIST, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_VT_LIST::New), UIW_VT_LIST::_className, 0 },
	{ ID_WINDOW, (UI_WINDOW_OBJECT*)ZIL_VOIDF(UIW_WINDOW::New), UIW_WINDOW::_className, 0 },
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;
