//	Zinc Interface Library - G_EVENT.CPP
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


#define USE_RAW_KEYS
#include "ui_win.hpp"

void z_event_dummy(void) { } // Bug fix for broken linkers.

// ----- UI_WINDOW_OBJECT ---------------------------------------------------

#if defined(ZIL_MSDOS)
static UI_EVENT_MAP ZIL_FARDATA eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_SELECT,						E_KEY, 				ENTER,						0 },
	{ ID_WINDOW_OBJECT, 	L_SELECT,						E_KEY, 				GRAY_ENTER,					0 },
	{ ID_WINDOW_OBJECT,	L_SELECT,			  			E_KEY,	 			CTRL_ENTER,	 				S_CTRL },
	{ ID_WINDOW_OBJECT,	L_SELECT, 						E_KEY,	 			GRAY_CTRL_ENTER,			S_CTRL },
	{ ID_WINDOW_OBJECT, 	L_CANCEL,						E_KEY, 				ESCAPE,						0 },
	{ ID_WINDOW_OBJECT, 	L_VIEW,							E_MOUSE, 			0,							0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,	 			E_MOUSE, 			M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_OBJECT,	L_BEGIN_SELECT,	 			E_MOUSE, 			S_CTRL | M_LEFT | M_LEFT_CHANGE, 0 },
	{ ID_WINDOW_OBJECT, 	S_UNKNOWN,			 			E_MOUSE, 			M_LEFT | M_RIGHT | M_LEFT_CHANGE | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT, 			E_MOUSE, 			M_LEFT,						0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,			E_MOUSE, 			M_LEFT | M_RIGHT,			0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,					E_MOUSE, 			M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT, 					E_MOUSE, 			M_RIGHT | M_LEFT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,					E_MOUSE, 			M_RIGHT_CHANGE | M_LEFT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_UP,								E_KEY, 				GRAY_UP_ARROW,				0 },
	{ ID_WINDOW_OBJECT, 	L_UP,								E_KEY, 				WHITE_UP_ARROW,				0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							E_KEY, 				GRAY_DOWN_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							E_KEY, 				WHITE_DOWN_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							E_KEY, 				GRAY_LEFT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							E_KEY, 				WHITE_LEFT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,							E_KEY, 				GRAY_RIGHT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,							E_KEY, 				WHITE_RIGHT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,						E_KEY, 				BACKTAB,					S_SHIFT },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,						E_KEY, 				SHIFT_F6,					S_SHIFT },
	{ ID_WINDOW_OBJECT, 	L_NEXT,							E_KEY, 				TAB,						0 },
	{ ID_WINDOW_OBJECT,	L_NEXT,							E_KEY, 				F6,							0 },
	{ ID_WINDOW_OBJECT,	L_HELP,							E_KEY, 				F1,							0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_ESCAPE,			 	E_MOUSE, 			M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_ESCAPE,		 	E_MOUSE, 			M_RIGHT,					0 },
	{ ID_WINDOW_OBJECT, 	L_END_ESCAPE,					E_MOUSE, 			M_RIGHT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_COPY_DRAG,		E_MOUSE, 			S_CTRL | M_LEFT,			0 },
	{ ID_WINDOW_OBJECT, 	L_END_COPY_DRAG,				E_MOUSE, 			S_CTRL | M_LEFT_CHANGE,		0 },

	{ ID_STRING,			L_MARK,							E_KEY,				CTRL_F5,					0 },
	{ ID_STRING,			L_BEGIN_MARK,					E_MOUSE, 			M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_STRING,			L_BEGIN_MARK,					E_MOUSE, 			S_CTRL | M_LEFT | M_LEFT_CHANGE, 0 },
	{ ID_STRING,			L_CONTINUE_MARK,				E_MOUSE, 			M_LEFT,						0 },
	{ ID_STRING,			L_END_MARK,						E_MOUSE, 			M_LEFT_CHANGE,				0 },
	{ ID_STRING,			L_CUT,							E_KEY,				CTRL_F6,					0 },
	{ ID_STRING,			L_CUT,							E_KEY,				SHIFT_WHITE_DELETE,			S_SHIFT },
	{ ID_STRING,			L_CUT,							E_KEY,				GRAY_DELETE,		 		S_SHIFT },
	{ ID_STRING,			L_COPY_MARK,					E_KEY,				CTRL_GRAY_INSERT,			0 },
	{ ID_STRING,			L_COPY_MARK,					E_KEY,				CTRL_WHITE_INSERT,			0 },
	{ ID_STRING,			L_PASTE,					 		E_KEY,				CTRL_F8,					0 },
	{ ID_STRING,			L_PASTE,							E_KEY,				SHIFT_WHITE_INSERT,			S_SHIFT },
	{ ID_STRING,			L_PASTE,							E_KEY,				GRAY_INSERT,				S_SHIFT },
	{ ID_STRING,			L_BACKSPACE,					E_KEY,				BACKSPACE,					0 },
	{ ID_STRING,			L_DELETE,						E_KEY,				WHITE_DELETE,				0 },
	{ ID_STRING,			L_DELETE,						E_KEY,				GRAY_DELETE,				0 },
	{ ID_STRING,			L_DELETE_EOL,					E_KEY,				CTRL_WHITE_DELETE,			0 },
	{ ID_STRING,			L_DELETE_EOL,					E_KEY,				CTRL_GRAY_DELETE,			0 },
	{ ID_STRING,			L_WORD_LEFT,					E_KEY,				CTRL_WHITE_LEFT_ARROW,		0 },
	{ ID_STRING,			L_WORD_LEFT,					E_KEY,				CTRL_GRAY_LEFT_ARROW,		0 },
	{ ID_STRING,			L_WORD_LEFT,					E_KEY,				ALT_GRAY_LEFT_ARROW,		0 },
	{ ID_STRING,			L_WORD_RIGHT,					E_KEY,				CTRL_WHITE_RIGHT_ARROW,		0 },
	{ ID_STRING,			L_WORD_RIGHT,					E_KEY,				CTRL_GRAY_RIGHT_ARROW,		0 },
	{ ID_STRING,			L_WORD_RIGHT,					E_KEY,				ALT_GRAY_RIGHT_ARROW,		0 },
	{ ID_STRING,			L_BOL,							E_KEY,				WHITE_HOME,					0 },
	{ ID_STRING,			L_BOL,							E_KEY,				GRAY_HOME,					0 },
	{ ID_STRING,			L_BOL,							E_KEY,				CTRL_WHITE_HOME,			0 },
	{ ID_STRING,			L_BOL,							E_KEY,				CTRL_GRAY_HOME,				0 },
	{ ID_STRING,			L_EOL,							E_KEY,				WHITE_END,					0 },
	{ ID_STRING,			L_EOL,							E_KEY,				GRAY_END,					0 },
	{ ID_STRING,			L_EOL,							E_KEY,				CTRL_WHITE_END,				0 },
	{ ID_STRING,			L_EOL,							E_KEY,				CTRL_GRAY_END,				0 },
	{ ID_STRING,			L_INSERT_TOGGLE,				E_KEY,				WHITE_INSERT,				0 },
	{ ID_STRING,			L_INSERT_TOGGLE,				E_KEY,				GRAY_INSERT,				0 },
	{ ID_STRING,			L_PGUP,							E_KEY, 				WHITE_PGUP,					0 },
	{ ID_STRING,			L_PGUP,							E_KEY, 				GRAY_PGUP,					0 },
	{ ID_STRING,			L_PGDN,							E_KEY, 				WHITE_PGDN,					0 },
	{ ID_STRING,			L_PGDN,							E_KEY, 				GRAY_PGDN,					0 },

	{ ID_STRING,			L_MARK_UP,						E_KEY,				SHIFT_WHITE_UP_ARROW,		S_SHIFT },
	{ ID_STRING,			L_MARK_UP,						E_KEY,				GRAY_UP_ARROW,				S_SHIFT },
	{ ID_STRING,			L_MARK_DOWN,					E_KEY,				SHIFT_WHITE_DOWN_ARROW,		S_SHIFT },
	{ ID_STRING,			L_MARK_DOWN,					E_KEY,				GRAY_DOWN_ARROW,			S_SHIFT },
	{ ID_STRING,			L_MARK_LEFT,					E_KEY,				SHIFT_WHITE_LEFT_ARROW,		S_SHIFT },
	{ ID_STRING,			L_MARK_LEFT,					E_KEY,				GRAY_LEFT_ARROW,			S_SHIFT },
	{ ID_STRING,			L_MARK_RIGHT,					E_KEY,				SHIFT_WHITE_RIGHT_ARROW,	S_SHIFT },
	{ ID_STRING,			L_MARK_RIGHT,					E_KEY,				GRAY_RIGHT_ARROW,			S_SHIFT },
	{ ID_STRING,			L_MARK_BOL,						E_KEY,				SHIFT_WHITE_HOME,			S_SHIFT },
	{ ID_STRING,			L_MARK_BOL,						E_KEY,				GRAY_HOME,					S_SHIFT },
	{ ID_STRING,			L_MARK_EOL,						E_KEY,				SHIFT_WHITE_END,			S_SHIFT },
	{ ID_STRING,			L_MARK_EOL,						E_KEY,				GRAY_END,					S_SHIFT },
	{ ID_STRING,			L_MARK_PGUP,					E_KEY,				SHIFT_WHITE_PGUP,			S_SHIFT },
	{ ID_STRING,			L_MARK_PGUP,					E_KEY,				GRAY_PGUP,					S_SHIFT },
	{ ID_STRING,			L_MARK_PGDN,					E_KEY,				SHIFT_WHITE_PGDN,		  	S_SHIFT },
	{ ID_STRING,			L_MARK_PGDN,					E_KEY,				GRAY_PGDN,					S_SHIFT },
	{ ID_STRING,			L_MARK_EOL,						E_KEY,				GRAY_END,					S_SHIFT },
	{ ID_STRING,			L_MARK_WORD_LEFT,	  			E_KEY,				CTRL_WHITE_LEFT_ARROW,		S_SHIFT },
	{ ID_STRING,			L_MARK_WORD_LEFT,	  			E_KEY,				CTRL_GRAY_LEFT_ARROW,		S_SHIFT },
	{ ID_STRING,			L_MARK_WORD_RIGHT,  			E_KEY,				CTRL_WHITE_RIGHT_ARROW,		S_SHIFT },
	{ ID_STRING,			L_MARK_WORD_RIGHT,  			E_KEY,				CTRL_GRAY_RIGHT_ARROW,		S_SHIFT },

	{ ID_WINDOW,			L_PGUP,							E_KEY, 				WHITE_PGUP,					0 },
	{ ID_WINDOW,			L_PGUP,							E_KEY, 				GRAY_PGUP,					0 },
	{ ID_WINDOW,			L_PGDN,							E_KEY, 				WHITE_PGDN,					0 },
	{ ID_WINDOW,			L_PGDN,							E_KEY, 				GRAY_PGDN,					0 },
	{ ID_WINDOW,			L_TOP,							E_KEY,	 			WHITE_HOME,					0 },
	{ ID_WINDOW,			L_TOP,							E_KEY,	 			GRAY_HOME,					0 },
	{ ID_WINDOW,			L_BOTTOM,						E_KEY,	 			WHITE_END,					0 },
	{ ID_WINDOW,			L_BOTTOM,						E_KEY,	 			GRAY_END,					0 },
	{ ID_WINDOW,			L_MOVE,							E_KEY, 				ALT_F7,						0 },
	{ ID_WINDOW,			L_SIZE,							E_KEY, 				ALT_F8,						0 },
	{ ID_WINDOW,			L_RESTORE,						E_KEY, 				ALT_F5,						0 },
	{ ID_WINDOW,			S_ALT_KEY,						E_KEY, 				F10,						0 },
	{ ID_WINDOW, 			L_MDICHILD_EVENT+L_NEXT_WINDOW,	E_KEY,				CTRL_F6,					S_CTRL },
	{ ID_WINDOW, 			L_MDICHILD_EVENT+L_NEXT_WINDOW,	E_KEY,				CTRL_TAB,					S_CTRL },
	{ ID_WINDOW, 			S_MDICHILD_EVENT+S_CLOSE,	   	E_KEY,				CTRL_F4,					S_CTRL },

	{ ID_BUTTON,			L_SELECT,						E_KEY, 				ENTER,						0 },
	{ ID_BUTTON,			L_SELECT,						E_KEY, 				GRAY_ENTER,					0 },
	{ ID_BUTTON,			L_SELECT,						E_KEY, 				SPACE,						0 },
	{ ID_LIST,			  	L_SELECT,						E_KEY, 				ENTER,						0 },
	{ ID_LIST,	  			L_SELECT,						E_KEY, 				GRAY_ENTER,					0 },
	{ ID_GROUP,				L_SELECT,						E_KEY, 				SPACE,						0 },
	{ ID_COMBO_BOX,	  	L_SELECT,						E_KEY, 				F4,							0 },
	{ ID_COMBO_BOX,	  	L_SELECT,						E_KEY, 				ALT_GRAY_DOWN_ARROW,		0 },

	{ ID_TEXT,				E_KEY,							E_KEY, 				ENTER,						0 },
	{ ID_TEXT,			 	E_KEY,							E_KEY, 				GRAY_ENTER,					0 },
	{ ID_TEXT,				L_TOP,							E_KEY,	 			CTRL_WHITE_HOME,			0 },
	{ ID_TEXT,				L_TOP,							E_KEY,	 			CTRL_GRAY_HOME,				0 },
	{ ID_TEXT,				L_TOP,							E_KEY,	 			CTRL_WHITE_PGUP,			0 },
	{ ID_TEXT,				L_TOP,							E_KEY,	 			CTRL_GRAY_PGUP,				0 },
	{ ID_TEXT,				L_BOTTOM,						E_KEY,	 			CTRL_WHITE_END,				0 },
	{ ID_TEXT,				L_BOTTOM,						E_KEY,	 			CTRL_GRAY_END,				0 },
	{ ID_TEXT,				L_BOTTOM,						E_KEY,	 			CTRL_WHITE_PGDN,			0 },
	{ ID_TEXT,				L_BOTTOM,						E_KEY,	 			CTRL_GRAY_PGDN,				0 },
	{ ID_TEXT,				L_INSERT_TOGGLE,				E_KEY,	 			WHITE_INSERT,				0 },
	{ ID_TEXT,				L_INSERT_TOGGLE,				E_KEY,	 			GRAY_INSERT,				0 },

	{ ID_WINDOW_MANAGER,	L_CONTINUE_MOVE_DRAG,	 	E_MOUSE, 			M_LEFT,						0 },
	{ ID_WINDOW_MANAGER,	L_CONTINUE_MOVE_DRAG,	 	E_MOUSE, 			M_LEFT | M_RIGHT,			0 },
	{ ID_WINDOW_MANAGER,	L_CONTINUE_COPY_DRAG,	 	E_MOUSE, 			S_CTRL | M_LEFT,			0 },
	{ ID_WINDOW_MANAGER,	L_END_MOVE_DRAG,				E_MOUSE, 			M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_MANAGER,	L_END_MOVE_DRAG,				E_MOUSE, 			M_RIGHT_CHANGE | M_LEFT_CHANGE,	0 },
	{ ID_WINDOW_MANAGER,	L_END_MOVE_DRAG,				E_MOUSE, 			M_RIGHT | M_LEFT_CHANGE,	0 },
	{ ID_WINDOW_MANAGER,	L_END_COPY_DRAG,				E_MOUSE, 			S_CTRL | M_LEFT_CHANGE,	0 },
	{ ID_WINDOW_MANAGER,	L_BEGIN_SELECT,		  		E_MOUSE, 			M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_MANAGER,	L_BEGIN_SELECT,				E_MOUSE, 			S_CTRL | M_LEFT | M_LEFT_CHANGE, 0 },
	{ ID_WINDOW_MANAGER,	L_CONTINUE_SELECT, 			E_MOUSE, 			M_RIGHT,					0 },
	{ ID_WINDOW_MANAGER,	L_END_SELECT,					E_MOUSE, 			M_RIGHT_CHANGE,				0 },
	{ ID_WINDOW_MANAGER,	L_EXIT_FUNCTION,				E_KEY, 				SHIFT_F3,					S_SHIFT },
	{ ID_WINDOW_MANAGER,	L_EXIT_FUNCTION,				E_KEY, 				CTRL_BREAK,					0 },
	{ ID_WINDOW_MANAGER,	L_EXIT_FUNCTION,				E_KEY, 				CTRL_C,						0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE_TEMPORARY, 			E_KEY, 				ESCAPE,						0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE,							E_KEY, 				SHIFT_F4,					0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE,							E_KEY, 				ALT_F4,						0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE,							E_KEY, 				F3,							0 },
	{ ID_WINDOW_MANAGER,	S_REDISPLAY,					E_KEY, 				F5,							0 },
	{ ID_WINDOW_MANAGER,	L_NEXT_WINDOW,					E_KEY, 				ALT_F6,						0 },
	{ ID_WINDOW_MANAGER,	L_NEXT_WINDOW,					E_KEY, 				CTRL_TAB,					0 },
	{ ID_WINDOW_MANAGER,	L_MOVE,							E_KEY, 				ALT_F7,						0 },
	{ ID_WINDOW_MANAGER,	L_SIZE,							E_KEY, 				ALT_F8,						0 },
	{ ID_WINDOW_MANAGER,	L_HELP,							E_KEY, 				ALT_F1,						0 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_MSWINDOWS)
