//	Zinc Interface Library - Z_WIN.CPP
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


#if defined(ZIL_MSWINDOWS)
static UI_WINDOW_MANAGER *_object = ZIL_NULLP(UI_WINDOW_MANAGER);
#if defined(ZIL_WINNT)
static int _offset = GWL_USERDATA;
#	define DS_FIXUP
#else
static int _offset = 0;
#	define DS_FIXUP _export
#endif

LRESULT CALLBACK DS_FIXUP TaskJumpProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	UI_WINDOW_MANAGER *object = (UI_WINDOW_MANAGER *)GetWindowLong(hWnd, _offset);
	return (object->Event(UI_EVENT(E_MSWINDOWS, hWnd, wMsg, wParam, lParam)));
}

LRESULT CALLBACK DS_FIXUP TaskInitProcedure(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	_object->Information(I_SET_TASKID, &hWnd);
	SetWindowLong(hWnd, _offset, (LONG)_object);
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)TaskJumpProcedure);
	UI_EVENT event(E_MSWINDOWS, hWnd, wMsg, wParam, lParam);
	return ((LRESULT)_object->Event(event));
}

static HHOOK _keyboardHook = 0;
DWORD FAR PASCAL KeyboardHook(int code, WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_F1 && (lParam & 0x80000000) && code == HC_ACTION)
	{
		ZIL_SCREENID frameID;
		UI_WINDOW_OBJECT::windowManager->Information(I_GET_TASKID, &frameID);
		// Send a user message in case the window has an open menu.
		SendMessage(frameID, ZM_HELPKEY, 0, 0);
		return 0;
	}

	return CallNextHookEx(_keyboardHook, code, wParam, lParam);
}
#elif defined(ZIL_OS2)
MRESULT EXPENTRY TaskCallback(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	if (msg == WM_MENUSELECT || msg == WM_MENUEND)
	{
		HWND hwndMenu = (HWND)mp2;
		return WinSendMsg(hwndMenu, msg, mp1, mp2);
	}
	return WinDefWindowProc(hwnd, msg, mp1, mp2);
}
#elif (ZIL_MOTIF >= 1002)
#	include <Xm/AtomMgr.h>
#elif defined(ZIL_MACINTOSH)
#	include <AppleEvents.h>
#	include <GestaltEqu.h>

OSErr CheckForRequiredParams(AppleEvent *event)
{
	DescType returnedType;
	Size actualSize;
	OSErr err = AEGetAttributePtr(event, keyMissedKeywordAttr, typeWildCard,
		&returnedType, nil, 0, &actualSize);
	if (err == errAEDescNotFound)	// Got all the required parameters.
		return (noErr);
	else if (err == noErr)			// Missed a required parameter.
		return (errAEParamMissed);
	else							// The call to AEGetAttributePtr() failed.
		return (err);
}

pascal OSErr DoOpenApp(AppleEvent *, AppleEvent *, long)
{
	// Send S_OPEN_DOCUMENT without a filename to allow opening an untitled document window.
	UI_EVENT docEvent(S_OPEN_DOCUMENT);
	docEvent.text = ZIL_NULLP(ZIL_ICHAR);
	UI_WINDOW_OBJECT::eventManager->Put(docEvent);

	return (noErr);
}

pascal OSErr DoOpenDoc(AppleEvent *event, AppleEvent *, long)
{
	// Get the direct parameter - a descriptor list - and put it into docList.
	AEDescList docList = { typeNull, nil };
	OSErr err = AEGetParamDesc(event, keyDirectObject, typeAEList, &docList);

	// Check for missing required parameters.
	err = CheckForRequiredParams(event);
	if (err)
		return (AEDisposeDesc(&docList));

	// Count the number of descriptor records in the list.
	// Should be at least 1, since we got called and no error.
	long numDocs;
	err = AECountItems(&docList, &numDocs);
	if (err)
		return (AEDisposeDesc(&docList));

	// Get each descriptor record from the list.
	AEKeyword keywd;
	DescType returnedType;
	FSSpec fileSpec;
	Size actualSize;
	Str255 fsFilename;
	UI_EVENT docEvent(S_OPEN_DOCUMENT);
	for (long i = 1; i <= numDocs; i++)
	{
		err = AEGetNthPtr(&docList, i, typeFSS, &keywd, &returnedType,
			(Ptr)&fileSpec, sizeof(fileSpec), &actualSize);

		// Construct the full pathname of the file from the FSSpec.
		ZIL_INTERNATIONAL::GetPathNameFromDirID(fsFilename, fileSpec.parID, fileSpec.vRefNum);
		ZIL_INTERNATIONAL::pstrcat(fsFilename, fileSpec.name);

		// Send S_OPEN_DOCUMENT with the filename.
		// The object that processes this event must delete the text member of the event.
		docEvent.text = ZIL_INTERNATIONAL::UnMapText(p2cstr(fsFilename));
		UI_WINDOW_OBJECT::eventManager->Put(docEvent);
	}

	return (AEDisposeDesc(&docList));
}

