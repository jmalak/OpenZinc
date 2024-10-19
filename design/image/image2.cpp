//	Zinc Application Framework - IMAGE2.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#if defined(ZIL_NEXTSTEP)
#	include "n_image.hpp"
#endif

#define SPECIAL_GROUPING ID_WINDOW
#define GENERAL_GROUPING ID_WINDOW_OBJECT
const EVENT_TYPE L_BEGIN_GROUPING			= 6015;
const EVENT_TYPE L_CONTINUE_GROUPING		= 6016;
const EVENT_TYPE L_END_GROUPING				= 6017;
const EVENT_TYPE L_BEGIN_PATTERN			= 6018;
const EVENT_TYPE L_CONTINUE_PATTERN			= 6019;
const EVENT_TYPE L_END_PATTERN				= 6020;
static UI_EVENT_MAP _eventTable[] =
{
#if defined(ZIL_MSDOS)
	// General grouping operations.
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		E_MOUSE,		S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ SPECIAL_GROUPING,	L_CONTINUE_GROUPING,	E_MOUSE, 		S_CTRL | M_LEFT,				0 },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			E_MOUSE, 		S_CTRL | M_LEFT_CHANGE,			0 },
	// Command-line invoked grouping operations.
	{ GENERAL_GROUPING, L_BEGIN_PATTERN,		E_MOUSE, 		M_LEFT | M_LEFT_CHANGE,			0 },
	{ GENERAL_GROUPING, L_BEGIN_PATTERN,		E_MOUSE, 		M_RIGHT | M_RIGHT_CHANGE,		0 },
	{ GENERAL_GROUPING, L_BEGIN_PATTERN,		E_MOUSE, 		M_LEFT | M_RIGHT | M_LEFT_CHANGE | M_RIGHT_CHANGE,	0 },
	{ GENERAL_GROUPING,	L_BEGIN_PATTERN,		E_MOUSE,		S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ GENERAL_GROUPING, L_CONTINUE_PATTERN,		E_MOUSE, 		M_LEFT,							0 },
	{ GENERAL_GROUPING, L_CONTINUE_PATTERN,		E_MOUSE, 		M_RIGHT,						0 },
	{ GENERAL_GROUPING, L_CONTINUE_PATTERN,		E_MOUSE, 		M_LEFT | M_RIGHT,				0 },
	{ GENERAL_GROUPING,	L_CONTINUE_PATTERN,		E_MOUSE, 		S_CTRL | M_LEFT,				0 },
	{ GENERAL_GROUPING, L_END_PATTERN,			E_MOUSE, 		M_LEFT_CHANGE,					0 },
	{ GENERAL_GROUPING, L_END_PATTERN,			E_MOUSE, 		M_RIGHT_CHANGE,					0 },
	{ GENERAL_GROUPING, L_END_PATTERN,			E_MOUSE, 		M_RIGHT | M_LEFT_CHANGE,		0 },
	{ GENERAL_GROUPING,	L_END_PATTERN,			E_MOUSE, 		S_CTRL | M_LEFT_CHANGE,			0 },
#elif defined(ZIL_MSWINDOWS)
	// General grouping operations.
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		WM_LBUTTONDOWN,	S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ SPECIAL_GROUPING,	L_CONTINUE_GROUPING,	WM_MOUSEMOVE, 	S_CTRL | M_LEFT,				0 },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			WM_LBUTTONUP, 	S_CTRL | M_LEFT_CHANGE,			0 },
	// Command-line invoked grouping operations.
	{ GENERAL_GROUPING, L_BEGIN_PATTERN,		WM_LBUTTONDOWN, M_LEFT | M_LEFT_CHANGE,			0 },
	{ GENERAL_GROUPING, L_BEGIN_PATTERN,		WM_RBUTTONDOWN, M_RIGHT | M_RIGHT_CHANGE,		0 },
	{ GENERAL_GROUPING,	L_BEGIN_PATTERN,		WM_LBUTTONDOWN,	S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ GENERAL_GROUPING, L_CONTINUE_PATTERN,		WM_MOUSEMOVE, 	M_LEFT,							0 },
	{ GENERAL_GROUPING, L_CONTINUE_PATTERN,		WM_MOUSEMOVE, 	M_RIGHT,						0 },
	{ GENERAL_GROUPING,	L_CONTINUE_PATTERN,		WM_MOUSEMOVE, 	S_CTRL | M_LEFT,				0 },
	{ GENERAL_GROUPING, L_END_PATTERN,			WM_LBUTTONUP, 	M_LEFT_CHANGE,					0 },
	{ GENERAL_GROUPING, L_END_PATTERN,			WM_RBUTTONUP, 	M_RIGHT_CHANGE,					0 },
	{ GENERAL_GROUPING,	L_END_PATTERN,			WM_LBUTTONUP, 	S_CTRL | M_LEFT_CHANGE,			0 },
#elif defined(ZIL_OS2)
	// General grouping operations.
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		WM_BUTTON1DOWN,	S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ SPECIAL_GROUPING,	L_CONTINUE_GROUPING,	WM_MOUSEMOVE, 	S_CTRL | M_LEFT,				0 },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			WM_BUTTON1UP, 	S_CTRL | M_LEFT_CHANGE,			0 },
	// Command-line invoked grouping operations.
	{ GENERAL_GROUPING, L_BEGIN_PATTERN,		WM_BUTTON1DOWN, M_LEFT | M_LEFT_CHANGE,			0 },
	{ GENERAL_GROUPING, L_BEGIN_PATTERN,		WM_BUTTON2DOWN, M_RIGHT | M_RIGHT_CHANGE,		0 },
	{ GENERAL_GROUPING,	L_BEGIN_PATTERN,		WM_BUTTON1DOWN,	S_CTRL | M_LEFT | M_LEFT_CHANGE,0 },
	{ GENERAL_GROUPING, L_CONTINUE_PATTERN,		WM_MOUSEMOVE, 	M_LEFT,							0 },
	{ GENERAL_GROUPING, L_CONTINUE_PATTERN,		WM_MOUSEMOVE, 	M_RIGHT,						0 },
	{ GENERAL_GROUPING,	L_CONTINUE_PATTERN,		WM_MOUSEMOVE, 	S_CTRL | M_LEFT,				0 },
	{ GENERAL_GROUPING, L_END_PATTERN,			WM_BUTTON1UP, 	M_LEFT_CHANGE,					0 },
	{ GENERAL_GROUPING, L_END_PATTERN,			WM_BUTTON2UP, 	M_RIGHT_CHANGE,					0 },
	{ GENERAL_GROUPING,	L_END_PATTERN,			WM_BUTTON1UP, 	S_CTRL | M_LEFT_CHANGE,			0 },