static UI_EVENT_MAP ZIL_FARDATA eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_VIEW,							WM_MOUSEMOVE,		0,	 						0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,					WM_LBUTTONDOWN,		M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_OBJECT, 	L_DOUBLE_CLICK,					WM_LBUTTONDBLCLK, 	M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,				WM_MOUSEMOVE, 		M_LEFT,						0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,					WM_LBUTTONUP, 		M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_ESCAPE,					WM_RBUTTONDOWN,		M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_ESCAPE,					WM_RBUTTONDBLCLK, 	M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_ESCAPE,				WM_MOUSEMOVE, 		M_RIGHT,					0 },
	{ ID_WINDOW_OBJECT, 	L_END_ESCAPE,					WM_RBUTTONUP, 		M_RIGHT_CHANGE,				0 },

	{ ID_WINDOW_OBJECT, 	L_BEGIN_COPY_DRAG,				WM_LBUTTONDOWN, 	M_LEFT | M_LEFT_CHANGE | S_CTRL,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_COPY_DRAG,			WM_MOUSEMOVE, 		M_LEFT | S_CTRL,					0 },
	{ ID_WINDOW_OBJECT, 	L_END_COPY_DRAG,				WM_LBUTTONUP, 		M_LEFT_CHANGE | S_CTRL,				0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_MOVE_DRAG,				WM_LBUTTONDOWN, 	M_LEFT | M_LEFT_CHANGE | S_SHIFT,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_MOVE_DRAG,			WM_MOUSEMOVE, 		M_LEFT | S_SHIFT,					0 },
	{ ID_WINDOW_OBJECT, 	L_END_MOVE_DRAG,				WM_LBUTTONUP, 		M_LEFT_CHANGE | S_SHIFT,			0 },

	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,						WM_CHAR, 			BACKTAB,					0 },
	{ ID_WINDOW_OBJECT, 	L_NEXT,							WM_CHAR, 			TAB,						0 },
	{ ID_WINDOW_OBJECT,		L_UP,							WM_KEYDOWN, 		GRAY_UP_ARROW,				0 },
	{ ID_WINDOW_OBJECT,		L_UP,							WM_KEYDOWN, 		WHITE_UP_ARROW,				0 },
	{ ID_WINDOW_OBJECT,		L_DOWN,							WM_KEYDOWN, 		GRAY_DOWN_ARROW,			0 },
	{ ID_WINDOW_OBJECT,		L_DOWN,							WM_KEYDOWN, 		WHITE_DOWN_ARROW,			0 },
	{ ID_WINDOW_OBJECT,		L_LEFT,							WM_KEYDOWN, 		GRAY_LEFT_ARROW,			0 },
	{ ID_WINDOW_OBJECT,		L_LEFT,							WM_KEYDOWN, 		WHITE_LEFT_ARROW,			0 },
	{ ID_WINDOW_OBJECT,		L_RIGHT,						WM_KEYDOWN, 		GRAY_RIGHT_ARROW,			0 },
	{ ID_WINDOW_OBJECT,		L_RIGHT,						WM_KEYDOWN, 		WHITE_RIGHT_ARROW,			0 },
	{ ID_WINDOW_OBJECT,		L_PGDN,							WM_KEYDOWN, 		GRAY_PGDN,					0 },
	{ ID_WINDOW_OBJECT,		L_PGDN,							WM_KEYDOWN, 		WHITE_PGDN,					0 },
	{ ID_WINDOW_OBJECT,		L_PGUP,							WM_KEYDOWN, 		GRAY_PGUP,					0 },
	{ ID_WINDOW_OBJECT,		L_PGUP,							WM_KEYDOWN, 		WHITE_PGUP,					0 },
	{ ID_WINDOW_OBJECT, 	L_SELECT,						WM_CHAR, 			ENTER,						0 },
	{ ID_WINDOW_OBJECT,		L_HELP,							WM_KEYUP, 			F1,							0 },
	{ ID_WINDOW_OBJECT,		S_CLOSE_TEMPORARY,				WM_KEYUP, 			ESCAPE,						0 },

	{ ID_STRING, 			L_CUT,							WM_KEYDOWN, 		GRAY_DELETE | (S_SHIFT << 8),	0 },
	{ ID_STRING, 			L_CUT,							WM_KEYDOWN, 		WHITE_DELETE | (S_SHIFT << 8),	0 },
	{ ID_STRING, 			L_COPY_MARK,					WM_KEYDOWN, 		GRAY_INSERT | (S_CTRL << 8),	0 },
	{ ID_STRING, 			L_COPY_MARK,					WM_KEYDOWN, 		WHITE_INSERT | (S_CTRL << 8),	0 },
	{ ID_STRING, 			L_PASTE,						WM_KEYDOWN, 		GRAY_INSERT | (S_SHIFT << 8),	0 },
	{ ID_STRING, 			L_PASTE,						WM_KEYDOWN, 		WHITE_INSERT | (S_SHIFT << 8),	0 },
	{ ID_STRING, 			L_BACKSPACE,					WM_KEYDOWN, 		BACKSPACE,					0 },
	{ ID_STRING, 			L_DELETE,						WM_KEYDOWN, 		GRAY_DELETE,				0 },
	{ ID_STRING, 			L_DELETE,						WM_KEYDOWN, 		WHITE_DELETE,				0 },
	{ ID_STRING, 			L_DELETE_EOL,					WM_KEYDOWN,			CTRL_WHITE_DELETE,			0 },
	{ ID_STRING, 			L_DELETE_EOL,					WM_KEYDOWN,			CTRL_GRAY_DELETE,			0 },
	{ ID_STRING, 			L_BOL,							WM_KEYDOWN,			GRAY_HOME,					0 },
	{ ID_STRING, 			L_BOL,							WM_KEYDOWN,			WHITE_HOME,					0 },

	{ ID_TEXT,			 	E_MSWINDOWS,					WM_CHAR, 			ENTER,						0 },
	{ ID_TEXT,			 	L_SELECT,						WM_CHAR, 			CTRL_ENTER,					0 },

	{ ID_BUTTON, 			L_SELECT,						WM_CHAR, 			ENTER,						0 },
	{ ID_BUTTON, 			L_SELECT,						WM_CHAR, 			SPACE,						0 },
	{ ID_BUTTON, 			L_SELECT,						WM_CHAR, 			CTRL_SPACE,					0 },
	{ ID_BUTTON, 			L_SELECT,						WM_CHAR, 			SHIFT_SPACE,				0 },

	{ ID_LIST, 				L_SELECT,						WM_CHAR, 			SPACE,						0 },
	{ ID_LIST, 				L_SELECT,						WM_CHAR, 			CTRL_SPACE,					0 },
	{ ID_LIST,	 			L_SELECT,						WM_CHAR, 			SHIFT_SPACE,				0 },

	{ ID_TABLE_RECORD,		L_SELECT,						WM_CHAR, 			ENTER,						0 },

	{ ID_LIST_ITEM,			E_MSWINDOWS,					WM_CHAR, 			ENTER,						0 },
	{ ID_LIST_ITEM,			L_SELECT,						WM_CHAR, 			SPACE,						0 },

	{ ID_WINDOW_MANAGER,	L_NEXT_WINDOW,					WM_SYSKEYDOWN,		ALT_F6,						0 },
	{ ID_WINDOW_MANAGER,	L_HELP,							WM_SYSKEYUP, 		ALT_F1,						0 },

	// End of array.
	{ ID_END, 0, 0, 0, 0 }
};
#elif defined(ZIL_OS2)
static UI_EVENT_MAP ZIL_FARDATA eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_EXIT,							WM_QUIT,			0, 							0 },
	{ ID_WINDOW_OBJECT, 	L_VIEW,							WM_MOUSEMOVE, 		0,							0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,					WM_BUTTON1DOWN,		M_LEFT | M_LEFT_CHANGE, 	0 },
	{ ID_WINDOW_OBJECT, 	L_DOUBLE_CLICK,					WM_BUTTON1DBLCLK, 	M_LEFT | M_LEFT_CHANGE, 	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,				WM_MOUSEMOVE, 		M_LEFT,						0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,					WM_BUTTON1UP, 		M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT,		L_HELP,							WM_CHAR, 			F1,							0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_ESCAPE,					WM_BUTTON2DOWN, 	M_RIGHT | M_RIGHT_CHANGE, 	0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_ESCAPE,					WM_BUTTON2DBLCLK, 	M_RIGHT | M_RIGHT_CHANGE, 	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_ESCAPE,				WM_MOUSEMOVE, 		M_RIGHT,					0 },
	{ ID_WINDOW_OBJECT, 	L_END_ESCAPE,					WM_BUTTON2UP, 		M_RIGHT_CHANGE, 			0 },
	{ ID_WINDOW_OBJECT, 	L_UP,							WM_CHAR, 			GRAY_UP_ARROW, 				0 },
	{ ID_WINDOW_OBJECT, 	L_UP,							WM_CHAR, 			WHITE_UP_ARROW, 			0 },
	{ ID_WINDOW_OBJECT, 	L_UP,							WM_CHAR, 			CTRL_GRAY_UP_ARROW, 		0 },
	{ ID_WINDOW_OBJECT, 	L_UP,							WM_CHAR, 			CTRL_WHITE_UP_ARROW, 		0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							WM_CHAR, 			GRAY_DOWN_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							WM_CHAR, 			WHITE_DOWN_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							WM_CHAR, 			CTRL_GRAY_DOWN_ARROW,		0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							WM_CHAR, 			CTRL_WHITE_DOWN_ARROW,		0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							WM_CHAR, 			GRAY_LEFT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							WM_CHAR, 			WHITE_LEFT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							WM_CHAR, 			CTRL_GRAY_LEFT_ARROW,		0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							WM_CHAR, 			CTRL_WHITE_LEFT_ARROW,		0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						WM_CHAR, 			GRAY_RIGHT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						WM_CHAR, 			WHITE_RIGHT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						WM_CHAR, 			CTRL_GRAY_RIGHT_ARROW,		0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						WM_CHAR, 			CTRL_WHITE_RIGHT_ARROW,		0 },
	{ ID_WINDOW_OBJECT, 	L_NEXT,							WM_CHAR, 			TAB,						0 },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,						WM_CHAR, 			BACKTAB,					0 },
	{ ID_WINDOW_OBJECT, 	L_MDICHILD_EVENT+L_NEXT,		WM_CHAR,			CTRL_F6,  					0 },
	{ ID_WINDOW_OBJECT, 	S_MDICHILD_EVENT+S_CLOSE,		WM_CHAR,			CTRL_F4,  					0 },
	{ ID_WINDOW_OBJECT, 	L_SELECT,						WM_CHAR, 			ENTER,						0 },
	{ ID_WINDOW_OBJECT, 	L_SELECT,						WM_CHAR, 			GRAY_ENTER,					0 },
	{ ID_WINDOW_OBJECT, 	L_CANCEL,						WM_CHAR, 			ESCAPE,						0 },

	{ ID_STRING,			L_DELETE,						WM_CHAR,			WHITE_DELETE,				0 },
	{ ID_STRING,			L_DELETE,						WM_CHAR,			GRAY_DELETE,				0 },
	{ ID_STRING,			L_BACKSPACE,					WM_CHAR,			BACKSPACE,					0 },
	{ ID_STRING,			L_BOL,							WM_CHAR,			WHITE_HOME,					0 },
	{ ID_STRING,			L_BOL,							WM_CHAR,			GRAY_HOME,					0 },

 	{ ID_WINDOW, 			L_PGUP,							WM_CHAR, 			WHITE_PGUP,					0 },
 	{ ID_WINDOW, 			L_PGUP,							WM_CHAR, 			GRAY_PGUP,					0 },
 	{ ID_WINDOW, 			L_PGDN,							WM_CHAR, 			WHITE_PGDN,					0 },
 	{ ID_WINDOW, 			L_PGDN,							WM_CHAR, 			GRAY_PGDN,					0 },
 	{ ID_WINDOW, 			L_TOP,							WM_CHAR, 			WHITE_HOME,					0 },
 	{ ID_WINDOW, 			L_TOP,							WM_CHAR, 			GRAY_HOME,					0 },
 	{ ID_WINDOW, 			L_BOTTOM,						WM_CHAR, 			WHITE_END,					0 },
 	{ ID_WINDOW, 			L_BOTTOM,						WM_CHAR, 			GRAY_END,					0 },

	{ ID_TEXT,			 	E_OS2,							WM_CHAR, 			ENTER,						0 },
	{ ID_TEXT,			 	E_OS2,							WM_CHAR, 			GRAY_ENTER,					0 },

	{ ID_BUTTON,		 	S_UNKNOWN,						WM_CHAR, 			ENTER,						0 },
	{ ID_BUTTON, 			S_UNKNOWN,						WM_CHAR, 			GRAY_ENTER,					0 },
	{ ID_BUTTON, 			L_SELECT,						WM_CHAR, 			SPACE,						KC_KEYUP },

	{ ID_LIST,				L_SELECT,						WM_CHAR, 			SPACE,						0 },

	{ ID_LIST_ITEM, 		L_SELECT,						WM_CHAR, 			SPACE,						0 },
	{ ID_LIST_ITEM,			S_UNKNOWN,						WM_CHAR, 			ENTER,						0 },
	{ ID_LIST_ITEM,			S_UNKNOWN,						WM_CHAR, 			GRAY_ENTER,					0 },

	{ ID_WINDOW_MANAGER,	L_HELP,							WM_CHAR, 			ALT_F1,						0 },

	// End of array.
	{ ID_END, 0, 0, 0, 0 }
};
#elif defined(ZIL_MOTIF)
static UI_EVENT_MAP ZIL_FARDATA eventMapTable[] =
{
	{ ID_WINDOW_OBJECT,		L_BEGIN_SELECT,					ButtonPress,		M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_SELECT,				MotionNotify,		M_LEFT,						0 },
	{ ID_WINDOW_OBJECT,		L_END_SELECT,					ButtonRelease,		M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT,		L_BEGIN_SELECT,					ButtonPress,		M_LEFT | M_LEFT_CHANGE,		LockMask },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_SELECT,				MotionNotify,		M_LEFT,						LockMask },
	{ ID_WINDOW_OBJECT,		L_END_SELECT,					ButtonRelease,		M_LEFT_CHANGE,				LockMask },
	{ ID_WINDOW_OBJECT,		L_BEGIN_ESCAPE,					ButtonPress,		M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_ESCAPE,				MotionNotify,		M_RIGHT,					0 },
	{ ID_WINDOW_OBJECT,		L_END_ESCAPE,					ButtonRelease,		M_RIGHT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT,		L_BEGIN_ESCAPE,					ButtonPress,		M_RIGHT | M_RIGHT_CHANGE,	LockMask },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_ESCAPE,				MotionNotify,		M_RIGHT,					LockMask },
	{ ID_WINDOW_OBJECT,		L_END_ESCAPE,					ButtonRelease,		M_RIGHT_CHANGE,				LockMask },
