//	Zinc Interface Library - UI_EVT.HPP
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


#if !defined(UI_EVT_HPP)
#	define UI_EVT_HPP
//#	if !defined(UI_MAP_HPP)
#		include <ui_map.hpp>
//#	endif
#if defined(ZIL_NEXTSTEP)
extern "C"
{
#	include <dpsclient/event.h>
#	include <appkit/timer.h>
}
#endif

// --- EVENT_TYPE ---

#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	typedef ZIL_INT16 ZIL_EVENT_TYPE;
#elif defined(ZIL_WINNT)
	typedef LRESULT ZIL_EVENT_TYPE;
#	define ZM_CONVERTPOINT WM_USER + 0x1234
#	define ZM_CONVERTNCPOINT WM_USER + 0x1235
#	define ZM_HELPKEY WM_USER + 0x1236
#elif defined(ZIL_MSWINDOWS)
	typedef ZIL_INT32 ZIL_EVENT_TYPE;
#	define ZM_CONVERTPOINT WM_USER + 0x1234
#	define ZM_CONVERTNCPOINT WM_USER + 0x1235
#	define ZM_HELPKEY WM_USER + 0x1236
#elif defined(ZIL_OS2)
	typedef ZIL_INT32 ZIL_EVENT_TYPE;
#elif defined(ZIL_XT)
	typedef ZIL_INT32 ZIL_EVENT_TYPE;
#elif defined(ZIL_MACINTOSH)
	typedef ZIL_INT32 ZIL_EVENT_TYPE;
#elif defined(ZIL_NEXTSTEP)
	typedef NXEvent MSG;
	typedef ZIL_INT32 ZIL_EVENT_TYPE;
#endif
typedef ZIL_EVENT_TYPE EVENT_TYPE; // Will be moved to olddefs in the future.

// --------------------------------------------------------------------------
// ----- UI_KEY -------------------------------------------------------------
// --------------------------------------------------------------------------
// --- event.type is E_KEY
// --- event.rawCode is the key shift state (high byte) and scan code (low byte)
// --- event.key contains the key shift-state and character code

// --- shiftState ---
const ZIL_RAW_CODE S_SHIFT					= 0x0003;	// left- or right-shift
const ZIL_RAW_CODE S_RIGHT_SHIFT			= 0x0001;
const ZIL_RAW_CODE S_LEFT_SHIFT				= 0x0002;
const ZIL_RAW_CODE S_CTRL					= 0x0004;
const ZIL_RAW_CODE S_ALT					= 0x0008;
const ZIL_RAW_CODE S_SCROLL_LOCK			= 0x0010;
const ZIL_RAW_CODE S_NUM_LOCK				= 0x0020;
const ZIL_RAW_CODE S_CAPS_LOCK				= 0x0040;
const ZIL_RAW_CODE S_INSERT					= 0x0080;
#if defined(ZIL_MACINTOSH)
const ZIL_RAW_CODE S_OPT					= S_ALT;
const ZIL_RAW_CODE S_CMD					= 0x0100;
#endif

struct ZIL_EXPORT_CLASS UI_KEY
{
	ZIL_RAW_CODE shiftState;
	ZIL_RAW_CODE value;
};

// --------------------------------------------------------------------------
// ----- Mouse Information --------------------------------------------------
// --------------------------------------------------------------------------
// --- event.type is E_MOUSE
// --- event.rawCode is the mouse shift state (high byte) and button status (low byte)
// --- event.position.line and event.position.column contains the mouse position

const ZIL_RAW_CODE M_LEFT					= 0x0100;
const ZIL_RAW_CODE M_RIGHT					= 0x0200;
const ZIL_RAW_CODE M_MIDDLE					= 0x0400;
const ZIL_RAW_CODE M_TOP_CHANGE				= 0x0800;	// only used in sizing operation
const ZIL_RAW_CODE M_LEFT_CHANGE			= 0x1000;
const ZIL_RAW_CODE M_RIGHT_CHANGE			= 0x2000;
const ZIL_RAW_CODE M_MIDDLE_CHANGE			= 0x4000;
const ZIL_RAW_CODE M_BOTTOM_CHANGE			= 0x8000;	// only used in sizing operation

// --------------------------------------------------------------------------
// ----- UI_SCROLL_INFORMATION ----------------------------------------------
// --------------------------------------------------------------------------
// --- event.type is S_VSCROLL_* or S_HSCROLL_*
// --- event.scroll.current is current scroll position
// --- event.scroll.minimun is minimum scroll position
// --- event.scroll.maximum is maximum scroll position
// --- event.scroll.showing is fraction of scroll showing on screen.
// --- event.scroll.delta is scrolling increment

