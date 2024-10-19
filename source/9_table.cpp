//	Zinc Interface Library - W_TABLE.CPP
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


#include <limits.h>
#include "ui_win.hpp"

EVENT_TYPE UIW_TABLE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	display->VirtualGet(screenID, trueRegion);
	lastPalette = LogicalPalette(ccode);

	if (!tableRecord || records == 0)
	{
		display->Rectangle(screenID, event.region, lastPalette, 0, TRUE);
		display->VirtualPut(screenID);
		return(ccode);
	}

	UI_REGION client;
	UIW_WINDOW::Information(I_GET_CLIPREGION, &client);
	UI_REGION updateRegion = event.region;
	int rows = records / columns;
	int topRow = topRecord / columns;
	int recordWidth = tableRecord->trueRegion.Width();
	int recordHeight = tableRecord->trueRegion.Height();

	if (rows - topRow < INT_MAX / recordHeight)  // Protect against overflow.
	{
		int bottom = (rows - topRow) * recordHeight + scroll.top;
		if (client.top + bottom < updateRegion.bottom)
		{
			UI_REGION drawRegion = updateRegion;
			drawRegion.top = bottom;
			display->Rectangle(screenID, drawRegion, lastPalette, 0, TRUE);
			updateRegion.bottom = drawRegion.top;
		}
	}

	int leftColumn = topRecord % columns; 
	if (columns - leftColumn < INT_MAX / recordWidth)  // Protect against overflow.
	{
		int right = (columns - leftColumn) * recordWidth + scroll.left;
		if (client.left + right < updateRegion.right)
		{
			UI_REGION drawRegion = updateRegion;
			drawRegion.left = right;
			display->Rectangle(screenID, drawRegion, lastPalette, 0, TRUE);
		}
	}

	display->VirtualPut(screenID);
	return (ccode);
}

void UIW_TABLE::DrawRecord(int recordNum)
{
#if defined(ZIL_EDIT)
	if (FlagSet(woStatus, WOS_EDIT_MODE))
		return;
#endif

	UI_REGION client;
	UIW_WINDOW::Information(I_GET_CLIPREGION, &client);
	int column = recordNum % columns;
	int row = recordNum / columns;
	int trueWidth = virtualRecord->trueRegion.Width();
	int trueHeight = virtualRecord->trueRegion.Height();
	int leftColumn = topRecord % columns;
	int rightColumn = leftColumn + client.Width() / trueWidth;
	int topRow = topRecord / columns;
	int bottomRow = topRow + client.Height() / trueHeight;

	// Test for visibility.
	if (recordNum < topRecord || column > rightColumn || row > bottomRow)
		return;

	// Set the new position.
	UI_REGION newRegion;
	newRegion.left = (column - leftColumn) * trueWidth;;
	newRegion.top = (row - topRow) * trueHeight +
		(FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL) ? 0 : display->preSpace);
	newRegion.right = newRegion.left + virtualRecord->relative.Width() - 1;
	newRegion.bottom = newRegion.top + virtualRecord->relative.Height() - 1;

	virtualRecord->relative = newRegion;
	RegionMax(virtualRecord);

	ZIL_SCREENID virtualID;
	virtualRecord->Information(I_GET_FRAMEID, &virtualID);
	MoveWindow(virtualID, virtualRecord->trueRegion.left,
		virtualRecord->trueRegion.top, virtualRecord->trueRegion.Width(),
		virtualRecord->trueRegion.Height(), FALSE);

	UI_EVENT recordEvent(S_SET_DATA, recordNum);
	recordEvent.data = GetRecord(recordNum);
	virtualRecord->Event(recordEvent);

	// Enable update.
	SendMessage(virtualID, WM_SETREDRAW, (WPARAM)TRUE, 0);

	// Display Record.
	InvalidateRect(virtualID, ZIL_NULLP(RECT), TRUE);
	UpdateWindow(virtualID);

	// Fix for bug in WindowsNT version 3.5.
	// (UpdateWindow() not updating children)
	HWND childID = GetWindow(virtualID, GW_CHILD);
	while (childID)
	{
		UpdateWindow(childID);
		HWND nextID = childID;
		while ((nextID = GetWindow(nextID, GW_HWNDNEXT)) != 0)
			UpdateWindow(nextID);
		childID = GetWindow(childID, GW_CHILD);
	}

	// Disable update.
	SendMessage(virtualID, WM_SETREDRAW, (WPARAM)FALSE, 0);
}