#if (ZIL_MOTIF >= 1002)
	{ ID_WINDOW_OBJECT,		L_BEGIN_COPY_DRAG,				ButtonPress,		M_MIDDLE | M_MIDDLE_CHANGE,	0 },
	{ ID_WINDOW_OBJECT,		L_BEGIN_COPY_DRAG,				ButtonPress,		M_MIDDLE | M_MIDDLE_CHANGE,	LockMask },
	{ ID_WINDOW_OBJECT,		L_BEGIN_COPY_DRAG,				ButtonPress,		M_MIDDLE | M_MIDDLE_CHANGE,	ControlMask },
	{ ID_WINDOW_OBJECT,		L_BEGIN_MOVE_DRAG,				ButtonPress,		M_MIDDLE | M_MIDDLE_CHANGE,	ShiftMask },
	{ ID_WINDOW_OBJECT,		L_END_COPY_DRAG,				ButtonRelease,		M_MIDDLE,	0 },
	{ ID_WINDOW_OBJECT,		L_END_COPY_DRAG,				ButtonRelease,		M_MIDDLE,	LockMask },
	{ ID_WINDOW_OBJECT,		L_END_COPY_DRAG,				ButtonRelease,		M_MIDDLE,	ControlMask },
	{ ID_WINDOW_OBJECT,		L_END_MOVE_DRAG,				ButtonRelease,		M_MIDDLE,	ShiftMask },
