//	Zinc Interface Library - UI_WIN.HPP
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



// ----- Include file and environment definition ----------------------------

#if !defined(UI_WIN_HPP)
#	define UI_WIN_HPP
#	if !defined(UI_EVT_HPP)
#		if defined(__OS2__)
#			if !defined(INCL_WINSYS)
#				define INCL_WINSYS
#			endif
#			if !defined(INCL_WINFRAMEMGR)
#				define INCL_WINFRAMEMGR
#			endif
#			if !defined(INCL_WINWINDOWMGR)
#				define INCL_WINWINDOWMGR
#			endif
#		endif
#		include <ui_evt.hpp>
#	endif
#if defined(ZIL_NEXTSTEP)
extern "C"
{
#	include <dpsclient/dpsNeXT.h>
}
#endif
// ----- NUMBERID -----------------------------------------------------------

typedef ZIL_NUMBERID NUMBERID;

const ZIL_NUMBERID NUMID_BORDER				= 0xFFFF;
const ZIL_NUMBERID NUMID_MAXIMIZE			= 0xFFFE;
const ZIL_NUMBERID NUMID_MINIMIZE			= 0xFFFD;
const ZIL_NUMBERID NUMID_SYSTEM				= 0xFFFC;
const ZIL_NUMBERID NUMID_TITLE				= 0xFFFB;
const ZIL_NUMBERID NUMID_HZ_SCROLL			= 0xFFFA;
const ZIL_NUMBERID NUMID_VT_SCROLL			= 0xFFF9;
const ZIL_NUMBERID NUMID_C_SCROLL			= 0xFFF8;
const ZIL_NUMBERID NUMID_GEOMETRY			= 0xFFF7;
const ZIL_NUMBERID NUMID_COL_HEADER			= 0xFFF6;
const ZIL_NUMBERID NUMID_ROW_HEADER			= 0xFFF5;
const ZIL_NUMBERID NUMID_CNR_HEADER			= 0xFFF4;

const ZIL_NUMBERID NUMID_OPT_RESTORE		= 0xFFEF;
const ZIL_NUMBERID NUMID_OPT_MOVE			= 0xFFEE;
const ZIL_NUMBERID NUMID_OPT_SIZE			= 0xFFED;
const ZIL_NUMBERID NUMID_OPT_MINIMIZE		= 0xFFEC;
const ZIL_NUMBERID NUMID_OPT_MAXIMIZE		= 0xFFEB;
const ZIL_NUMBERID NUMID_OPT_CLOSE			= 0xFFEA;
const ZIL_NUMBERID NUMID_OPT_SWITCH			= 0xFFE9;

// ----- UI_ITEM ------------------------------------------------------------

struct ZIL_EXPORT_CLASS UI_ITEM
{
	EVENT_TYPE value;
	void *data;
	ZIL_ICHAR *text;
	UIF_FLAGS flags;
};

// ----- Window object identifications --------------------------------------

// WARNING: All ZIL_OBJECTID numbers are pinned and cannot be changed!

// --- ZIL_SIMPLE_OBJECTID ---
const ZIL_SIMPLE_OBJECTID ID_BORDER				= 1;
const ZIL_SIMPLE_OBJECTID ID_BUTTON				= 2;
const ZIL_SIMPLE_OBJECTID ID_DATE				= 3;
const ZIL_SIMPLE_OBJECTID ID_FORMATTED_STRING	= 4;
const ZIL_SIMPLE_OBJECTID ID_ICON				= 5;
const ZIL_SIMPLE_OBJECTID ID_INTEGER			= 6;
const ZIL_SIMPLE_OBJECTID ID_MAXIMIZE_BUTTON	= 7;
const ZIL_SIMPLE_OBJECTID ID_MINIMIZE_BUTTON	= 8;
const ZIL_SIMPLE_OBJECTID ID_NUMBER				= 9;
const ZIL_SIMPLE_OBJECTID ID_PROMPT				= 11;
const ZIL_SIMPLE_OBJECTID ID_REAL				= 12;
const ZIL_SIMPLE_OBJECTID ID_STRING				= 13;
const ZIL_SIMPLE_OBJECTID ID_TEXT				= 15;
const ZIL_SIMPLE_OBJECTID ID_TIME				= 16;
const ZIL_SIMPLE_OBJECTID ID_TITLE				= 17;
const ZIL_SIMPLE_OBJECTID ID_WINDOW_OBJECT		= 18;
const ZIL_SIMPLE_OBJECTID ID_POP_UP_ITEM		= 19;
const ZIL_SIMPLE_OBJECTID ID_IMAGE				= 20;
const ZIL_SIMPLE_OBJECTID ID_ZAF_MESSAGE_WINDOW			= 21;
const ZIL_SIMPLE_OBJECTID ID_DIALOG_WINDOW			= 22;
const ZIL_SIMPLE_OBJECTID ID_GROUP				= 23;
const ZIL_SIMPLE_OBJECTID ID_BIGNUM				= 24;
const ZIL_SIMPLE_OBJECTID ID_CHECK_BOX			= 25;
const ZIL_SIMPLE_OBJECTID ID_RADIO_BUTTON		= 26;

// --- ZIL_COMPLEX_OBJECTID ---
// open											= 1000;
const ZIL_COMPLEX_OBJECTID ID_LIST				= 1001;
const ZIL_COMPLEX_OBJECTID ID_POP_UP_MENU		= 1002;
const ZIL_COMPLEX_OBJECTID ID_PULL_DOWN_ITEM	= 1003;
const ZIL_COMPLEX_OBJECTID ID_PULL_DOWN_MENU	= 1004;
const ZIL_COMPLEX_OBJECTID ID_SCROLL_BAR		= 1005;
const ZIL_COMPLEX_OBJECTID ID_SYSTEM_BUTTON		= 1006;
const ZIL_COMPLEX_OBJECTID ID_WINDOW			= 1007;
const ZIL_COMPLEX_OBJECTID ID_COMBO_BOX			= 1008;
const ZIL_COMPLEX_OBJECTID ID_TOOL_BAR			= 1009;
const ZIL_COMPLEX_OBJECTID ID_VT_LIST			= 1010;
const ZIL_COMPLEX_OBJECTID ID_VLIST				= 1010;
const ZIL_COMPLEX_OBJECTID ID_HZ_LIST			= 1011;
const ZIL_COMPLEX_OBJECTID ID_HLIST				= 1011;
const ZIL_COMPLEX_OBJECTID ID_LIST_ITEM			= 1012;
const ZIL_COMPLEX_OBJECTID ID_TABLE				= 1013;
const ZIL_COMPLEX_OBJECTID ID_TABLE_RECORD		= 1014;
const ZIL_COMPLEX_OBJECTID ID_STATUS_BAR		= 1015;
const ZIL_COMPLEX_OBJECTID ID_STATUS_ITEM 		= 1016;
const ZIL_COMPLEX_OBJECTID ID_SPIN_CONTROL		= 1017;
const ZIL_COMPLEX_OBJECTID ID_NOTEBOOK			= 1018;
const ZIL_COMPLEX_OBJECTID ID_CONSTRAINT		= 1019;
const ZIL_COMPLEX_OBJECTID ID_ATTACHMENT		= 1020;
const ZIL_COMPLEX_OBJECTID ID_DIMENSION_CONSTRAINT	= 1021;
const ZIL_COMPLEX_OBJECTID ID_RELATIVE_CONSTRAINT	= 1022;
const ZIL_COMPLEX_OBJECTID ID_TABLE_HEADER		= 1023;

// --- ZIL_COMPOSITE_OBJECTID ---
const ZIL_COMPOSITE_OBJECTID ID_DISPLAY			= 2000;
const ZIL_COMPOSITE_OBJECTID ID_EVENT_MANAGER	= 2001;
const ZIL_COMPOSITE_OBJECTID ID_WINDOW_MANAGER	= 2002;
const ZIL_COMPOSITE_OBJECTID ID_GEOMETRY_MANAGER= 2003;
const ZIL_COMPOSITE_OBJECTID ID_OUTLINE			= 2100;
const ZIL_COMPOSITE_OBJECTID ID_WHITE_SHADOW	= 2101;
const ZIL_COMPOSITE_OBJECTID ID_LIGHT_SHADOW	= 2102;
const ZIL_COMPOSITE_OBJECTID ID_DARK_SHADOW		= 2103;
const ZIL_COMPOSITE_OBJECTID ID_BLACK_SHADOW	= 2104;
const ZIL_COMPOSITE_OBJECTID ID_MENU			= 2105;
const ZIL_COMPOSITE_OBJECTID ID_MENU_ITEM		= 2106;
const ZIL_COMPOSITE_OBJECTID ID_HOT_KEY			= 2107;

// ----- Window object system messages --------------------------------------

// --- ZIL_SYSTEM_EVENT ---
const ZIL_SYSTEM_EVENT S_MINIMIZE				= -4;
const ZIL_SYSTEM_EVENT S_MAXIMIZE				= -5;
const ZIL_SYSTEM_EVENT S_CLOSE					= -11;
const ZIL_SYSTEM_EVENT S_NON_CURRENT			= -13;
const ZIL_SYSTEM_EVENT S_MOVE					= -14;
const ZIL_SYSTEM_EVENT S_CLOSE_TEMPORARY		= -18;
const ZIL_SYSTEM_EVENT S_REGION_DEFINE			= -21;
const ZIL_SYSTEM_EVENT S_CASCADE				= -24;		// No Union information
const ZIL_SYSTEM_EVENT S_RESTORE				= -25;
const ZIL_SYSTEM_EVENT S_REGISTER_OBJECT		= -26;

const ZIL_SYSTEM_EVENT S_SIZE					= -200;
const ZIL_SYSTEM_EVENT S_CHANGED				= -201;
const ZIL_SYSTEM_EVENT S_DISPLAY_ACTIVE			= -202;
const ZIL_SYSTEM_EVENT S_DISPLAY_INACTIVE		= -203;
const ZIL_SYSTEM_EVENT S_CURRENT				= -204;		// UI_REGION information

const ZIL_SYSTEM_EVENT S_SHOW_DEFAULT           = -210;
const ZIL_SYSTEM_EVENT S_HIDE_DEFAULT           = -211;

const ZIL_SYSTEM_EVENT S_VSCROLL				= -300;		// Scroll Events
const ZIL_SYSTEM_EVENT S_HSCROLL				= -301;
const ZIL_SYSTEM_EVENT S_VSCROLL_SET			= -302;
const ZIL_SYSTEM_EVENT S_HSCROLL_SET			= -303;
const ZIL_SYSTEM_EVENT S_VSCROLL_CHECK 		  	= -304;
const ZIL_SYSTEM_EVENT S_HSCROLL_CHECK 	  		= -305;
const ZIL_SYSTEM_EVENT S_VSCROLL_WINDOW   		= -306;
const ZIL_SYSTEM_EVENT S_HSCROLL_WINDOW	  		= -307;
const ZIL_SYSTEM_EVENT S_SCROLLRANGE		  	= -308;

const ZIL_SYSTEM_EVENT S_ADD_OBJECT				= -400;		// *windowObject, *device
const ZIL_SYSTEM_EVENT S_SUBTRACT_OBJECT		= -401;		// *windowObject, *device
const ZIL_SYSTEM_EVENT S_ROUTE_EVENT			= -402;		// *event
const ZIL_SYSTEM_EVENT S_DRAG_MOVE_OBJECT		= -403;		// *windowObject
const ZIL_SYSTEM_EVENT S_DRAG_COPY_OBJECT		= -404;		// *windowObject
const ZIL_SYSTEM_EVENT S_DROP_MOVE_OBJECT		= -405;		// *windowObject
const ZIL_SYSTEM_EVENT S_DROP_COPY_OBJECT		= -406;		// *windowObject
const ZIL_SYSTEM_EVENT S_DRAG_DEFAULT			= -411;		// *windowObject
const ZIL_SYSTEM_EVENT S_DROP_DEFAULT			= -412;		// *windowObject
const ZIL_SYSTEM_EVENT S_OPEN_DOCUMENT			= -413;		// *text
const ZIL_SYSTEM_EVENT S_PRINT_DOCUMENT			= -414;		// *text

const ZIL_SYSTEM_EVENT S_VERIFY_STATUS			= -450;		// Motif private message!

const ZIL_SYSTEM_EVENT S_MDICHILD_EVENT			= -500;

const ZIL_SYSTEM_EVENT S_CHANGE_PAGE			= -600;		// NoteBook class message

const ZIL_SYSTEM_EVENT S_SET_DATA				= -700;		// Table class message

// --- ZIL_LOGICAL_EVENT ---
const ZIL_LOGICAL_EVENT L_VIEW					= 1001;		// General
const ZIL_LOGICAL_EVENT L_SELECT				= 1002;
const ZIL_LOGICAL_EVENT L_BEGIN_SELECT			= 1003;
const ZIL_LOGICAL_EVENT L_CONTINUE_SELECT		= 1004;
const ZIL_LOGICAL_EVENT L_END_SELECT			= 1005;
const ZIL_LOGICAL_EVENT L_BEGIN_ESCAPE			= 1006;
const ZIL_LOGICAL_EVENT L_CONTINUE_ESCAPE		= 1007;
const ZIL_LOGICAL_EVENT L_END_ESCAPE			= 1008;
const ZIL_LOGICAL_EVENT L_HELP					= 1009;
const ZIL_LOGICAL_EVENT L_CANCEL				= 1010;
const ZIL_LOGICAL_EVENT L_EXIT_FUNCTION			= 1011;
const ZIL_LOGICAL_EVENT L_DOUBLE_CLICK			= 1012;
const ZIL_LOGICAL_EVENT L_MOVE					= 1013;
const ZIL_LOGICAL_EVENT L_SIZE					= 1014;
const ZIL_LOGICAL_EVENT L_RESTORE				= S_RESTORE;
const ZIL_LOGICAL_EVENT L_MINIMIZE				= S_MINIMIZE;
const ZIL_LOGICAL_EVENT L_MAXIMIZE				= S_MAXIMIZE;

const ZIL_LOGICAL_EVENT L_NEXT_WINDOW			= 1040;		// Window Manager

const ZIL_LOGICAL_EVENT L_UP					= 1050;		// Window Object
const ZIL_LOGICAL_EVENT L_DOWN					= 1051;
const ZIL_LOGICAL_EVENT L_LEFT					= 1052;
const ZIL_LOGICAL_EVENT L_RIGHT					= 1053;
const ZIL_LOGICAL_EVENT L_PREVIOUS				= 1054;
const ZIL_LOGICAL_EVENT L_NEXT					= 1055;
const ZIL_LOGICAL_EVENT L_FIRST					= 1056;
const ZIL_LOGICAL_EVENT L_TOP					= 1056;
const ZIL_LOGICAL_EVENT L_LAST					= 1057;
const ZIL_LOGICAL_EVENT L_BOTTOM				= 1057;
const ZIL_LOGICAL_EVENT L_PGUP					= 1058;
const ZIL_LOGICAL_EVENT L_PGDN					= 1059;

const ZIL_LOGICAL_EVENT L_BEGIN_MARK			= 1101;	// String
const ZIL_LOGICAL_EVENT L_CONTINUE_MARK			= 1102;
const ZIL_LOGICAL_EVENT L_END_MARK				= 1103;
const ZIL_LOGICAL_EVENT L_CUT					= 1104;
const ZIL_LOGICAL_EVENT L_PASTE					= 1105;
const ZIL_LOGICAL_EVENT L_CUT_PASTE				= 1106;
const ZIL_LOGICAL_EVENT L_MARK					= 1107;
const ZIL_LOGICAL_EVENT L_COPY_MARK				= 1127;
const ZIL_LOGICAL_EVENT L_DELETE				= 1108;
const ZIL_LOGICAL_EVENT L_DELETE_WORD			= 1109;
const ZIL_LOGICAL_EVENT L_DELETE_EOL			= 1110;
const ZIL_LOGICAL_EVENT L_INSERT_TOGGLE			= 1114;
const ZIL_LOGICAL_EVENT L_WORD_LEFT				= 1115;
const ZIL_LOGICAL_EVENT L_WORD_RIGHT			= 1116;
const ZIL_LOGICAL_EVENT L_BOL					= 1117;
const ZIL_LOGICAL_EVENT L_EOL					= 1118;
const ZIL_LOGICAL_EVENT L_BACKSPACE				= 1119;

const ZIL_LOGICAL_EVENT L_MARK_LEFT	  			= 1130;
const ZIL_LOGICAL_EVENT L_MARK_RIGHT	  		= 1131;
const ZIL_LOGICAL_EVENT L_MARK_UP		  		= 1132;
const ZIL_LOGICAL_EVENT L_MARK_DOWN	  			= 1133;
const ZIL_LOGICAL_EVENT L_MARK_BOL	  			= 1134;
const ZIL_LOGICAL_EVENT L_MARK_EOL	  			= 1135;
const ZIL_LOGICAL_EVENT L_MARK_PGUP	  			= 1136;
const ZIL_LOGICAL_EVENT L_MARK_PGDN	  			= 1137;
const ZIL_LOGICAL_EVENT L_MARK_WORD_RIGHT		= 1138;
const ZIL_LOGICAL_EVENT L_MARK_WORD_LEFT		= 1139;

const ZIL_LOGICAL_EVENT L_BEGIN_MOVE_DRAG		= 1150;
const ZIL_LOGICAL_EVENT L_CONTINUE_MOVE_DRAG	= 1151;
const ZIL_LOGICAL_EVENT L_END_MOVE_DRAG			= 1152;
const ZIL_LOGICAL_EVENT L_BEGIN_COPY_DRAG		= 1153;
const ZIL_LOGICAL_EVENT L_CONTINUE_COPY_DRAG	= 1154;
const ZIL_LOGICAL_EVENT L_END_COPY_DRAG			= 1155;

const ZIL_LOGICAL_EVENT L_MDICHILD_EVENT		= 500;

// --- Private designer events - DO NOT USE! ---
const ZIL_DESIGNER_EVENT D_SET_OBJECT			= 5000;
const ZIL_DESIGNER_EVENT D_EDIT_OBJECT			= 5001;
const ZIL_DESIGNER_EVENT D_SET_POSITION			= 5002;
const ZIL_DESIGNER_EVENT D_CREATE_OBJECT		= 5003;

// ----- UI_PALETTE_MAP -----------------------------------------------------

// --- ZIL_LOGICAL_PALETTE ---
typedef ZIL_LOGICAL_EVENT ZIL_LOGICAL_PALETTE;
const ZIL_LOGICAL_PALETTE PM_ANY			= 0x0000;
const ZIL_LOGICAL_PALETTE PM_ACTIVE			= 0x0001;
const ZIL_LOGICAL_PALETTE PM_INACTIVE		= 0x0002;
const ZIL_LOGICAL_PALETTE PM_CURRENT		= 0x0004;
const ZIL_LOGICAL_PALETTE PM_SELECTED		= 0x0008;
const ZIL_LOGICAL_PALETTE PM_NON_SELECTABLE	= 0x0010;
const ZIL_LOGICAL_PALETTE PM_HOT_KEY		= 0x0020;
const ZIL_LOGICAL_PALETTE PM_SPECIAL		= 0x0040;	// Special mode palettes (WOAF_DIALOG_OBJECT border)