#elif defined(ZIL_MOTIF)
	{ GENERAL_GROUPING,	L_BEGIN_PATTERN,		ButtonPress,	M_LEFT | M_LEFT_CHANGE,			0 },
	{ GENERAL_GROUPING,	L_CONTINUE_PATTERN,		MotionNotify,	M_LEFT,							0 },
	{ GENERAL_GROUPING,	L_END_PATTERN,			ButtonRelease,	M_LEFT_CHANGE,					0 },
	{ GENERAL_GROUPING,	L_BEGIN_PATTERN,		ButtonPress,	M_RIGHT | M_RIGHT_CHANGE,		0 },
	{ GENERAL_GROUPING,	L_CONTINUE_PATTERN,		MotionNotify,	M_RIGHT,						0 },
	{ GENERAL_GROUPING,	L_END_PATTERN,			ButtonRelease,	M_RIGHT_CHANGE,					0 },
#elif defined(ZIL_MACINTOSH)
	// Special grouping operations.
	{ SPECIAL_GROUPING,	L_BEGIN_GROUPING,		mouseDown,		M_LEFT | M_LEFT_CHANGE,			S_OPT },
	{ SPECIAL_GROUPING,	L_CONTINUE_PATTERN,		osEvt,			mouseMovedMessage,				S_OPT },
	{ SPECIAL_GROUPING,	L_END_GROUPING,			mouseUp, 		M_LEFT_CHANGE,					S_OPT },
	// General grouping operations.
	{ GENERAL_GROUPING,	L_BEGIN_PATTERN,		mouseDown,		M_LEFT | M_LEFT_CHANGE,			0 },
	{ GENERAL_GROUPING,	L_CONTINUE_PATTERN,		osEvt,			mouseMovedMessage,				0 },
	{ GENERAL_GROUPING,	L_END_PATTERN,			mouseUp,		M_LEFT_CHANGE,					0 },
	{ GENERAL_GROUPING,	L_BEGIN_PATTERN,		mouseDown,		M_LEFT | M_LEFT_CHANGE,			S_OPT },
	{ GENERAL_GROUPING,	L_CONTINUE_PATTERN,		osEvt,			mouseMovedMessage,				S_OPT },
	{ GENERAL_GROUPING,	L_END_PATTERN,			mouseUp, 		M_LEFT_CHANGE,					S_OPT },
#elif defined(ZIL_NEXTSTEP)
	{ GENERAL_GROUPING,	L_BEGIN_PATTERN,		NX_MOUSEDOWN,	M_LEFT | M_LEFT_CHANGE,			0 },
	{ GENERAL_GROUPING,	L_CONTINUE_PATTERN,		NX_MOUSEMOVED,	M_LEFT,							0 },
	{ GENERAL_GROUPING,	L_END_PATTERN,			NX_MOUSEUP,		M_LEFT_CHANGE,					0 },
#endif
	{ ID_END, 0, 0, 0 } // End of array.
};

// Metrowerks bug requires taking the address of the member function.
#if defined(__MWERKS__) || defined(__DECCXX) || defined(__linux)
#	define ZIL_PROCESS_REFERENCE(x) &ZAF_IMAGE::x
#else
#	define ZIL_PROCESS_REFERENCE(x) x
#endif

// --------------------------------------------------------------------------
// ----- ZAF_IMAGE ----------------------------------------------------------
// --------------------------------------------------------------------------

ZAF_IMAGE::ZAF_IMAGE(int left, int top, int width, int height, IMF_FLAGS _imFlags, WOF_FLAGS _woFlags) :
	UI_WINDOW_OBJECT(left, top, width, height, _woFlags, WOAF_NO_FLAGS),
	zoomRatio(1), pixelWidth(1), pixelHeight(1),
	imFlags(_imFlags), updateDisplay(FALSE),
	hotspotX(0), hotspotY(0)
{
	woStatus |= WOS_OWNERDRAW;
	searchID = windowID[0] = ID_IMAGE;
	grid = FlagSet(_imFlags, IMF_GRID) ? 1 : 0;
	bitmapWidth = bitmapHeight = 0;
	bitmapArray = ZIL_NULLP(ZIL_UINT8);

	imageEditor = _serviceManager->Get(ID_IMAGE_EDITOR);
}

ZAF_IMAGE::~ZAF_IMAGE(void)
{
	if (pasteObject == this)
		pasteObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	if (bitmapArray && !FlagSet(imFlags, IMF_STATIC_IMAGE))
		delete bitmapArray;
}

void ZAF_IMAGE::DataGet(ZIL_UINT8 **_bitmapArray, int *_bitmapWidth,
	int *_bitmapHeight, int *_hotspotX, int *_hotspotY)
{
	if (_bitmapWidth)
		*_bitmapWidth = bitmapWidth;
	if (_bitmapHeight)
		*_bitmapHeight = bitmapHeight;
	if (_bitmapArray)
		*_bitmapArray = bitmapArray;
	if (_hotspotX)
		*_hotspotX = hotspotX;
	if (_hotspotY)
		*_hotspotY = hotspotY;
}