struct ZIL_EXPORT_CLASS UI_SCROLL_INFORMATION
{
	ZIL_INT16 current;
	ZIL_INT16 minimum;
	ZIL_INT16 maximum;
	ZIL_INT16 showing;
	ZIL_INT16 delta;
};

// --------------------------------------------------------------------------
// ----- UI_EVENT -----------------------------------------------------------
// --------------------------------------------------------------------------

typedef EVENT_TYPE ZIL_DIALOG_EVENT;
const ZIL_DIALOG_EVENT DLG_DIALOG_FIRST		= -2999;
const ZIL_DIALOG_EVENT DLG_DIALOG_LAST		= -2000;
const ZIL_DIALOG_EVENT DLG_DIALOG_USER		= -2499;

typedef EVENT_TYPE ZIL_DEVICE_STATE;
const ZIL_DEVICE_STATE D_DEVICE_FIRST		= -1999;
const ZIL_DEVICE_STATE D_DEVICE_LAST		= -1000;

typedef EVENT_TYPE ZIL_SYSTEM_EVENT;
const ZIL_SYSTEM_EVENT S_SYSTEM_FIRST		= -999;
const ZIL_SYSTEM_EVENT S_SYSTEM_LAST		= -1;

typedef EVENT_TYPE ZIL_DEVICE_TYPE;
const ZIL_DEVICE_TYPE E_DEVICE_FIRST		= 0;
const ZIL_DEVICE_TYPE E_DEVICE_LAST			= 99;

typedef EVENT_TYPE ZIL_LOGICAL_EVENT;
const ZIL_LOGICAL_EVENT L_LOGICAL_FIRST		= 100;
const ZIL_LOGICAL_EVENT L_LOGICAL_LAST		= 9999;

typedef EVENT_TYPE ZIL_DESIGNER_EVENT;
const ZIL_DESIGNER_EVENT D_DESIGNER_FIRST	= 5000;
const ZIL_DESIGNER_EVENT D_DESIGNER_LAST	= 9999;

typedef EVENT_TYPE ZIL_USER_EVENT;
const ZIL_USER_EVENT U_USER_FIRST			= 10000;
const ZIL_USER_EVENT U_USER_LAST			= 32767;

class ZIL_EXPORT_CLASS UI_DEVICE;
class ZIL_EXPORT_CLASS UI_EVENT_MANAGER;
class ZIL_EXPORT_CLASS UI_WINDOW_MANAGER;
class ZIL_EXPORT_CLASS UIW_WINDOW;

struct ZIL_EXPORT_CLASS UI_EVENT
{
	// Declaration of classes used by UI_EVENT.
	friend class ZIL_EXPORT_CLASS UI_DEVICE;
	friend class ZIL_EXPORT_CLASS UI_DISPLAY;
	friend class ZIL_EXPORT_CLASS UI_EVENT_MANAGER;
	friend class ZIL_EXPORT_CLASS UI_WINDOW_OBJECT;
	friend class ZIL_EXPORT_CLASS UIW_WINDOW;
	friend class ZIL_EXPORT_CLASS UI_WINDOW_MANAGER;

	EVENT_TYPE type;
	ZIL_RAW_CODE rawCode;
	ZIL_RAW_CODE modifiers;
#if defined(ZIL_MSWINDOWS)
	MSG message;
#elif defined(ZIL_OS2)
	QMSG message;
#elif defined(ZIL_XT)
	union
	{
		XEvent message;
		UI_WINDOW_OBJECT *routeTarget;	// Private member -- Do Not Use!
	};
#elif defined(ZIL_MACINTOSH)
	EventRecord message;
#elif defined(ZIL_NEXTSTEP)
	NXEvent message;
#endif

	union
	{
		UI_KEY key;
		UI_REGION region;
		UI_POSITION position;
		UI_SCROLL_INFORMATION scroll;
		UI_EVENT *event;

		UI_DEVICE *device;
		UI_DISPLAY *display;
		UI_EVENT_MANAGER *eventManager;
		UI_WINDOW_OBJECT *windowObject;
		UIW_WINDOW *window;
		UI_WINDOW_MANAGER *windowManager;

		ZIL_ICHAR *text;

		void *data;
	};