struct ZIL_EXPORT_CLASS UI_PALETTE_MAP
{
	ZIL_OBJECTID objectID;
	ZIL_LOGICAL_PALETTE logicalPalette;
	UI_PALETTE palette;

	static UI_PALETTE *MapPalette(UI_PALETTE_MAP *mapTable,
		ZIL_LOGICAL_PALETTE logicalPalette, ZIL_OBJECTID id1 = ID_WINDOW_OBJECT,
		ZIL_OBJECTID id2 = ID_WINDOW_OBJECT, ZIL_OBJECTID id3 = ID_WINDOW_OBJECT,
		ZIL_OBJECTID id4 = ID_WINDOW_OBJECT, ZIL_OBJECTID id5 = ID_WINDOW_OBJECT);
};

// ----- UI_EVENT_MAP -------------------------------------------------------

struct ZIL_EXPORT_CLASS UI_EVENT_MAP
{
	ZIL_OBJECTID objectID;
	ZIL_LOGICAL_EVENT logicalValue;
	EVENT_TYPE eventType;
	ZIL_RAW_CODE rawCode;
	ZIL_RAW_CODE modifiers;

	static ZIL_LOGICAL_EVENT MapEvent(UI_EVENT_MAP *mapTable,
		const UI_EVENT &event, ZIL_OBJECTID id1 = ID_WINDOW_OBJECT,
		ZIL_OBJECTID id2 = ID_WINDOW_OBJECT, ZIL_OBJECTID id3 = ID_WINDOW_OBJECT,
		ZIL_OBJECTID id4 = ID_WINDOW_OBJECT, ZIL_OBJECTID id5 = ID_WINDOW_OBJECT);
};

// --------------------------------------------------------------------------
// ----- UI_WINDOW_OBJECT ---------------------------------------------------
// --------------------------------------------------------------------------
class ZIL_EXPORT_CLASS UI_ERROR_STUB;
class ZIL_EXPORT_CLASS UI_HELP_STUB;
class ZIL_EXPORT_CLASS UI_WINDOW_OBJECT;
class ZIL_EXPORT_CLASS UI_WINDOW_MANAGER;
class ZIL_EXPORT_CLASS UIW_WINDOW;

// --- WOF_FLAGS ---
typedef UIF_FLAGS WOF_FLAGS;
const WOF_FLAGS WOF_NO_FLAGS			= 0x0000;
const WOF_FLAGS WOF_JUSTIFY_CENTER		= 0x0001;	// center justify data
const WOF_FLAGS WOF_JUSTIFY_RIGHT		= 0x0002;	// right justify data
const WOF_FLAGS WOF_BORDER				= 0x0004;	// draw a border
const WOF_FLAGS WOF_NO_ALLOCATE_DATA	= 0x0008;	// do not allocate the data buffer
const WOF_FLAGS WOF_VIEW_ONLY			= 0x0010;	// object can be viewed, but not edited
const WOF_FLAGS WOF_SUPPORT_OBJECT		= 0x0020;	// place object in the support list
const WOF_FLAGS WOF_MINICELL			= 0x0040;	// original object region is in mini-cell coordinates
const WOF_FLAGS WOF_UNANSWERED			= 0x0080;	// Sets initial status bit to "unanswered"
const WOF_FLAGS WOF_INVALID				= 0x0100;	// Sets invalid status
const WOF_FLAGS WOF_NON_FIELD_REGION	= 0x0200;	// object does not share screen space
const WOF_FLAGS WOF_NON_SELECTABLE		= 0x0400;	// object cannot be selected
const WOF_FLAGS WOF_AUTO_CLEAR			= 0x0800;	// auto clear option for edit fields
const WOF_FLAGS WOF_PIXEL				= 0x1000;	// original object region is in pixel coordinates

// --- WOAF_FLAGS ---
typedef UIF_FLAGS WOAF_FLAGS;
const WOAF_FLAGS WOAF_NO_FLAGS			= 0x0000;
const WOAF_FLAGS WOAF_OUTSIDE_REGION	= 0x0001;	// object occupies space outside the trueRegion
const WOAF_FLAGS WOAF_NON_CURRENT		= 0x0002;	// object cannot be made current
const WOAF_FLAGS WOAF_TEMPORARY			= 0x0004;	// object is only temporarily on the display
const WOAF_FLAGS WOAF_NO_DESTROY		= 0x0010;	// don't destroy the object
const WOAF_FLAGS WOAF_NORMAL_HOT_KEYS	= 0x0020;	// look for hot keys on normal input.
const WOAF_FLAGS WOAF_NO_SIZE			= 0x0040;	// object cannot be sized
const WOAF_FLAGS WOAF_NO_MOVE			= 0x0080;	// object cannot be moved
const WOAF_FLAGS WOAF_MODAL				= 0x0100;	// object is modal (gets all events)
const WOAF_FLAGS WOAF_LOCKED			= 0x0200;	// object cannot be removed from the window manager
const WOAF_FLAGS WOAF_COPY_DRAG_OBJECT	= 0x0400;	// object can be copy dragged
const WOAF_FLAGS WOAF_ACCEPTS_DROP		= 0x1000;	// object accepts objects that have been dragged
const WOAF_FLAGS WOAF_MOVE_DRAG_OBJECT	= 0x2000;	// object can be move dragged
const WOAF_FLAGS WOAF_MDI_OBJECT		= 0x4000;	// object is an mdi window
const WOAF_FLAGS WOAF_DIALOG_OBJECT		= 0x8000;	// object is a dialog window

// --- WOS_STATUS ---
typedef UIS_STATUS WOS_STATUS;
const WOS_STATUS WOS_NO_STATUS			= 0x0000;
const WOS_STATUS WOS_GRAPHICS			= 0x0001;	// object region is in graphics coordinates
const WOS_STATUS WOS_CURRENT			= 0x0002;	// object is current
const WOS_STATUS WOS_CHANGED			= 0x0004;	// object data has changed
const WOS_STATUS WOS_SELECTED			= 0x0008;	// object has been selected
const WOS_STATUS WOS_UNANSWERED			= 0x0010;	// object data is answered
const WOS_STATUS WOS_INVALID			= 0x0020;	// object has invalid data
const WOS_STATUS WOS_MAXIMIZED			= 0x0040;	// object is maximized
const WOS_STATUS WOS_MINIMIZED			= 0x0080;	// object is minimized
const WOS_STATUS WOS_REDISPLAY			= 0x0100;	// data needs updating
const WOS_STATUS WOS_READ_ERROR			= 0x0200;	// read error occurred
const WOS_STATUS WOS_OWNERDRAW			= 0x1000;

// --- Private status flags - DO NOT USE! ---
const WOS_STATUS WOS_SYSTEM_OBJECT		= 0x0400;	// object was created by the OS (OS/2, Windows, Machintosh)
const WOS_STATUS WOS_WINDOWS_ACTION		= 0x0800;
const WOS_STATUS WOS_MACINTOSH_WINDOW	= 0x0800;	// object is an actual Macintosh Toolbox window
const WOS_STATUS WOS_NEXTSTEP_WINDOW	= 0x0800;	// object is an actual NEXTSTEP window
const WOS_STATUS WOS_EDIT_MODE			= 0x2000;
const WOS_STATUS WOS_INTERNAL_ACTION	= 0x4000;
const WOS_STATUS WOS_DESTROY_ICON		= 0x8000;	// for UIW_ICON only
const WOS_STATUS WOS_AUTO_MARK			= 0x8000;	// for UIW_STRING and UIW_TEXT only
const WOS_STATUS WOS_ACTIVE_MDI			= 0x8000;	// for UIW_WINDOW only

// --- UI_WINDOW_OBJECT::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_GET_NUMBERID_OBJECT	= 1;
const ZIL_INFO_REQUEST I_GET_STRINGID_OBJECT	= 2;
const ZIL_INFO_REQUEST I_GET_FLAGS				= 3;
const ZIL_INFO_REQUEST I_SET_FLAGS				= 4;
const ZIL_INFO_REQUEST I_CLEAR_FLAGS			= 5;
const ZIL_INFO_REQUEST I_CHANGED_FLAGS			= 6;
const ZIL_INFO_REQUEST I_GET_STATUS				= 7;
const ZIL_INFO_REQUEST I_SET_STATUS				= 8;
const ZIL_INFO_REQUEST I_CLEAR_STATUS			= 9;
const ZIL_INFO_REQUEST I_CHANGED_STATUS			= 10;
const ZIL_INFO_REQUEST I_GET_TEXT				= 11;
const ZIL_INFO_REQUEST I_SET_TEXT				= 12;
const ZIL_INFO_REQUEST I_GET_MAXLENGTH			= 13;
const ZIL_INFO_REQUEST I_SET_MAXLENGTH			= 14;
const ZIL_INFO_REQUEST I_COPY_TEXT				= 15;
const ZIL_INFO_REQUEST I_GET_FIRST 				= 16;
const ZIL_INFO_REQUEST I_GET_LAST 				= 17;
const ZIL_INFO_REQUEST I_GET_CURRENT 			= 18;
const ZIL_INFO_REQUEST I_GET_SUPPORT_FIRST 		= 19;
const ZIL_INFO_REQUEST I_GET_SUPPORT_LAST 		= 20;
const ZIL_INFO_REQUEST I_GET_SUPPORT_CURRENT 	= 21;
const ZIL_INFO_REQUEST I_GET_DEFAULT_OBJECT		= 22;
const ZIL_INFO_REQUEST I_SET_DEFAULT_OBJECT		= 23;
const ZIL_INFO_REQUEST I_GET_VALUE				= 24;
const ZIL_INFO_REQUEST I_SET_VALUE				= 25;
const ZIL_INFO_REQUEST I_INCREMENT_VALUE		= 26;
const ZIL_INFO_REQUEST I_DECREMENT_VALUE		= 27;
const ZIL_INFO_REQUEST I_GET_SUBWINDOW			= 28;

#if defined(ZIL_MSWINDOWS)
const ZIL_INFO_REQUEST I_GET_DWSTYLE			= 100;	// Windows only requests.
const ZIL_INFO_REQUEST I_SET_DWSTYLE			= 101;
const ZIL_INFO_REQUEST I_CLEAR_DWSTYLE			= 102;
const ZIL_INFO_REQUEST I_CHANGED_DWSTYLE		= 103;
const ZIL_INFO_REQUEST I_HIDE_SUBWINDOW			= 104;
const ZIL_INFO_REQUEST I_SET_TASKID 			= 105;
const ZIL_INFO_REQUEST I_GET_TASKID				= 118;
#elif defined(ZIL_OS2)
const ZIL_INFO_REQUEST I_GET_FLSTYLE 			= 106;	// OS/2 only requests.
const ZIL_INFO_REQUEST I_SET_FLSTYLE 			= 107;
const ZIL_INFO_REQUEST I_CLEAR_FLSTYLE			= 108;
const ZIL_INFO_REQUEST I_CHANGED_FLSTYLE 		= 109;
const ZIL_INFO_REQUEST I_GET_FLFLAG				= 110;
const ZIL_INFO_REQUEST I_SET_FLFLAG				= 111;
const ZIL_INFO_REQUEST I_CLEAR_FLFLAG			= 112;
const ZIL_INFO_REQUEST I_CHANGED_FLFLAG			= 113;
const ZIL_INFO_REQUEST I_GET_TASKID				= 118;
#elif defined(ZIL_MOTIF)
const ZIL_INFO_REQUEST I_GET_SYSTEM_OPTIONS		= 114;	// MOTIF only requests.
const ZIL_INFO_REQUEST I_SET_SYSTEM_OPTIONS		= 115;
const ZIL_INFO_REQUEST I_GET_DECORATIONS		= 116;
const ZIL_INFO_REQUEST I_SET_DECORATIONS		= 117;
#endif
const ZIL_INFO_REQUEST I_INITIALIZE_CLASS		= 200;	// Special initialization

// Private class messages --- DO NOT USE!
const ZIL_INFO_REQUEST I_PRINT_INFORMATION		= 201;	// Special hpp print
const ZIL_INFO_REQUEST I_PRINT_USER_FUNCTION	= 202;	// Special cpp print
const ZIL_INFO_REQUEST I_PRINT_COMPARE_FUNCTION = 203;	// Special cpp print
const ZIL_INFO_REQUEST I_INITIALIZE_CLASS_FIRST	= 204;	// Special initialization

// --- UI_HELP_CONTEXT ---
typedef ZIL_UINT16 UI_HELP_CONTEXT;
const UI_HELP_CONTEXT NO_HELP_CONTEXT			= 0x0000;

// --- Underline character information ---
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_CURSES) || defined(ZIL_MACINTOSH)
const ZIL_ICHAR ZIL_UNDERLINE_CHARACTER 		= '&';
const ZIL_ICHAR ZIL_OLD_UNDERLINE_CHARACTER 	= '~';
#elif defined(ZIL_OS2) || defined(ZIL_XT) || defined(ZIL_NEXTSTEP)
const ZIL_ICHAR ZIL_UNDERLINE_CHARACTER 		= '~';
const ZIL_ICHAR ZIL_OLD_UNDERLINE_CHARACTER 	= '&';
#endif

// Border widths for WOF_BORDER flag option when set
#if defined(ZIL_MSWINDOWS_STYLE) || defined(ZIL_OS2_STYLE)
	const int ZIL_BORDER_WIDTH = 1;
#elif defined(ZIL_MOTIF_STYLE)
	const int ZIL_BORDER_WIDTH = 2;
#else
	const int ZIL_BORDER_WIDTH = 0;
#endif

#if defined(ZIL_MOTIF)
	typedef Widget (*ZIL_MOTIF_CONVENIENCE_FUNCTION)(Widget, String, ArgList, Cardinal);
#elif defined(ZIL_OS2)
	extern MRESULT EXPENTRY ObjectJumpProcedure(HWND, ULONG, MPARAM, MPARAM);
#endif

typedef UI_WINDOW_OBJECT *(*ZIL_NEW_FUNCTION)(const ZIL_ICHAR *, ZIL_STORAGE_READ_ONLY *, ZIL_STORAGE_OBJECT_READ_ONLY *, UI_ITEM *, UI_ITEM *);
typedef EVENT_TYPE (*ZIL_USER_FUNCTION)(UI_WINDOW_OBJECT *, UI_EVENT &, EVENT_TYPE);
typedef EVENT_TYPE (*ZIL_EXIT_FUNCTION)(UI_DISPLAY *, UI_EVENT_MANAGER *, UI_WINDOW_MANAGER *);

class ZIL_EXPORT_CLASS UI_WINDOW_OBJECT : public UI_ELEMENT, public ZIL_INTERNATIONAL
{
	friend class ZIL_EXPORT_CLASS UIW_WINDOW;
	friend class ZIL_EXPORT_CLASS UIW_COMBO_BOX;
	friend class ZIL_EXPORT_CLASS UIW_SCROLL_BAR;
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
	friend class ZIL_EXPORT_CLASS ZAF_WINDOW_EDITOR;
	friend class ZIL_EXPORT_CLASS ZAF_WINDOW;
public:
	// Forward declaration of classes used by UI_WINDOW_OBJECT.
	friend class ZIL_EXPORT_CLASS UI_WINDOW_MANAGER;
	friend class ZIL_EXPORT_CLASS UI_ERROR_STUB;
	friend class ZIL_EXPORT_CLASS UI_ERROR_SYSTEM;
	friend class ZIL_EXPORT_CLASS UI_HELP_STUB;

	static int repeatRate;
	static int delayLength;
	static int doubleClickRate;
	static WOS_STATUS defaultStatus;
	static UI_DISPLAY *display;
	static UI_EVENT_MANAGER *eventManager;
	static UI_WINDOW_MANAGER *windowManager;
	static UI_ERROR_STUB *errorSystem;
	static UI_HELP_STUB *helpSystem;
	static ZIL_STORAGE_READ_ONLY *defaultStorage;
	static UI_ITEM *objectTable;
	static UI_ITEM *userTable;
	static ZIL_ICHAR _className[];

	UI_EVENT_MAP *eventMapTable;
	UI_EVENT_MAP *hotKeyMapTable;
	UI_PALETTE_MAP *paletteMapTable;

#if defined(ZIL_MACINTOSH)
	union
	{
		ZIL_SCREENID	screenID;
		ControlHandle	controlScreenID;
		ListHandle		listScreenID;
		MenuHandle		menuScreenID;
		TEHandle		textScreenID;
		WindowPtr		windowScreenID;
	};
#elif defined(ZIL_MSWINDOWS)
	union
	{
		ZIL_SCREENID	screenID;
		HMENU			menuID;
		HANDLE			genericID;
	};
#else
	ZIL_SCREENID screenID;
#endif
	WOF_FLAGS woFlags;
	WOAF_FLAGS woAdvancedFlags;
#if defined (ZIL_EDIT)
	WOAF_FLAGS designerAdvancedFlags;
#endif
	WOS_STATUS woStatus;
	UI_REGION trueRegion;
	UI_REGION relative;
	UI_WINDOW_OBJECT *parent;
	UI_HELP_CONTEXT helpContext;