void ZAF_IMAGE::DataSet(ZIL_UINT8 *_bitmapArray, int _bitmapWidth,
	int _bitmapHeight, int _hotspotX, int _hotspotY)
{
	// Check for a re-sized image.
	if (_bitmapWidth == -1)
		_bitmapWidth = bitmapWidth;
	if (_bitmapHeight == -1)
		_bitmapHeight = bitmapHeight;
	if (!_bitmapArray && !FlagSet(imFlags, IMF_STATIC_IMAGE))
	{
		_bitmapArray = new ZIL_UINT8[_bitmapWidth * _bitmapHeight];
		memset(_bitmapArray, (char)WHITE, _bitmapWidth * _bitmapHeight);
		int minWidth = MinValue(_bitmapWidth, bitmapWidth);
		int oldX = (bitmapWidth - minWidth) / 2;
		int newX = (_bitmapWidth - minWidth) / 2;
		int minHeight = MinValue(_bitmapHeight, bitmapHeight);
		int oldY = (bitmapHeight - minHeight) / 2;
		int newY = (_bitmapHeight - minHeight) / 2;
		for (int i = 0; i < minHeight; i++)
			memcpy(&_bitmapArray[(i+newY)*_bitmapWidth+newX], &bitmapArray[(i+oldY)*bitmapWidth+oldX], minWidth);
	}

	// Reset the member variables and redisplay the image.
	if (bitmapArray && _bitmapArray && bitmapArray != _bitmapArray && !FlagSet(imFlags, IMF_STATIC_IMAGE))
		delete bitmapArray;
	if (_bitmapArray)
		bitmapArray = _bitmapArray;
	if (_hotspotX != -1)
		hotspotX = _hotspotX;
	if (_hotspotY != -1)
		hotspotY = _hotspotY;
	if (bitmapWidth != _bitmapWidth || bitmapHeight != _bitmapHeight)
	{
		if (screenID)
			display->Rectangle(screenID, trueRegion, lastPalette, 0, TRUE, FALSE, &clip);
		bitmapWidth = _bitmapWidth;
		bitmapHeight = _bitmapHeight;
		if (screenID)
			Event(S_CREATE);
	}
	if (screenID)
		Event(S_REDISPLAY);
}

EVENT_TYPE ZAF_IMAGE::DrawFill(const UI_EVENT &event, EVENT_TYPE ccode, EVENT_TYPE )
{
	updateDisplay = TRUE;
	display->VirtualGet(screenID, trueRegion);
	int column = (event.position.column - trueRegion.left) / pixelWidth;
	int line = (event.position.line - trueRegion.top) / pixelHeight;
	Fill(column, line);
	display->VirtualPut(screenID);
	updateDisplay = FALSE;
	return (ccode);
}

EVENT_TYPE ZAF_IMAGE::DrawLine(const UI_EVENT &_event, EVENT_TYPE ccode, EVENT_TYPE )
{
	// Make sure the specified window is current.
	if (Root() != windowManager->First())
		*windowManager + Root();
	if (!FlagSet(woStatus, WOS_CURRENT))
	{
		UI_EVENT addEvent(S_ADD_OBJECT);
		addEvent.windowObject = this;
		parent->Event(addEvent);
	}

	// Draw the image pattern.
	UI_EVENT event = _event;
	UI_POSITION origin, current;
	int halfWidth = pixelWidth / 2, halfHeight = pixelHeight / 2;
#	if defined(ZIL_MSWINDOWS)
	SetCapture(screenID);
#	elif defined(ZIL_OS2)
	WinSetCapture(HWND_DESKTOP, screenID);
#	endif
	origin.column = origin.line = -1;

	eventManager->DeviceState(E_MOUSE, DM_POSITION_IMAGE);
	Root()->woAdvancedFlags |= WOAF_LOCKED; // Lock the window while drawing the pattern.
	ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, GENERAL_GROUPING);
	do
	{
		// Get the user input.
		if (ccode != L_BEGIN_PATTERN)
		{
#			if defined(ZIL_NEXTSTEP)
			NXEvent *theEvent, tmpEvent;
			int oldMask = [[screenID window] addToEventMask:NX_LMOUSEDRAGGEDMASK];
			[NXApp peekNextEvent:NX_ALLEVENTS
				into:&tmpEvent
				waitFor:NX_FOREVER
				threshold:5];
			NXRect rect;
			NXPoint  where = tmpEvent.location;
			[screenID getBounds:&rect];
			[screenID convertPoint:&where fromView:nil];
			event.position.column = (int)where.x;
			event.position.line = (int)where.y;
			
			if ((tmpEvent.type == NX_LMOUSEDRAGGED ) || (tmpEvent.type == NX_RMOUSEDRAGGED))
			{	
				theEvent = [NXApp getNextEvent:(NX_LMOUSEDRAGGEDMASK | NX_RMOUSEDRAGGEDMASK)];
				event.type = ccode = L_CONTINUE_PATTERN;
			}
			else
			{
				[[screenID window] setEventMask:oldMask];
				event.type = ccode = L_END_PATTERN;
			}
#			else
			eventManager->Get(event);
			ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, GENERAL_GROUPING);
#			endif

			// Normalize the coordinates for later use.
			NormalizePosition(this, event, event.position);
		}

		// Check for mouse events.
		if (ccode == L_END_PATTERN ||
			(ccode == L_BEGIN_PATTERN && !trueRegion.Overlap(event.position)))
		{
			// System must process begin/end mouse messages.
			eventManager->Put(event, Q_BEGIN);
			break;
		}
		else if (event.InputType() != E_MOUSE)
        {
			// Account for system events.
			windowManager->Event(event);
            continue;
        }
		else if (!trueRegion.Overlap(event.position))
            continue; // Mouse doesn't overlap boundary.

		// Modify the group coordinates.
		event.position.column -= (event.position.column - trueRegion.left) % pixelWidth - halfWidth;
		event.position.line -= (event.position.line - trueRegion.top) % pixelHeight - halfHeight;
		if (ccode == L_BEGIN_PATTERN)
		{
			origin = current = event.position;
			display->VirtualGet(screenID, trueRegion);
			display->Line(screenID, origin.column, origin.line,
				current.column, current.line, display->xorPalette, 1, TRUE);
			display->VirtualPut(screenID);
		}
		else if (ccode == L_CONTINUE_PATTERN)
		{
			// Change the line.
			if (current != event.position)
			{
				display->VirtualGet(screenID, trueRegion);
				display->Line(screenID, origin.column, origin.line, current.column, current.line, display->xorPalette, 1, TRUE);
				current = event.position;
				display->Line(screenID, origin.column, origin.line, current.column, current.line, display->xorPalette, 1, TRUE);
				display->VirtualPut(screenID);
			}
		}

		// Modify the begin selection.
		if (ccode == L_BEGIN_PATTERN)
			ccode = L_CONTINUE_PATTERN;
	} while (ccode != L_END_PATTERN);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	Root()->woAdvancedFlags &= ~WOAF_LOCKED;

	display->VirtualGet(screenID, trueRegion);
	display->Line(screenID, origin.column, origin.line, current.column, current.line, display->xorPalette, 1, TRUE);
	display->VirtualPut(screenID);