EVENT_TYPE UIW_TABLE::Event(const UI_EVENT &event)
{
	if (event.type == E_MSWINDOWS && event.message.message == WM_PAINT &&
		event.message.hwnd == screenID)
	{
		RECT updateRect;
		GetUpdateRect(screenID, &updateRect, FALSE);

		if (tableRecord)
		{
			UI_REGION updateRegion;
			updateRegion.Assign(updateRect);
			int recordWidth = tableRecord->trueRegion.Width();
			int recordHeight = tableRecord->trueRegion.Height();
			int leftColumn = topRecord % columns + updateRegion.left / recordWidth;
			int rightColumn = topRecord % columns + updateRegion.right / recordWidth;
			int topRow = topRecord / columns + updateRegion.top / recordHeight;
			int bottomRow = topRecord / columns + updateRegion.bottom / recordHeight;

			int i = topRecord;
			while (i < records)
			{
				int column = i % columns;
				int row = i / columns;

				if (row > bottomRow)
					break;
				else if (row < topRow)
					i += columns;
				else if (column > rightColumn)
					i += columns - (i % columns);
				else if (column < leftColumn)
					i++;
				else if (i != currentRecord)
					DrawRecord(i++);
				else
					i++;
			}
		}

		return (UI_WINDOW_OBJECT::Event(event));
	}

	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TABLE);
	switch (ccode)
	{
		case S_INITIALIZE:
			currentRecord = topRecord = 0;
			ccode = UIW_WINDOW::Event(event);

			if (FlagSet(woFlags, WOF_BORDER))
				dwStyle |= WS_BORDER;

			if (virtualRecord)
			{
				virtualRecord->parent = this;
				virtualRecord->Event(event);
			}
			break;

		case S_CREATE:
			if (tableRecord && records > 0)
			{
				UI_EVENT dataEvent(S_SET_DATA);
				dataEvent.rawCode = currentRecord;
				dataEvent.data = GetRecord(currentRecord);
				tableRecord->Event(dataEvent);
			}
			ccode = UIW_WINDOW::Event(event);
			if (virtualRecord)
			{
				virtualRecord->Event(event);
			  	ZIL_SCREENID virtualID;
				virtualRecord->Information(I_GET_FRAMEID, &virtualID);
				SendMessage(virtualID, WM_SETREDRAW, (WPARAM)FALSE, 0);
			}
			break;

		case S_REDISPLAY:
			if (columnHeader)
				columnHeader->Event(event);
			if (rowHeader)
				rowHeader->Event(event);

#if defined(ZIL_EDIT)
			if (tableRecord	&& !FlagSet(woStatus, WOS_EDIT_MODE))
#else
			if (tableRecord)
#endif
			{
				ZIL_SCREENID recordID;
				tableRecord->Information(I_GET_FRAMEID, &recordID);
				if (records > 0 || Inherited(ID_TABLE_HEADER))
				{
					SendMessage(recordID, WM_SETREDRAW, (WPARAM)TRUE, 0);
					UI_EVENT dataEvent(S_SET_DATA, currentRecord);
					dataEvent.data = GetRecord(currentRecord);
					tableRecord->Event(dataEvent);
				}
				else
					SendMessage(recordID, WM_SETREDRAW, (WPARAM)FALSE, 0);
			}
			ccode = UIW_WINDOW::Event(event);
			UpdateWindow(screenID);
			break;

		case L_PGUP:
		case L_PGDN:
			{
			int delta = vScrollInfo.showing;
			int column = currentRecord % columns;
			int row = currentRecord / columns +	(ccode == L_PGDN ? delta : -delta);
			int index = row * columns + column;
			if (index >= records)
				index = records - 1;
			else if (index < 0)
				index = 0;
			SetCurrent(index);
			}
			break;

		case L_LEFT:
		case L_RIGHT:
		case L_UP:
		case L_DOWN:
			if (event.type == E_MSWINDOWS)
				ccode = UIW_WINDOW::Event(event);
			else
			{
			int column = currentRecord % columns;
			int row = currentRecord / columns;
			if (ccode == L_LEFT && column > 0)
				column -= 1;
			else if (ccode == L_RIGHT && column < columns - 1)
				column += 1;
			else if (ccode == L_UP && row > 0)
				row -= 1;
			else if (ccode == L_DOWN && (row + 1) * columns + column <= records - 1)
				row += 1;
			else
				break;
			SetCurrent(row * columns + column);
			}
			break;

		case L_NEXT:
			if (event.type == E_MSWINDOWS)
				ccode = UIW_WINDOW::Event(event);
			else if (columns > 1)
			{
				if (currentRecord + 1 >= records)
					SetCurrent(currentRecord - currentRecord % columns);
				else if ((currentRecord + 1) % columns)
					SetCurrent(currentRecord + 1);
				else
					SetCurrent(currentRecord + 1 - columns);
			}
			break;

		case L_PREVIOUS:
			if (event.type == E_MSWINDOWS)
				ccode = UIW_WINDOW::Event(event);
			else if (columns > 1)
			{
				if (currentRecord % columns)
					SetCurrent(currentRecord - 1);
				else
					SetCurrent(MinValue(currentRecord - 1 + columns, records - 1));
			}
			break;

		case S_ADD_OBJECT:
		case S_SUBTRACT_OBJECT:
			if (event.windowObject->Inherited(ID_TABLE_HEADER) ||
				event.windowObject->Inherited(ID_TABLE_RECORD))
					ccode = UIW_WINDOW::Event(event);
			else
				tableRecord->Event(event);
			break;

		case L_BEGIN_SELECT:
#if defined(ZIL_EDIT)
			if (FlagSet(woStatus, WOS_EDIT_MODE))
			{
				ccode = UIW_WINDOW::Event(event);
				break;
			}
#endif
			if (!FlagSet(woFlags, WOF_NON_SELECTABLE) &&
				!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
			{
				if (tableRecord)
				{
					UI_POSITION position;
					POINTS point = MAKEPOINTS(event.message.lParam);
					position.column = point.x;
					position.line = point.y;
					int recordWidth = tableRecord->trueRegion.Width();
					int recordHeight = tableRecord->trueRegion.Height();
					int newCurrent = topRecord + (position.line - scroll.top) / recordHeight *
						columns + (position.column - scroll.left) / recordWidth;

					if (newCurrent < records && newCurrent != currentRecord)
					{
						UI_EVENT tEvent = event;
						tEvent.position.column = position.column -
							(newCurrent % columns) * tableRecord->trueRegion.Width() +
							tableRecord->trueRegion.left;
						tEvent.position.line = position.line -
						(newCurrent / columns) * tableRecord->trueRegion.Height() +
							tableRecord->trueRegion.top;
						SetCurrent(newCurrent);
						tableRecord->Event(tEvent);
					}
				}
				UIW_WINDOW::Event(event);
			}
			break;

		default:
			ccode = UIW_WINDOW::Event(event);
			break;
	}
	return(ccode);
}