	UI_EVENT(void);
	UI_EVENT(EVENT_TYPE type, ZIL_RAW_CODE rawCode = 0);
	UI_EVENT(EVENT_TYPE type, ZIL_RAW_CODE rawCode, const UI_KEY &key);
	UI_EVENT(EVENT_TYPE type, ZIL_RAW_CODE rawCode, const UI_REGION &region);
	UI_EVENT(EVENT_TYPE type, ZIL_RAW_CODE rawCode, const UI_POSITION &position);
	UI_EVENT(EVENT_TYPE type, ZIL_RAW_CODE rawCode, const UI_SCROLL_INFORMATION &scroll);
#if defined(ZIL_MSWINDOWS)
	UI_EVENT(EVENT_TYPE type, HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
#elif defined(ZIL_OS2)
	UI_EVENT(EVENT_TYPE type, HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#elif defined(ZIL_XT)
	UI_EVENT(EVENT_TYPE _type, XEvent &xevent);
#elif defined(ZIL_MACINTOSH)
	UI_EVENT(EVENT_TYPE type, EventRecord &mevent);
#elif defined(ZIL_NEXTSTEP)
	UI_EVENT(EVENT_TYPE type, NXEvent &nevent);
#endif
	EVENT_TYPE InputType(void) const;
};

// ----- System wide messages -----------------------------------------------

// --- ZIL_SYSTEM_EVENT ---
const ZIL_SYSTEM_EVENT S_ALT_KEY			= -6;	// No union information
const ZIL_SYSTEM_EVENT S_CONTINUE			= -7;
const ZIL_SYSTEM_EVENT S_CREATE				= -10;
const ZIL_SYSTEM_EVENT S_DEINITIALIZE		= -20;
const ZIL_SYSTEM_EVENT S_ERROR				= -1;
const ZIL_SYSTEM_EVENT S_NO_OBJECT			= -3;
const ZIL_SYSTEM_EVENT S_INITIALIZE			= -9;
const ZIL_SYSTEM_EVENT S_REDISPLAY			= -17;
const ZIL_SYSTEM_EVENT S_UNKNOWN			= -2;

const ZIL_SYSTEM_EVENT S_POSITION			= -110;	// UI_POSITION information

const ZIL_SYSTEM_EVENT S_RESET_DISPLAY		= -410;	// UI_DISPLAY

// --- ZIL_LOGICAL_EVENT ---
const ZIL_LOGICAL_EVENT L_EXIT				= 1000;

// --------------------------------------------------------------------------
// ----- UI_DEVICE ----------------------------------------------------------
// --------------------------------------------------------------------------

// --- Device type messages ---
const ZIL_DEVICE_TYPE E_MSWINDOWS			= 1;
const ZIL_DEVICE_TYPE E_OS2					= 2;
const ZIL_DEVICE_TYPE E_XT					= 3;
const ZIL_DEVICE_TYPE E_MOTIF				= E_XT;
const ZIL_DEVICE_TYPE E_MACINTOSH			= 4;
const ZIL_DEVICE_TYPE E_KEY 				= 10;
const ZIL_DEVICE_TYPE E_NEXTSTEP			= 11;
const ZIL_DEVICE_TYPE E_MOUSE 				= 30;
const ZIL_DEVICE_TYPE E_CURSOR				= 50;
const ZIL_DEVICE_TYPE E_TIMER				= 70;
const ZIL_DEVICE_TYPE E_DEVICE				= 99;

// --- Device state messages ---
const ZIL_DEVICE_STATE D_OFF				= -1000;
const ZIL_DEVICE_STATE D_ON					= -1001;
const ZIL_DEVICE_STATE D_HIDE				= -1002;
const ZIL_DEVICE_STATE D_ACTIVATE			= -1003;
const ZIL_DEVICE_STATE D_DEACTIVATE			= -1004;
const ZIL_DEVICE_STATE D_STATE				= -1005;

// --- Device image messages ---
typedef ZIL_DEVICE_STATE DEVICE_IMAGE;

// Private ALT key state enum for UI_DEVICE.
enum ALT_STATE
{
	ALT_NOT_PRESSED,
	ALT_PRESSED_NO_EVENTS,
	ALT_PRESSED_EVENTS
};

class ZIL_EXPORT_CLASS UI_DEVICE : public UI_ELEMENT
{
	friend class ZIL_EXPORT_CLASS UI_EVENT_MANAGER;
public:
	static ALT_STATE altState;
	static UI_DISPLAY *display;
	static UI_EVENT_MANAGER *eventManager;

	int installed;
	ZIL_DEVICE_TYPE type;
	ZIL_DEVICE_STATE state;

	virtual ~UI_DEVICE(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event) = 0;

	// List members.
	UI_DEVICE *Next(void) { return((UI_DEVICE *)next); }
	UI_DEVICE *Previous(void) { return((UI_DEVICE *)previous); }

protected:
	UI_DEVICE(ZIL_DEVICE_TYPE type, ZIL_DEVICE_STATE state);
	static int CompareDevices(void *device1, void *device2);
	virtual void Poll(void) = 0;
};

// --------------------------------------------------------------------------
// ----- UID_CURSOR ---------------------------------------------------------
// --------------------------------------------------------------------------

// --- Cursor image messages ---
const DEVICE_IMAGE DC_INSERT				= -1100;
const DEVICE_IMAGE DC_OVERSTRIKE			= -1101;

class ZIL_EXPORT_CLASS UID_CURSOR : public UI_DEVICE, public ZIL_INTERNATIONAL
{
public:
	static int blinkRate;

	DEVICE_IMAGE image;
	UI_POSITION position;

	UID_CURSOR(ZIL_DEVICE_STATE state = D_OFF, DEVICE_IMAGE image = DC_INSERT);
	virtual ~UID_CURSOR(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	UI_POSITION offset;

	virtual void Poll(void);
};

// --------------------------------------------------------------------------
// ----- UID_KEYBOARD -------------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UID_KEYBOARD : public UI_DEVICE, public ZIL_INTERNATIONAL
{
public:
	static EVENT_TYPE breakHandlerSet;

	UID_KEYBOARD(ZIL_DEVICE_STATE state = D_ON);
	virtual ~UID_KEYBOARD(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	virtual void Poll(void);
};

// --------------------------------------------------------------------------
// ----- UID_MOUSE ----------------------------------------------------------
// --------------------------------------------------------------------------

// --- Mouse image messages ---
const DEVICE_IMAGE DM_VIEW					= -1200;
const DEVICE_IMAGE DM_EDIT					= -1201;
const DEVICE_IMAGE DM_WAIT					= -1202;
const DEVICE_IMAGE DM_MOVE					= -1203;
const DEVICE_IMAGE DM_HORIZONTAL			= -1204;
const DEVICE_IMAGE DM_VERTICAL				= -1205;
const DEVICE_IMAGE DM_DIAGONAL_ULLR			= -1206;
const DEVICE_IMAGE DM_DIAGONAL_LLUR			= -1207;
const DEVICE_IMAGE DM_POSITION_IMAGE				= -1208;
const DEVICE_IMAGE DM_CANCEL				= -1209;
const DEVICE_IMAGE DM_DRAG_MOVE				= -1210;
const DEVICE_IMAGE DM_DRAG_COPY				= -1211;
const DEVICE_IMAGE DM_DRAG_MOVE_MULTIPLE	= -1212;
const DEVICE_IMAGE DM_DRAG_COPY_MULTIPLE	= -1213;
const DEVICE_IMAGE DM_DRAG					= -1214;

class ZIL_EXPORT_CLASS UID_MOUSE : public UI_DEVICE
{
public:
#if defined(ZIL_MSDOS) && !defined(ZIL_TEXT_ONLY)
	static int defaultInitialized;
	static ZIL_ICHAR _className[];
#endif
	DEVICE_IMAGE image;
	UI_POSITION position;

	UID_MOUSE(ZIL_DEVICE_STATE state = D_ON, DEVICE_IMAGE image = DM_WAIT);
	virtual ~UID_MOUSE(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

protected:
	UI_POSITION offset;

	virtual void Poll(void);

	// I18N member variables and functions.
#if defined(ZIL_MSDOS) && !defined(ZIL_TEXT_ONLY)
	const ZIL_DECORATION *myDecorations;
#endif

private:
#if defined (ZIL_MSWINDOWS)
	HCURSOR cancelCursor;
	HCURSOR moveCursor;
	HCURSOR copyCursor;
	HCURSOR moveMultipleCursor;
	HCURSOR copyMultipleCursor;
#elif defined (ZIL_XT)
	Cursor waitCursor;
	Cursor dragCursor;
	Cursor cancelCursor;
#endif

public:
#if defined(ZIL_MSDOS)
	void MouseMove(int deltaX, int deltaY);
#endif
};

// --------------------------------------------------------------------------
// ----- UID_TIMER ----------------------------------------------------------
// --------------------------------------------------------------------------

// --- TMR_FLAGS ---
typedef UIF_FLAGS TMR_FLAGS;
const TMR_FLAGS TMR_NO_FLAGS				= 0x0000;
const TMR_FLAGS TMR_QUEUE_EVENTS			= 0x0001;

class ZIL_EXPORT_CLASS UID_TIMER : public UI_DEVICE
{
#if defined(ZIL_MSWINDOWS)
	friend void FAR PASCAL TimerCallback(HWND, UINT, UINT, DWORD);
#elif defined(ZIL_XT)
	friend void TimerCallback(XtPointer client_data, XtIntervalId *id);
#endif

public:
	TMR_FLAGS tmrFlags;

	UID_TIMER(ZIL_DEVICE_STATE state = D_ON, unsigned interval = 0,
		TMR_FLAGS tmrFlags = TMR_NO_FLAGS);
	virtual ~UID_TIMER(void);
	virtual EVENT_TYPE Event(const UI_EVENT &event);

	// NOTIFY_ELEMENT and NOTIFY_LIST should be treated as protected,
	// but this causes compiler errors on some systems.
	class NOTIFY_ELEMENT : public UI_ELEMENT
	{
	public:
		UI_WINDOW_OBJECT *notifyObject;
#if defined(ZIL_NEXTSTEP)
		NOTIFY_ELEMENT(UI_WINDOW_OBJECT *object)
		{
			notifyObject = object;
		};
		virtual ~NOTIFY_ELEMENT(void){ };
		NOTIFY_ELEMENT *Next(void) { return ((UID_TIMER::NOTIFY_ELEMENT *)next); }
		NOTIFY_ELEMENT *Previous(void) { return ((UID_TIMER::NOTIFY_ELEMENT *)previous); }
#else
		NOTIFY_ELEMENT(UI_WINDOW_OBJECT *object);
		virtual ~NOTIFY_ELEMENT(void);
		NOTIFY_ELEMENT *Next(void) { return ((NOTIFY_ELEMENT *)next); }
		NOTIFY_ELEMENT *Previous(void) { return ((NOTIFY_ELEMENT *)previous); }
#endif
	};

	class NOTIFY_LIST : public UI_LIST
	{
	public:
#if defined(ZIL_NEXTSTEP)
		NOTIFY_ELEMENT *First(void) { return ((UID_TIMER::NOTIFY_ELEMENT *)first); }
		NOTIFY_ELEMENT *Last(void) { return ((UID_TIMER::NOTIFY_ELEMENT *)last); }
#else
		NOTIFY_ELEMENT *First(void) { return ((NOTIFY_ELEMENT *)first); }
		NOTIFY_ELEMENT *Last(void) { return ((NOTIFY_ELEMENT *)last); }
#endif
	};

protected:
	unsigned msec;
	ZIL_UTIME interval;
	ZIL_UTIME lastTime;
	NOTIFY_LIST notifyList;

	virtual void Poll(void);

private:
#if defined (ZIL_OS2)
	ULONG timerHandle;
#elif defined (ZIL_MSWINDOWS)
	WORD timerHandle;
#elif defined(ZIL_XT)
	XtIntervalId timerHandle;
#endif
};

// --------------------------------------------------------------------------
// ----- UI_QUEUE_ELEMENT & UI_QUEUE_BLOCK ----------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS UI_QUEUE_ELEMENT : public UI_ELEMENT
{
public:
	UI_QUEUE_ELEMENT(void);
	~UI_QUEUE_ELEMENT(void);
	UI_EVENT event;

	UI_QUEUE_ELEMENT *Next(void) { return((UI_QUEUE_ELEMENT *)next); }
	UI_QUEUE_ELEMENT *Previous(void) { return((UI_QUEUE_ELEMENT *)previous); }
};

class ZIL_EXPORT_CLASS UI_QUEUE_BLOCK : public UI_LIST_BLOCK
{
public:
	UI_QUEUE_BLOCK(int noOfElements);
	~UI_QUEUE_BLOCK(void);

	UI_QUEUE_ELEMENT *Current(void) { return((UI_QUEUE_ELEMENT *)current); }
	UI_QUEUE_ELEMENT *First(void) { return((UI_QUEUE_ELEMENT *)first); }
	UI_QUEUE_ELEMENT *Last(void) { return((UI_QUEUE_ELEMENT *)last); }
};

// --------------------------------------------------------------------------
// ----- UI_EVENT_MANAGER ---------------------------------------------------
// --------------------------------------------------------------------------

// --- Q_FLAGS ---
typedef unsigned Q_FLAGS;
const Q_FLAGS Q_NORMAL						= 0x0000;	// block, begin, destroy and poll
const Q_FLAGS Q_BLOCK						= 0x0000;
const Q_FLAGS Q_NO_BLOCK					= 0x0001;
const Q_FLAGS Q_BEGIN						= 0x0000;
const Q_FLAGS Q_END							= 0x0002;
const Q_FLAGS Q_DESTROY						= 0x0000;
const Q_FLAGS Q_NO_DESTROY					= 0x0004;
const Q_FLAGS Q_POLL						= 0x0000;
const Q_FLAGS Q_NO_POLL						= 0x0008;

class ZIL_EXPORT_CLASS UI_EVENT_MANAGER : public UI_LIST
{
public:
	UI_EVENT_MANAGER(UI_DISPLAY *display, int noOfElements = 100);
	virtual ~UI_EVENT_MANAGER(void);
	EVENT_TYPE DevicePosition(ZIL_DEVICE_TYPE deviceType, int column, int line);
	EVENT_TYPE DeviceState(ZIL_DEVICE_TYPE deviceType, ZIL_DEVICE_STATE deviceState);
	EVENT_TYPE DeviceImage(ZIL_DEVICE_TYPE deviceType, DEVICE_IMAGE deviceImage);
	virtual EVENT_TYPE Event(const UI_EVENT &event, ZIL_DEVICE_TYPE deviceType = E_DEVICE);
	virtual int Get(UI_EVENT &event, Q_FLAGS flags = Q_NORMAL);
	virtual void Put(const UI_EVENT &event, Q_FLAGS flags = Q_END);
	Q_FLAGS QFlags(void) { return (qFlags); }

	// List members.
	void Add(UI_DEVICE *device);
	UI_DEVICE *Current(void) { return((UI_DEVICE *)current); }
	UI_DEVICE *First(void) { return((UI_DEVICE *)first); }
	UI_DEVICE *Last(void) { return((UI_DEVICE *)last); }
	void Subtract(UI_DEVICE *device);
	UI_EVENT_MANAGER &operator+(UI_DEVICE *device) { Add(device); return(*this); }
	UI_EVENT_MANAGER &operator-(UI_DEVICE *device) { Subtract(device); return(*this); }

	// Version 2.0 and 1.0 compatibility.
	UI_EVENT_MANAGER(int noOfElements, UI_DISPLAY *display);

protected:
	int level;
	Q_FLAGS qFlags;
	UI_DISPLAY *display;
	UI_QUEUE_BLOCK queueBlock;
#if defined(ZIL_OS2)
	HMQ hmq;
#elif defined(ZIL_MACINTOSH)
	RgnHandle eventMouseRegion;
#endif
};

// --- Version 3.6 compatibility ---
#if defined(ZIL_OLD_DEFS)

#define DIALOG_EVENT			ZIL_DIALOG_EVENT
#define DEVICE_STATE			ZIL_DEVICE_STATE
#define SYSTEM_EVENT			ZIL_SYSTEM_EVENT
#define DEVICE_TYPE				ZIL_DEVICE_TYPE
#define LOGICAL_EVENT			ZIL_LOGICAL_EVENT
#define DESIGNER_EVENT			ZIL_DESIGNER_EVENT
#define USER_EVENT				ZIL_USER_EVENT

// --- Version 2.0 and 1.0 compatibility ---
#	define UI_CURSOR				UID_CURSOR
#	define UI_BIOS_KEYBOARD			UID_KEYBOARD
#	define UI_MS_MOUSE				UID_MOUSE
#	define S_SCROLL_VERTICAL		S_VSCROLL
#	define S_SCROLL_HORIZONTAL		S_HSCROLL
#	define S_SCROLL_VERTICAL_SET	S_VSCROLL_SET
#	define S_SCROLL_HORIZONTAL_SET	S_HSCROLL_SET
#	define S_DELETE					S_CLOSE
#	define S_DELETE_LEVEL			S_CLOSE_TEMPORARY
#	define S_CLEAR					S_DEINITIALIZE
#	define S_DEFINE_REGION			S_REGION_DEFINE
#	define S_CANCEL					L_CANCEL
#	define L_ALT_KEY				S_ALT_KEY
#endif

#endif