	UIF_FLAGS userFlags;
	UIS_STATUS userStatus;
	void *userObject;
	EVENT_TYPE (*userFunction)(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	EVENT_TYPE UserFunction(const UI_EVENT &event, EVENT_TYPE ccode);

	// --- General class members ---
	virtual ~UI_WINDOW_OBJECT(void);
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
	virtual int Validate(int processError = TRUE);

	// --- Flag members ---
	// coordinates
	int HasConvertedCoordinate(void) { return ((woStatus & WOS_GRAPHICS) ? TRUE : FALSE); }
	int UsesDefaultCellCoordinate(void) { return ((woFlags & (WOF_MINICELL | WOF_PIXEL)) ? FALSE : TRUE); }
	int UsesDefaultMinicellCoordinate(void) { return ((woFlags & WOF_MINICELL) ? TRUE : FALSE); }
	int UsesDefaultPixelCoordinate(void) { return ((woFlags & WOF_PIXEL) ? TRUE : FALSE); }
	// justification
	int IsLeftJustified(void) { return ((woFlags & (WOF_JUSTIFY_CENTER | WOF_JUSTIFY_RIGHT)) ? FALSE : TRUE); }
	int IsRightJustified(void) { return ((woFlags & WOF_JUSTIFY_RIGHT) ? TRUE : FALSE); }
	int IsCenterJustified(void) { return ((woFlags & WOF_JUSTIFY_CENTER) ? TRUE : FALSE); }
	// status
	int HasChanged(void) { return ((woStatus & WOS_CHANGED) ? TRUE : FALSE); }
	int HasReadError(void) { return ((woStatus & WOS_READ_ERROR) ? TRUE : FALSE); }
	int IsCurrent(void) { return ((woStatus & WOS_CURRENT) ? TRUE : FALSE); }
	int IsSelected(void) { return ((woStatus & WOS_SELECTED) ? TRUE : FALSE); }
	int IsDefaultUnanswered(void) { return ((woFlags & WOF_UNANSWERED) ? TRUE : FALSE); }
	int IsDefaultInvalid(void) { return ((woFlags & WOF_INVALID) ? TRUE : FALSE); }
	int IsInvalid(void) { return ((woStatus & WOS_INVALID) ? TRUE : FALSE); }
	int IsNonselectable(void) { return ((woFlags & WOF_NON_SELECTABLE) ? TRUE : FALSE); }
	int IsNoncurrent(void) { return ((woAdvancedFlags & WOAF_NON_CURRENT) ? TRUE : FALSE); }
	int IsTemporary(void) { return ((woAdvancedFlags & WOAF_TEMPORARY) ? TRUE : FALSE); }
	int IsUnanswered(void) { return ((woStatus & WOS_UNANSWERED) ? TRUE : FALSE); }
	int IsUpToDate(void) { return ((woStatus & WOS_REDISPLAY) ? FALSE : TRUE); }
	int IsViewOnly(void) { return ((woFlags & WOF_VIEW_ONLY) ? TRUE : FALSE); }
	// attributes
	int IsBordered(void) { return ((woFlags & WOF_BORDER) ? TRUE : FALSE); }
	int IsAutoClear(void) { return ((woFlags & WOF_AUTO_CLEAR) ? TRUE : FALSE); }
	int IsDestroyable(void) { return ((woAdvancedFlags & WOAF_NO_DESTROY) ? FALSE : TRUE); }
	int UsesAvailableRegion(void) { return ((woFlags & WOF_NON_FIELD_REGION) ? TRUE : FALSE); }
	int UsesNormalHotKeys(void) { return ((woAdvancedFlags & WOAF_NORMAL_HOT_KEYS) ? TRUE : FALSE); }
	int UsesOutsideRegion(void) { return ((woAdvancedFlags & WOAF_OUTSIDE_REGION) ? TRUE : FALSE); }
	int UsesUserData(void) { return ((woFlags & WOF_NO_ALLOCATE_DATA) ? TRUE : FALSE); }
	// presentation
	int IsLocked(void) { return ((woAdvancedFlags & WOAF_LOCKED) ? TRUE : FALSE); }
	int IsMaximized(void) { return ((woStatus & WOS_MAXIMIZED) ? TRUE : FALSE); }
	int IsMinimized(void) { return ((woStatus & WOS_MINIMIZED) ? TRUE : FALSE); }
	int IsModal(void) { return ((woAdvancedFlags & WOAF_MODAL) ? TRUE : FALSE); }
	int IsMoveable(void) { return ((woAdvancedFlags & WOAF_NO_MOVE) ? FALSE : TRUE); }
	int IsSizeable(void) { return ((woAdvancedFlags & WOAF_NO_SIZE) ? FALSE : TRUE); }
	// drag & drop	
	int AcceptsDrop(void) { return ((woAdvancedFlags & WOAF_ACCEPTS_DROP) ? TRUE : FALSE); }
	int IsCopyDraggable(void) { return ((woAdvancedFlags & WOAF_COPY_DRAG_OBJECT) ? TRUE : FALSE); }
	int IsMoveDraggable(void) { return ((woAdvancedFlags & WOAF_MOVE_DRAG_OBJECT) ? TRUE : FALSE); }
	// types
	int IsDialog(void) { return ((woAdvancedFlags & WOAF_DIALOG_OBJECT) ? TRUE : FALSE); }
	int IsMDI(void) { return ((woAdvancedFlags & WOAF_MDI_OBJECT) ? TRUE : FALSE); }
	int IsOwnerDraw(void) { return ((woStatus & WOS_OWNERDRAW) ? TRUE : FALSE); }
	int IsSupport(void) { return ((woFlags & WOF_SUPPORT_OBJECT) ? TRUE : FALSE); }

	// --- Window object members ---
	ZIL_LOGICAL_FONT Font(ZIL_LOGICAL_FONT font = FNT_NONE);
	UI_WINDOW_OBJECT *Get(const ZIL_ICHAR *name) { return ((UI_WINDOW_OBJECT *)Information(I_GET_STRINGID_OBJECT, (void *)name, ID_DEFAULT)); }
	UI_WINDOW_OBJECT *Get(ZIL_NUMBERID _numberID) { return ((UI_WINDOW_OBJECT *)Information(I_GET_NUMBERID_OBJECT, (void *)&_numberID, ID_DEFAULT)); }
	unsigned HotKey(unsigned hotKey = 0);
	unsigned HotKey(ZIL_ICHAR *text);
	int Inherited(ZIL_OBJECTID matchID);
	EVENT_TYPE LogicalEvent(const UI_EVENT &event, ZIL_OBJECTID currentID = 0, int nativeType = TRUE);
	UI_PALETTE *LogicalPalette(ZIL_LOGICAL_EVENT logicalEvent, ZIL_OBJECTID currentID = 0);
	static void NormalizePosition(UI_WINDOW_OBJECT *object, const UI_EVENT &event, UI_POSITION &position);
	static void NormalizeString(ZIL_ICHAR *desination, const char *source);
	ZIL_NUMBERID NumberID(ZIL_NUMBERID numberID = 0);
	EVENT_TYPE RedisplayType(void);
	void RegionConvert(UI_REGION &region, int absolute);
	UI_WINDOW_OBJECT *Root(int mdiChild = FALSE);
	ZIL_OBJECTID SearchID(void) { return (searchID); }
	ZIL_ICHAR *StringID(const ZIL_ICHAR *stringID = ZIL_NULLP(ZIL_ICHAR));
#if defined(ZIL_MOTIF)
	static XmString CreateMotifString(ZIL_ICHAR *, ZIL_ICHAR ** = ZIL_NULLP(ZIL_ICHAR *), int = TRUE);
	static void StripHotKeyMark(ZIL_ICHAR *text);
	virtual ZIL_SCREENID TopWidget(void);
	Widget shell;
#elif defined(ZIL_NEXTSTEP)
	static void StripHotKeyMark(ZIL_ICHAR *text);
#endif
	static void TruetoRelative(UI_WINDOW_OBJECT *object, const UI_POSITION &trueRegion, UI_POSITION &relative);
	ZIL_OBJECTID WindowID(ZIL_OBJECTID windowID);

	// --- List members ---
	UI_WINDOW_OBJECT *Next(void) { return((UI_WINDOW_OBJECT *)next); }
	UI_WINDOW_OBJECT *Previous(void) { return((UI_WINDOW_OBJECT *)previous); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UI_WINDOW_OBJECT::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	UI_WINDOW_OBJECT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name,
		ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
		ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
#endif

protected:
	ZIL_OBJECTID searchID;
	ZIL_NUMBERID numberID;
	ZIL_ICHAR stringID[32];
	ZIL_OBJECTID windowID[5];

	unsigned hotKey;
	ZIL_LOGICAL_FONT font;
	UI_PALETTE *lastPalette;
	ZIL_ICHAR *userObjectName;				// Used for storage purposes only.
	ZIL_ICHAR *userFunctionName;			// Used for storage purposes only.
	UI_REGION clip;
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	static ZIL_ICHAR *pasteBuffer;	// There is only one global paste buffer.
	static int pasteLength;
#elif defined(ZIL_WINNT)
	static HBRUSH ctlHBrush;
	DWORD dwStyle;
	WNDPROC defaultCallback;
	void RegisterObject(char *className, char *winClassName,
		WNDPROC *defProcInstance, ZIL_ICHAR *title = ZIL_NULLP(ZIL_ICHAR), HMENU menu = 0);
#elif defined(ZIL_MSWINDOWS)
	static HBRUSH ctlHBrush;
	DWORD dwStyle;
	FARPROC defaultCallback;

	void RegisterObject(char *className, char *winClassName, int *offset,
		FARPROC *procInstance, FARPROC *defProcInstance,
		ZIL_ICHAR *title = ZIL_NULLP(ZIL_ICHAR), HMENU menu = 0);
#elif defined(ZIL_OS2)
	ZIL_UINT32 flStyle;
	ZIL_UINT32 flFlag;
	PFNWP defaultCallback;

	EVENT_TYPE DefaultCallback(const UI_EVENT &event);
	void RegisterObject(char *className, PSZ os2ClassName,
		PFNWP *baseCallback, ZIL_ICHAR *title, void *controlData = ZIL_NULLP(void),
		ZIL_SCREENID parentID = 0);
#elif defined(ZIL_MOTIF)
	static Arg args[50];
	static int nargs;
	void RegisterObject(WidgetClass widgetClass,
		ZIL_MOTIF_CONVENIENCE_FUNCTION convenienceFunction, EVENT_TYPE ccode,
		int useArgs = FALSE, int manage = TRUE, ZIL_SCREENID parent = 0);
#	if (ZIL_MOTIF >= 1002)
	static Boolean DragConvertProc(Widget w, Atom *selection, Atom *target,
		Atom *typeRtn, XtPointer *valueRtn, unsigned long *lengthRtn,
		int *formatRtn, unsigned long *max_lengthRtn, XtPointer client_data,
		XtRequestId *);
	static void HandleDrop(Widget w, XtPointer clientData, XtPointer callData);
	static void TransferProc(Widget w, XtPointer closure, Atom *selType,
		Atom *type, XtPointer value, unsigned long *length, int *format);
	static void DragDropFinish(Widget dc, XtPointer clientData,
		XtPointer callData);
#	endif
#endif

	UI_WINDOW_OBJECT(int left, int top, int width, int height,
		WOF_FLAGS woFlags, WOAF_FLAGS woAdvancedFlags);
	EVENT_TYPE DrawBorder(ZIL_SCREENID screenID, UI_REGION &region, int fillRegion, EVENT_TYPE ccode);
	EVENT_TYPE DrawFocus(ZIL_SCREENID screenID, UI_REGION &region, EVENT_TYPE ccode);
	EVENT_TYPE DrawShadow(ZIL_SCREENID screenID, UI_REGION &region, int depth, int fillRegion, EVENT_TYPE ccode);
	EVENT_TYPE DrawText(ZIL_SCREENID screenID, UI_REGION &region, const ZIL_ICHAR *text, UI_PALETTE *palette, int fillRegion, EVENT_TYPE ccode);
	void Modify(const UI_EVENT &event);
	int NeedsUpdate(const UI_EVENT &event, EVENT_TYPE ccode);
	void RegisterObject(char *name);

private:
	void OSInitialize(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_WINDOW ---------------------------------------------------------
// --------------------------------------------------------------------------

// --- WNF_FLAGS ---
typedef UIF_FLAGS WNF_FLAGS;
const WNF_FLAGS WNF_NO_FLAGS			= 0x0000;
const WNF_FLAGS WNF_NO_WRAP				= 0x0001;
const WNF_FLAGS WNF_SELECT_MULTIPLE		= 0x0002;
const WNF_FLAGS WNF_BITMAP_CHILDREN		= 0x0004;	// Option used for list, matrix, combo.
const WNF_FLAGS WNF_OWNERDRAW_CHILDREN	= 0x0004;
const WNF_FLAGS WNF_AUTO_SORT			= 0x0008;
const WNF_FLAGS WNF_CONTINUE_SELECT		= 0x0010;
const WNF_FLAGS WNF_AUTO_SELECT			= 0x0020;	// Automatically select when item is current.
const WNF_FLAGS WNF_MOVE_FORWARD		= 0x0040;
const WNF_FLAGS WNF_MOVE_REVERSE		= 0x0080;
const WNF_FLAGS WNF_NO_SCROLL			= 0x0100;	// Window doesn't handle scrolling.

// --- UIW_WINDOW::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_SET_VSCROLL		= 1000;
const ZIL_INFO_REQUEST I_SET_HSCROLL		= 1001;
const ZIL_INFO_REQUEST I_CHECK_SELECTION	= 1002;
const ZIL_INFO_REQUEST I_GET_CLIPREGION     = 1003;
const ZIL_INFO_REQUEST I_DESTROY_LIST       = 1004;

// --- Private class messages - DO NOT USE! ---
const ZIL_INFO_REQUEST I_GET_ICON			= 1100;
const ZIL_INFO_REQUEST I_SET_ICON			= 1101;
const ZIL_INFO_REQUEST I_GET_TABCOLUMN      = 1102;
const ZIL_INFO_REQUEST I_SET_TABCOLUMN      = 1103;
const ZIL_INFO_REQUEST I_GET_MENU			= 1104;
const ZIL_INFO_REQUEST I_SET_MENU			= 1105;
const ZIL_INFO_REQUEST I_GET_TITLE			= 1106;
const ZIL_INFO_REQUEST I_SET_TITLE			= 1107;
const ZIL_INFO_REQUEST I_GET_FRAMEID		= 1108;
const ZIL_INFO_REQUEST I_SET_FRAMEID		= 1109;

const ZIL_NUMBERID ZIL_UNK_OBJ = 1;
const ZIL_NUMBERID ZIL_NO_OBJ  = 2;
const ZIL_NUMBERID ZIL_NO_FILE = 3;

class ZIL_EXPORT_CLASS UIW_WINDOW : public UI_WINDOW_OBJECT, public UI_LIST
{
	friend class UI_WINDOW_MANAGER;
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	WNF_FLAGS wnFlags;
	UI_LIST support;

	// --- General class members ---
	UIW_WINDOW(int left, int top, int width, int height,
		WOF_FLAGS woFlags = WOF_NO_FLAGS,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS,
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT,
		UI_WINDOW_OBJECT *minObject = ZIL_NULLP(UI_WINDOW_OBJECT));
	virtual ~UIW_WINDOW(void);
	static UIW_WINDOW *Generic(int left, int top, int width, int height,
		ZIL_ICHAR *title, UI_WINDOW_OBJECT *minObject = ZIL_NULLP(UI_WINDOW_OBJECT),
		WOF_FLAGS woFlags = WOF_NO_FLAGS,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS,
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT);

	// --- Flag members ---
	int AllowsDragSelection(void) { return ((wnFlags & WNF_CONTINUE_SELECT) ? TRUE : FALSE); }
	int AllowsMultipleSelection(void) { return ((wnFlags & WNF_SELECT_MULTIPLE) ? TRUE : FALSE); }
	int AutoSortsData(void) { return ((wnFlags & WNF_AUTO_SORT) ? TRUE : FALSE); }
	int HasWrappedData(void) { return ((wnFlags & WNF_NO_WRAP) ? FALSE : TRUE); }
	int HasOwnerDrawChildren(void) { return ((wnFlags & WNF_OWNERDRAW_CHILDREN) ? TRUE : FALSE); }
	int MovesForward(void) { return ((wnFlags & WNF_MOVE_FORWARD) ? TRUE : FALSE); }
	int MovesBackward(void) { return ((wnFlags & WNF_MOVE_REVERSE) ? TRUE : FALSE); }
	int IsScrollable(void) { return ((wnFlags & WNF_NO_SCROLL) ? FALSE : TRUE); }
	int SelectsOnDownClick(void) { return ((wnFlags & WNF_AUTO_SELECT) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	UI_WINDOW_OBJECT *Get(const ZIL_ICHAR *name) { return ((UI_WINDOW_OBJECT *)Information(I_GET_STRINGID_OBJECT, (void *)name, ID_DEFAULT)); }
	UI_WINDOW_OBJECT *Get(ZIL_NUMBERID _numberID) { return ((UI_WINDOW_OBJECT *)Information(I_GET_NUMBERID_OBJECT, (void *)&_numberID, ID_DEFAULT)); }
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
	virtual EVENT_TYPE ScrollEvent(UI_EVENT &event);
	static int StringCompare(void *object1, void *object2);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	UI_WINDOW_OBJECT *Current(void) { return((UI_WINDOW_OBJECT *)current); }
	virtual void Destroy(void);
	UI_WINDOW_OBJECT *First(void) { return((UI_WINDOW_OBJECT *)first); }
	UI_WINDOW_OBJECT *Last(void) { return((UI_WINDOW_OBJECT *)last); }
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_WINDOW &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_WINDOW &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_WINDOW::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_WINDOW(name, file, object, _objectTable, _userTable)); }
	UIW_WINDOW(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);

	UIW_WINDOW(const ZIL_ICHAR *name, const ZIL_ICHAR *deltaName,
		const ZIL_ICHAR *deltaPathName = ZIL_NULLP(ZIL_ICHAR),
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_READ_ONLY *deltaFile = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	void DeltaLoad(const ZIL_ICHAR *name, const ZIL_ICHAR *deltaName,
		const ZIL_ICHAR *deltaPathName,
		ZIL_STORAGE_READ_ONLY *file, ZIL_STORAGE_READ_ONLY *deltaFile,
		UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name,
		ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
		ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	void DeltaStore(const ZIL_ICHAR *name, const ZIL_ICHAR *deltaName,
		const ZIL_ICHAR *deltaPathName = ZIL_NULLP(ZIL_ICHAR),
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE *deltaFile = ZIL_NULLP(ZIL_STORAGE),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM), int appendNames = FALSE);
#endif

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }

protected:
	UI_REGION scroll;
	UI_REGION_LIST clipList;
	UI_WINDOW_OBJECT *vScroll;
	UI_WINDOW_OBJECT *hScroll;
	UI_SCROLL_INFORMATION hScrollInfo;
	UI_SCROLL_INFORMATION vScrollInfo;
	ZIL_ICHAR *compareFunctionName;			// Used for storage purposes only.
	UI_WINDOW_OBJECT *defaultObject;
	const ZIL_LANGUAGE *myLanguage;

	void CheckSelection(UI_WINDOW_OBJECT *selectedObject = ZIL_NULLP(UI_WINDOW_OBJECT));

#ifdef ZIL_MOTIF
	ZIL_UINT32 supportDecorations;
#endif

private:
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	UI_WINDOW_OBJECT *icon;
	UI_REGION restore;
#elif defined(ZIL_MSWINDOWS)
	UI_WINDOW_OBJECT *title;
	ZIL_SCREENID frameID;
	HMENU menu;
	ZIL_ICON_HANDLE icon;
	LRESULT (FAR PASCAL *mdiCallback)(HWND, HWND, UINT, WPARAM, LRESULT);
	UI_REGION restore;
#elif defined(ZIL_OS2)
	friend MRESULT EXPENTRY FrameJumpProcedure(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
	ZIL_SCREENID frameID;
	EVENT_TYPE FrameEvent(const UI_EVENT &event);
	UI_WINDOW_OBJECT *title;
	UI_REGION restore;
#elif defined(ZIL_MOTIF)
	static void ExitShell(Widget w, XtPointer closure, XtPointer call_data);
	void ShellRegion(UI_REGION &);
	UI_REGION GetMotifRegion(void);
#elif defined(ZIL_MACINTOSH)
	UI_WINDOW_OBJECT *menu;
#elif defined(ZIL_NEXTSTEP)
	UI_WINDOW_OBJECT *title;
	id menuBar;
	id scrollView;
#endif

	void Error(UIS_STATUS status, const ZIL_ICHAR *format, ...);

private:
	void OSDestroy(void);
	void OSInitialize(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UI_WINDOW_MANAGER --------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UI_WINDOW_MANAGER : public UIW_WINDOW
{
	friend class UI_WINDOW_OBJECT;
public:
	ZIL_EXIT_FUNCTION exitFunction;
	UI_WINDOW_OBJECT *dragObject;

	// --- General class members ---
	UI_WINDOW_MANAGER(UI_DISPLAY *display, UI_EVENT_MANAGER *eventManager,
		ZIL_EXIT_FUNCTION exitFunction = ZIL_NULLF(ZIL_EXIT_FUNCTION));
	virtual ~UI_WINDOW_MANAGER(void);
	void Center(UI_WINDOW_OBJECT *object);

	// --- Window object members ---
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UI_WINDOW_MANAGER &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UI_WINDOW_MANAGER &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); };

private:
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	static ZIL_SCREENID currentScreenID;
#endif
#if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_XT)
	static ZIL_SCREENID taskID;
#endif
#if (ZIL_MOTIF >= 1002)
	static Atom ZINC_DND;
#endif
};

// --------------------------------------------------------------------------
// ----- UIW_BORDER ---------------------------------------------------------
// --------------------------------------------------------------------------

// --- BDF_FLAGS ---
typedef UIF_FLAGS BDF_FLAGS;
const BDF_FLAGS BDF_NO_FLAGS			= 0x0000;

class ZIL_EXPORT_CLASS UIW_BORDER : public UI_WINDOW_OBJECT
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static int width;
	static ZIL_ICHAR _className[];
	BDF_FLAGS bdFlags;

	// --- General class members ---
	UIW_BORDER(BDF_FLAGS bdFlags = BDF_NO_FLAGS);
	virtual ~UIW_BORDER(void);
	int DataGet(void);
	void DataSet(int width);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_BORDER::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_BORDER(name, file, object, _objectTable, _userTable)); }
	UIW_BORDER(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void OSDataGet(void);
	void OSDataSet(void);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_PROMPT ---------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_PROMPT : public UI_WINDOW_OBJECT
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_PROMPT(int left, int top, ZIL_ICHAR *text, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	UIW_PROMPT(int left, int top, int width, ZIL_ICHAR *text, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	virtual ~UIW_PROMPT(void);
	ZIL_ICHAR *DataGet(void);
	void DataSet(ZIL_ICHAR *text);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_PROMPT::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_PROMPT(name, file, object, _objectTable, _userTable)); }
	UIW_PROMPT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	ZIL_ICHAR *text;

private:
	void OSDataSet(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_BUTTON ---------------------------------------------------------
// --------------------------------------------------------------------------

// --- BTF_FLAGS ---
typedef UIF_FLAGS BTF_FLAGS;
const BTF_FLAGS BTF_NO_FLAGS			= 0x0000;
const BTF_FLAGS BTF_NO_TOGGLE			= 0x0001;	// don't toggle (select) the button
const BTF_FLAGS BTF_DOWN_CLICK			= 0x0002;	// call user-function on down click
const BTF_FLAGS BTF_REPEAT				= 0x0008;	// repeat the down-click action
const BTF_FLAGS BTF_DOUBLE_CLICK		= 0x0010;	// perform the action on a double-mouse click.
const BTF_FLAGS BTF_AUTO_SIZE			= 0x0020;	// size according to mode of operation
const BTF_FLAGS BTF_NO_3D				= 0x0040;	// do not shadow button
const BTF_FLAGS BTF_CHECK_BOX			= 0x0080;	// display a check box
const BTF_FLAGS BTF_RADIO_BUTTON		= 0x0100;	// display a radio button
const BTF_FLAGS BTF_SEND_MESSAGE		= 0x0200;	// use the Message function
const BTF_FLAGS BTF_STATIC_BITMAPARRAY	= 0x0400;	// do not destroy the bitmap array.
const BTF_FLAGS BTF_DEFAULT_BUTTON      = 0x0800;   // act as the default button.

// --- BTS_STATUS ---
typedef UIS_STATUS BTS_STATUS;
const BTS_STATUS BTS_NO_STATUS			= 0x0000;
const BTS_STATUS BTS_DEPRESSED			= 0x0001;	// button is depressed
const BTS_STATUS BTS_DEFAULT			= 0x0002;	// button has heavy border

// --- UIW_BUTTON::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_GET_BITMAP_WIDTH	= 500;
const ZIL_INFO_REQUEST I_SET_BITMAP_WIDTH	= 501;
const ZIL_INFO_REQUEST I_GET_BITMAP_HEIGHT	= 502;
const ZIL_INFO_REQUEST I_SET_BITMAP_HEIGHT	= 503;
const ZIL_INFO_REQUEST I_GET_BITMAP_ARRAY	= 504;
const ZIL_INFO_REQUEST I_SET_BITMAP_ARRAY	= 505;

const ZIL_NUMBERID ZIL_CheckNormal = 1;
const ZIL_NUMBERID ZIL_CheckSelected = 2;
const ZIL_NUMBERID ZIL_RadioNormal = 3;
const ZIL_NUMBERID ZIL_RadioSelected = 4;
const ZIL_NUMBERID ZIL_CheckNormalHiRes = 5;
const ZIL_NUMBERID ZIL_CheckSelectedHiRes = 6;
const ZIL_NUMBERID ZIL_RadioNormalHiRes = 7;
const ZIL_NUMBERID ZIL_RadioSelectedHiRes = 8;

class ZIL_EXPORT_CLASS UIW_BUTTON : public UI_WINDOW_OBJECT
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	BTF_FLAGS btFlags;
	EVENT_TYPE value;

	// --- General class members ---
	UIW_BUTTON(int left, int top, int width, ZIL_ICHAR *text,
		BTF_FLAGS btFlags = BTF_NO_TOGGLE | BTF_AUTO_SIZE,
		WOF_FLAGS woFlags = WOF_JUSTIFY_CENTER,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION),
		EVENT_TYPE value = 0, ZIL_ICHAR *bitmapName = ZIL_NULLP(ZIL_ICHAR));
	virtual ~UIW_BUTTON(void);
	ZIL_ICHAR *DataGet(int stripText = FALSE);
	void DataSet(ZIL_ICHAR *text);
	static EVENT_TYPE Message(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);

	// --- Flag members ---
	int AllowsToggling(void) { return ((btFlags & BTF_NO_TOGGLE) ? FALSE : TRUE); }
	int AutoRepeatsSelection(void) { return ((btFlags & BTF_REPEAT) ? TRUE : FALSE); }
	int AutoSizes(void) { return ((btFlags & BTF_AUTO_SIZE) ? TRUE : FALSE); }
	int HasStaticArray(void) { return ((btFlags & BTF_STATIC_BITMAPARRAY) ? TRUE : FALSE); }
	int IsCurrentDefault(void) { return ((btStatus & BTS_DEFAULT) ? TRUE : FALSE); }
	int IsDepressed(void) { return ((btStatus & BTS_DEPRESSED) ? TRUE : FALSE); }
	int Is3D(void) { return ((btFlags & BTF_NO_3D) ? FALSE : TRUE); }
	int SelectsOnDoubleClick(void) { return ((btFlags & BTF_DOUBLE_CLICK) ? TRUE : FALSE); }
	int SelectsOnDownClick(void) { return ((btFlags & BTF_DOWN_CLICK) ? TRUE : FALSE); }
	int SendsMessageWhenSelected(void) { return ((btFlags & BTF_SEND_MESSAGE) ? TRUE : FALSE); }
	// types
	int IsCheckBox(void) { return ((btFlags & BTF_CHECK_BOX) ? TRUE : FALSE); }
	int IsDefaultButton(void) { return ((btFlags & BTF_DEFAULT_BUTTON) ? TRUE : FALSE); }
	int IsRadioButton(void) { return ((btFlags & BTF_RADIO_BUTTON) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_BUTTON::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_BUTTON(name, file, object, _objectTable, _userTable)); }
	UIW_BUTTON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetDecorations(const ZIL_ICHAR *decorationName)
		{ ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations); myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className, decorationName); }

protected:
	BTS_STATUS btStatus;
	int depth;
	ZIL_ICHAR *text;
	ZIL_ICHAR *bitmapName;
	int bitmapWidth;
	int bitmapHeight;
	ZIL_UINT8 *bitmapArray;
	ZIL_BITMAP_HANDLE colorBitmap, monoBitmap;
	const ZIL_DECORATION *myDecorations;

private:
	void OSDataSet(void);
	void OSImageInitialize(void);
	void OSSystemImage(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_IMAGE ----------------------------------------------------------
// --------------------------------------------------------------------------

typedef UIF_FLAGS IMF_FLAGS;
const IMF_FLAGS IMF_NO_FLAGS				= 0x0000;
const IMF_FLAGS IMF_TILED					= 0x0001;
const IMF_FLAGS IMF_SCALED					= 0x0002;
const IMF_FLAGS IMF_BACKGROUND				= 0x0004;
const IMF_FLAGS IMF_STATIC_HANDLE			= 0x0008;
const IMF_FLAGS IMF_AUTO_SIZE				= 0x0010;

class ZIL_EXPORT_CLASS UIW_IMAGE : public UI_WINDOW_OBJECT
{
public:
	static ZIL_ICHAR _className[];
	IMF_FLAGS imFlags;

	// --- General class members ---
	UIW_IMAGE(int left, int top, int width, int height, ZIL_ICHAR *pathName,
		int pathID = -1, IMF_FLAGS imFlags = IMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_NO_FLAGS);
	virtual ~UIW_IMAGE(void);

	// --- Flag members ---
	int AutoSizes(void) { return ((imFlags & IMF_AUTO_SIZE) ? TRUE : FALSE); }
	int HasStaticHandle(void) { return ((imFlags & IMF_STATIC_HANDLE) ? TRUE : FALSE); }
	int IsBackground(void) { return ((imFlags & IMF_BACKGROUND) ? TRUE : FALSE); }
	int IsScaled(void) { return ((imFlags & IMF_SCALED) ? TRUE : FALSE); }
	int IsTiled(void) { return ((imFlags & IMF_TILED) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_IMAGE::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_IMAGE(name, file, object, _objectTable, _userTable)); }
	UIW_IMAGE(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	ZIL_ICHAR *pathName;
	int pathID;
	int imageWidth, imageHeight;
	ZIL_IMAGE_HANDLE image;
#if defined (ZIL_MSWINDOWS)
	HPALETTE hPalette;
#endif

	// --- Import filter definitions ---
	int LoadImageFromApplication(void);
	int LoadImageFromFile(void);
	int LoadImageFromStorage(void);

	void DestroyImageHandle(void);
};

// --------------------------------------------------------------------------
// ----- UIW_TITLE ----------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_TITLE : public UIW_BUTTON
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_TITLE(ZIL_ICHAR *text, WOF_FLAGS woFlags = WOF_BORDER | WOF_JUSTIFY_CENTER);
	virtual ~UIW_TITLE(void);
	ZIL_ICHAR *DataGet(void);
	void DataSet(ZIL_ICHAR *text);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_TITLE::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_TITLE(name, file, object, _objectTable, _userTable)); }
	UIW_TITLE(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

private:
#if defined(ZIL_MOTIF) && defined(ZIL_UNICODE)
	char *mappedTitle;
#endif

private:
	void OSDataSet(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_MAXIMIZE_BUTTON ------------------------------------------------
// --------------------------------------------------------------------------

const ZIL_NUMBERID ZIL_MaxNormal = 1;
const ZIL_NUMBERID ZIL_Maximized = 2;

class ZIL_EXPORT_CLASS UIW_MAXIMIZE_BUTTON : public UIW_BUTTON
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;

	// --- General class members ---
	UIW_MAXIMIZE_BUTTON(void);
	virtual ~UIW_MAXIMIZE_BUTTON(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_MAXIMIZE_BUTTON::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_MAXIMIZE_BUTTON(name, file, object, _objectTable, _userTable)); }
	UIW_MAXIMIZE_BUTTON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetDecorations(const ZIL_ICHAR *decorationName)
		{ ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations); myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className, decorationName); }

protected:
	const ZIL_DECORATION *myDecorations;

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_MINIMIZE_BUTTON ------------------------------------------------
// --------------------------------------------------------------------------

const ZIL_NUMBERID ZIL_MinNormal = 1;
const ZIL_NUMBERID ZIL_Minimized = 2;

class ZIL_EXPORT_CLASS UIW_MINIMIZE_BUTTON : public UIW_BUTTON
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;