void UIW_TABLE::SetCurrent(int recordNum)
{
	UI_EVENT recordEvent;

	ZIL_SCREENID recordID;
	tableRecord->Information(I_GET_FRAMEID, &recordID);

	// Send S_NON_CURRENT event, remove focus, and disable update.
	SendMessage(recordID, WM_SETREDRAW, (WPARAM)FALSE, 0);
	if (currentRecord >= 0)
	{
		tableRecord->Event(UI_EVENT(S_NON_CURRENT));
 		DrawRecord(currentRecord); // Draw virtual record in current positon.
	}

	// Scroll current record into view if necessary.
	currentRecord = recordNum;
	UI_EVENT scrollEvent(S_VSCROLL_CHECK);
	ScrollEvent(scrollEvent);
	scrollEvent.type = S_HSCROLL_CHECK;
	ScrollEvent(scrollEvent);

	// Set the new position.
	int column = recordNum % columns;
	int row = recordNum / columns;
	int leftColumn = topRecord % columns;
	int topRow = topRecord / columns;
	int trueWidth = virtualRecord->trueRegion.Width();
	int trueHeight = virtualRecord->trueRegion.Height();

	UI_REGION newRegion;
	newRegion.left = (column - leftColumn) * trueWidth;
	newRegion.top = (row - topRow) * trueHeight + 
		(FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL) ? 0 : display->preSpace);
	newRegion.right = newRegion.left + tableRecord->relative.Width() - 1;
	newRegion.bottom = newRegion.top + tableRecord->relative.Height() - 1;
	tableRecord->relative = newRegion;
	RegionMax(tableRecord);

	MoveWindow(recordID, tableRecord->trueRegion.left, tableRecord->trueRegion.top,
		tableRecord->trueRegion.Width(), tableRecord->trueRegion.Height(), TRUE);

	UI_EVENT dataEvent(S_SET_DATA, currentRecord);
	dataEvent.data = GetRecord(currentRecord);
	tableRecord->Event(dataEvent);

	SendMessage(recordID, WM_SETREDRAW, (WPARAM)TRUE, 0);
	InvalidateRect(tableRecord->screenID, ZIL_NULLP(RECT), FALSE);
	tableRecord->Event(UI_EVENT(S_CURRENT, 0, tableRecord->trueRegion));
}


