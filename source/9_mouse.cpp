//	Zinc Interface Library - W_MOUSE.CPP
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

#define ZIL_UID_MOUSE_DATA
#include "img_def.cpp"

// ----- UID_MOUSE ----------------------------------------------------------

UID_MOUSE::UID_MOUSE(ZIL_DEVICE_STATE _state, DEVICE_IMAGE _image) :
	UI_DEVICE(E_MOUSE, _state), image(_image)
{
	// Initialize the new mouse information.
	position.column = position.line = 0;
	offset.column = offset.line = 0;
	installed = TRUE;
}

UID_MOUSE::~UID_MOUSE(void)
{
	DestroyCursor(cancelCursor);
	DestroyCursor(moveCursor);
	DestroyCursor(copyCursor);
	DestroyCursor(moveMultipleCursor);
	DestroyCursor(copyMultipleCursor);
}

EVENT_TYPE UID_MOUSE::Event(const UI_EVENT &event)
{
	// Initialize the standard windows cursors.
	struct MOUSE_ITEM
	{
		EVENT_TYPE image;
		void *identification;
	} _mouseTable[] = {
		{ DM_VIEW, 			(void *)IDC_ARROW },
		{ DM_EDIT, 			(void *)IDC_IBEAM },
		{ DM_WAIT, 			(void *)IDC_WAIT },
		{ DM_MOVE, 			(void *)IDC_SIZE },
		{ DM_HORIZONTAL, 	(void *)IDC_SIZEWE },
		{ DM_VERTICAL, 		(void *)IDC_SIZENS },
		{ DM_DIAGONAL_ULLR, (void *)IDC_SIZENWSE },
		{ DM_DIAGONAL_LLUR, (void *)IDC_SIZENESW },
		{ DM_POSITION_IMAGE, 		(void *)IDC_CROSS },
		{ 0, 0 } };

	// Switch on the raw code.
	EVENT_TYPE ccode = event.type;
	switch (ccode)
	{
	case S_POSITION:
		position = event.position;
		SetCursorPos(position.column, position.line);
		break;

	case D_STATE:
		return ((state == D_OFF) ? D_OFF : image);

	case S_INITIALIZE:
		ccode = image;
		image = 0;
		// Create Drag and drop cursors.
		cancelCursor = CreateCursor(display->hInstance,
			_cancelAND[0], _cancelAND[1], 32, 32, &_cancelAND[2], _cancelXOR);
		moveCursor = CreateCursor(display->hInstance,
			_dragMoveAND[0], _dragMoveAND[1], 32, 32, &_dragMoveAND[2], _dragMoveXOR);
		copyCursor = CreateCursor(display->hInstance,
			_dragCopyAND[0], _dragCopyAND[1], 32, 32, &_dragCopyAND[2], _dragCopyXOR);
		moveMultipleCursor = CreateCursor(display->hInstance,
			_dragMoveMultipleAND[0], _dragMoveMultipleAND[1], 32, 32, &_dragMoveMultipleAND[2], _dragMoveMultipleXOR);
		copyMultipleCursor = CreateCursor(display->hInstance,
			_dragCopyMultipleAND[0], _dragCopyMultipleAND[1], 32, 32, &_dragCopyMultipleAND[2], _dragCopyMultipleXOR);

		// Continue to default.
	default:
	{
		int i;
		for (i = 0; _mouseTable[i].image && _mouseTable[i].image != ccode; i++)
			;
        HCURSOR hCursor = ZIL_NULLH(HCURSOR);
		if (_mouseTable[i].image == ccode)
			hCursor = LoadCursor(0, (ZIL_ICHAR *)_mouseTable[i].identification);
		else if (ccode == DM_CANCEL)
			hCursor = cancelCursor;
		else if (ccode == DM_DRAG_MOVE)
			hCursor = moveCursor;
		else if (ccode == DM_DRAG_COPY)
			hCursor = copyCursor;
		else if (ccode == DM_DRAG_MOVE_MULTIPLE)
			hCursor = moveMultipleCursor;
		else if (ccode == DM_DRAG_COPY_MULTIPLE)
			hCursor = copyMultipleCursor;

		if (hCursor)
			SetCursor(hCursor);
	}
		break;
	}

	// Return the control code.
	return (ccode);
}

void UID_MOUSE::Poll(void)
{
}