#	if defined(ZIL_MSWINDOWS)
	ReleaseCapture();
#	elif defined(ZIL_OS2)
	WinSetCapture(HWND_DESKTOP, 0);
#	endif

	// Draw the resulting image.
	// normalize the bounding rectangle.
	updateDisplay = TRUE;
	display->VirtualGet(screenID, trueRegion);
	origin.column = (origin.column - trueRegion.left) / pixelWidth;
	origin.line = (origin.line - trueRegion.top) / pixelHeight;
	current.column = (current.column - trueRegion.left) / pixelWidth;
	current.line = (current.line - trueRegion.top) / pixelHeight;
	Line(origin.column, origin.line, current.column, current.line);
	display->VirtualPut(screenID);
	updateDisplay = FALSE;
	return (ccode);
}

EVENT_TYPE ZAF_IMAGE::DrawPixel(const UI_EVENT &_event, EVENT_TYPE ccode, EVENT_TYPE pattern)
{
	// Make sure the specified window is current.
	if (Root() != windowManager->First())
		*windowManager + Root();
	if (!FlagSet(woStatus, WOS_CURRENT))
	{
		UI_EVENT addEvent(S_ADD_OBJECT);
		addEvent.windowObject = this;
		parent->Event(addEvent);
	}

	// Draw the image pattern.
	UI_EVENT event = _event;
	UI_POSITION origin = event.position;
	int halfWidth = pixelWidth / 2, halfHeight = pixelHeight / 2;
	int rollerSize = 1;
	if (pattern == OPT_PATTERN_ROLLER)
		imageEditor->Information(I_GET_ROLLER_SIZE, &rollerSize);
#	if defined(ZIL_MSWINDOWS)
	SetCapture(screenID);
#	elif defined(ZIL_OS2)
	WinSetCapture(HWND_DESKTOP, screenID);
#	endif

	eventManager->DeviceState(E_MOUSE, DM_POSITION_IMAGE);
	Root()->woAdvancedFlags |= WOAF_LOCKED; // Lock the window while drawing the pattern.
	ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, GENERAL_GROUPING);
	do
	{
		// Get the user input.
		if (ccode != L_BEGIN_PATTERN)
		{
#			if defined(ZIL_NEXTSTEP)
			NXEvent *theEvent, tmpEvent;
			int oldMask = [[screenID window] addToEventMask:NX_LMOUSEDRAGGEDMASK];
			[NXApp peekNextEvent:NX_ALLEVENTS
				into:&tmpEvent
				waitFor:NX_FOREVER
				threshold:5];
			NXRect rect;
			NXPoint  where = tmpEvent.location;
			[screenID getBounds:&rect];
			[screenID convertPoint:&where fromView:nil];
			event.position.column = (int)where.x;
			event.position.line = (int)where.y;
			
			if ((tmpEvent.type == NX_LMOUSEDRAGGED ) || (tmpEvent.type == NX_RMOUSEDRAGGED))
			{	
				theEvent = [NXApp getNextEvent:(NX_LMOUSEDRAGGEDMASK | NX_RMOUSEDRAGGEDMASK)];
				event.type = ccode = L_CONTINUE_PATTERN;
			}
			else
			{
				[[screenID window] setEventMask:oldMask];
				event.type = ccode = L_END_PATTERN;
			}
#			else
			eventManager->Get(event);
			ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, GENERAL_GROUPING);
#			endif

			// Normalize the coordinates for later use.
			NormalizePosition(this, event, event.position);
		}
		else
			ccode = L_CONTINUE_PATTERN;

		// Check for mouse events.
		if (ccode == L_END_PATTERN ||
			(ccode == L_BEGIN_PATTERN && !trueRegion.Overlap(event.position)))
		{
			// System must process begin/end mouse messages.
			eventManager->Put(event, Q_BEGIN);
			break;
		}
		else if (event.InputType() != E_MOUSE)
        {
			// Account for system events.
			windowManager->Event(event);
            continue;
        }
		else if (!trueRegion.Overlap(event.position))
            continue; // Mouse doesn't overlap boundary.

		// Modify the image.
		event.position.column -= (event.position.column - trueRegion.left) % pixelWidth - halfWidth;
		event.position.line -= (event.position.line - trueRegion.top) % pixelHeight - halfHeight;
		if (origin != event.position)
		{
			int left = (event.position.column - trueRegion.left) / pixelWidth - rollerSize / 2;
			if (left < 0) left = 0;
			int top = (event.position.line - trueRegion.top) / pixelHeight;
			if (top < 0) top = 0;
			int right = (event.position.column - trueRegion.left) / pixelWidth - rollerSize / 2 + rollerSize - 1;
			if (right >= bitmapWidth) right = bitmapWidth - 1;
			int bottom = top;
			if (!pattern) // special paste definition.
			{
				pasteObject = this;
				pasteRegion.right = left + pasteRegion.Width() - 1;
				pasteRegion.bottom = top + pasteRegion.Height() - 1;
				pasteRegion.left = left;
				pasteRegion.top = top;
			}
			else
			{
				updateDisplay = TRUE;
				display->VirtualGet(screenID, trueRegion);
				Rectangle(left, top, right, bottom);
				display->VirtualPut(screenID);
				updateDisplay = FALSE;
			}
			origin = event.position;
		}
	} while (ccode != L_END_PATTERN);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	Root()->woAdvancedFlags &= ~WOAF_LOCKED;

