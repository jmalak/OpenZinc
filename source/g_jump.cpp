//	Zinc Interface Library - G_JUMP.CPP
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

#if defined(ZIL_LINKBUG)
void z_jump_dummy(void) { } // Bug fix for broken linkers.
#endif

// ----- Static member variables --------------------------------------------

static UI_ITEM _objectTable[] =
{
	// Uncomment the objects below that are used in your application.
	// Zinc Designer generates the necessary table below to link in.
//	{ ID_BIGNUM, 			ZIL_VOIDF(UIW_BIGNUM::New), 			UIW_BIGNUM::_className, 			0 },
//	{ ID_BORDER, 			ZIL_VOIDF(UIW_BORDER::New), 			UIW_BORDER::_className, 			0 },
//	{ ID_BUTTON, 			ZIL_VOIDF(UIW_BUTTON::New), 			UIW_BUTTON::_className, 			0 },
//	{ ID_COMBO_BOX, 		ZIL_VOIDF(UIW_COMBO_BOX::New),			UIW_COMBO_BOX::_className, 			0 },
//	{ ID_DATE, 				ZIL_VOIDF(UIW_DATE::New),				UIW_DATE::_className, 				0 },
//	{ ID_FORMATTED_STRING, 	ZIL_VOIDF(UIW_FORMATTED_STRING::New),	UIW_FORMATTED_STRING::_className,	0 },
//	{ ID_GROUP, 			ZIL_VOIDF(UIW_GROUP::New), 				UIW_GROUP::_className, 				0 },
//	{ ID_HZ_LIST, 			ZIL_VOIDF(UIW_HZ_LIST::New), 			UIW_HZ_LIST::_className, 			0 },
//	{ ID_ICON, 				ZIL_VOIDF(UIW_ICON::New), 				UIW_ICON::_className, 				0 },
//	{ ID_INTEGER, 			ZIL_VOIDF(UIW_INTEGER::New), 			UIW_INTEGER::_className, 			0 },
//	{ ID_MAXIMIZE_BUTTON, 	ZIL_VOIDF(UIW_MAXIMIZE_BUTTON::New), 	UIW_MAXIMIZE_BUTTON::_className, 	0 },
//	{ ID_MINIMIZE_BUTTON, 	ZIL_VOIDF(UIW_MINIMIZE_BUTTON::New), 	UIW_MINIMIZE_BUTTON::_className, 	0 },
//	{ ID_POP_UP_ITEM, 		ZIL_VOIDF(UIW_POP_UP_ITEM::New), 		UIW_POP_UP_ITEM::_className, 		0 },
//	{ ID_POP_UP_MENU, 		ZIL_VOIDF(UIW_POP_UP_MENU::New), 		UIW_POP_UP_MENU::_className, 		0 },
//	{ ID_PROMPT, 			ZIL_VOIDF(UIW_PROMPT::New), 			UIW_PROMPT::_className, 			0 },
//	{ ID_PULL_DOWN_ITEM, 	ZIL_VOIDF(UIW_PULL_DOWN_ITEM::New), 	UIW_PULL_DOWN_ITEM::_className, 	0 },
//	{ ID_PULL_DOWN_MENU, 	ZIL_VOIDF(UIW_PULL_DOWN_MENU::New), 	UIW_PULL_DOWN_MENU::_className, 	0 },
//	{ ID_REAL, 				ZIL_VOIDF(UIW_REAL::New), 				UIW_REAL::_className, 				0 },
//	{ ID_SCROLL_BAR, 		ZIL_VOIDF(UIW_SCROLL_BAR::New), 		UIW_SCROLL_BAR::_className, 		0 },
//	{ ID_SPIN_CONTROL,		ZIL_VOIDF(UIW_SPIN_CONTROL::New),		UIW_SPIN_CONTROL::_className,		0 },
//	{ ID_STATUS_BAR, 		ZIL_VOIDF(UIW_STATUS_BAR::New), 		UIW_STATUS_BAR::_className, 		0 },
//	{ ID_STRING, 			ZIL_VOIDF(UIW_STRING::New), 			UIW_STRING::_className, 			0 },
//	{ ID_SYSTEM_BUTTON, 	ZIL_VOIDF(UIW_SYSTEM_BUTTON::New), 		UIW_SYSTEM_BUTTON::_className, 		0 },
//	{ ID_TABLE, 			ZIL_VOIDF(UIW_TABLE::New), 				UIW_TABLE::_className, 				0 },
//	{ ID_TABLE_HEADER, 		ZIL_VOIDF(UIW_TABLE_HEADER::New), 		UIW_TABLE_HEADER::_className, 		0 },
//	{ ID_TABLE_RECORD, 		ZIL_VOIDF(UIW_TABLE_RECORD::New), 		UIW_TABLE_RECORD::_className, 		0 },
//	{ ID_TEXT, 				ZIL_VOIDF(UIW_TEXT::New), 				UIW_TEXT::_className, 				0 },
//	{ ID_TIME, 				ZIL_VOIDF(UIW_TIME::New), 				UIW_TIME::_className, 				0 },
//	{ ID_TITLE, 			ZIL_VOIDF(UIW_TITLE::New), 				UIW_TITLE::_className, 				0 },
//	{ ID_TOOL_BAR, 			ZIL_VOIDF(UIW_TOOL_BAR::New), 			UIW_TOOL_BAR::_className, 			0 },
//	{ ID_VT_LIST, 			ZIL_VOIDF(UIW_VT_LIST::New), 			UIW_VT_LIST::_className, 			0 },
//	{ ID_WINDOW, 			ZIL_VOIDF(UIW_WINDOW::New), 			UIW_WINDOW::_className, 			0 },
//	{ ID_GEOMETRY_MANAGER,	ZIL_VOIDF(UI_GEOMETRY_MANAGER::New),	UI_GEOMETRY_MANAGER::_className, 	0 },
//	{ ID_ATTACHMENT,		ZIL_VOIDF(UI_ATTACHMENT::New),			ZIL_NULLP(ZIL_ICHAR),		 		0 },
//	{ ID_DIMENSION_CONSTRAINT,ZIL_VOIDF(UI_DIMENSION_CONSTRAINT::New),ZIL_NULLP(ZIL_ICHAR),		 		0 },
//	{ ID_RELATIVE_CONSTRAINT,ZIL_VOIDF(UI_RELATIVE_CONSTRAINT::New),ZIL_NULLP(ZIL_ICHAR),		 		0 },
//	{ ID_IMAGE,				ZIL_VOIDF(UIW_IMAGE::New),				UIW_IMAGE::_className,		 		0 },
	{ ID_END, 				ZIL_NULLP(void), 						ZIL_NULLP(ZIL_ICHAR), 				0 }
};
UI_ITEM *UI_WINDOW_OBJECT::objectTable = _objectTable;

static UI_ITEM _userTable[] =
{
	{ ID_END, ZIL_NULLP(void), ZIL_NULLP(ZIL_ICHAR), 0 }
};
UI_ITEM *UI_WINDOW_OBJECT::userTable = _userTable;

