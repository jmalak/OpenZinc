//      Zinc Interface Library - D_MOUSEX.CPP
//
//      Version of Zinc d_mouse.cpp source file to add
//      support for X-32VM and FlashGraphics
//

#if defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
#include <mem.h>
#endif
#if defined(ZIL_MSDOS)
#       include <dos.h>
#endif
#include "ui_evt.hpp"

// ----- UID_MOUSE ----------------------------------------------------------

#if defined(DOS386) && defined(FG)
extern void MouseRead();
#endif

#if defined(ZIL_MSDOS)
static UID_MOUSE *_mouse;
#endif

static int _upToDate = TRUE;
static int _horizontalRatio = 1;
static int _verticalRatio = 1;

#define ZIL_UID_MOUSE_DATA
#include "img_def.cpp"

#define MAX_EVENTS      20
static unsigned sFifo[MAX_EVENTS];
static int xFifo[MAX_EVENTS];
static int yFifo[MAX_EVENTS];
static int zState = 0;
static int volatile in, out;
extern char _devicePainting;

static void mouseQueueEventFunc(unsigned state, int *horizontalMickeys, int *verticalMickeys)
{
	static int i, deltaX, deltaY;

#if defined(FG) && defined(DOS386)
	// Accumulate mickey count if less than ratios.
	static int xAmount, yAmount;
	if (state == zState &&
		AbsValue(*horizontalMickeys + xAmount) < _horizontalRatio &&
		AbsValue(*verticalMickeys + yAmount) < _verticalRatio)
	{
		xAmount += *horizontalMickeys;
		yAmount += *verticalMickeys;
		return;
	}
	else
	{
		*horizontalMickeys += xAmount;
		*verticalMickeys += yAmount;
		xAmount = *horizontalMickeys % _horizontalRatio;
		yAmount = *verticalMickeys % _verticalRatio;
	}
#endif
	deltaX = *horizontalMickeys / _horizontalRatio;
	deltaY = *verticalMickeys / _verticalRatio;

	i = in + 1;
	if (i >= MAX_EVENTS)
		i = 0;
	if (i != out && (state != zState || deltaX || deltaY))
	{
		// Update the mickey count.
		*horizontalMickeys %= _horizontalRatio;
		*verticalMickeys %= _verticalRatio;

		if (state == zState && in != out)
		{
			// Update existing event in the queue, don't add a new one.
			xFifo[in] += deltaX;
			yFifo[in] += deltaY;
			return;
		}
		in = i;
		sFifo[i] = zState = state;
		xFifo[i] = deltaX;
		yFifo[i] = deltaY;
	}
}

