//	Zinc Interface Library - D_CURSOR.CPP
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


#include "ui_evt.hpp"

// WATCOM has a compiler bug if the constructor and destructor don't
// have any other functions in the file (Poll, Event). So definition
// of constructor & destructor are in d_cursor.
#if defined(__WATCOMC__)

UID_CURSOR::UID_CURSOR(ZIL_DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_CURSOR, _state), image(_image)
{
	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_CURSOR::~UID_CURSOR(void)
{
	// No cursor information needs to be destroyed.
}

#endif

#define gnd	BACKGROUND
#define BLK	BLACK
static ZIL_UINT8 _insert[46] =
{
	0, 0, 2, 14,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK
};
static ZIL_UINT8 _overstrike[25] =
{
	0, 0, 1, 14,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,BLK,
	BLK,BLK,BLK,BLK,BLK,BLK,BLK
};

// ----- UID_CURSOR ---------------------------------------------------------
extern char _devicePainting;
static int _upToDate = TRUE;

EVENT_TYPE UID_CURSOR::Event(const UI_EVENT &event)
{
	// Switch on the raw code.
	ZIL_DEVICE_STATE oldState = state;
	DEVICE_IMAGE oldImage = image;
	UI_POSITION oldPosition = position;
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_INITIALIZE:
		oldState = D_OFF;
		break;

	case S_DEINITIALIZE:
		state = D_HIDE;
		break;

	case D_OFF:
	case D_ON:
		state = event.type;
		break;

	case D_STATE:
		return ((state == D_OFF) ? D_OFF : image);

	case S_POSITION:
		position = event.position;
		if (state == D_HIDE)
			state = D_ON;
		break;

	case DC_INSERT:
	case DC_OVERSTRIKE:
		image = DC_INSERT;
		state = D_ON;
		break;
	}

	// Reset the cursor information.
	if (oldImage == image && oldState == state && oldPosition == position)
		return (ccode);

	_devicePainting = TRUE;
	if (state == D_ON && display->isText && position.column >= 0 &&
		position.column < display->columns && position.line >= 0 &&
		position.line < display->lines)
		I_CursorPosition(position.line, position.column, image);
	else if (display->isText)
		I_CursorRemove();		
	else if (state == D_ON)
	{
		ZIL_UINT8 *bitmapImage = (image == DC_INSERT) ? _insert : _overstrike;
		offset.column = offset.line = 0;
		_upToDate = display->DeviceSet(IMT_CURSOR_IMAGE, position.column,
			position.line, bitmapImage[2], MinValue(15, display->TextHeight(ZIL_NULLP(ZIL_ICHAR)) + 1),
			&bitmapImage[4]);
	}
	else
		_upToDate = display->DeviceMove(IMT_CURSOR_IMAGE, display->columns,
			display->lines);
	_devicePainting = FALSE;

	// Return the control code.
	return (ccode);
}

void UID_CURSOR::Poll(void)
{
	// Make sure the cursor is valid.
	static ZIL_UTIME lastTime, currentTime;
	if (!installed || state == D_OFF || display->isText)
		return;

	// Check the blink state of the cursor.
	currentTime.Import();
	if (currentTime.Difference(lastTime) > blinkRate)
	{
		_devicePainting = TRUE;
		lastTime = currentTime;
		if (state == D_ON)
		{
			_upToDate = display->DeviceMove(IMT_CURSOR_IMAGE, display->columns,
				display->lines);
			state = D_HIDE;
		}
		else
		{
			_upToDate = display->DeviceMove(IMT_CURSOR_IMAGE,
				position.column + offset.column, position.line + offset.line);
			state = D_ON;
		}
		_devicePainting = FALSE;
	}
	// Cursor could not be updated during a previous paint operation.
	else if (!_upToDate)
	{
		DEVICE_IMAGE newImage = image;
		image = 0;
		Event(UI_EVENT(newImage));
		_upToDate = TRUE;
	}
}