#endif
	{ ID_WINDOW_OBJECT, 	L_CANCEL,						KeyPress, 			XK_Escape,					0 },
	{ ID_WINDOW_OBJECT,		S_CHANGED,						ConfigureNotify,	0,							0 },
	{ ID_WINDOW_OBJECT,		L_HELP,							KeyPress, 			XK_F1,						0 },
	{ ID_WINDOW_OBJECT,		L_HELP,							KeyPress, 			XK_F1,						Mod1Mask },
#if defined(__hpux)
	{ ID_WINDOW_OBJECT, 	L_UP,							KeyPress, 			osfXK_Up,					0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							KeyPress, 			osfXK_Down,					0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							KeyPress, 			osfXK_Left,					0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						KeyPress, 			osfXK_Right,				0 },
	{ ID_WINDOW_OBJECT, 	L_PGUP,							KeyPress, 			osfXK_PageUp,				0 },
	{ ID_WINDOW_OBJECT, 	L_PGDN,							KeyPress, 			osfXK_PageDown,				0 },
#endif
	{ ID_WINDOW_OBJECT, 	L_UP,							KeyPress, 			XK_Up,						0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							KeyPress, 			XK_Down,					0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							KeyPress, 			XK_Left,					0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						KeyPress, 			XK_Right,					0 },
	{ ID_WINDOW_OBJECT, 	L_PGUP,							KeyPress, 			XK_Prior,					0 },
	{ ID_WINDOW_OBJECT, 	L_PGDN,							KeyPress, 			XK_Next,					0 },

	{ ID_STRING,			L_BEGIN_MARK,					ButtonPress,		M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_STRING,			L_CONTINUE_MARK,				MotionNotify,		M_LEFT,						0 },
	{ ID_STRING,			L_END_MARK,						ButtonRelease,		M_LEFT_CHANGE,				0 },