#	if defined(ZIL_MSWINDOWS)
	ReleaseCapture();
#	elif defined(ZIL_OS2)
	WinSetCapture(HWND_DESKTOP, 0);
#	endif

	return (ccode);
}

EVENT_TYPE ZAF_IMAGE::DrawRectangle(const UI_EVENT &_event, EVENT_TYPE ccode, EVENT_TYPE pattern)
{
	// Make sure the specified window is current.
	if (Root() != windowManager->First())
		*windowManager + Root();
	if (!FlagSet(woStatus, WOS_CURRENT))
	{
		UI_EVENT addEvent(S_ADD_OBJECT);
		addEvent.windowObject = this;
		parent->Event(addEvent);
	}

	// Get the image information.
	UI_EVENT event = _event;
	fill = (pattern == OPT_PATTERN_FILL_RECTANGLE ||
		pattern == OPT_PATTERN_FILL_ELLIPSE ||
		pattern == OPT_PATTERN_FILL) ? TRUE : FALSE;

	// Draw the image pattern.
	UI_POSITION origin;
	UI_REGION newRegion, oldRegion;
	int halfWidth = pixelWidth / 2, halfHeight = pixelHeight / 2;
#	if defined(ZIL_MSWINDOWS)
	SetCapture(screenID);
#	elif defined(ZIL_OS2)
	WinSetCapture(HWND_DESKTOP, screenID);
#	endif
	newRegion.left = newRegion.right = newRegion.top = newRegion.bottom = -1;

	eventManager->DeviceState(E_MOUSE, DM_POSITION_IMAGE);
	Root()->woAdvancedFlags |= WOAF_LOCKED; // Lock the window while drawing the pattern.
	ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, GENERAL_GROUPING);
	do
	{
		// Get the user input.
		if (ccode != L_BEGIN_PATTERN)
		{
#			if defined(ZIL_NEXTSTEP)
			NXEvent *theEvent, tmpEvent;
			int oldMask = [[screenID window] addToEventMask:NX_LMOUSEDRAGGEDMASK];
			[NXApp peekNextEvent:NX_ALLEVENTS
				into:&tmpEvent
				waitFor:NX_FOREVER
				threshold:5];
			NXRect rect;
			NXPoint  where = tmpEvent.location;
			[screenID getBounds:&rect];
			[screenID convertPoint:&where fromView:nil];
			event.position.column = (int)where.x;
			event.position.line = (int)where.y;
			
			if ((tmpEvent.type == NX_LMOUSEDRAGGED ) || (tmpEvent.type == NX_RMOUSEDRAGGED))
			{	
				theEvent = [NXApp getNextEvent:(NX_LMOUSEDRAGGEDMASK | NX_RMOUSEDRAGGEDMASK)];
				event.type = ccode = L_CONTINUE_PATTERN;
			}
			else
			{
				[[screenID window] setEventMask:oldMask];
				event.type = ccode = L_END_PATTERN;
			}
#			else
			eventManager->Get(event);
			ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, GENERAL_GROUPING);
#			endif

			// Normalize the coordinates for later use.
			NormalizePosition(this, event, event.position);
		}

		// Check for mouse events.
		if (ccode == L_END_PATTERN ||
			(ccode == L_BEGIN_PATTERN && !trueRegion.Overlap(event.position)))
		{
			// System must process begin/end mouse messages.
			eventManager->Put(event, Q_BEGIN);
			break;
		}
		else if (event.InputType() != E_MOUSE)
        {
			// Account for system events.
			windowManager->Event(event);
            continue;
        }
		else if (!trueRegion.Overlap(event.position))
            continue; // Mouse doesn't overlap boundary.

		// Modify the group coordinates.
		event.position.column -= (event.position.column - trueRegion.left) % pixelWidth - halfWidth;
		event.position.line -= (event.position.line - trueRegion.top) % pixelHeight - halfHeight;
		if (ccode == L_BEGIN_PATTERN)
		{
			origin = event.position;
			newRegion.left = newRegion.right = origin.column;
			newRegion.top = newRegion.bottom = origin.line;
			oldRegion = newRegion;
#			if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_XT) || defined(ZIL_NEXTSTEP)
			display->VirtualGet(screenID, trueRegion);
			display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
			display->VirtualPut(screenID);
#			else
			display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE);