	// --- General class members ---
	UIW_MINIMIZE_BUTTON(void);
	virtual ~UIW_MINIMIZE_BUTTON(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_MINIMIZE_BUTTON::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_MINIMIZE_BUTTON(name, file, object, _objectTable, _userTable)); }
	UIW_MINIMIZE_BUTTON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetDecorations(const ZIL_ICHAR *decorationName)
		{ ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations); myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className, decorationName); }

protected:
	const ZIL_DECORATION *myDecorations;

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_ICON -----------------------------------------------------------
// --------------------------------------------------------------------------

// --- ICF_FLAGS ---
typedef UIF_FLAGS ICF_FLAGS;
const ICF_FLAGS ICF_NO_FLAGS			= 0x0000;
const ICF_FLAGS ICF_DOUBLE_CLICK		= 0x0001;
const ICF_FLAGS ICF_MINIMIZE_OBJECT		= 0x0002;
const ICF_FLAGS ICF_STATIC_ICONARRAY	= 0x0400;	// do not destroy the icon array.

// --- UIW_ICON::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_GET_ICON_WIDTH		= I_GET_BITMAP_WIDTH;
const ZIL_INFO_REQUEST I_GET_ICON_HEIGHT	= I_GET_BITMAP_HEIGHT;
const ZIL_INFO_REQUEST I_GET_ICON_ARRAY		= I_GET_BITMAP_ARRAY;
const ZIL_INFO_REQUEST I_SET_ICON_ARRAY		= I_SET_BITMAP_ARRAY;

class ZIL_EXPORT_CLASS UIW_ICON : public UI_WINDOW_OBJECT
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static ZIL_ICHAR _applicationIconName[];
	static ZIL_ICHAR _asteriskIconName[];
	static ZIL_ICHAR _exclamationIconName[];
	static ZIL_ICHAR _handIconName[];
	static ZIL_ICHAR _questionIconName[];
	ICF_FLAGS icFlags;

	// --- General class members ---
	UIW_ICON(int left, int top, ZIL_ICHAR *iconName,
		ZIL_ICHAR *title = ZIL_NULLP(ZIL_ICHAR), ICF_FLAGS icFlags = ICF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_JUSTIFY_CENTER | WOF_NON_SELECTABLE,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_ICON(void);
	ZIL_ICHAR *DataGet(void);
	void DataSet(ZIL_ICHAR *text);

	// --- Flag members ---
	int HasStaticArray(void) { return ((icFlags & ICF_STATIC_ICONARRAY) ? TRUE : FALSE); }
	int IsMinimizeIcon(void) { return ((icFlags & ICF_MINIMIZE_OBJECT) ? TRUE : FALSE); }
	int SelectsOnDoubleClick(void) { return ((icFlags & ICF_DOUBLE_CLICK) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_ICON::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_ICON(name, file, object, _objectTable, _userTable)); }
	UIW_ICON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	ZIL_ICHAR *title;
	ZIL_ICHAR *iconName;
	int iconWidth;
	int iconHeight;
	ZIL_UINT8 *iconArray;
	ZIL_ICON_HANDLE icon;
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	UI_REGION iconRegion;
	UI_REGION titleRegion;
#endif

private:
#if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	BTS_STATUS icStatus;
#endif

private:
	void OSDataSet(void);
	void OSImageInitialize(void);
	void OSSystemImage(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_POP_UP_MENU ----------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_POP_UP_MENU : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_POP_UP_MENU(int left, int top, WNF_FLAGS wnFlags,
		WOF_FLAGS woFlags = WOF_BORDER, WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_POP_UP_MENU(int left, int top, WNF_FLAGS wnFlags, UI_ITEM *item);
	virtual ~UIW_POP_UP_MENU(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_POP_UP_MENU &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_POP_UP_MENU &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_POP_UP_MENU::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_POP_UP_MENU(name, file, object, _objectTable, _userTable)); }
	UIW_POP_UP_MENU(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

#if defined (ZIL_MSDOS) || defined (ZIL_CURSES) || defined (ZIL_XT)
private:
	int tabColumn;
#endif
};

// --------------------------------------------------------------------------
// ----- UIW_POP_UP_ITEM ----------------------------------------------------
// --------------------------------------------------------------------------

// --- MNIF_FLAGS ---
typedef UIF_FLAGS MNIF_FLAGS;
const MNIF_FLAGS MNIF_NO_FLAGS			= 0x0000;
const MNIF_FLAGS MNIF_SEPARATOR			= 0x0001;	// Menu item seperator
const MNIF_FLAGS MNIF_MAXIMIZE			= 0x0002;	// Maximize window
const MNIF_FLAGS MNIF_MINIMIZE			= 0x0004;	// Minimize window
const MNIF_FLAGS MNIF_MOVE				= 0x0008;	// Move window
const MNIF_FLAGS MNIF_SIZE				= 0x0010;	// Size window
const MNIF_FLAGS MNIF_SWITCH			= 0x0020;	// Switch to another window.
const MNIF_FLAGS MNIF_RESTORE			= 0x0040;	// Restore window size
const MNIF_FLAGS MNIF_CLOSE				= 0x0080;	// Close window
const MNIF_FLAGS MNIF_CHECK_MARK		= 0x0100;	// Show check-mark when selected.
const MNIF_FLAGS MNIF_SEND_MESSAGE		= 0x0200;	// use the Message function
const MNIF_FLAGS MNIF_NON_SELECTABLE	= 0x0400;	// item cannot be selected (grayed)
const MNIF_FLAGS MNIF_ABOUT				= 0x0800;	// About... menu item

const ZIL_NUMBERID ZIL_PopupCheckMark = 1;
const ZIL_NUMBERID ZIL_PopupRightArrow = 2;

class ZIL_EXPORT_CLASS UIW_POP_UP_ITEM : public UIW_BUTTON
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	MNIF_FLAGS mniFlags;
	UIW_POP_UP_MENU menu;

	// --- General class members ---
	UIW_POP_UP_ITEM(void);
	UIW_POP_UP_ITEM(ZIL_ICHAR *text, MNIF_FLAGS mniFlags = MNIF_NO_FLAGS,
		BTF_FLAGS btFlags = BTF_NO_3D, WOF_FLAGS woFlags = WOF_NO_FLAGS,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION), EVENT_TYPE value = 0);
	UIW_POP_UP_ITEM(int left, int top, int width, ZIL_ICHAR *text,
		MNIF_FLAGS mniFlags = MNIF_NO_FLAGS, BTF_FLAGS btFlags = BTF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_NO_FLAGS,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION),
		EVENT_TYPE value = 0);
	virtual ~UIW_POP_UP_ITEM(void);

	// --- Flag members ---
	int IsSelectable(void) { return ((mniFlags & MNIF_NON_SELECTABLE) ? FALSE : TRUE); }
	int IsSeparator(void) { return ((mniFlags & MNIF_SEPARATOR) ? TRUE : FALSE); }
	int SendsMessageWhenSelected(void) { return ((mniFlags & MNIF_SEND_MESSAGE) ? TRUE : FALSE); }
	// presentation
	int AllowsCheckMark(void) { return ((mniFlags & MNIF_CHECK_MARK) ? TRUE : FALSE); }
	// options
	int IsAboutOption(void) { return ((mniFlags & MNIF_ABOUT) ? TRUE : FALSE); }
	int IsCloseOption(void) { return ((mniFlags & MNIF_CLOSE) ? TRUE : FALSE); }
	int IsMaximizeOption(void) { return ((mniFlags & MNIF_MAXIMIZE) ? TRUE : FALSE); }
	int IsMinimizeOption(void) { return ((mniFlags & MNIF_MINIMIZE) ? TRUE : FALSE); }
	int IsMoveOption(void) { return ((mniFlags & MNIF_MOVE) ? TRUE : FALSE); }
	int IsRestoreOption(void) { return ((mniFlags & MNIF_RESTORE) ? TRUE : FALSE); }
	int IsSizeOption(void) { return ((mniFlags & MNIF_SIZE) ? TRUE : FALSE); }
	int IsSwitchOption(void) { return ((mniFlags & MNIF_SWITCH) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object) { return(menu.Add(object)); }
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object) { return(menu.Subtract(object)); }
	UIW_POP_UP_ITEM &operator+(UI_WINDOW_OBJECT *object) { menu.Add(object); return(*this); }
	UIW_POP_UP_ITEM &operator-(UI_WINDOW_OBJECT *object) { menu.Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_POP_UP_ITEM::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_POP_UP_ITEM(name, file, object, _objectTable, _userTable)); }
	UIW_POP_UP_ITEM(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetDecorations(const ZIL_ICHAR *decorationName)
		{ ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations); myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className, decorationName); }