#if ZIL_MOTIF <= 1001
	// Let Enter activate buttons in Motif 1.1, as per OSF Style Guide.
	{ ID_BUTTON,			L_SELECT,						KeyPress,			XK_Return,					0 },
	{ ID_BUTTON,			L_SELECT,						KeyPress,			XK_KP_Enter,				0 },
	{ ID_BUTTON,			L_SELECT,						KeyPress,			XK_Return,					LockMask },
	{ ID_BUTTON,			L_SELECT,						KeyPress,			XK_KP_Enter,				LockMask },
#else
	{ ID_PULL_DOWN_ITEM,	L_SELECT,	 					KeyPress,			XK_Return,					0 },
	{ ID_PULL_DOWN_ITEM,	L_SELECT,	 					KeyPress,			XK_Return,					LockMask },
	{ ID_POP_UP_ITEM,		L_SELECT,						KeyPress,			XK_Return,					0 },
	{ ID_POP_UP_ITEM,		L_SELECT,						KeyPress,			XK_Return,					LockMask },
#endif
//	{ ID_TEXT,				E_MOTIF,						KeyPress,			XK_Return,					0 },
//	{ ID_TEXT,				E_MOTIF,						KeyPress,			XK_KP_Enter,				0 },
//	{ ID_TEXT,				E_MOTIF,						KeyPress,			XK_Return,					LockMask },
//	{ ID_TEXT,				E_MOTIF,						KeyPress,			XK_KP_Enter,				LockMask },

	{ ID_WINDOW_OBJECT,		L_SELECT,						KeyPress,			XK_Return,					0 },
	{ ID_WINDOW_OBJECT,		L_SELECT,						KeyPress,			XK_KP_Enter,				0 },
	{ ID_WINDOW_OBJECT,		L_SELECT,						KeyPress,			XK_Return,					LockMask },
	{ ID_WINDOW_OBJECT,		L_SELECT,						KeyPress,			XK_KP_Enter,				LockMask },

	{ ID_TEXT,				L_SELECT,						KeyPress,			XK_Return,					0 },
	{ ID_TEXT,				L_SELECT,						KeyPress,			XK_KP_Enter,				0},
	{ ID_TEXT,				L_SELECT,						KeyPress,			XK_Return,					LockMask },
	{ ID_TEXT,				L_SELECT,						KeyPress,			XK_KP_Enter,				LockMask },
	{ ID_BUTTON, 			L_SELECT,						KeyPress, 			XK_space,					LockMask },
	{ ID_BUTTON, 			L_SELECT,						KeyPress, 			XK_space,					0 },
	{ ID_LIST, 				L_SELECT,						KeyPress, 			XK_space,					0 },
	{ ID_GROUP, 			L_SELECT,						KeyPress, 			XK_space,					0 },
	{ ID_COMBO_BOX,			S_CONTINUE,						KeyPress, 			XK_space,					0 },
	{ ID_TABLE_RECORD,		L_SELECT,						KeyPress, 			XK_space,					0 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_CURSES)