pascal OSErr DoPrintDoc(AppleEvent *event, AppleEvent *, long)
{
	// Get the direct parameter - a descriptor list - and put it into docList.
	AEDescList docList = { typeNull, nil };
	OSErr err = AEGetParamDesc(event, keyDirectObject, typeAEList, &docList);

	// Check for missing required parameters.
	err = CheckForRequiredParams(event);
	if (err)
		return (AEDisposeDesc(&docList));

	// Count the number of descriptor records in the list.
	// Should be at least 1, since we got called and no error.
	long numDocs;
	err = AECountItems(&docList, &numDocs);
	if (err)
		return (AEDisposeDesc(&docList));

	// Get each descriptor record from the list.
	AEKeyword keywd;
	DescType returnedType;
	FSSpec fileSpec;
	Size actualSize;
	Str255 fsFilename;
	UI_EVENT docEvent(S_PRINT_DOCUMENT);
	for (long i = 1; i <= numDocs; i++)
	{
		err = AEGetNthPtr(&docList, i, typeFSS, &keywd, &returnedType,
			(Ptr)&fileSpec, sizeof(fileSpec), &actualSize);

		// Construct the full pathname of the file from the FSSpec.
		ZIL_INTERNATIONAL::GetPathNameFromDirID(fsFilename, fileSpec.parID, fileSpec.vRefNum);
		ZIL_INTERNATIONAL::pstrcat(fsFilename, fileSpec.name);

		// Send S_PRINT_DOCUMENT with the filename.
		// The object that processes this event must delete the text member of the event.
		docEvent.text = ZIL_INTERNATIONAL::UnMapText(p2cstr(fsFilename));
		UI_WINDOW_OBJECT::eventManager->Put(docEvent);
	}

	return (AEDisposeDesc(&docList));
}

pascal OSErr DoQuitApp(AppleEvent *, AppleEvent *, long)
{
	UI_WINDOW_OBJECT::eventManager->Put(L_EXIT_FUNCTION);
	return (noErr);
}
#endif

// ----- UI_WINDOW_MANAGER --------------------------------------------------

UI_WINDOW_MANAGER::UI_WINDOW_MANAGER(UI_DISPLAY *_display,
	UI_EVENT_MANAGER *_eventManager, ZIL_EXIT_FUNCTION _exitFunction) :
	UIW_WINDOW(0, 0, _display->columns, _display->lines),
	exitFunction(_exitFunction), dragObject(ZIL_NULLP(UI_WINDOW_OBJECT))
{
	extern void z_win_dummy(void);
	z_win_dummy();
	extern void z_event_dummy(void);
	z_event_dummy();
	extern void z_pnorm_dummy(void);
	z_pnorm_dummy();
#if (defined(ZIL_LOAD) || defined(ZIL_STORE)) && defined(ZIL_LINKBUG)
	extern void z_jump_dummy(void);
	z_jump_dummy();
#endif

	// Initialize the window manager information.
	windowID[0] = ID_WINDOW_MANAGER;
	windowID[1] = ID_WINDOW;

#if !defined(ZIL_TEXT_ONLY)
	if (!_display->isText)
		woStatus |= WOS_GRAPHICS;
#endif
	screenID = ID_SCREEN;
	display = _display;
	eventManager = _eventManager;
	windowManager = this;

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	currentScreenID = ID_SCREEN;
#elif defined(ZIL_MSWINDOWS)

	// Set up the task name.
#	if defined(ZIL_UNICODE) && defined(ZIL_WINNT)
	wchar_t *taskClassName = ISOtoUNICODE("ZINC_TASK_MANAGER");
	wchar_t *minIconName = ISOtoUNICODE("minIcon");
#	else
	char *taskClassName = "ZINC_TASK_MANAGER";
	char *minIconName = "minIcon";
#	endif

	WNDCLASS wc;
	wc.style = 0;
//	wc.style = WS_MINIMIZE;
	wc.lpfnWndProc = TaskInitProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(UI_WINDOW_OBJECT *);
	wc.hInstance = display->hInstance;
	wc.hIcon = LoadIcon(display->hInstance, minIconName);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = taskClassName;
	RegisterClass(&wc);

#	if defined (ZIL_UNICODE) && !defined(ZIL_WINNT)
	char title[256];
	GetModuleFileName(display->hInstance, title, 256);
	char *taskName = title + ::strlen(title);
#	else
	ZIL_ICHAR title[256];
	GetModuleFileName(display->hInstance, title, 256);
	ZIL_ICHAR *taskName = title + strlen(title);
#	endif
	while (taskName != title && *taskName != '\\' && *taskName != ':')
		taskName--;
	taskName++;
	_object = this;
	CreateWindowEx(WS_EX_NOPARENTNOTIFY, taskClassName, taskName,
		0, 0, 0, 100, 100, ZIL_NULLH(HWND), ZIL_NULLH(HMENU), display->hInstance, 0);
//		0, -32767, -32767, 0, 0, ZIL_NULLH(HWND), ZIL_NULLH(HMENU), display->hInstance, 0);
#	if defined (ZIL_UNICODE) && defined(ZIL_WINNT)
	delete taskClassName;
#	endif
//	ShowWindow(taskID, SW_SHOWNOACTIVATE);
#	if !defined(ZIL_WINNT)
	// Initialize the class registration module.
	extern int ZincClassInitialize(HANDLE hInstance);
	ZincClassInitialize(display->hInstance);
#	endif

	FARPROC lpfnKbrdHook = MakeProcInstance((FARPROC)KeyboardHook, display->hInstance);
#	if !defined(ZIL_WINNT)
	_keyboardHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)lpfnKbrdHook, display->hInstance, GetCurrentTask());