protected:
	const ZIL_DECORATION *myDecorations;

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_PULL_DOWN_MENU -------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_PULL_DOWN_MENU : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_PULL_DOWN_MENU(int indentation = 0,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_PULL_DOWN_MENU(int indentation, UI_ITEM *item);
	virtual ~UIW_PULL_DOWN_MENU(void);
#if defined(ZIL_MACINTOSH)
	UIW_POP_UP_ITEM *ItemDepthSearch(long mSelect);
#endif

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_PULL_DOWN_MENU &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_PULL_DOWN_MENU &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_PULL_DOWN_MENU::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_PULL_DOWN_MENU(name, file, object, _objectTable, _userTable)); }
	UIW_PULL_DOWN_MENU(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	int indentation;

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_PULL_DOWN_ITEM -------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_PULL_DOWN_ITEM : public UIW_BUTTON
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	UIW_POP_UP_MENU menu;

	// --- General class members ---
	UIW_PULL_DOWN_ITEM(ZIL_ICHAR *text, WNF_FLAGS wnFlags = WNF_NO_FLAGS,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION),
		EVENT_TYPE value = 0);
	UIW_PULL_DOWN_ITEM(ZIL_ICHAR *text, WNF_FLAGS wnFlags, UI_ITEM *item);
	virtual ~UIW_PULL_DOWN_ITEM(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object) { return(menu.Add(object)); }
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object) { return(menu.Subtract(object)); }
	UIW_PULL_DOWN_ITEM &operator+(UI_WINDOW_OBJECT *object) { menu.Add(object); return(*this); }
	UIW_PULL_DOWN_ITEM &operator-(UI_WINDOW_OBJECT *object) { menu.Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_PULL_DOWN_ITEM::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_PULL_DOWN_ITEM(name, file, object, _objectTable, _userTable)); }
	UIW_PULL_DOWN_ITEM(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_SYSTEM_BUTTON --------------------------------------------------
// --------------------------------------------------------------------------

// --- SYF_FLAGS ---
typedef UIF_FLAGS SYF_FLAGS;
const SYF_FLAGS SYF_NO_FLAGS			= 0x0000;
const SYF_FLAGS SYF_GENERIC				= 0x0001;	// Generic system menu

const ZIL_NUMBERID ZIL_SysButton = 1;

class ZIL_EXPORT_CLASS UIW_SYSTEM_BUTTON : public UIW_BUTTON
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	SYF_FLAGS syFlags;
	UIW_POP_UP_MENU menu;

	// --- General class members ---
	UIW_SYSTEM_BUTTON(SYF_FLAGS syFlags = SYF_NO_FLAGS);
	UIW_SYSTEM_BUTTON(UI_ITEM *item);
	virtual ~UIW_SYSTEM_BUTTON(void);
	static UIW_SYSTEM_BUTTON *Generic(void);

	// --- Flag members ---
	int IsGeneric(void) { return ((syFlags & SYF_GENERIC) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object) { return(menu.Add(object)); }
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object) { return(menu.Subtract(object)); }
	UIW_SYSTEM_BUTTON &operator+(UI_WINDOW_OBJECT *object) { menu.Add(object); return(*this); }
	UIW_SYSTEM_BUTTON &operator-(UI_WINDOW_OBJECT *object) { menu.Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_SYSTEM_BUTTON::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_SYSTEM_BUTTON(name, file, object, _objectTable, _userTable)); }
	UIW_SYSTEM_BUTTON(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }
	void SetDecorations(const ZIL_ICHAR *decorationName)
		{ ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations); myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className, decorationName); }

protected:
	const ZIL_LANGUAGE *myLanguage;
	const ZIL_DECORATION *myDecorations;
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
private:
	UIW_POP_UP_MENU *systemMenu;
#endif

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_STRING ---------------------------------------------------------
// --------------------------------------------------------------------------

// --- STF_FLAGS ---
typedef UIF_FLAGS STF_FLAGS;
const STF_FLAGS STF_NO_FLAGS			= 0x0000;
const STF_FLAGS STF_VARIABLE_NAME		= 0x0001;
const STF_FLAGS STF_LOWER_CASE			= 0x0002;
const STF_FLAGS STF_UPPER_CASE			= 0x0004;
const STF_FLAGS STF_PASSWORD			= 0x0008;
const STF_FLAGS STF_SUBSTRING			= 0x8000;	// sub-string (used only for UIW_TEXT)

class ZIL_EXPORT_CLASS UIW_STRING : public UI_WINDOW_OBJECT
{
	friend class ZIL_EXPORT_CLASS UIW_TEXT;
	friend class ZIL_EXPORT_CLASS UIW_FORMATTED_STRING;
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	STF_FLAGS stFlags;
	int insertMode;