#if defined(ZIL_PC_KEYBRD)
static UI_EVENT_MAP ZIL_FARDATA eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_SELECT,						E_KEY, 				ENTER,					0 },
	{ ID_WINDOW_OBJECT, 	L_CANCEL,						E_KEY, 				ESCAPE,						0 },
	{ ID_WINDOW_OBJECT, 	L_UP,							E_KEY, 				UP_ARROW,					0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							E_KEY, 				DOWN_ARROW,					0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							E_KEY, 				LEFT_ARROW,					0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						E_KEY, 				RIGHT_ARROW,				0 },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,						E_KEY, 				BACKTAB,					0 },
	{ ID_WINDOW_OBJECT, 	L_NEXT,							E_KEY, 				TAB,						0 },
	{ ID_WINDOW_OBJECT, 	L_NEXT,							E_KEY, 				F6,							0 },
	{ ID_WINDOW_OBJECT, 	L_HELP,							E_KEY, 				F1,							0 },
	{ ID_WINDOW_OBJECT, 	L_VIEW,							E_MOUSE, 			0,							0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,					E_MOUSE, 			M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,					E_MOUSE, 			M_LEFT | M_RIGHT | M_LEFT_CHANGE | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,				E_MOUSE, 			M_LEFT,						0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,				E_MOUSE, 			M_LEFT | M_RIGHT,			0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,					E_MOUSE, 			M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,					E_MOUSE, 			M_RIGHT | M_LEFT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_ESCAPE,					E_MOUSE, 			M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_ESCAPE,				E_MOUSE, 			M_RIGHT,					0 },
	{ ID_WINDOW_OBJECT, 	L_END_ESCAPE,					E_MOUSE, 			M_RIGHT_CHANGE,				0 },

	{ ID_STRING, 			L_BACKSPACE,					E_KEY,	 			CTRL_H,						0 },
	{ ID_STRING, 			L_BACKSPACE,					E_KEY,	 			BACKSPACE,					0 },
	{ ID_STRING,			L_MARK,							E_KEY,				F5,							0 },
	{ ID_STRING,			L_CUT,							E_KEY,				F6,							0 },
	{ ID_STRING,			L_PASTE,						E_KEY,				F8,							0 },
	{ ID_STRING,			L_BEGIN_MARK,					E_MOUSE, 			M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_STRING,			L_CONTINUE_MARK,				E_MOUSE, 			M_LEFT,						0 },
	{ ID_STRING,			L_END_MARK,						E_MOUSE, 			M_LEFT_CHANGE,				0 },

	{ ID_TEXT,				E_KEY,							E_KEY, 				ENTER,						0 },

	{ ID_WINDOW,			L_RESTORE,						E_KEY, 				F5,							S_ALT },
	{ ID_WINDOW,			L_MOVE,							E_KEY, 				F7,							0 },
	{ ID_WINDOW,			L_MOVE,							E_KEY, 				F7,							S_ALT },
	{ ID_WINDOW,			L_SIZE,							E_KEY, 				F8,							S_ALT },
	{ ID_WINDOW,			S_ALT_KEY,						E_KEY, 				F10,						0 },

	{ ID_BUTTON,			L_SELECT,						E_KEY, 				ENTER,						0 },
	{ ID_BUTTON,			L_SELECT,						E_KEY, 				SPACE,						0 },
	{ ID_LIST,				L_SELECT,						E_KEY, 				SPACE,						0 },
	{ ID_GROUP,				L_SELECT,						E_KEY, 				SPACE,						0 },

	{ ID_WINDOW_MANAGER,	L_EXIT_FUNCTION,				E_KEY, 				CTRL_BREAK,					0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE_TEMPORARY,				E_KEY,				ESCAPE,						0 },
	{ ID_WINDOW_MANAGER,	L_HELP,							E_KEY, 				F1,							S_ALT },
	{ ID_WINDOW_MANAGER,	S_CLOSE,						E_KEY, 				F3,							0 },
	{ ID_WINDOW_MANAGER,	L_EXIT_FUNCTION,				E_KEY, 				F3,							S_ALT },
	{ ID_WINDOW_MANAGER,	L_EXIT_FUNCTION,				E_KEY, 				F4,							0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE,						E_KEY, 				F4,							S_ALT },
	{ ID_WINDOW_MANAGER,	S_REDISPLAY,					E_KEY, 				F5,							0 },
	{ ID_WINDOW_MANAGER,	L_NEXT_WINDOW,					E_KEY, 				F6,							S_ALT },
	{ ID_WINDOW_MANAGER,	L_MOVE,							E_KEY, 				F7,							S_ALT },
	{ ID_WINDOW_MANAGER,	L_SIZE,							E_KEY, 				F8,							S_ALT },
	{ ID_WINDOW_MANAGER,	L_SIZE,							E_KEY, 				F8,							0 },
	{ ID_WINDOW_MANAGER,	L_BEGIN_SELECT,					E_MOUSE, 			M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_MANAGER,	L_CONTINUE_SELECT,				E_MOUSE, 			M_RIGHT,					0 },
	{ ID_WINDOW_MANAGER,	L_END_SELECT,					E_MOUSE, 			M_RIGHT_CHANGE,				0 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#else
static UI_EVENT_MAP ZIL_FARDATA eventMapTable[] =
{
	{ ID_WINDOW_OBJECT, 	L_SELECT,						E_KEY, 				ENTER,					0 },
	{ ID_WINDOW_OBJECT, 	L_CANCEL,						E_KEY, 				KEY_CANCEL,					0 },
	{ ID_WINDOW_OBJECT, 	L_CANCEL,						E_KEY, 				ESCAPE,						0 },
	{ ID_WINDOW_OBJECT, 	L_UP,							E_KEY, 				KEY_UP,						0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							E_KEY, 				KEY_DOWN,					0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							E_KEY, 				KEY_LEFT,					0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						E_KEY, 				KEY_RIGHT,					0 },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,						E_KEY, 				KEY_BTAB,					0 },
	{ ID_WINDOW_OBJECT, 	L_NEXT,							E_KEY, 				'\t',						0 },
	{ ID_WINDOW_OBJECT, 	L_HELP,							E_KEY, 				KEY_HELP,					0 },
	{ ID_WINDOW_OBJECT, 	L_HELP,							E_KEY, 				CTRL_E,						0 },
	{ ID_WINDOW_OBJECT, 	L_VIEW,							E_MOUSE, 			0,							0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,					E_MOUSE, 			M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_SELECT,					E_MOUSE, 			M_LEFT | M_RIGHT | M_LEFT_CHANGE | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,				E_MOUSE, 			M_LEFT,						0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_SELECT,				E_MOUSE, 			M_LEFT | M_RIGHT,			0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,					E_MOUSE, 			M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT, 	L_END_SELECT,					E_MOUSE, 			M_RIGHT | M_LEFT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_ESCAPE,					E_MOUSE, 			M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_ESCAPE,				E_MOUSE, 			M_RIGHT,					0 },
	{ ID_WINDOW_OBJECT, 	L_END_ESCAPE,					E_MOUSE, 			M_RIGHT_CHANGE,				0 },

	{ ID_STRING, 			L_BACKSPACE,					E_KEY,	 			CTRL_H,						0 },
	{ ID_STRING, 			L_BACKSPACE,					E_KEY,	 			KEY_BACKSPACE,				0 },
	{ ID_STRING,			L_MARK,							E_KEY,				KEY_MARK,					0 },
	{ ID_STRING,			L_MARK,							E_KEY,				CTRL_K,						0 },
	{ ID_STRING,			L_CUT,							E_KEY,				KEY_DL,						0 },
	{ ID_STRING,			L_CUT,							E_KEY,				CTRL_T,						0 },
	{ ID_STRING,			L_PASTE,						E_KEY,				KEY_IL,						0 },
	{ ID_STRING,			L_PASTE,						E_KEY,				CTRL_P,						0 },
	{ ID_STRING,			L_BEGIN_MARK,					E_MOUSE, 			M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_STRING,			L_CONTINUE_MARK,				E_MOUSE, 			M_LEFT,						0 },
	{ ID_STRING,			L_END_MARK,						E_MOUSE, 			M_LEFT_CHANGE,				0 },

	{ ID_TEXT,				E_KEY,							E_KEY, 				13,							0 },

	{ ID_WINDOW,			L_RESTORE,						E_KEY, 				KEY_RESUME,					0 },
	{ ID_WINDOW,			L_RESTORE,						E_KEY, 				CTRL_O,						0 },
	{ ID_WINDOW,			L_MOVE,							E_KEY, 				KEY_MOVE,					0 },
	{ ID_WINDOW,			L_MOVE,							E_KEY, 				CTRL_V,						0 },
	{ ID_WINDOW,			L_SIZE,							E_KEY, 				KEY_SMOVE,					0 },
	{ ID_WINDOW,			L_SIZE,							E_KEY, 				CTRL_B,						0 },
	{ ID_WINDOW,			S_ALT_KEY,						E_KEY,				KEY_COMMAND,				0 },
	{ ID_WINDOW,			S_ALT_KEY,						E_KEY, 				F10,						0 },

    { ID_BUTTON,			L_SELECT,						E_KEY, 				13,							0 },
	{ ID_BUTTON,			L_SELECT,						E_KEY, 				' ',						0 },
	{ ID_LIST,				L_SELECT,						E_KEY, 				' ',						0 },
	{ ID_GROUP,				L_SELECT,						E_KEY, 				' ',						0 },

	{ ID_WINDOW_MANAGER,	L_EXIT_FUNCTION,				E_KEY, 				KEY_SEXIT,					0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE_TEMPORARY,				E_KEY,				KEY_CANCEL,					0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE_TEMPORARY,				E_KEY,				ESCAPE,						0 },
	{ ID_WINDOW_MANAGER,	L_HELP,							E_KEY, 				KEY_SHELP,					0 },
	{ ID_WINDOW_MANAGER,	L_HELP,							E_KEY, 				CTRL_R,						0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE,						E_KEY, 				KEY_CLOSE,					0 },
	{ ID_WINDOW_MANAGER,	S_CLOSE,						E_KEY, 				CTRL_D,						0 },
	{ ID_WINDOW_MANAGER,	S_REDISPLAY,					E_KEY, 				KEY_REFRESH,				0 },
	{ ID_WINDOW_MANAGER,	S_REDISPLAY,					E_KEY, 				CTRL_L,						0 },
	{ ID_WINDOW_MANAGER,	L_NEXT_WINDOW,					E_KEY, 				KEY_SNEXT,					0 },
	{ ID_WINDOW_MANAGER,	L_NEXT_WINDOW,					E_KEY, 				CTRL_N,						0 },
	{ ID_WINDOW_MANAGER,	L_MOVE,							E_KEY, 				KEY_MOVE,					0 },
	{ ID_WINDOW_MANAGER,	L_MOVE,							E_KEY, 				CTRL_V,						0 },
	{ ID_WINDOW_MANAGER,	L_SIZE,							E_KEY, 				KEY_SMOVE,					0 },
	{ ID_WINDOW_MANAGER,	L_SIZE,							E_KEY, 				CTRL_B,						0 },
	{ ID_WINDOW_MANAGER,	L_BEGIN_SELECT,					E_MOUSE, 			M_RIGHT | M_RIGHT_CHANGE,	0 },
	{ ID_WINDOW_MANAGER,	L_CONTINUE_SELECT,				E_MOUSE, 			M_RIGHT,					0 },
	{ ID_WINDOW_MANAGER,	L_END_SELECT,					E_MOUSE, 			M_RIGHT_CHANGE,				0 },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#endif

#elif defined(ZIL_MACINTOSH)
static UI_EVENT_MAP ZIL_FARDATA eventMapTable[] =
{
	{ ID_WINDOW_OBJECT,		L_VIEW,							osEvt,				M_LEFT,						0 },
	{ ID_WINDOW_OBJECT,		L_BEGIN_SELECT,					mouseDown,			M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_OBJECT,		L_CONTINUE_SELECT,				osEvt,				M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_OBJECT,		L_END_SELECT,					mouseUp,			M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_OBJECT, 	L_UP,							keyDown, 			WHITE_UP_ARROW,				0 },
	{ ID_WINDOW_OBJECT, 	L_UP,							autoKey, 			WHITE_UP_ARROW,				0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							keyDown, 			WHITE_DOWN_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_DOWN,							autoKey, 			WHITE_DOWN_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							keyDown, 			WHITE_LEFT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_LEFT,							autoKey, 			WHITE_LEFT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						keyDown, 			WHITE_RIGHT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,						autoKey, 			WHITE_RIGHT_ARROW,			0 },
	{ ID_WINDOW_OBJECT, 	L_PGUP,							keyDown, 			WHITE_PGUP,					0 },
	{ ID_WINDOW_OBJECT, 	L_PGUP,							autoKey, 			WHITE_PGUP,					0 },
	{ ID_WINDOW_OBJECT, 	L_PGDN,							keyDown, 			WHITE_PGDN,					0 },
	{ ID_WINDOW_OBJECT, 	L_PGDN,							autoKey, 			WHITE_PGDN,					0 },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,						keyDown, 			TAB,						S_SHIFT },
	{ ID_WINDOW_OBJECT, 	L_NEXT,							keyDown, 			TAB,						0 },
	{ ID_WINDOW_OBJECT,		L_HELP,							keyDown, 			WHITE_QUESTION,				S_CMD },
	{ ID_WINDOW_OBJECT,		L_HELP,							keyDown, 			WHITE_QUESTION,				S_CMD | S_SHIFT },

	{ ID_STRING, 			L_BACKSPACE,					keyDown, 			BACKSPACE,					0 },
	{ ID_STRING, 			L_BACKSPACE,					autoKey, 			BACKSPACE,					0 },
	{ ID_STRING, 			L_DELETE,						keyDown, 			WHITE_DELETE,				0 },
	{ ID_STRING, 			L_DELETE,						autoKey, 			WHITE_DELETE,				0 },
	{ ID_STRING,			L_CUT,							keyDown,			WHITE_X,					S_CMD },
	{ ID_STRING,			L_COPY_MARK,					keyDown,			WHITE_C,					S_CMD },
	{ ID_STRING,			L_PASTE,						keyDown,			WHITE_V,					S_CMD },
	{ ID_STRING, 			L_SELECT,						keyDown, 			ENTER,						0 },

	{ ID_LIST,				L_SELECT,						keyDown, 			SPACE,						0 },
	{ ID_LIST,				S_UNKNOWN,						keyDown, 			ENTER,						0 },

	{ ID_WINDOW, 			L_SELECT,						keyDown, 			ENTER,						0 },
	{ ID_WINDOW,			L_SELECT,			  			keyDown,	 		ENTER,	 					S_OPT },

	{ ID_WINDOW_MANAGER, 	L_BEGIN_SELECT,					mouseDown, 			M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_MANAGER,	L_CONTINUE_SELECT,				osEvt,				M_LEFT | M_LEFT_CHANGE,		0 },
	{ ID_WINDOW_MANAGER, 	L_END_SELECT,					mouseUp, 			M_LEFT_CHANGE,				0 },
	{ ID_WINDOW_MANAGER,	L_EXIT_FUNCTION,				keyDown,			WHITE_Q,					S_CMD },
	{ ID_WINDOW_MANAGER,	L_NEXT_WINDOW,					keyDown,			TAB,						S_OPT },
	{ ID_WINDOW_MANAGER,	S_CLOSE,						keyDown, 			WHITE_W,					S_CMD },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#elif defined(ZIL_NEXTSTEP)
static UI_EVENT_MAP ZIL_FARDATA eventMapTable[] =
{
	{ ID_WINDOW_OBJECT,	L_BEGIN_SELECT,		NX_MOUSEDOWN,		M_LEFT | M_LEFT_CHANGE },
	
	{ ID_WINDOW_OBJECT,	L_CONTINUE_SELECT,	NX_MOUSEDRAGGED,	M_LEFT },
	{ ID_WINDOW_OBJECT,	L_END_SELECT,		NX_MOUSEUP,		M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_ESCAPE,		NX_RMOUSEDOWN, 		M_RIGHT | M_RIGHT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_ESCAPE,	NX_RMOUSEDRAGGED, 	M_RIGHT },
	{ ID_WINDOW_OBJECT, 	L_END_ESCAPE,		NX_RMOUSEUP, 		M_RIGHT_CHANGE },
	
/*
	{ ID_WINDOW_MANAGER,	L_BEGIN_MOVE_DRAG,	NX_MOUSEDOWN, 		M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_MANAGER,	L_CONTINUE_MOVE_DRAG,	NX_MOUSEDRAGGED, 	M_LEFT },
	{ ID_WINDOW_MANAGER,	L_END_MOVE_DRAG,	NX_MOUSEUP, 		M_LEFT_CHANGE },
	{ ID_WINDOW_OBJECT, 	L_BEGIN_COPY_DRAG,	NX_MOUSEDOWN, 		M_LEFT | M_LEFT_CHANGE | S_CTRL },
	{ ID_WINDOW_OBJECT, 	L_CONTINUE_COPY_DRAG,	NX_MOUSEDRAGGED, 	M_LEFT | S_CTRL },
	{ ID_WINDOW_OBJECT, 	L_END_COPY_DRAG,	NX_MOUSEUP, 		M_LEFT_CHANGE | S_CTRL },
*/
	{ ID_WINDOW_OBJECT, 	L_UP,			NX_KEYDOWN, 		GRAY_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_UP,			NX_KEYDOWN, 		WHITE_UP_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,			NX_KEYDOWN, 		GRAY_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_DOWN,			NX_KEYDOWN, 		WHITE_DOWN_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,			NX_KEYDOWN, 		GRAY_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_LEFT,			NX_KEYDOWN, 		WHITE_LEFT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,		NX_KEYDOWN, 		GRAY_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_RIGHT,		NX_KEYDOWN, 		WHITE_RIGHT_ARROW },
	{ ID_WINDOW_OBJECT, 	L_PREVIOUS,		NX_KEYDOWN, 		TAB, S_SHIFT },
	{ ID_WINDOW_OBJECT, 	L_NEXT,			NX_KEYDOWN, 		TAB },

	{ ID_STRING, 		L_BACKSPACE,		NX_KEYDOWN, 		BACKSPACE },

	
	{ ID_WINDOW_MANAGER, 	L_BEGIN_SELECT,		NX_MOUSEDOWN, 	M_LEFT | M_LEFT_CHANGE },
	{ ID_WINDOW_MANAGER, 	L_END_SELECT,		NX_MOUSEUP, 	M_LEFT_CHANGE },

	// End of array.
	{ ID_END, 0, 0, 0 }
};
#endif

UI_EVENT_MAP *_eventMapTable = eventMapTable;