#	else
	_keyboardHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)lpfnKbrdHook, display->hInstance, GetCurrentThreadId());
#	endif

#elif defined(ZIL_OS2)
	WinRegisterClass(display->hab, (PSZ)"UI_WINDOW_MANAGER", TaskCallback,
		0, 0);
	taskID = WinCreateWindow(HWND_DESKTOP, (PSZ)"UI_WINDOW_MANAGER", (PSZ)0,
		0, 0, 0, display->lines - 1, display->columns - 1, 0, HWND_BOTTOM, 0,
		ZIL_NULLP(void), ZIL_NULLP(void));
#elif defined(ZIL_XT)
	screenID = shell = taskID = display->topShell;
#	if (ZIL_MOTIF >= 1002)
	ZINC_DND = XmInternAtom(display->xDisplay, "ZINC_DND", False);
#	endif
#elif defined(ZIL_MACINTOSH)
	InitCursor();

	// Initialize the Apple Event Manager for high-level events.
	long feature;
	if (Gestalt(gestaltAppleEventsAttr, &feature) == noErr &&
		(feature & (1L << gestaltAppleEventsPresent)))
	{
		// Install event handlers for the four required events.
		AEInstallEventHandler(kCoreEventClass, kAEOpenApplication,
			NewAEEventHandlerProc(DoOpenApp), 0L, FALSE);
		AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
			NewAEEventHandlerProc(DoOpenDoc), 0L, FALSE);
		AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments,
			NewAEEventHandlerProc(DoPrintDoc), 0L, FALSE);
		AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
			NewAEEventHandlerProc(DoQuitApp), 0L, FALSE);
	}
#endif
}

UI_WINDOW_MANAGER::~UI_WINDOW_MANAGER(void)
{
	UI_WINDOW_OBJECT *object;

	// Remove all objects from the window manager.
	for (object = First(); object; object = First())
	{
		UI_LIST::Subtract(object);
#if defined(ZIL_MSWINDOWS)
		if (object->screenID)
		{
			object->woStatus |= WOS_WINDOWS_ACTION;
			ZIL_SCREENID frameID;
			object->Information(I_GET_FRAMEID, &frameID);
			DestroyWindow(frameID);
		}
#elif defined(ZIL_OS2)
		UI_EVENT event(S_DEINITIALIZE);
		event.windowObject = object;
		object->Event(event);
#else
		object->Event(UI_EVENT(S_DEINITIALIZE, 0));
#endif
		if (!FlagSet(object->woAdvancedFlags, WOAF_NO_DESTROY))
			delete object;
	}

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	if (pasteBuffer)
		delete pasteBuffer;
	pasteBuffer = ZIL_NULLP(ZIL_ICHAR);
	pasteLength = 0;
#elif defined(ZIL_MSWINDOWS)
	if (ctlHBrush)
		DeleteObject(ctlHBrush);
	if (taskID)
		DestroyWindow(taskID);
#	if !defined(ZIL_WINNT)
	extern int ZincClassRestore(HANDLE hInstance);
	ZincClassRestore(display->hInstance);
#	endif

	UnhookWindowsHookEx(_keyboardHook);

#elif defined(ZIL_OS2)
	WinDestroyWindow(taskID);
#endif

	windowManager = ZIL_NULLP(UI_WINDOW_MANAGER);
	eventManager = ZIL_NULLP(UI_EVENT_MANAGER);
	display = ZIL_NULLP(UI_DISPLAY);
}