	// --- General class members ---
	UIW_STRING(int left, int top, int width, ZIL_ICHAR *text, int maxLength = -1,
		STF_FLAGS stFlags = STF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_STRING(void);
	ZIL_ICHAR *DataGet(void);
	void DataSet(ZIL_ICHAR *text, int maxLength = -1);

	// --- Flag members ---
	int HasVariableName(void) { return ((stFlags & STF_VARIABLE_NAME) ? TRUE : FALSE); }
	int IsLowerCase(void) { return ((stFlags & STF_LOWER_CASE) ? TRUE : FALSE); }
	int IsUpperCase(void) { return ((stFlags & STF_UPPER_CASE) ? TRUE : FALSE); }
	int IsPasswordStyle(void) { return ((stFlags & STF_PASSWORD) ? TRUE : FALSE); }
	int IsSubstringOfText(void) { return ((stFlags & STF_SUBSTRING) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_STRING::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_STRING(name, file, object, _objectTable, _userTable)); }
	UIW_STRING(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	int maxLength;
	ZIL_ICHAR *text;

	ZIL_ICHAR *ParseRange(ZIL_ICHAR *buffer, ZIL_ICHAR *minValue, ZIL_ICHAR *maxValue);

private:
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	int length;
	int leftClip;
	int lineWidth;
	UI_REGION active;
	int cursor, beginMark, endMark;
	int scrollCount;
	void StringSet(ZIL_ICHAR *string, ZIL_ICHAR replaceChar);
	ZIL_ICHAR CharacterConvert(ZIL_ICHAR key);
	int CursorUpdate(int newOffset, int scrollCount, int showCursor);
	UIW_STRING *NextString(void) { return ((UIW_STRING *)next); }
	UIW_STRING *PreviousString(void) { return ((UIW_STRING *)previous); }
	void StringUpdate(int insertCount, int deleteCount, int scrollCount);
#elif defined(ZIL_MACINTOSH)
	long selectPosition;

#	if defined(ZIL_UNICODE)
	int BytePosToCharPos(int bytePos);
	int CharPosToBytePos(int charPos);
#	endif
#elif defined(ZIL_XT)
	Pixel colorForeground, colorBackground;
#endif

private:
	void OSDataGet(void);
	void OSDataSet(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_DATE -----------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_DATE : public UIW_STRING
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	DTF_FLAGS dtFlags;
#if defined(ZIL_3x_COMPAT)
	static DTF_FLAGS rangeFlags;
#endif

	// --- General class members ---
	UIW_DATE(int left, int top, int width, ZIL_DATE *date,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		DTF_FLAGS dtFlags = DTF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_DATE(void);
	ZIL_DATE *DataGet(void);
	void DataSet(ZIL_DATE *date);

	// --- Flag members ---
	int DefaultsToSystem(void) { return ((dtFlags & DTF_SYSTEM) ? TRUE : FALSE); }
	int HasAlphaMonth(void) { return ((dtFlags & DTF_ALPHA_MONTH) ? TRUE : FALSE); }
	int HasDashSeparator(void) { return ((dtFlags & DTF_DASH) ? TRUE : FALSE); }
	int HasSlashSeparator(void) { return ((dtFlags & DTF_SLASH) ? TRUE : FALSE); }
	int HasZeroFill(void) { return ((dtFlags & DTF_ZERO_FILL) ? TRUE : FALSE); }
	int IsAsian(void) { return ((dtFlags & DTF_ASIAN_FORMAT) ? TRUE : FALSE); }
	int IsEuropean(void) { return ((dtFlags & DTF_EUROPEAN_FORMAT) ? TRUE : FALSE); }
	int IsMilitary(void) { return ((dtFlags & DTF_MILITARY_FORMAT) ? TRUE : FALSE); }
	int IsUS(void) { return ((dtFlags & DTF_US_FORMAT) ? TRUE : FALSE); }
	int IsUpperCase(void) { return ((dtFlags & DTF_UPPER_CASE) ? TRUE : FALSE); }
	int UsesDayOfWeekFormat(void) { return ((dtFlags & DTF_DAY_OF_WEEK) ? TRUE : FALSE); }
	int UsesShortDay(void) { return ((dtFlags & DTF_SHORT_DAY) ? TRUE : FALSE); }
	int UsesShortMonth(void) { return ((dtFlags & DTF_SHORT_MONTH) ? TRUE : FALSE); }
	int UsesShortYear(void) { return ((dtFlags & DTF_SHORT_YEAR) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual int Validate(int processError = TRUE);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_DATE::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_DATE(name, file, object, _objectTable, _userTable)); }
	UIW_DATE(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }

protected:
	ZIL_DATE *date;
	ZIL_ICHAR *range;
	const ZIL_LANGUAGE *myLanguage;

private:
	void OSDataGet(void);
	void OSDataSet(void);
	int OSEvent(const UI_EVENT &event, EVENT_TYPE *ccode);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
	void ValueDecrement(ZIL_INT32 delta);
	void ValueIncrement(ZIL_INT32 delta);
};

// --------------------------------------------------------------------------
// ----- UIW_FORMATTED_STRING -----------------------------------------------
// --------------------------------------------------------------------------

// --- FMI_RESULT ---
enum FMI_RESULT
{
	FMI_OK = 0,				// Formatted string successfully imported.
	FMI_INVALID_CHARACTERS	// Formatted string has invalid characters.
};

class ZIL_EXPORT_CLASS UIW_FORMATTED_STRING : public UIW_STRING
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_FORMATTED_STRING(int left, int top, int width, ZIL_ICHAR *compressedText,
		ZIL_ICHAR *editMask, ZIL_ICHAR *deleteText,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_FORMATTED_STRING(void);
	ZIL_ICHAR *DataGet(int compressedText = FALSE);
	void DataSet(ZIL_ICHAR *text);
	void Export(ZIL_ICHAR *destination, int expanded);
	FMI_RESULT Import(ZIL_ICHAR *source);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_FORMATTED_STRING::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_FORMATTED_STRING(name, file, object, _objectTable, _userTable)); }
	UIW_FORMATTED_STRING(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	ZIL_ICHAR *compressedText;
	ZIL_ICHAR *editMask;
	ZIL_ICHAR *deleteText;

private:
	void Expand(ZIL_ICHAR *destination);
#if defined(ZIL_MOTIF)
	int Reposition(EVENT_TYPE ccode, int distance = 0, int position = -1);
#else
	int Reposition(EVENT_TYPE ccode, int distance = 0);
#endif
	int Position(EVENT_TYPE ccode, int position);
	int LegalChar(ZIL_ICHAR &textValue, int pos);

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_BIGNUM ---------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_BIGNUM : public UIW_STRING
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	NMF_FLAGS nmFlags;
#if defined(ZIL_3x_COMPAT)
	static NMF_FLAGS rangeFlags;
#endif

	// --- General class members ---
	UIW_BIGNUM(int left, int top, int width, ZIL_BIGNUM *value,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_BIGNUM(void);
	ZIL_BIGNUM *DataGet(void);
	void DataSet(ZIL_BIGNUM *value);

	// --- Flag members ---
	int HasCreditSymbol(void) { return ((nmFlags & NMF_CREDIT) ? TRUE : FALSE); }
	int HasCurrencySymbol(void) { return ((nmFlags & NMF_CURRENCY) ? TRUE : FALSE); }
	int HasDecimalPoint(void) { return ((nmFlags & NMF_DECIMAL_FLAGS) ? TRUE : FALSE); }
	int HasPercentageSymbol(void) { return ((nmFlags & NMF_PERCENT) ? TRUE : FALSE); }
	int IsCommaDelimited(void) { return ((nmFlags & NMF_COMMAS) ? TRUE : FALSE); }
	int UsesScientificFormat(void) { return ((nmFlags & NMF_SCIENTIFIC) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (UIW_BIGNUM::_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual int Validate(int processError = TRUE);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_BIGNUM::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_BIGNUM(name, file, object, _objectTable, _userTable)); }
	UIW_BIGNUM(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }

protected:
	ZIL_BIGNUM *number;
	ZIL_ICHAR *range;
	const ZIL_LANGUAGE *myLanguage;

private:
	void OSDataGet(void);
	void OSDataSet(void);
	int OSEvent(const UI_EVENT &event, EVENT_TYPE *ccode);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
	void ValueDecrement(ZIL_INT32 delta);
	void ValueIncrement(ZIL_INT32 delta);
};

// --------------------------------------------------------------------------
// ----- UIW_INTEGER ---------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_INTEGER : public UIW_STRING
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	NMF_FLAGS nmFlags;

	// --- General class members ---
	UIW_INTEGER(int left, int top, int width, int *value,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_INTEGER(void);
	int DataGet(void);
	void DataSet(int *value);

	// --- Flag members ---
	int HasCreditSymbol(void) { return ((nmFlags & NMF_CREDIT) ? TRUE : FALSE); }
	int HasCurrencySymbol(void) { return ((nmFlags & NMF_CURRENCY) ? TRUE : FALSE); }
	int HasDecimalPoint(void) { return ((nmFlags & NMF_DECIMAL_FLAGS) ? TRUE : FALSE); }
	int HasPercentageSymbol(void) { return ((nmFlags & NMF_PERCENT) ? TRUE : FALSE); }
	int IsCommaDelimited(void) { return ((nmFlags & NMF_COMMAS) ? TRUE : FALSE); }
	int IsScientific(void) { return ((nmFlags & NMF_SCIENTIFIC) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual int Validate(int processError = TRUE);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_INTEGER::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_INTEGER(name, file, object, _objectTable, _userTable)); }
	UIW_INTEGER(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }

protected:
	int *number;
	ZIL_ICHAR *range;
	const ZIL_LANGUAGE *myLanguage;

private:
	void OSDataGet(void);
	void OSDataSet(void);
	int OSEvent(const UI_EVENT &event, EVENT_TYPE *ccode);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
	void ValueDecrement(int delta);
	void ValueIncrement(int delta);
};

// --------------------------------------------------------------------------
// ----- UIW_REAL ---------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_REAL : public UIW_STRING
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	NMF_FLAGS nmFlags;

	// --- General class members ---
	UIW_REAL(int left, int top, int width, double *value,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		NMF_FLAGS nmFlags = NMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_REAL(void);
	double DataGet(void);
	void DataSet(double *value);

	// --- Flag members ---
	int HasCreditSymbol(void) { return ((nmFlags & NMF_CREDIT) ? TRUE : FALSE); }
	int HasCurrencySymbol(void) { return ((nmFlags & NMF_CURRENCY) ? TRUE : FALSE); }
	int HasDecimalPoint(void) { return ((nmFlags & NMF_DECIMAL_FLAGS) ? TRUE : FALSE); }
	int HasPercentageSymbol(void) { return ((nmFlags & NMF_PERCENT) ? TRUE : FALSE); }
	int IsCommaDelimited(void) { return ((nmFlags & NMF_COMMAS) ? TRUE : FALSE); }
	int IsScientific(void) { return ((nmFlags & NMF_SCIENTIFIC) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual int Validate(int processError = TRUE);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_REAL::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_REAL(name, file, object, _objectTable, _userTable)); }
	UIW_REAL(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }

protected:
	double *number;
	ZIL_ICHAR *range;
	const ZIL_LANGUAGE *myLanguage;
	static void Format(ZIL_ICHAR *text, double number, NMF_FLAGS flags);

private:
	void OSDataGet(void);
	void OSDataSet(void);
	int OSEvent(const UI_EVENT &event, EVENT_TYPE *ccode);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
	void ValueDecrement(ZIL_INT32 delta);
	void ValueIncrement(ZIL_INT32 delta);
};

// --------------------------------------------------------------------------
// ----- UIW_TIME -----------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_TIME : public UIW_STRING
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	TMF_FLAGS tmFlags;
#if defined(ZIL_3x_COMPAT)
	static TMF_FLAGS rangeFlags;
#endif

	// --- General class members ---
	UIW_TIME(int left, int top, int width, ZIL_TIME *time,
		const ZIL_ICHAR *range = ZIL_NULLP(ZIL_ICHAR),
		TMF_FLAGS tmFlags = TMF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_TIME(void);
	ZIL_TIME *DataGet(void);
	void DataSet(ZIL_TIME *time);

	// --- Flag members ---
	int DefaultsToSystem(void) { return ((tmFlags & TMF_SYSTEM) ? TRUE : FALSE); }
	int HasColonSeparator(void) { return ((tmFlags & TMF_COLON_SEPARATOR) ? TRUE : FALSE); }
	int HasHundredths(void) { return ((tmFlags & TMF_HUNDREDTHS) ? TRUE : FALSE); }
	int HasHours(void) { return ((tmFlags & TMF_NO_HOURS) ? FALSE : TRUE); }
	int HasMinutes(void) { return ((tmFlags & TMF_NO_MINUTES) ? FALSE : TRUE); }
	int HasNullSeparator(void) { return ((tmFlags & TMF_NO_SEPARATOR) ? TRUE : FALSE); }
	int HasSeconds(void) { return ((tmFlags & TMF_SECONDS) ? TRUE : FALSE); }
	int HasZeroFill(void) { return ((tmFlags & TMF_ZERO_FILL) ? TRUE : FALSE); }
	int IsLowerCase(void) { return ((tmFlags & TMF_LOWER_CASE) ? TRUE : FALSE); }
	int IsUpperCase(void) { return ((tmFlags & TMF_UPPER_CASE) ? TRUE : FALSE); }
	int Is12Hour(void) { return ((tmFlags & TMF_TWELVE_HOUR) ? TRUE : FALSE); }
	int Is24Hour(void) { return ((tmFlags & TMF_TWENTY_FOUR_HOUR) ? TRUE : FALSE); }

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual int Validate(int processError = TRUE);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_TIME::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_TIME(name, file, object, _objectTable, _userTable)); }
	UIW_TIME(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }

protected:
	ZIL_TIME *time;
	ZIL_ICHAR *range;
	const ZIL_LANGUAGE *myLanguage;

private:
	void OSDataGet(void);
	void OSDataSet(void);
	int OSEvent(const UI_EVENT &event, EVENT_TYPE *ccode);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
	void ValueDecrement(ZIL_INT32 delta);
	void ValueIncrement(ZIL_INT32 delta);
};

// --------------------------------------------------------------------------
// ----- UIW_TEXT -----------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_TEXT : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	int insertMode;

	// --- General class members ---
	UIW_TEXT(int left, int top, int width, int height, ZIL_ICHAR *text,
		int maxLength = -1, WNF_FLAGS wnFlags = WNF_NO_WRAP,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_AUTO_CLEAR,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_TEXT(void);
	ZIL_ICHAR *DataGet(void);
	void DataSet(ZIL_ICHAR *text, int maxLength = -1);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_TEXT::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_TEXT(name, file, object, _objectTable, _userTable)); }
	UIW_TEXT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	int maxLength;
	ZIL_ICHAR *text;

	int CursorOffset(int offset = -1);
	void GetCursorPos(UI_POSITION *position);
	void SetCursorPos(const UI_POSITION &position);

private:
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	// Edit buffer data.
	ZIL_ICHAR *editBuffer;
	int editSize;
	int offsetCursor;
	int offsetMarkAnchor;
	int offsetMarkFloat;
	int offsetSave;
	int offsetSplit;
	int offsetTopLine;
	int splitSize;
	int textLength;

	// Cursor Data.
	UI_POSITION cursorPos;
	int lastCursorColumn;

	// Other Data.
	UI_REGION client;
	int lineHeight;
	int lines;
	int marginSize;
	int width;

	int GetTextWidth(ZIL_ICHAR *string);
	int CharWidth(int offset);
	ZIL_ICHAR Delete(int *offset);
	void Draw(int startOffset, int endOffset = 0, int erase = FALSE,
		int markOnly = FALSE);
	void Insert(int *offset, ZIL_ICHAR value, ZIL_ICHAR code = 0);
	int EditOffset(int textOffset);
	void FormatText(void);
	void GetPosition(int offset, UI_POSITION *position);
	int LineWidth(int offset, int wrap = FALSE);
	int Move(int *offset, int moveCode);
	void SetCursor(int offset);
	void SetCursor(const UI_POSITION &position);
	void SetMark(int offset, int repaint = TRUE);
	void SetSplit(int *offset);
	int TextOffset(int editOffset);
	int UnformatText(ZIL_ICHAR *string, int startOffset = 0, int endOffset = -1);
	void UpdateCursor(void);
	void UpdateScroll(int redisplay = FALSE, int scrollCursor = TRUE);
	void Wrap(int offset, int testPrevLine = FALSE,
		int draw = TRUE, int drawToEnd = FALSE);

#elif defined(ZIL_MSWINDOWS)
	GLOBALHANDLE  ghEditDS;

#elif defined(ZIL_MACINTOSH)
	long selectPosition;

	void CalculateHorizontalScroll(UI_SCROLL_INFORMATION *hScroll);
	void CalculateVerticalScroll(UI_SCROLL_INFORMATION *vScroll);

#	if defined(ZIL_UNICODE)
	int BytePosToCharPos(int bytePos);
	int CharPosToBytePos(int charPos);
#	endif
#elif defined(ZIL_NEXTSTEP)
	id theTextObject;
#endif

private:
	void OSDataGet(void);
	void OSDataSet(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_GROUP ----------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_GROUP : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
#if defined(ZIL_MOTIF)
	Widget labelWidget;
#endif

	// --- General class members ---
	UIW_GROUP(int left, int top, int width, int height, ZIL_ICHAR *text,
		WNF_FLAGS wnFlags = WNF_AUTO_SELECT, WOF_FLAGS woFlags = WOF_NO_FLAGS);
	virtual ~UIW_GROUP(void);
	ZIL_ICHAR *DataGet(void);
	void DataSet(ZIL_ICHAR *text);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
#if defined(ZIL_MOTIF) || defined(ZIL_NEXTSTEP) || defined(ZIL_OS2)
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
#endif

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_GROUP::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_GROUP(name, file, object, _objectTable, _userTable)); }
	UIW_GROUP(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	ZIL_ICHAR *text;

private:
	void OSDataSet(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_VT_LIST --------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_VT_LIST : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS UIW_COMBO_BOX;
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_VT_LIST(int left, int top, int width, int height,
		ZIL_COMPARE_FUNCTION compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION),
		WNF_FLAGS wnFlags = WNF_NO_WRAP | WNF_CONTINUE_SELECT, WOF_FLAGS woFlags = WOF_BORDER,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_VT_LIST(int left, int top, int width, int height,
		ZIL_COMPARE_FUNCTION compareFunction, WOF_FLAGS flagSetting,
		UI_ITEM *item);
	virtual ~UIW_VT_LIST(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
#if defined(ZIL_MOTIF)
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
	virtual ZIL_SCREENID TopWidget(void);
#endif

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	virtual void Destroy(void);
	virtual void Sort(void);
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_VT_LIST &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_VT_LIST &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_VT_LIST::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_VT_LIST(name, file, object, _objectTable, _userTable)); }
	UIW_VT_LIST(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
public:
	virtual EVENT_TYPE ScrollEvent(UI_EVENT &event);
private:
	int cellHeight;
#elif defined (ZIL_OS2)
	virtual EVENT_TYPE ScrollEvent(UI_EVENT &event);
private:
	int cellHeight;
	UI_WINDOW_OBJECT *topObject;
#elif defined (ZIL_MOTIF)
private:
	ZIL_SCREENID comboShell;
#endif

private:
	void OSDestroy(void);
	void OSSort(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_HZ_LIST ---------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_HZ_LIST : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_HZ_LIST(int left, int top, int width, int height,
		int cellWidth, int cellHeight,
		ZIL_COMPARE_FUNCTION compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION),
		WNF_FLAGS wnFlags = WNF_NO_WRAP | WNF_CONTINUE_SELECT, WOF_FLAGS woFlags = WOF_BORDER,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_HZ_LIST(int left, int top, int width, int height,
		ZIL_COMPARE_FUNCTION compareFunction, WOF_FLAGS flagSetting,
		UI_ITEM *item);
	virtual ~UIW_HZ_LIST(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	virtual void Destroy(void);
	virtual void Sort(void);
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_HZ_LIST &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_HZ_LIST &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_HZ_LIST::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_HZ_LIST(name, file, object, _objectTable, _userTable)); }
	UIW_HZ_LIST(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	int cellWidth;
	int cellHeight;

#if defined (ZIL_MSDOS) || defined(ZIL_CURSES)
public:
	virtual EVENT_TYPE ScrollEvent(UI_EVENT &event);
private:
	int rows, columns;
#elif defined (ZIL_OS2)
	virtual EVENT_TYPE ScrollEvent(UI_EVENT &event);
private:
	UI_WINDOW_OBJECT *topObject;
#elif defined(ZIL_NEXTSTEP)
private:
	id myMatrix;
#endif

private:
	void OSDestroy(void);
	void OSSort(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_COMBO_BOX ------------------------------------------------------
// --------------------------------------------------------------------------

// --- UIW_COMBO_BOX::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_RESET_SELECTION		= 1500;
const ZIL_INFO_REQUEST I_MATCH_INPUT			= 1501;

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
const ZIL_NUMBERID ZIL_ComboDownArrow = 1;
#endif

class ZIL_EXPORT_CLASS UIW_COMBO_BOX : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	UIW_VT_LIST list;

	// --- General class members ---
	UIW_COMBO_BOX(int left, int top, int width, int height,
		ZIL_COMPARE_FUNCTION compareFunction = ZIL_NULLF(ZIL_COMPARE_FUNCTION),
		WNF_FLAGS wnFlags = WNF_NO_WRAP, WOF_FLAGS woFlags = WOF_NO_FLAGS,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	UIW_COMBO_BOX(int left, int top, int width, int height,
		ZIL_COMPARE_FUNCTION compareFunction, WOF_FLAGS flagSetting,
		UI_ITEM *item);
	virtual ~UIW_COMBO_BOX(void);
	ZIL_ICHAR *DataGet(void);
	void DataSet(ZIL_ICHAR *text);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
#if defined(ZIL_OS2)
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
#endif
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	int Count(void) { return (list.Count()); };
	UI_WINDOW_OBJECT *Current(void) { return ((UI_WINDOW_OBJECT *)list.Current()); }
	virtual void Destroy(void);
	UI_WINDOW_OBJECT *First(void) { return ((UI_WINDOW_OBJECT *)list.First()); }
	UI_WINDOW_OBJECT *Get(int index) { return ((UI_WINDOW_OBJECT *)list.UI_LIST::Get(index)); }
	int Index(UI_WINDOW_OBJECT const *element) { return (list.Index(element)); }
	UI_WINDOW_OBJECT *Last(void) { return((UI_WINDOW_OBJECT *)list.Last()); }
	virtual void Sort(void);
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_COMBO_BOX &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_COMBO_BOX &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_COMBO_BOX::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_COMBO_BOX(name, file, object, _objectTable, _userTable)); }
	UIW_COMBO_BOX(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

private:
#	if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	// Private DOS class definitions for combo support objects.
	class UIW_COMBO_BUTTON : public UIW_BUTTON
	{
	public:
		static int defaultInitialized;

		UIW_COMBO_BUTTON(void);
		~UIW_COMBO_BUTTON(void);
		virtual EVENT_TYPE Event(const UI_EVENT &event);

		void SetDecorations(const ZIL_ICHAR *decorationName)
			{ ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations); myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className, decorationName); }
	protected:
		const ZIL_DECORATION *myDecorations;
	};

	class UIW_COMBO_STRING : public UIW_STRING
	{
	public:
		UIW_COMBO_STRING(void);
		virtual EVENT_TYPE Event(const UI_EVENT &event);
	};
#	endif

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	UIW_STRING *string;
	UIW_BUTTON *button;
#elif defined(ZIL_MOTIF)
	Widget arrow, listShell;
	static void PopupCallback(Widget, XtPointer, XtPointer);
	static Cursor popupCursor;
#elif defined(ZIL_MACINTOSH)
	UIW_STRING *string;
#elif defined(ZIL_NEXTSTEP)
	id pulTrigger;
#endif
	ZIL_ICHAR *text;

private:
	void OSDataGet(void);
	void OSDataSet(void);
	void OSDestroy(void);
	void OSResetSelection(UI_WINDOW_OBJECT *object);
	void OSMatchInput(void);
	void OSSort(void);
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_SPIN_CONTROL ---------------------------------------------------
// --------------------------------------------------------------------------

const ZIL_NUMBERID ZIL_SpinUpArrow = 1;
const ZIL_NUMBERID ZIL_SpinDownArrow = 2;
const ZIL_NUMBERID ZIL_SpinUpArrowDisabled = 3;
const ZIL_NUMBERID ZIL_SpinDownArrowDisabled = 4;

class ZIL_EXPORT_CLASS UIW_SPIN_CONTROL : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_SPIN_CONTROL(int left, int top, int width, UI_WINDOW_OBJECT *fieldObject,
		ZIL_INT32 _delta = 1, WNF_FLAGS wnFlags = WNF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_NO_FLAGS,
		ZIL_USER_FUNCTION _userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_SPIN_CONTROL(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
#if defined(ZIL_MOTIF) || defined(ZIL_NEXTSTEP)
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
#endif

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_SPIN_CONTROL::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_SPIN_CONTROL(name, file, object, _objectTable, _userTable)); }
	UIW_SPIN_CONTROL(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name,
		ZIL_STORAGE *file = ZIL_NULLP(ZIL_STORAGE),
		ZIL_STORAGE_OBJECT *object = ZIL_NULLP(ZIL_STORAGE_OBJECT),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
#endif

	// --- Language and locale members ---
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_XT) || defined(ZIL_NEXTSTEP)
	void SetDecorations(const ZIL_ICHAR *decorationName)
		{ upButton->SetDecorations(decorationName); downButton->SetDecorations(decorationName); }
#elif defined(ZIL_MSWINDOWS) && !defined(ZIL_WIN32)
	void SetDecorations(const ZIL_ICHAR *decorationName)
		{ spinButton->SetDecorations(decorationName); }
#endif

private:
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_XT) || (defined(ZIL_MSWINDOWS) && !defined(ZIL_WIN32)) || defined(ZIL_NEXTSTEP)
	class ZIL_EXPORT_CLASS UIW_SPIN_BUTTON : public UIW_BUTTON
	{
		friend class ZIL_EXPORT_CLASS UIW_SPIN_CONTROL;
	public:
		static int defaultInitialized;
#	if defined(ZIL_NEXTSTEP)
		~UIW_SPIN_BUTTON(){ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations);};
#	else
		~UIW_SPIN_BUTTON();
#	endif
		virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
		virtual EVENT_TYPE Event(const UI_EVENT &event);
		virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	
		const ZIL_DECORATION *myDecorations;
		void SetDecorations(const ZIL_ICHAR *decorationName)
			{ ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations); myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className, decorationName); }
	private:
		EVENT_TYPE direction;
#	if defined(ZIL_MSWINDOWS)
		UIW_SPIN_BUTTON();
#	else
		UIW_SPIN_BUTTON(EVENT_TYPE direction);
#	endif
	};
#endif

	UI_WINDOW_OBJECT *fieldObject;
	ZIL_INT32 delta;
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES) || defined(ZIL_XT) || defined(ZIL_NEXTSTEP)
	UIW_SPIN_BUTTON *upButton;
	UIW_SPIN_BUTTON *downButton;
#elif defined(ZIL_MSWINDOWS)
#	if defined(ZIL_WIN32)
	HWND upDownControl;
#	else
	UIW_SPIN_BUTTON *spinButton;
#	endif
#elif defined(ZIL_MACINTOSH)
	ControlHandle littleArrows;
#endif
	ZIL_ICHAR *text;
};

// --------------------------------------------------------------------------
// ----- UIW_SCROLL_BAR -----------------------------------------------------
// --------------------------------------------------------------------------

// --- sbFlags ---
typedef UIF_FLAGS SBF_FLAGS;
const SBF_FLAGS SBF_NO_FLAGS			= 0x0000;
const SBF_FLAGS SBF_CORNER				= 0x0001;
const SBF_FLAGS SBF_VERTICAL			= 0x0002;
const SBF_FLAGS SBF_HORIZONTAL			= 0x0004;
const SBF_FLAGS SBF_SLIDER				= 0x0008;
const SBF_FLAGS SBF_SCALE				= 0x0008;

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
// Private DOS class definitions for scroll support objects.
typedef UIF_FLAGS BUTTON_TYPE;
const BUTTON_TYPE VTOP_BUTTON			= 0x0001;
const BUTTON_TYPE VMIDDLE_BUTTON		= 0x0002;
const BUTTON_TYPE VBOTTOM_BUTTON		= 0x0004;
const BUTTON_TYPE HLEFT_BUTTON			= 0x0010;
const BUTTON_TYPE HMIDDLE_BUTTON		= 0x0020;
const BUTTON_TYPE HRIGHT_BUTTON			= 0x0040;

const BUTTON_TYPE VBUTTON				= 0x000F;
const BUTTON_TYPE HBUTTON				= 0x00F0;
const BUTTON_TYPE LTBUTTON				= 0x0011;
const BUTTON_TYPE MBUTTON				= 0x0022;
const BUTTON_TYPE RBBUTTON				= 0x0044;
const ZIL_NUMBERID ZIL_ScrollUpArrow = VTOP_BUTTON;
const ZIL_NUMBERID ZIL_ScrollDownArrow = VBOTTOM_BUTTON;
const ZIL_NUMBERID ZIL_ScrollLeftArrow = HLEFT_BUTTON;
const ZIL_NUMBERID ZIL_ScrollRightArrow = HRIGHT_BUTTON;
const ZIL_NUMBERID ZIL_ScrollMiddleArrow = HMIDDLE_BUTTON;
#endif