UID_MOUSE::UID_MOUSE(ZIL_DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_MOUSE, _state), image(_image)
{
	if (!I_MouseOpen())     // Try to initialize the mouse driver.
		return;
#if defined(ZIL_MSDOS) && !defined(ZIL_TEXT_ONLY)
	if (!defaultInitialized)
	{
		ZIL_DECORATION_MANAGER::SetDecorations(_className, TEXT_TABLE, BITMAP_TABLE);
		defaultInitialized = TRUE;
	}
	myDecorations = ZIL_DECORATION_MANAGER::UseDecorations(_className);
#endif
#if defined(ZIL_MSDOS)
	_mouse = this;
#endif
	mouseQueueEvent = mouseQueueEventFunc;
	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_MOUSE::~UID_MOUSE(void)
{
	// Restore the old mouse information.
	if (installed)
	{
		mouseQueueEvent = ZIL_NULLF(void (*)(unsigned, int *, int *));
		I_MouseClose();
	}
	installed = FALSE;
}

EVENT_TYPE UID_MOUSE::Event(const UI_EVENT &event)
{
	// Make sure the mouse is installed.
	if (!installed)
		return (D_OFF);

	ZIL_DEVICE_STATE oldState = state;
	DEVICE_IMAGE oldImage = image;
	UI_POSITION oldPosition = position;
	EVENT_TYPE ccode = event.type;

	// Switch on the raw code.
	switch (ccode)
	{
	case S_INITIALIZE:
		_horizontalRatio = display->isText ? (display->columns == 40 ? 16 : 8) : 1;
		_verticalRatio = display->isText ? 8 : 1;
		position.column = display->columns / 2;
		position.line = display->lines / 2;
		oldImage = 0;
		oldState = D_OFF;
		break;

	case S_DEINITIALIZE:
		display->DeviceMove(IMT_MOUSE_IMAGE, display->columns, display->lines);
		state = D_HIDE;
		break;

	case S_POSITION:
		position = event.position;
		break;

	case DM_VIEW:
	case DM_EDIT:
	case DM_WAIT:
	case DM_MOVE:
	case DM_HORIZONTAL:
	case DM_VERTICAL:
	case DM_DIAGONAL_ULLR:
	case DM_DIAGONAL_LLUR:
	case DM_POSITION_IMAGE:
	case DM_CANCEL:
	case DM_DRAG_MOVE:
	case DM_DRAG_COPY:
	case DM_DRAG_MOVE_MULTIPLE:
	case DM_DRAG_COPY_MULTIPLE:
		if (state != D_OFF)
		{
			image = ccode;
			state = D_ON;
		}
		break;

	case D_ON:
		display->DeviceMove(IMT_MOUSE_IMAGE, position.column - offset.column,
			position.line - offset.line);

		if (state == D_OFF)
		{
			out = in;
			state = D_ON;
		}
		break;

	case D_OFF:
		state = D_OFF;
		break;

	case D_STATE:
		return ((state == D_OFF) ? D_OFF : image);

	case D_HIDE:
		display->DeviceMove(IMT_MOUSE_IMAGE, display->columns, display->lines);
		state = event.type;
		break;

	default:
		return (ccode);
	}

	// Reset the mouse information.
	if (oldImage == image && oldState == state && oldPosition == position)
		return (ccode);
	if ((oldPosition != position && state != D_HIDE))
		_upToDate = display->DeviceMove(IMT_MOUSE_IMAGE,
			position.column - offset.column, position.line - offset.line);
	if (oldImage != image)
	{
		if (display->isText)
		{
			_upToDate = display->DeviceSet(IMT_MOUSE_IMAGE, position.column,
				position.line, 1, 1, ZIL_NULLP(ZIL_UINT8));
		}
#if !defined(ZIL_TEXT_ONLY)
		else
		{
			ZIL_UINT8 *bitmapImage = myDecorations->GetBitmap(image);
			if (bitmapImage)
			{
				offset.column = bitmapImage[0];
				offset.line = bitmapImage[1];
				_upToDate = display->DeviceSet(IMT_MOUSE_IMAGE,
					position.column - offset.column,
					position.line - offset.line, bitmapImage[2],
					bitmapImage[3], &bitmapImage[4]);
			}
		}
#endif
	}

	// Return the control code.
	return (ccode);
}


#if defined(ZIL_MSDOS)
void UID_MOUSE::MouseMove(int deltaX, int deltaY)
{
	if (!_devicePainting)
	{
		_devicePainting = TRUE;
		position.column += deltaX;
		position.line += deltaY;

		if (position.column >= display->columns)
			position.column = display->columns - 1;
		else if (position.column < 0)
			position.column = 0;
		if (position.line >= display->lines)
			position.line = display->lines - 1;
		else if (position.line < 0)
			position.line = 0;
		display->DeviceMove(IMT_CURSOR_IMAGE,
				display->columns, display->lines);
		display->DeviceMove(IMT_MOUSE_IMAGE,
				position.column - offset.column, position.line - offset.line);
		_devicePainting = FALSE;
	}
}
#endif

void UID_MOUSE::Poll(void)
{
	if (!installed)
		return;
#if defined(DOS386) && defined(FG)
	MouseRead();
#endif
	// Make sure the mouse is valid.
	if (state != D_OFF && in != out)
	{
		UI_EVENT event;
		event.type = E_MOUSE;
		while (in != out)       // Get all mouse events.
		{
			if (++out >= MAX_EVENTS)
				out = 0;
			event.rawCode = sFifo[out];
			position.column += xFifo[out];
			position.line   += yFifo[out];
			if (position.column >= display->columns)
				position.column = display->columns - 1;
			else if (position.column < 0)
				position.column = 0;
			if (position.line >= display->lines)
				position.line = display->lines - 1;
			else if (position.line < 0)
				position.line = 0;
			event.position = position;
			eventManager->Put(event);
		}
		if (state != D_HIDE)
			_upToDate = display->DeviceMove(IMT_MOUSE_IMAGE,
				position.column - offset.column,
				position.line - offset.line);
	}
	// Mouse could not be updated during a previous paint operation.
	else if (!_upToDate)
	{
		DEVICE_IMAGE newImage = image;
		image = 0;
		Event(UI_EVENT(newImage));
		_upToDate = TRUE;
	}
}