void UI_WINDOW_MANAGER::Center(UI_WINDOW_OBJECT *object)
{
	// Change coordinates so we can work with the window.
	object->RegionConvert(object->relative, TRUE);

	// Check the window size.
	int columns = display->columns;
	int lines = display->lines;
	int width = object->relative.Width();
	if (width > columns)
		width = (int)columns;
	int height = object->relative.Height();
	if (height > lines)
		height = (int)lines;

	// Update the window according to the center position.
	object->relative.left = (columns - width) / 2;
	object->relative.top = (lines - height) / 2;
	object->relative.right = object->relative.left + width - 1;
	object->relative.bottom = object->relative.top + height - 1;

#if defined(ZIL_MACINTOSH)
	// Account for growIcon and title bar.
	if (!FlagSet(object->woAdvancedFlags, WOAF_NO_SIZE))
	{
		int delta = UIW_BORDER::width / 2;
		object->relative.left -= delta;
		object->relative.right -= delta;
		object->relative.top -= delta;
		object->relative.bottom -= delta;
	}
	UI_WINDOW_OBJECT *supportObj = ZIL_NULLP(UI_WINDOW_OBJECT);
	object->Information(I_GET_SUPPORT_FIRST, &supportObj);
	for (; supportObj; supportObj = supportObj->Next())
		if (supportObj->Inherited(ID_TITLE))
		{
			object->relative.top -= 10;
			object->relative.bottom -= 10;
			break;
		}
#endif

	object->trueRegion = object->relative;

	// Re-display the window if it already is present on the screen.
	if (Index(object) != -1)
		UI_WINDOW_MANAGER::Add(object);
}

void *UI_WINDOW_MANAGER::Information(ZIL_INFO_REQUEST request, void *data, ZIL_OBJECTID objectID)
{
	// Switch on the request.
	if (objectID == ID_DEFAULT)
		objectID = ID_WINDOW_MANAGER;
	switch (request)
	{
	case I_GET_NUMBERID_OBJECT:
	case I_GET_STRINGID_OBJECT:
		{
		void *match = ZIL_NULLP(void);
		for (UI_WINDOW_OBJECT *object = First(); object && !match; object = object->Next())
			match = object->UI_WINDOW_OBJECT::Information(request, data, objectID);
		data = match;
		}
		break;

#if defined(ZIL_MSWINDOWS)
	case I_SET_TASKID:
		taskID = *(ZIL_SCREENID*)data;
		break;

	case I_SET_TEXT:
		{
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
		char *TEXT_ = MapText((ZIL_ICHAR *)data, ZIL_NULLP(char), FALSE);
#else
#		define TEXT_ data
#endif
		SendMessage(taskID, WM_SETTEXT, 0, (LONG)TEXT_);
		}
		break;

	case I_GET_TEXT:
		{
		static ZIL_ICHAR ZIL_FARDATA text[128];
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
		char tmpBuff[128*sizeof(ZIL_ICHAR)];
		SendMessage(taskID, WM_GETTEXT, 128*sizeof(ZIL_ICHAR), (LONG)tmpBuff);
		UnMapText(tmpBuff, text);
#else
		SendMessage(taskID, WM_GETTEXT, 128, (LONG)text);
#endif
		*(ZIL_ICHAR **)data = text;
		}
		break;

	case I_COPY_TEXT:
		{
#if defined(ZIL_UNICODE) && !defined(ZIL_WINNT)
		char tmpBuff[128];
		SendMessage(taskID, WM_GETTEXT, 128, (LONG)tmpBuff);
		UnMapText(tmpBuff, (ZIL_ICHAR *)data);
#else
		SendMessage(taskID, WM_GETTEXT, 128, (LONG)data);
#endif
		}
		break;
#endif
#if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	case I_GET_TASKID:
		*(ZIL_SCREENID *)data = taskID;
		break;
#endif

	default:
		return (ZIL_NULLP(void));
	}

	// Return the information.
	return (data);
}