class ZIL_EXPORT_CLASS UIW_SCROLL_BAR : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	SBF_FLAGS sbFlags;

	// --- General class members ---
	UIW_SCROLL_BAR(int left, int top, int width, int height,
		SBF_FLAGS sbFlags = SBF_VERTICAL,
		WOF_FLAGS woFlags = WOF_BORDER | WOF_SUPPORT_OBJECT | WOF_NON_FIELD_REGION);
	UIW_SCROLL_BAR(int left, int top, int width, int height,
		UI_SCROLL_INFORMATION *scroll, SBF_FLAGS sbFlags = SBF_VERTICAL,
		WOF_FLAGS woFlags = WOF_BORDER, WOAF_FLAGS woAdvancedFlags = WOAF_NON_CURRENT,
		ZIL_USER_FUNCTION userFunction = ZIL_NULLF(ZIL_USER_FUNCTION));
	virtual ~UIW_SCROLL_BAR(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_SCROLL_BAR::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_SCROLL_BAR(name, file, object, _objectTable, _userTable)); }
	UIW_SCROLL_BAR(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	UI_SCROLL_INFORMATION scroll;

private:
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	class UIW_SCROLL_BUTTON : public UIW_BUTTON
	{
		friend class ZIL_EXPORT_CLASS UIW_SCROLL_BAR;
	public:
		static int defaultInitialized;
		BUTTON_TYPE btType;

		UIW_SCROLL_BUTTON(BUTTON_TYPE btType);
		~UIW_SCROLL_BUTTON(void);
		virtual EVENT_TYPE Event(const UI_EVENT &event);

		void SetDecorations(const ZIL_ICHAR *decorationName)
			{ ZIL_DECORATION_MANAGER::FreeDecorations(myDecorations); myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className, decorationName); }
	protected:
		const ZIL_DECORATION *myDecorations;
	};

	UIW_SCROLL_BUTTON *lButton, *mButton, *rButton;
#elif defined(ZIL_MOTIF)
	static void ScrollCallback(Widget widget, XtPointer client, XtPointer call);
#endif

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_TOOL_BAR -------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_TOOL_BAR : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_TOOL_BAR(int left, int top, int width, int height,
		WNF_FLAGS wnFlags = WNF_NO_FLAGS, WOF_FLAGS woFlags = WOF_BORDER | WOF_NON_FIELD_REGION | WOF_SUPPORT_OBJECT,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS);
	virtual ~UIW_TOOL_BAR(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_TOOL_BAR::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_TOOL_BAR(name, file, object, _objectTable, _userTable)); }
	UIW_TOOL_BAR(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_STATUS_BAR -------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UIW_STATUS_BAR : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_STATUS_BAR(int _height = 1, WOF_FLAGS _woFlags = WOF_BORDER | WOF_SUPPORT_OBJECT);
	virtual ~UIW_STATUS_BAR(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
#if defined(ZIL_OS2) || defined(ZIL_WIN32)
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
#endif

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_STATUS_BAR &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_STATUS_BAR &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); };

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_STATUS_BAR::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_STATUS_BAR(name, file, object, _objectTable, _userTable)); }
	UIW_STATUS_BAR(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	int height;

private:
#if defined(ZIL_WIN32)
	HWND sizeGrip;
#endif
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UIW_NOTEBOOK -------------------------------------------------------
// --------------------------------------------------------------------------

// --- UIW_NOTEBOOK::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_SET_SEARCH_PAGE		= 1600;

class ZIL_EXPORT_CLASS UIW_NOTEBOOK : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static int borderWidth;
	static int shadowWidth;
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_NOTEBOOK(void);
	~UIW_NOTEBOOK(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	virtual UI_WINDOW_OBJECT *Add(UI_WINDOW_OBJECT *object);
	virtual UI_WINDOW_OBJECT *Subtract(UI_WINDOW_OBJECT *object);
	UIW_NOTEBOOK &operator+(UI_WINDOW_OBJECT *object) { Add(object); return(*this); }
	UIW_NOTEBOOK &operator-(UI_WINDOW_OBJECT *object) { Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_NOTEBOOK::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_NOTEBOOK(name, file, object, _objectTable, _userTable)); }
	UIW_NOTEBOOK(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

private:
	int tabWidth, tabHeight;
	int totalPages, currentPage, searchPage;
#if defined(ZIL_NEXTSTEP)
	id pageView;
#endif
};

// --------------------------------------------------------------------------
// ----- UIW_TABLE ----------------------------------------------------------
// --------------------------------------------------------------------------

// --- UIW_TABLE::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_SET_TABLE_RECORD	= 1700;
const ZIL_INFO_REQUEST I_SET_VIRTUAL_RECORD	= 1701;
const ZIL_INFO_REQUEST I_SET_COL_HEADER		= 1702;
const ZIL_INFO_REQUEST I_SET_ROW_HEADER		= 1703;
const ZIL_INFO_REQUEST I_GET_COLUMNS		= 1704;
const ZIL_INFO_REQUEST I_GET_RECORDS		= 1705;
const ZIL_INFO_REQUEST I_GET_WIDTH			= 1706;
const ZIL_INFO_REQUEST I_GET_HEIGHT			= 1707;
const ZIL_INFO_REQUEST I_GET_RECORD_WIDTH	= 1708;
const ZIL_INFO_REQUEST I_GET_RECORD_HEIGHT	= 1709;
const ZIL_INFO_REQUEST I_GET_CORNER_WIDTH	= 1710;
const ZIL_INFO_REQUEST I_GET_CORNER_HEIGHT	= 1711;

// --- tblFlags ---  (Table Flags)
typedef UIF_FLAGS TBLF_FLAGS;
const TBLF_FLAGS TBLF_NO_FLAGS			= 0x0000;
const TBLF_FLAGS TBLF_GRID				= 0x0001;

// --- thFlags ---  (Table Header Flags)
typedef UIF_FLAGS THF_FLAGS;
const THF_FLAGS THF_NO_FLAGS			= 0x0000;
const THF_FLAGS THF_GRID				= 0x0001;
const THF_FLAGS THF_CORNER_HEADER		= 0x0002;
const THF_FLAGS THF_COLUMN_HEADER		= 0x0004;
const THF_FLAGS THF_ROW_HEADER			= 0x0008;

class ZIL_EXPORT_CLASS UIW_TABLE_RECORD : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS UIW_TABLE;
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UIW_TABLE_RECORD(int width, int height,	ZIL_USER_FUNCTION userFunction =
		ZIL_NULLF(ZIL_USER_FUNCTION), WOF_FLAGS woFlags = WOF_NO_FLAGS);
	virtual ~UIW_TABLE_RECORD(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void RegionMax(UI_WINDOW_OBJECT *object);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_TABLE_RECORD::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_TABLE_RECORD(name, file, object, _objectTable, _userTable)); }
	UIW_TABLE_RECORD(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	void *data;
	int recordNum;
	int editMode;
	int virtualRecord;

	UIW_TABLE_RECORD *VirtualRecord(void);

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

class ZIL_EXPORT_CLASS UIW_TABLE : public UIW_WINDOW
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	TBLF_FLAGS tblFlags;

	// --- General class members ---
	UIW_TABLE(int left, int top, int width, int height, int columns = 1,
		int recordSize = 0, int maxRecords = -1, void *data = ZIL_NULLP(void),
		int records = 0, TBLF_FLAGS tblFlags = TBLF_NO_FLAGS,
		WOF_FLAGS woFlags = WOF_BORDER);
	virtual ~UIW_TABLE(void);
	int DataSet(void *data, int records = 0, int maxRecords = -1);
	void *DataGet(int *records = ZIL_NULLP(int));
	int CurrentRecord(void) { return (currentRecord); }
	void InsertRecord(int recordNum, void *data = ZIL_NULLP(void));
	void DeleteRecord(int recordNum);
	void *GetRecord(int recordNum) { return (((ZIL_INT8 *)data) + recordNum * recordSize); }
	void SetCurrent(int recordNum);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void RegionMax(UI_WINDOW_OBJECT *object);
	virtual EVENT_TYPE ScrollEvent(UI_EVENT &event);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_TABLE::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_TABLE(name, file, object, _objectTable, _userTable)); }
	UIW_TABLE(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	UI_WINDOW_OBJECT *columnHeader;
	UI_WINDOW_OBJECT *rowHeader;
	UI_WINDOW_OBJECT *tableRecord;
	UI_WINDOW_OBJECT *virtualRecord;
	void *data;
	int columns;
	int currentRecord;
	int maxRecords;
	int records;
	int recordSize;
	int topRecord;

	void DrawRecord(int recordNum);

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

class ZIL_EXPORT_CLASS UIW_TABLE_HEADER : public UIW_TABLE
{
	friend class ZIL_EXPORT_CLASS ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];
	THF_FLAGS thFlags;

	// --- General class members ---
	UIW_TABLE_HEADER(THF_FLAGS thFlags, int recordSize = 0,
		int maxRecords = -1, void *data = ZIL_NULLP(void),
		WOF_FLAGS woFlags = WOF_BORDER);
	virtual ~UIW_TABLE_HEADER(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UIW_TABLE_HEADER::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UIW_TABLE_HEADER(name, file, object, _objectTable, _userTable)); }
	UIW_TABLE_HEADER(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

private:
	void OSUpdateSettings(ZIL_OBJECTID objectID);
	void UpdateSettings(ZIL_OBJECTID objectID);
};

// --------------------------------------------------------------------------
// ----- UI_ERROR_SYSTEM ----------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UI_ERROR_STUB
{
public:
	// --- General class members ---
	virtual ~UI_ERROR_STUB(void);
	static void Beep(void);
	UIS_STATUS ReportError(UI_WINDOW_MANAGER *windowManager,
		UIS_STATUS errorStatus, ZIL_ICHAR *format, ...);
	UIS_STATUS ReportError(UI_WINDOW_MANAGER *windowManager,
		ZIL_ICHAR *titleMessage, UIS_STATUS errorStatus, ZIL_ICHAR *format, ...);
	virtual UIS_STATUS ErrorMessage(UI_WINDOW_MANAGER *windowManager,
		UIS_STATUS errorStatus, ZIL_ICHAR *message,
		ZIL_ICHAR *titleMessage = ZIL_NULLP(ZIL_ICHAR)) = 0;
};

class ZIL_EXPORT_CLASS UI_ERROR_SYSTEM : public UI_ERROR_STUB
{
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;

	// --- General class members ---
	UI_ERROR_SYSTEM(void);
	virtual ~UI_ERROR_SYSTEM(void);

	// --- Error stub members ---
	virtual UIS_STATUS ErrorMessage(UI_WINDOW_MANAGER *windowManager,
		UIS_STATUS errorStatus, ZIL_ICHAR *message, ZIL_ICHAR *titleMessage = ZIL_NULLP(ZIL_ICHAR));

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }

protected:
	const ZIL_LANGUAGE *myLanguage;
};

// --------------------------------------------------------------------------
// ----- UI_HELP_SYSTEM -----------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UI_HELP_STUB
{
public:
	// --- General class members ---
	UI_HELP_STUB(void);
	virtual ~UI_HELP_STUB(void);
	virtual void DisplayHelp(UI_WINDOW_MANAGER *windowManager, UI_HELP_CONTEXT helpContext);
	virtual void ResetStorage(ZIL_STORAGE_READ_ONLY *_helpFile, int deleteNewStorage);

protected:
	int deleteHelpFile;
	ZIL_STORAGE_READ_ONLY *helpFile;
};

class ZIL_EXPORT_CLASS UI_HELP_SYSTEM : public UIW_WINDOW, public UI_HELP_STUB
{
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;

	// --- General class members ---
	UI_HELP_SYSTEM(const ZIL_ICHAR *pathName = ZIL_NULLP(ZIL_ICHAR),
		UI_WINDOW_MANAGER *windowManager = ZIL_NULLP(UI_WINDOW_MANAGER),
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT);
	virtual ~UI_HELP_SYSTEM(void);

	// --- Help stub members ---
	virtual void DisplayHelp(UI_WINDOW_MANAGER *windowManager, UI_HELP_CONTEXT helpContext);
	virtual void DisplayHelp(UI_WINDOW_MANAGER *windowManager, ZIL_ICHAR *helpTitle);
	virtual void ResetStorage(ZIL_STORAGE_READ_ONLY *_helpFile, int deleteNewStorage);

	// --- Window object members ---
	virtual EVENT_TYPE Event(const UI_EVENT &event);

	// --- Language and locale members ---
	void SetLanguage(const ZIL_ICHAR *languageName)
		{ ZIL_LANGUAGE_MANAGER::FreeLanguage(myLanguage); myLanguage = ZIL_LANGUAGE_MANAGER::UseLanguage(_className, languageName); }

protected:
	// support fields
	const ZIL_LANGUAGE *myLanguage;
private:
	enum
	{
		OPT_SHOW_TOPIC			= -3004,
		OPT_SHOW_INDEX 		= -3003,
		OPT_CLOSE 				= -3002,
		OPT_UPDATE_NAME		= -3001,
		OPT_SELECT_TOPIC		= -3000
	};
	// general fields
	ZIL_ICHAR *context;
	UIW_BORDER *borderField;
	UIW_TITLE *titleField;
	UIW_BUTTON *showButton;
	UIW_BUTTON *closeButton;
	// index fields
	UIW_PROMPT *indexTopic;
	UIW_PROMPT *indexInstruction;
	UIW_STRING *indexTitle;
	UIW_VT_LIST *indexContext;
	// topic fields
	UIW_PROMPT *topicTitle;
	UIW_TEXT *topicContext;

	void AddIndexFields(void);
	void AddTopicFields(ZIL_ICHAR *title = ZIL_INTERNATIONAL::_blankString, ZIL_ICHAR *context = ZIL_INTERNATIONAL::_blankString);
	void ComputeGeometry(void);
	static EVENT_TYPE ItemCallback(UI_WINDOW_OBJECT *object, UI_EVENT &event, EVENT_TYPE ccode);
	void RemoveIndexFields(void);
	void RemoveTopicFields(void);
};

// --------------------------------------------------------------------------
// ----- UI_APPLICATION -----------------------------------------------------
// --------------------------------------------------------------------------

#if !defined(NO_ZIL_APP)
class ZIL_EXPORT_CLASS UI_APPLICATION : public ZIL_INTERNATIONAL
{
public:
	UI_DISPLAY *display;
	UI_EVENT_MANAGER *eventManager;
	UI_WINDOW_MANAGER *windowManager;
	UI_PATH *searchPath;
#if defined(ZIL_NEXTSTEP)
	DPSTimedEntry myTimedEvent;
#endif
#if defined(ZIL_MSWINDOWS)
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpszCmdLine;
	int nCmdShow;
	UI_APPLICATION(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow);
#else
	UI_APPLICATION(int argc, char **argv);
#endif	
	int argc;
	ZIL_ICHAR **argv;

	~UI_APPLICATION(void);
	int Main(void);
	EVENT_TYPE Control(void);
	void LinkMain(void);

public:
	static ZIL_LANGUAGE_ELEMENT *_textSwitches;
	static ZIL_LANGUAGE_ELEMENT *_graphicSwitches;
};
#endif

// --------------------------------------------------------------------------
// ----- ZAF_DIALOG_WINDOW --------------------------------------------------
// --------------------------------------------------------------------------

// --- ZIL_DIALOG_EVENT ---
const ZIL_DIALOG_EVENT ZIL_DLG_OK				= -2999;
const ZIL_DIALOG_EVENT ZIL_DLG_YES				= -2998;
const ZIL_DIALOG_EVENT ZIL_DLG_NO				= -2997;
const ZIL_DIALOG_EVENT ZIL_DLG_ABORT			= -2996;
const ZIL_DIALOG_EVENT ZIL_DLG_RETRY			= -2995;
const ZIL_DIALOG_EVENT ZIL_DLG_IGNORE			= -2994;
const ZIL_DIALOG_EVENT ZIL_DLG_CANCEL			= -2993;

class ZIL_EXPORT_CLASS ZAF_DIALOG_WINDOW : public UIW_WINDOW
{
public:
	// --- General class members ---
	ZAF_DIALOG_WINDOW(int left, int top, int width, int height,
		WOF_FLAGS woFlags = WOF_NO_FLAGS,
		WOAF_FLAGS woAdvancedFlags = WOAF_NO_FLAGS,
		UI_HELP_CONTEXT helpContext = NO_HELP_CONTEXT);
	virtual ~ZAF_DIALOG_WINDOW(void);
	EVENT_TYPE Control(void);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new ZAF_DIALOG_WINDOW(name, file, object, _objectTable, _userTable)); }
	ZAF_DIALOG_WINDOW(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *_objectTable, UI_ITEM *_userTable)
		{ UIW_WINDOW::Load(name, file, object, _objectTable, _userTable); }
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
};

// --------------------------------------------------------------------------
// ----- ZAF_MESSAGE_WINDOW -------------------------------------------------
// --------------------------------------------------------------------------

typedef UIF_FLAGS MSG_FLAGS;
const MSG_FLAGS ZIL_MSG_OK					= 0x0001;
const MSG_FLAGS ZIL_MSG_YES					= 0x0002;
const MSG_FLAGS ZIL_MSG_NO					= 0x0004;
const MSG_FLAGS ZIL_MSG_ABORT				= 0x0008;
const MSG_FLAGS ZIL_MSG_RETRY				= 0x0010;
const MSG_FLAGS ZIL_MSG_IGNORE				= 0x0020;
const MSG_FLAGS ZIL_MSG_CANCEL				= 0x0040;
const MSG_FLAGS ZIL_MSG_HELP				= 0x0080;

class ZIL_EXPORT_CLASS ZAF_MESSAGE_WINDOW : public UIW_WINDOW
{
public:
	static ZIL_ICHAR _className[];
	static int defaultInitialized;
	const ZIL_LANGUAGE *myLanguage;

	// --- General class members ---
	ZAF_MESSAGE_WINDOW(ZIL_ICHAR *title, ZIL_ICHAR *icon, MSG_FLAGS msgFlags,
		MSG_FLAGS defFlag, ZIL_ICHAR *format, ...);
	virtual ~ZAF_MESSAGE_WINDOW(void);
	EVENT_TYPE Control(void);
};

// --------------------------------------------------------------------------
// ----- UI_CONSTRAINT ------------------------------------------------------
// --------------------------------------------------------------------------

// --- UI_CONSTRAINT::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_GET_OBJECT		= 510;
const ZIL_INFO_REQUEST I_SET_OBJECT		= 511;
const ZIL_INFO_REQUEST I_GET_NUMBERID	= 512;
const ZIL_INFO_REQUEST I_SET_NUMBERID	= 513;

class ZIL_EXPORT_CLASS UI_GEOMETRY_MANAGER;

class ZIL_EXPORT_CLASS UI_CONSTRAINT : public UI_ELEMENT
{
	friend class ZAF_GEOMETRY_MANAGER;
	friend class ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	virtual ~UI_CONSTRAINT(void);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void Modify(void) = 0;
	UI_GEOMETRY_MANAGER *Manager(UI_GEOMETRY_MANAGER *_manager = ZIL_NULLP(UI_GEOMETRY_MANAGER));
	ZIL_OBJECTID SearchID(void) { return (searchID); }