#			endif
		}
		else if (ccode == L_CONTINUE_PATTERN)
		{
			// Change the group position.
			if (event.position.column > origin.column)
				newRegion.right = event.position.column, newRegion.left = origin.column;
			else
				newRegion.left = event.position.column, newRegion.right = origin.column;
			if (event.position.line > origin.line)
				newRegion.bottom = event.position.line, newRegion.top = origin.line;
			else
				newRegion.top = event.position.line, newRegion.bottom = origin.line;
			// Change the group rectangle.
			if (oldRegion != newRegion)
			{
#				if defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_XT) || defined(ZIL_NEXTSTEP)
				display->VirtualGet(screenID, trueRegion);
				display->Rectangle(screenID, oldRegion, display->xorPalette, 1, FALSE, TRUE);
				display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
				display->VirtualPut(screenID);
#				else
				display->RectangleXORDiff(oldRegion, newRegion);
#				endif
				oldRegion = newRegion;
			}
		}

		// Modify the begin selection.
		if (ccode == L_BEGIN_PATTERN)
			ccode = L_CONTINUE_PATTERN;
	} while (ccode != L_END_PATTERN);
	eventManager->DeviceState(E_MOUSE, DM_VIEW);
	Root()->woAdvancedFlags &= ~WOAF_LOCKED;

#	if defined(ZIL_MSWINDOWS)
	ReleaseCapture();
	display->VirtualGet(screenID, trueRegion);
	display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
	display->VirtualPut(screenID);
#	elif defined(ZIL_OS2)
	display->VirtualGet(screenID, trueRegion);
	display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
	display->VirtualPut(screenID);
	WinSetCapture(HWND_DESKTOP, 0);
#	elif defined(ZIL_XT) || defined(ZIL_NEXTSTEP)
	display->VirtualGet(screenID, trueRegion);
	display->Rectangle(screenID, newRegion, display->xorPalette, 1, FALSE, TRUE);
	display->VirtualPut(screenID);
#	else
	display->Rectangle(ID_DIRECT, newRegion, display->xorPalette, 1, FALSE, TRUE);
#	endif

	// Draw the resulting image.
	// normalize the bounding rectangle.
	int left = (newRegion.left - trueRegion.left) / pixelWidth;
	int top = (newRegion.top - trueRegion.top) / pixelHeight;
	int right = (newRegion.right - trueRegion.left) / pixelWidth;
	int bottom = (newRegion.bottom - trueRegion.top) / pixelHeight;
	newRegion.left -= halfWidth, newRegion.right += halfWidth;
	newRegion.top -= halfHeight, newRegion.bottom += halfHeight;
	// draw the pattern on the image.
	updateDisplay = TRUE;
	display->VirtualGet(screenID, trueRegion);
	if (!pattern) // special paste definition.
	{
		pasteObject = this;
		pasteRegion.left = left;
		pasteRegion.top = top;
		pasteRegion.right = right;
		pasteRegion.bottom = bottom;
		Event(OPT_REFRESH_PASTE);
	}
	else if (pattern == OPT_PATTERN_RECTANGLE || pattern == OPT_PATTERN_FILL_RECTANGLE)
		Rectangle(left, top, right, bottom);
	else
		Ellipse(left, top, right, bottom);
	display->VirtualPut(screenID);
	updateDisplay = FALSE;

	return (ccode);
}

EVENT_TYPE ZAF_IMAGE::Event(const UI_EVENT &event)
{
	// Check for user controls.
	EVENT_TYPE ccode = LogicalEvent(event, ID_WINDOW_OBJECT);
	if ((ccode >= D_DESIGNER_FIRST && ccode <= D_DESIGNER_LAST) ||
		(ccode <= -D_DESIGNER_FIRST && ccode >= -D_DESIGNER_LAST))
	{
		for (int i = 0; _processTable[i].option; i++)
			if (_processTable[i].option == ccode)
				return ((this->*(_processTable[i].Process))(event));
	}

	// Switch on the system event type.
	if (event.InputType() == E_MOUSE)
		ccode = UI_EVENT_MAP::MapEvent(_eventTable, event, SPECIAL_GROUPING);
	switch (ccode)
	{
	case S_CREATE:
	case S_CHANGED:
		ccode = UI_WINDOW_OBJECT::Event(event);
		grid = FlagSet(imFlags, IMF_GRID) ? 1 : 0;
		if (bitmapWidth != 0 && bitmapHeight != 0)
		{
			int border = FlagSet(woFlags, WOF_BORDER) ? 2 : 0;
			int width = relative.Width() - border;
			int height = relative.Height() - border;
			pixelWidth = pixelHeight = zoomRatio;
			if (FlagSet(imFlags, IMF_SCALED)) // Scaled.
			{
				pixelWidth = width / bitmapWidth;
				pixelHeight = height / bitmapHeight;
				width = bitmapWidth * pixelWidth + border;
				height = bitmapHeight * pixelHeight + border;
			}
			else if (FlagSet(imFlags, IMF_TILED)) // Tiled.
				;
			else if (FlagSet(imFlags, IMF_MAXFIT)) // Max Fit.
			{
				pixelWidth = width / bitmapWidth;
				pixelHeight = height / bitmapHeight;
				if (pixelWidth > pixelHeight)
					pixelWidth = pixelHeight;
				if (pixelHeight > pixelWidth)
					pixelHeight = pixelWidth;
				width = bitmapWidth * pixelWidth + border;
				height = bitmapHeight * pixelHeight + border;
			}
			else // Actual.
			{
				width = bitmapWidth * pixelWidth + border;
				height = bitmapHeight * pixelHeight + border;
				if (width > trueRegion.Width())
					width = trueRegion.Width();
				if (height > trueRegion.Height())
					height = trueRegion.Height();
			}

			// Compute the actual position.
			if (FlagSet(woFlags, WOF_JUSTIFY_CENTER))
				trueRegion.left += (relative.Width() - width) / 2;
			else if (FlagSet(woFlags, WOF_JUSTIFY_RIGHT))
				trueRegion.left = trueRegion.right - width + 1;
			trueRegion.top += (relative.Height() - height) / 2;
			trueRegion.right = trueRegion.left + width - 1;
			trueRegion.bottom = trueRegion.top + height - 1;
		}
		if (ccode == S_CREATE)
			UI_WINDOW_OBJECT::Event(S_REGISTER_OBJECT);
		break;

	case L_BEGIN_GROUPING:
		// Define the edit group.
		UI_WINDOW_OBJECT::Event(event);
		EditGroupDefine(event);
		break;

	case OPT_REFRESH_PASTE:
		{
		UI_EVENT rEvent(S_DISPLAY_ACTIVE);
		rEvent.region.left = trueRegion.left + pasteRegion.left * pixelWidth;
		rEvent.region.top = trueRegion.top + pasteRegion.top * pixelHeight;
		rEvent.region.right = trueRegion.left + (pasteRegion.right + 1) * pixelWidth;
		rEvent.region.bottom = trueRegion.top + (pasteRegion.bottom + 1) * pixelHeight;
		parent->Event(rEvent);
		}
		break;

	case L_BEGIN_PATTERN:
		{
		// Normalize the coordinates.
		UI_WINDOW_OBJECT::Event(event);
		UI_POSITION position = event.position;
		NormalizePosition(this, event, position);
		// Check the overlap.
		if (trueRegion.Overlap(position))
		{
			UI_EVENT dEvent = event;
			dEvent.position = position;
			EVENT_TYPE pattern;
			imageEditor->Information(I_GET_PATTERN, &pattern);
			imageEditor->Information(FlagSet(event.rawCode, M_LEFT) ? I_GET_LEFT_COLOR : I_GET_RIGHT_COLOR, &color);
			if (pattern == OPT_PATTERN_FILL)
				ccode = DrawFill(dEvent, ccode, pattern);
			else if (pattern == OPT_PATTERN_LINE)
				ccode = DrawLine(dEvent, ccode, pattern);
			else if (pattern == OPT_PATTERN_PENCIL || pattern == OPT_PATTERN_ROLLER)
				ccode = DrawPixel(dEvent, ccode, pattern);
			else
				ccode = DrawRectangle(dEvent, ccode, pattern);
			if (Next() && Next()->Inherited(ID_IMAGE))
				Next()->Event(S_REDISPLAY);
		}
		else
			ccode = S_ERROR;
		}
		break;

	case S_CURRENT:
	case S_NON_CURRENT:
		if (ccode == S_CURRENT)
			woStatus |= WOS_CURRENT;
		else
			woStatus &= ~WOS_CURRENT;
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;

	default:
		ccode = UI_WINDOW_OBJECT::Event(event);
		break;
	}

	// Return the control code.
	return (ccode);
}