EVENT_TYPE UIW_TABLE::ScrollEvent(UI_EVENT &event)
{
	EVENT_TYPE ccode = event.type;
	switch(ccode)
	{
	case S_VSCROLL:
		{
		// Send scroll event to scroll bar.
		if (vScroll)
			vScroll->Event(event);

		// Send scroll event to table header.
		if (rowHeader)
			rowHeader->Event(event);

		// Calculate 'topRecord' and 'scroll' values.
		UI_REGION client;
		UIW_WINDOW::Information(I_GET_CLIPREGION, &client);
		int recordHeight = tableRecord->trueRegion.Height();
		int cellHeight = (recordHeight < client.Height() ?
			recordHeight : display->cellHeight);
		int delta = event.scroll.delta;
		if (recordHeight <= client.Height())
			topRecord += delta * columns;
		else
		{
			ZIL_INT32 pixels = delta * cellHeight;
			ZIL_INT32 deltaTopRecord = pixels - scroll.top;
			if (delta < 0)
				deltaTopRecord -= recordHeight - 1;
			deltaTopRecord /= recordHeight;
			topRecord += (int)deltaTopRecord * columns;
			scroll.top += (int)(deltaTopRecord * recordHeight - pixels);
		}

		// Scroll the table.
		if (-vScrollInfo.showing <= delta && delta <= vScrollInfo.showing)
		{
			ScrollWindow(screenID, 0, -delta * cellHeight,
				ZIL_NULLP(RECT), ZIL_NULLP(RECT));
		}
		else
			InvalidateRect(screenID, ZIL_NULLP(RECT), FALSE);
		vScrollInfo.current += delta;

		// Calculate new table record position.
		UI_REGION newRegion = tableRecord->relative;
		int showing = client.Height() / recordHeight;
		if (topRecord <= currentRecord && currentRecord <= topRecord + showing * columns)
		{
			int row = currentRecord / columns;
			int topRow = topRecord / columns;
			newRegion.top = (row - topRow) * recordHeight + 
				(FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL) ? 0 : display->preSpace);
			newRegion.bottom = newRegion.top + tableRecord->relative.Height() - 1;
		}
		else
		{
			// Move table record out of client area.
			newRegion.bottom = scroll.top - 10;
			newRegion.top = newRegion.bottom - tableRecord->relative.Height() + 1;
		}

		// Move the table record.
		tableRecord->relative = newRegion;
		RegionMax(tableRecord);
		ZIL_SCREENID recordID;
		tableRecord->Information(I_GET_FRAMEID, &recordID);
		MoveWindow(recordID, tableRecord->trueRegion.left, tableRecord->trueRegion.top,
			tableRecord->trueRegion.Width(), tableRecord->trueRegion.Height(), FALSE);
		InvalidateRect(recordID, ZIL_NULLP(RECT), FALSE);

		// Force update before next scroll message.
		UpdateWindow(screenID);
		}
		break;

	case S_HSCROLL:
		{
		// Send scroll event to scroll bar.
		if (hScroll)
			hScroll->Event(event);

		// Send scroll event to table header.
		if (columnHeader)
			columnHeader->Event(event);

		// Calculate 'topRecord' and 'scroll' values.
		UI_REGION client;
		UIW_WINDOW::Information(I_GET_CLIPREGION, &client);
		int recordWidth = tableRecord->trueRegion.Width();
		int cellWidth = (recordWidth < client.Width() ?
			recordWidth : display->cellWidth);
		int delta = event.scroll.delta;
		if (recordWidth <= client.Width())
			topRecord += delta;
		else
		{
			ZIL_INT32 pixels = delta * cellWidth;
			ZIL_INT32 deltaTopRecord = pixels - scroll.left;
			if (delta < 0)
				deltaTopRecord -= recordWidth - 1;
			deltaTopRecord /= recordWidth;
			topRecord += (int)deltaTopRecord;
			scroll.left += (int)(deltaTopRecord * recordWidth - pixels);
		}

		// Scroll the table.
		if (-hScrollInfo.showing <= delta && delta <= hScrollInfo.showing)
		{
			ScrollWindow(screenID, -delta * cellWidth, 0,
				ZIL_NULLP(RECT), ZIL_NULLP(RECT));
		}
		else
			InvalidateRect(screenID, ZIL_NULLP(RECT), FALSE);
		hScrollInfo.current += delta;

		// Calculate new table record position.
		UI_REGION newRegion = tableRecord->relative;
		int showing = client.Height() / tableRecord->trueRegion.Height();
		if (topRecord <= currentRecord && currentRecord <= topRecord + showing * columns)
		{
			int column = currentRecord % columns;
			int leftColumn = topRecord % columns;
			newRegion.left = (column - leftColumn) * recordWidth;
			newRegion.right = newRegion.left + tableRecord->relative.Width() - 1;
		}
		else
		{
			// Move table record out of client area.
			newRegion.right = scroll.left - 10;
			newRegion.left = newRegion.right - tableRecord->relative.Width() + 1;
		}

		// Move the table record.
		tableRecord->relative = newRegion;
		RegionMax(tableRecord);
		ZIL_SCREENID recordID;
		tableRecord->Information(I_GET_FRAMEID, &recordID);
		MoveWindow(recordID, tableRecord->trueRegion.left, tableRecord->trueRegion.top,
			tableRecord->trueRegion.Width(), tableRecord->trueRegion.Height(), FALSE);
		InvalidateRect(recordID, ZIL_NULLP(RECT), FALSE);

		// Force update before next scroll message.
		UpdateWindow(screenID);
		}
		break;

	case S_SCROLLRANGE:
		if (tableRecord)
		{
			UI_REGION client;

			// Client Region currently includes scroll bar regions.
			UIW_WINDOW::Information(I_GET_CLIPREGION, &client);
			if (vScroll)
				client.right -= GetSystemMetrics(SM_CXVSCROLL);
			if (hScroll)
				client.bottom -= GetSystemMetrics(SM_CXVSCROLL);

			int recordWidth = tableRecord->trueRegion.Width();
			int recordHeight = tableRecord->trueRegion.Height();

			vScrollInfo.delta = 1;
			if (recordHeight < client.Height())
			{
				vScrollInfo.showing = client.Height() / recordHeight;
				vScrollInfo.maximum = MaxValue(0, records / columns - vScrollInfo.showing);
				vScrollInfo.current = topRecord / columns;
				scroll.top = 0;
			}
			else
			{
				vScrollInfo.showing = client.Height() / display->cellHeight;
				ZIL_UINT32 maximum = (records / columns) * recordHeight;
				maximum = maximum / display->cellHeight - vScrollInfo.showing;
				vScrollInfo.maximum = MaxValue(0, (int)maximum);
			}

			hScrollInfo.delta = 1;
			if (recordWidth < client.Width())
			{
				hScrollInfo.showing = client.Width() / recordWidth;
				hScrollInfo.maximum = MaxValue(0, columns - hScrollInfo.showing);
				hScrollInfo.current = topRecord % columns;
				scroll.left = 0;
			}
			else
			{
				hScrollInfo.showing = client.Width() / display->cellWidth;
				ZIL_UINT32 maximum = columns * recordWidth;
				maximum = maximum / display->cellWidth - hScrollInfo.showing;
				hScrollInfo.maximum = MaxValue(0, (int)maximum);
			}

			UI_EVENT scrollEvent;
			if (vScroll)
			{
				scrollEvent.type = S_VSCROLL_SET;
				scrollEvent.scroll = vScrollInfo;
				vScroll->Event(scrollEvent);
			}
			if (hScroll)
			{
				scrollEvent.type = S_HSCROLL_SET;
				scrollEvent.scroll = hScrollInfo;
				hScroll->Event(scrollEvent);
			}

			if (vScrollInfo.current > vScrollInfo.maximum)
			{
				scrollEvent.type = S_VSCROLL;
				scrollEvent.scroll.delta = vScrollInfo.maximum - vScrollInfo.current;
				ScrollEvent(scrollEvent);
			}
			if (hScrollInfo.current > hScrollInfo.maximum)
			{
				scrollEvent.type = S_HSCROLL;
				scrollEvent.scroll.delta = hScrollInfo.maximum - hScrollInfo.current;
				ScrollEvent(scrollEvent);
			}
		}
		break;

	case S_VSCROLL_CHECK:
		{
		int scrollDelta = 0;
		int row = currentRecord / columns - topRecord / columns;
		if (row < 0)
			scrollDelta = row;
		else if (row > vScrollInfo.showing - 1)
			scrollDelta = row - vScrollInfo.showing + 1;
					   
		if (vScrollInfo.current + scrollDelta < vScrollInfo.minimum)
			scrollDelta = vScrollInfo.minimum - vScrollInfo.current;
		else if (vScrollInfo.current + scrollDelta > vScrollInfo.maximum)
			scrollDelta = vScrollInfo.maximum - vScrollInfo.current;

		if (scrollDelta)
		{
			UI_EVENT scrollEvent(S_VSCROLL);
			scrollEvent.scroll.delta = scrollDelta;
			ScrollEvent(scrollEvent);
		}
		}
		break;

	case S_HSCROLL_CHECK:
		{
		int scrollDelta = 0;
		int column = currentRecord % columns - topRecord % columns;
		if (column < 0)
			scrollDelta = column;
		else if (column > hScrollInfo.showing - 1)
			scrollDelta = column - hScrollInfo.showing + 1;

		if (hScrollInfo.current + scrollDelta < hScrollInfo.minimum)
			scrollDelta = hScrollInfo.minimum - hScrollInfo.current;
		else if (hScrollInfo.current + scrollDelta > hScrollInfo.maximum)
			scrollDelta = hScrollInfo.maximum - hScrollInfo.current;

		if (scrollDelta)
		{
			UI_EVENT scrollEvent(S_HSCROLL);
			scrollEvent.scroll.delta = scrollDelta;
			ScrollEvent(scrollEvent);
		}
		}
		break;

	default:
		ccode = UIW_WINDOW::ScrollEvent(event);
		break;
	}
	return ccode;
	
}

void UIW_TABLE::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);
	if (!FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
	{
		if (!FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL))
		{
			object->trueRegion.top -= display->preSpace;
			object->trueRegion.bottom -=display->preSpace;
		}
#if defined(ZIL_MSWINDOWS_CTL3D)
		object->trueRegion.right += FlagSet(object->woFlags, WOF_BORDER) ? 6 : 4;
		object->trueRegion.bottom += FlagSet(object->woFlags, WOF_BORDER) ? 6 : 4;
#else
		object->trueRegion.right += FlagSet(object->woFlags, WOF_BORDER) ? 4 : 2;
		object->trueRegion.bottom += FlagSet(object->woFlags, WOF_BORDER) ? 4 : 2;
#endif
	}
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TABLE::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TABLE && FlagSet(woStatus, WOS_REDISPLAY))
	{
		Event(UI_EVENT(S_INITIALIZE));
		Event(UI_EVENT(S_CREATE));
	}
}