	// --- List members ---
	UI_CONSTRAINT *Next(void) { return((UI_CONSTRAINT *)next); }
	UI_CONSTRAINT *Previous(void) { return((UI_CONSTRAINT *)previous); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	static UI_CONSTRAINT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	UI_CONSTRAINT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	UI_CONSTRAINT(UI_WINDOW_OBJECT *_object);

	UI_WINDOW_OBJECT *object;
	ZIL_NUMBERID numberID;
	ZIL_OBJECTID searchID;
	UI_GEOMETRY_MANAGER *manager;
};

// --------------------------------------------------------------------------
// ----- UI_ATTACHMENT ------------------------------------------------------
// --------------------------------------------------------------------------

// --- ATCF_FLAGS ---
typedef UIF_FLAGS ATCF_FLAGS;
const ATCF_FLAGS ATCF_NO_FLAGS				= 0x0000;
const ATCF_FLAGS ATCF_LEFT					= 0x0001;	// Attach object's left side.
const ATCF_FLAGS ATCF_TOP					= 0x0002;	// Attach object's top side.
const ATCF_FLAGS ATCF_RIGHT					= 0x0004;	// Attach object's right side.
const ATCF_FLAGS ATCF_BOTTOM				= 0x0008;	// Attach object's bottom side.
const ATCF_FLAGS ATCF_OPPOSITE				= 0x0010;	// eg reference's left if ATCF_RIGHT.
const ATCF_FLAGS ATCF_STRETCH				= 0x0020;	// Child resizes.

// --- UI_ATTACHMENT::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_GET_REFERENCE_OBJECT	= 520;
const ZIL_INFO_REQUEST I_SET_REFERENCE_OBJECT	= 521;
const ZIL_INFO_REQUEST I_GET_OFFSET				= 522;
const ZIL_INFO_REQUEST I_SET_OFFSET				= 523;

class ZIL_EXPORT_CLASS UI_ATTACHMENT : public UI_CONSTRAINT
{
	friend class ZAF_GEOMETRY_MANAGER;
	friend class ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UI_ATTACHMENT(UI_WINDOW_OBJECT *_object,
		ATCF_FLAGS _atcFlags = ATCF_NO_FLAGS, int _offset = 0);
	UI_ATTACHMENT(UI_WINDOW_OBJECT *_object, UI_WINDOW_OBJECT *_reference,
		ATCF_FLAGS _atcFlags, int _offset = 0);
	virtual ~UI_ATTACHMENT(void);

	// --- Constraint members ---
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void Modify(void);

	// --- Flag members ---
	int IsLeftAttachment(void) { return ((atcFlags & ATCF_LEFT) ? TRUE : FALSE); }
	int IsTopAttachment(void) { return ((atcFlags & ATCF_TOP) ? TRUE : FALSE); }
	int IsRightAttachment(void) { return ((atcFlags & ATCF_RIGHT) ? TRUE : FALSE); }
	int IsBottomAttachment(void) { return ((atcFlags & ATCF_BOTTOM) ? TRUE : FALSE); }
	int AppliesToOppositeSide(void) { return ((atcFlags & ATCF_OPPOSITE) ? TRUE : FALSE); }
	int StretchesObject(void) { return ((atcFlags & ATCF_STRETCH) ? TRUE : FALSE); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UI_ATTACHMENT::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM))
		{ return ((UI_WINDOW_OBJECT *)new UI_ATTACHMENT(name, file, object, objectTable, userTable)); }
	UI_ATTACHMENT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	UI_WINDOW_OBJECT *reference;
	ZIL_NUMBERID refNumberID;
	ATCF_FLAGS atcFlags;
	int offset;
};

// --------------------------------------------------------------------------
// ----- UI_DIMENSION_CONSTRAINT ----------------------------------------------
// --------------------------------------------------------------------------

// --- DNCF_FLAG ---
typedef UIF_FLAGS DNCF_FLAGS;
const DNCF_FLAGS DNCF_NO_FLAGS				= 0x0000;
const DNCF_FLAGS DNCF_HEIGHT				= 0x0001;
const DNCF_FLAGS DNCF_WIDTH					= 0x0002;

// --- UI_DIMENSION_CONSTRAINT::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_GET_MAXDIMENSION		= 530;
const ZIL_INFO_REQUEST I_SET_MAXDIMENSION		= 531;
const ZIL_INFO_REQUEST I_GET_MINDIMENSION		= 532;
const ZIL_INFO_REQUEST I_SET_MINDIMENSION		= 533;

class ZIL_EXPORT_CLASS UI_DIMENSION_CONSTRAINT : public UI_CONSTRAINT
{
	friend class ZAF_GEOMETRY_MANAGER;
	friend class ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UI_DIMENSION_CONSTRAINT(UI_WINDOW_OBJECT *_object,
		DNCF_FLAGS _dncFlags = DNCF_NO_FLAGS, int _minimum = 0,
		int _maximum = 0);
	virtual ~UI_DIMENSION_CONSTRAINT(void);

	// --- Flag members ---
	int IsHeightConstraint(void) { return ((dncFlags & DNCF_HEIGHT) ? TRUE : FALSE); }
	int IsWidthConstraint(void) { return ((dncFlags & DNCF_WIDTH) ? TRUE : FALSE); }

	// --- Constraint members ---
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void Modify(void);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UI_DIMENSION_CONSTRAINT::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM))
		{ return ((UI_WINDOW_OBJECT *)new UI_DIMENSION_CONSTRAINT(name, file, object, objectTable, userTable)); }
	UI_DIMENSION_CONSTRAINT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	DNCF_FLAGS dncFlags;
	int maximum;
	int minimum;
};

// --------------------------------------------------------------------------
// ----- UI_RELATIVE_CONSTRAINT ----------------------------------------------
// --------------------------------------------------------------------------

// --- RLCF_FLAG ---
typedef UIF_FLAGS RLCF_FLAGS;
const RLCF_FLAGS RLCF_NO_FLAGS				= 0x0000;
const RLCF_FLAGS RLCF_LEFT					= 0x0001;
const RLCF_FLAGS RLCF_TOP					= 0x0002;
const RLCF_FLAGS RLCF_RIGHT					= 0x0004;
const RLCF_FLAGS RLCF_BOTTOM				= 0x0008;
const RLCF_FLAGS RLCF_OPPOSITE				= 0x0010;
const RLCF_FLAGS RLCF_STRETCH				= 0x0020;	// Object resizes.
const RLCF_FLAGS RLCF_HORIZONTAL_CENTER		= 0x0100;
const RLCF_FLAGS RLCF_VERTICAL_CENTER		= 0x0200;

// --- UI_RELATIVE_CONSTRAINT::ZIL_INFO_REQUEST ---
const ZIL_INFO_REQUEST I_GET_NUMERATOR			= 540;
const ZIL_INFO_REQUEST I_SET_NUMERATOR			= 541;
const ZIL_INFO_REQUEST I_GET_DENOMINATOR		= 542;
const ZIL_INFO_REQUEST I_SET_DENOMINATOR		= 543;

class ZIL_EXPORT_CLASS UI_RELATIVE_CONSTRAINT : public UI_CONSTRAINT
{
	friend class ZAF_GEOMETRY_MANAGER;
	friend class ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UI_RELATIVE_CONSTRAINT(UI_WINDOW_OBJECT *_object,
		RLCF_FLAGS _rlcFlags = RLCF_NO_FLAGS,
		int _numerator = 50, int _denominator = 100, int _offset = 0);
	virtual ~UI_RELATIVE_CONSTRAINT(void);

	// --- Flag members ---
	int AppliesToOppositeSide(void) { return ((rlcFlags & RLCF_OPPOSITE) ? TRUE : FALSE); }
	int CentersObjectHorizontally(void) { return ((rlcFlags & RLCF_HORIZONTAL_CENTER) ? TRUE : FALSE); }
	int CentersObjectVertically(void) { return ((rlcFlags & RLCF_VERTICAL_CENTER) ? TRUE : FALSE); }
	int IsBottomConstraint(void) { return ((rlcFlags & RLCF_BOTTOM) ? TRUE : FALSE); }
	int IsLeftConstraint(void) { return ((rlcFlags & RLCF_LEFT) ? TRUE : FALSE); }
	int IsRightConstraint(void) { return ((rlcFlags & RLCF_RIGHT) ? TRUE : FALSE); }
	int IsTopConstraint(void) { return ((rlcFlags & RLCF_TOP) ? TRUE : FALSE); }
	int StretchesObject(void) { return ((rlcFlags & RLCF_STRETCH) ? TRUE : FALSE); }

	// --- Constraint members ---
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	virtual void Modify(void);

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UI_RELATIVE_CONSTRAINT::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM))
		{ return ((UI_WINDOW_OBJECT *)new UI_RELATIVE_CONSTRAINT(name, file, object, objectTable, userTable)); }
	UI_RELATIVE_CONSTRAINT(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif

protected:
	int numerator;
	int denominator;
	RLCF_FLAGS rlcFlags;
	int offset;
};

// --------------------------------------------------------------------------
// ----- UI_GEOMETRY_MANAGER ------------------------------------------------
// --------------------------------------------------------------------------

class UI_GEOMETRY_MANAGER : public UI_WINDOW_OBJECT, public UI_LIST
{
	friend class ZAF_GEOMETRY_MANAGER;
	friend class ZAF_OBJECT_EDITOR;
public:
	static ZIL_ICHAR _className[];

	// --- General class members ---
	UI_GEOMETRY_MANAGER(void);
	virtual ~UI_GEOMETRY_MANAGER(void);

	// --- Window object members ---
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE DrawItem(const UI_EVENT &event, EVENT_TYPE ccode);
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);

	// --- List members ---
	UI_CONSTRAINT *Add(UI_CONSTRAINT *object);
	UI_CONSTRAINT *Current(void) { return((UI_CONSTRAINT *)current); }
	UI_CONSTRAINT *First(void) { return((UI_CONSTRAINT *)first); }
	UI_CONSTRAINT *Last(void) { return((UI_CONSTRAINT *)last); }
	UI_CONSTRAINT *Subtract(UI_CONSTRAINT *object);
	UI_GEOMETRY_MANAGER &operator+(UI_CONSTRAINT *object) { Add(object); return(*this); }
	UI_GEOMETRY_MANAGER &operator-(UI_CONSTRAINT *object) { Subtract(object); return(*this); }

	// --- Persistent object members ---
#if defined(ZIL_LOAD)
	virtual ZIL_NEW_FUNCTION NewFunction(void);// { return (UI_GEOMETRY_MANAGER::New); }
	static UI_WINDOW_OBJECT *New(const ZIL_ICHAR *name,
		ZIL_STORAGE_READ_ONLY *file = ZIL_NULLP(ZIL_STORAGE_READ_ONLY),
		ZIL_STORAGE_OBJECT_READ_ONLY *object = ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY),
		UI_ITEM *_objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *_userTable = ZIL_NULLP(UI_ITEM))
		{ return (new UI_GEOMETRY_MANAGER(name, file, object, _objectTable, _userTable)); }
	UI_GEOMETRY_MANAGER(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object,
		UI_ITEM *objectTable = ZIL_NULLP(UI_ITEM),
		UI_ITEM *userTable = ZIL_NULLP(UI_ITEM),
		const ZIL_ICHAR *languageName = ZIL_NULLP(ZIL_ICHAR));
	virtual void Load(const ZIL_ICHAR *name, ZIL_STORAGE_READ_ONLY *file,
		ZIL_STORAGE_OBJECT_READ_ONLY *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
#if defined(ZIL_STORE)
	virtual void Store(const ZIL_ICHAR *name, ZIL_STORAGE *file,
		ZIL_STORAGE_OBJECT *object, UI_ITEM *objectTable, UI_ITEM *userTable);
#endif
};

// Message indexes for the help and error system windows.
const ZIL_UINT8 MSG_TITLE 		= 0; // error
const ZIL_UINT8 MSG_OK 			= 1; // error
const ZIL_UINT8 MSG_CANCEL 		= 2; // error
const ZIL_UINT8 MSG_ICONNAME 	= 3; // help, error
const ZIL_UINT8 MSG_MOVE 		= 4; // error
const ZIL_UINT8 MSG_CLOSE 		= 5; // help, error
const ZIL_UINT8 MSG_MESSAGE1 	= 6; // help
const ZIL_UINT8 MSG_ICONTITLE 	= 7; // help
const ZIL_UINT8 MSG_MESSAGE2 	= 8; // help
const ZIL_UINT8 MSG_HELP_TOPIC 	= 9; // help
const ZIL_UINT8 MSG_HELP_INDEX 	= 10; // help
const ZIL_UINT8 MSG_SHOW_TOPIC 	= 11; // help
const ZIL_UINT8 MSG_SHOW_INDEX 	= 12; // help

#if defined(ZIL_OLD_DEFS)
// --- Version 3.6 compatibility ---
#define NUMBERID					ZIL_NUMBERID
#define SYSTEM_EVENT				ZIL_SYSTEM_EVENT
#define DESIGNER_EVENT				ZIL_DESIGNER_EVENT
#define LOGICAL_PALETTE				ZIL_LOGICAL_PALETTE
#define EXIT_FUNCTION				ZIL_EXIT_FUNCTION
#define USER_FUNCTION				ZIL_USER_FUNCTION
#define NEW_FUNCTION				ZIL_NEW_FUNCTION

#define GET_NUMBERID_OBJECT			I_GET_NUMBERID_OBJECT
#define GET_STRINGID_OBJECT			I_GET_STRINGID_OBJECT
#define GET_FLAGS					I_GET_FLAGS
#define SET_FLAGS					I_SET_FLAGS
#define CLEAR_FLAGS					I_CLEAR_FLAGS
#define CHANGED_FLAGS				I_CHANGED_FLAGS
#define GET_STATUS					I_GET_STATUS
#define SET_STATUS					I_SET_STATUS
#define CLEAR_STATUS				I_CLEAR_STATUS
#define CHANGED_STATUS				I_CHANGED_STATUS
#define GET_TEXT					I_GET_TEXT
#define SET_TEXT					I_SET_TEXT
#define GET_TEXT_LENGTH				I_GET_MAXLENGTH
#define SET_TEXT_LENGTH				I_SET_MAXLENGTH
#define COPY_TEXT					I_COPY_TEXT
#define GET_FIRST					I_GET_FIRST
#define GET_LAST					I_GET_LAST
#define GET_CURRENT					I_GET_CURRENT
#define GET_SUPPORT_FIRST			I_GET_SUPPORT_FIRST
#define GET_SUPPORT_LAST			I_GET_SUPPORT_LAST
#define GET_SUPPORT_CURRENT			I_GET_SUPPORT_CURRENT
#define GET_DEFAULT_OBJECT			I_GET_DEFAULT_OBJECT
#define SET_DEFAULT_OBJECT			I_SET_DEFAULT_OBJECT
#define GET_VALUE					I_GET_VALUE
#define SET_VALUE					I_SET_VALUE
#define INCREMENT_VALUE				I_INCREMENT_VALUE
#define DECREMENT_VALUE				I_DECREMENT_VALUE
#define GET_DWSTYLE					I_GET_DWSTYLE
#define SET_DWSTYLE					I_SET_DWSTYLE
#define CLEAR_DWSTYLE				I_CLEAR_DWSTYLE
#define CHANGED_DWSTYLE				I_CHANGED_DWSTYLE
#define HIDE_SUBWINDOW				I_HIDE_SUBWINDOW
#define SET_TASKID					I_SET_TASKID
#define GET_FLSTYLE					I_GET_FLSTYLE
#define SET_FLSTYLE					I_SET_FLSTYLE
#define SET_FLSTYLE					I_SET_FLSTYLE
#define CLEAR_FLSTYLE				I_CLEAR_FLSTYLE
#define CHANGED_FLSTYLE				I_CHANGED_FLSTYLE
#define GET_FLFLAG					I_GET_FLFLAG
#define SET_FLFLAG					I_SET_FLFLAG
#define CLEAR_FLFLAG				I_CLEAR_FLFLAG
#define CHANGED_FLFLAG				I_CHANGED_FLFLAG
#define GET_SYSTEM_OPTIONS			I_GET_SYSTEM_OPTIONS
#define SET_SYSTEM_OPTIONS			I_SET_SYSTEM_OPTIONS
#define GET_DECORATIONS				I_GET_DECORATIONS
#define SET_DECORATIONS				I_SET_DECORATIONS
#define INITIALIZE_CLASS			I_INITIALIZE_CLASS
#define PRINT_INFORMATION			I_PRINT_INFORMATION
#define PRINT_USER_FUNCTION			I_PRINT_USER_FUNCTION
#define PRINT_COMPARE_FUNCTION		I_PRINT_COMPARE_FUNCTION

#define UNDERLINE_CHARACTER			ZIL_UNDERLINE_CHARACTER
#define OLD_UNDERLINE_CHARACTER		ZIL_OLD_UNDERLINE_CHARACTER
#define BORDER_WIDTH				ZIL_BORDER_WIDTH
#define MOTIF_CONVENIENCE_FUNCTION	ZIL_MOTIF_CONVENIENCE_FUNCTION

#define SET_VSCROLL					I_SET_VSCROLL
#define SET_HSCROLL					I_SET_HSCROLL
#define CHECK_SELECTION				I_CHECK_SELECTION
#define GET_CLIPREGION				I_GET_CLIPREGION
#define DESTROY_LIST				I_DESTROY_LIST

#define GET_ICON					I_GET_ICON
#define SET_ICON					I_SET_ICON
#define GET_TABCOLUMN				I_GET_TABCOLUMN
#define SET_TABCOLUMN				I_SET_TABCOLUMN
#define GET_MENU					I_GET_MENU
#define SET_MENU					I_SET_MENU
#define GET_TITLE					I_GET_TITLE
#define SET_TITLE					I_SET_TITLE
#define GET_FRAMEID				I_GET_FRAMEID
#define SET_FRAMEID				I_SET_FRAMEID

#define GET_BITMAP_WIDTH			I_GET_BITMAP_WIDTH
#define SET_BITMAP_WIDTH			I_SET_BITMAP_WIDTH
#define GET_BITMAP_HEIGHT			I_GET_BITMAP_HEIGHT
#define SET_BITMAP_HEIGHT			I_SET_BITMAP_HEIGHT
#define GET_BITMAP_ARRAY			I_GET_BITMAP_ARRAY
#define SET_BITMAP_ARRAY			I_SET_BITMAP_ARRAY
#define GET_ICON_WIDTH				I_GET_ICON_WIDTH
#define GET_ICON_HEIGHT				I_GET_ICON_HEIGHT
#define GET_ICON_ARRAY				I_GET_ICON_ARRAY
#define SET_ICON_ARRAY				I_SET_ICON_ARRAY
#define RESET_SELECTION				I_RESET_SELECTION
#define MATCH_INPUT					I_MATCH_INPUT

// --- Version 2.0 and 1.0 compatibility ---
#	define UIW_NUMBER				UIW_BIGNUM
#	define UI_HELP_WINDOW_SYSTEM	UI_HELP_SYSTEM
#	define UI_ERROR_WINDOW_SYSTEM	UI_ERROR_SYSTEM
#	define _helpSystem				UI_WINDOW_OBJECT::helpSystem
#	define _errorSystem				UI_WINDOW_OBJECT::errorSystem
#endif

#endif