// ----- Edit Options -------------------------------------------------------

EVENT_TYPE ZAF_IMAGE::EditCopy(const UI_EVENT &event)
{
	// Check for a valid paste area.
	if (pasteRegion.left == -1)
		return (S_ERROR);

	// Update the old paste group.
	UI_WINDOW_OBJECT *object = pasteObject;
	pasteObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	if (object)
		object->Event(OPT_REFRESH_PASTE);

	// Determine the paste area.
	if (pasteArray)
		delete pasteArray;
	int pasteWidth = pasteRegion.Width();
	int pasteHeight = pasteRegion.Height();
	pasteArray = new ZIL_UINT8[pasteWidth * pasteHeight];
	memset(pasteArray, (char)WHITE, pasteWidth * pasteHeight);

	// Copy the paste contents.
	int minWidth = MinValue(pasteWidth, bitmapWidth - pasteRegion.left);
	int minHeight = MinValue(pasteHeight, bitmapHeight - pasteRegion.top);
	for (int i = 0; i < minHeight; i++)
		memcpy(&pasteArray[i*pasteWidth], &bitmapArray[(pasteRegion.top + i) * bitmapWidth + pasteRegion.left], minWidth);
	_serviceManager->Get(ID_IMAGE_EDITOR)->Information(I_STATUS_UPDATE, ZIL_NULLP(void));
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE::EditCut(const UI_EVENT &event)
{
	// Check for a valid paste area.
	if (pasteRegion.left == -1)
		return (S_ERROR);

	EditCopy(event);
	int pasteWidth = pasteRegion.Width();
	int pasteHeight = pasteRegion.Height();
	int minWidth = MinValue(pasteWidth, bitmapWidth - pasteRegion.left);
	int minHeight = MinValue(pasteHeight, bitmapHeight - pasteRegion.top);
	for (int i = 0; i < minHeight; i++)
		memset(&bitmapArray[(pasteRegion.top+i)*bitmapWidth+pasteRegion.left], (char)WHITE, minWidth);
	Event(OPT_REFRESH_PASTE);
	if (Next() && Next()->Inherited(ID_IMAGE))
		Next()->Event(S_REDISPLAY);
	_serviceManager->Get(ID_IMAGE_EDITOR)->Information(I_STATUS_UPDATE, ZIL_NULLP(void));
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE::EditDelete(const UI_EVENT &event)
{
	// Check for a valid paste area.
	if (pasteRegion.left == -1)
		return (S_ERROR);
	EditCut(event);
	EditGroupUndefine(event);

	// Refresh the image.
	Event(S_REDISPLAY);
	if (Next() && Next()->Inherited(ID_IMAGE))
		Next()->Event(S_REDISPLAY);
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE::EditGroupDefine(const UI_EVENT &event)
{
	// Define the paste area.
	EditGroupUndefine(event);
	EVENT_TYPE ccode = DrawRectangle(event, event.type, 0);
	_serviceManager->Get(ID_IMAGE_EDITOR)->Information(I_STATUS_UPDATE, ZIL_NULLP(void));
	return (ccode);
}

EVENT_TYPE ZAF_IMAGE::EditGroupUndefine(const UI_EVENT &event)
{
	// Update the old paste group.
	UI_WINDOW_OBJECT *object = pasteObject;
	pasteObject = ZIL_NULLP(UI_WINDOW_OBJECT);
	if (object)
		object->Event(OPT_REFRESH_PASTE);

	// Remove the old paste area.
	if (pasteArray)
		delete pasteArray;
	pasteArray = ZIL_NULLP(ZIL_UINT8);
	pasteRegion.left = pasteRegion.top = pasteRegion.right = pasteRegion.bottom = -1;
	_serviceManager->Get(ID_IMAGE_EDITOR)->Information(I_STATUS_UPDATE, ZIL_NULLP(void));
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE::EditMove(const UI_EVENT &event)
{
	return (event.type);
}

EVENT_TYPE ZAF_IMAGE::EditPaste(const UI_EVENT &event)
{
	if (pasteArray)
	{
		// Update the old paste group.
		UI_WINDOW_OBJECT *object = pasteObject;
		pasteObject = ZIL_NULLP(UI_WINDOW_OBJECT);
		if (object)
			object->Event(OPT_REFRESH_PASTE);

		// Get the paste area.
		DrawPixel(event, event.type, 0);
		// Set the paste contents.
		int pasteWidth = pasteRegion.Width();
		int pasteHeight = pasteRegion.Height();
		int minWidth = MinValue(pasteWidth, bitmapWidth - pasteRegion.left);
		int minHeight = MinValue(pasteHeight, bitmapHeight - pasteRegion.top);
		for (int i = 0; i < minHeight; i++)
			memcpy(&bitmapArray[(pasteRegion.top + i) * bitmapWidth + pasteRegion.left], &pasteArray[i*pasteWidth], minWidth);
		Event(OPT_REFRESH_PASTE);
		if (Next() && Next()->Inherited(ID_IMAGE))
			Next()->Event(S_REDISPLAY);
	}
	return (event.type);
}

// --------------------------------------------------------------------------

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::GetError(void)
{
	return (error);
}

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::SetError(Z_ERROR _error)
{
	error = _error;
	return (error);
}

ZAF_IMAGE::IMPORT_FILTER_STRUCT ZAF_IMAGE::_importTable[] =
{
	{ F_ODIB, ZIL_PROCESS_REFERENCE(ImportODIB) },
	{ F_WDIB, ZIL_PROCESS_REFERENCE(ImportWDIB) },
//	{ F_OICO, ZIL_PROCESS_REFERENCE(ImportOICO) },	// Not available.
	{ F_WICO, ZIL_PROCESS_REFERENCE(ImportWICO) },
	{ F_XPM, ZIL_PROCESS_REFERENCE(ImportXPM) },
	{ F_ZNC, ZIL_PROCESS_REFERENCE(ImportZNC) },
	// F_NONE must be last
	{ F_NONE, ZIL_PROCESS_REFERENCE(Export) }
};

ZAF_IMAGE::EXPORT_FILTER_STRUCT ZAF_IMAGE::_exportTable[] =
{
	{ F_ODIB, ZIL_PROCESS_REFERENCE(ExportODIB) },
	{ F_WDIB, ZIL_PROCESS_REFERENCE(ExportWDIB) },
//	{ F_OICO, ZIL_PROCESS_REFERENCE(ExportOICO) },	// Not available.
	{ F_WICO, ZIL_PROCESS_REFERENCE(ExportWICO) },
	{ F_XPM, ZIL_PROCESS_REFERENCE(ExportXPM) },
	{ F_ZNC, ZIL_PROCESS_REFERENCE(ExportZNC) },
	// F_NONE must be last
	{ F_NONE, ZIL_PROCESS_REFERENCE(Export) }
};

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::SetImportFilter(int _FilterImport)
{
	int i;
	for (i = 0; _importTable[i].tag != F_NONE; i++)
		if (_importTable[i].tag == _FilterImport)
			break;
	FilterImport = _importTable[i].filter;
	return (GetError());
}

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::SetExportFilter(int _FilterExport)
{
	int i;
	for (i = 0; _exportTable[i].tag != F_NONE; i++)
		if (_exportTable[i].tag == _FilterExport)
			break;
	FilterExport = _exportTable[i].filter;
	return (GetError());
}

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::Import(ZIL_FILE *file)
{
	return (this->*FilterImport)(file);
}

ZAF_IMAGE::Z_ERROR ZAF_IMAGE::Export(ZIL_FILE *file)
{
	return (this->*FilterExport)(file);
}

// ----- Static tables ------------------------------------------------------

UI_WINDOW_OBJECT *ZAF_IMAGE::pasteObject = ZIL_NULLP(UI_WINDOW_OBJECT);
ZIL_UINT8 *ZAF_IMAGE::pasteArray = ZIL_NULLP(ZIL_UINT8);
UI_REGION ZAF_IMAGE::pasteRegion = { -1, -1, -1, -1 };

ZAF_IMAGE::PROCESS ZAF_IMAGE::_processTable[] =
{
	{ OPT_EDIT_COPY, 			ZIL_PROCESS_REFERENCE(EditCopy) },				// edit requests
	{ OPT_EDIT_CUT, 			ZIL_PROCESS_REFERENCE(EditCut) },
	{ OPT_EDIT_DELETE, 			ZIL_PROCESS_REFERENCE(EditDelete) },
	{ OPT_EDIT_GROUP_DEFINE, 	ZIL_PROCESS_REFERENCE(EditGroupDefine) },
	{ OPT_EDIT_GROUP_UNDEFINE, 	ZIL_PROCESS_REFERENCE(EditGroupUndefine) },
	{ OPT_EDIT_MOVE, 			ZIL_PROCESS_REFERENCE(EditMove) },
	{ OPT_EDIT_PASTE, 			ZIL_PROCESS_REFERENCE(EditPaste) },
	{ 0, 0 }
};
