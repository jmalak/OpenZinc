//	Zinc Interface Library - D_TABLE.CPP
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


#include <ui_win.hpp>

EVENT_TYPE UIW_TABLE::DrawItem(const UI_EVENT &event, EVENT_TYPE ccode)
{
	UI_REGION updateRegion;
	UI_REGION client = clipList.First()->region;
	if (!event.region.Overlap(trueRegion, updateRegion))
		return ccode;

	display->VirtualGet(screenID, updateRegion);

	int recordWidth = 0;
	int recordHeight = 0;

	int firstColumn = topRecord % columns;
	if (virtualRecord)
	{
		recordWidth = virtualRecord->trueRegion.Width();
		recordHeight = virtualRecord->trueRegion.Height();
		int i = topRecord;
		while (i < records)
		{
			if (client.left + scroll.left + (i % columns - firstColumn) * recordWidth > updateRegion.right)
				i += columns - (i % columns);
			else if (client.left + scroll.left + ((i % columns) + 1 - firstColumn) * recordWidth <= updateRegion.left)
				i++;
			else if (client.top + scroll.top + (i - topRecord) / columns * recordHeight > updateRegion.bottom)
				break;
			else
				DrawRecord(i++);
		}
	}

	int rows = (records - topRecord + columns - 1) / columns;
	if (client.top + rows * recordHeight <= updateRegion.bottom)
	{
		UI_REGION drawRegion = updateRegion;
		drawRegion.top = client.top + (scroll.top + rows) * recordHeight;
		display->Rectangle(screenID, drawRegion, lastPalette, 0, TRUE, FALSE, &clip);
		updateRegion.bottom = drawRegion.top;
	}

	if (client.left + scroll.left + (columns - firstColumn) * recordWidth <= updateRegion.right)
	{
		UI_REGION drawRegion = updateRegion;
		drawRegion.left = client.left + scroll.left + (columns - firstColumn) * recordWidth;
		display->Rectangle(screenID, drawRegion, lastPalette, 0, TRUE, FALSE, &clip);
	}

	UI_REGION region = trueRegion;
	if (!display->isText && FlagSet(woFlags, WOF_BORDER) && !parent->Inherited(ID_TABLE))
		DrawBorder(screenID, region, FALSE, ccode);

	display->VirtualPut(screenID);
	return (ccode);
}

void UIW_TABLE::DrawRecord(int recordNum)
{

#if defined(ZIL_EDIT)
	if (FlagSet(parent->woStatus, WOS_EDIT_MODE))
		return;
#endif
	int column = (recordNum - topRecord) % columns;
	int row = (recordNum - topRecord) / columns;
	UI_REGION newRegion;
	newRegion.left = column * virtualRecord->trueRegion.Width();
	newRegion.top = row * virtualRecord->trueRegion.Height() +
		(FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL) ? 0 : display->preSpace);
	newRegion.right = newRegion.left + virtualRecord->relative.Width() - 1;
	newRegion.bottom = newRegion.top + virtualRecord->relative.Height() - 1;
	virtualRecord->relative = newRegion;
	virtualRecord->Event(UI_EVENT(S_CHANGED));

	UI_EVENT dataEvent(S_SET_DATA);
	dataEvent.rawCode = recordNum;
	dataEvent.data = GetRecord(recordNum);
	virtualRecord->Event(dataEvent);

	UI_EVENT updateEvent(S_DISPLAY_ACTIVE);
	updateEvent.region = virtualRecord->trueRegion;
	virtualRecord->woStatus |= WOS_REDISPLAY;
	virtualRecord->woStatus &= ~WOS_CURRENT;
	virtualRecord->UI_WINDOW_OBJECT::Event(updateEvent);
}

EVENT_TYPE UIW_TABLE::Event(const UI_EVENT &event)
{
	// Switch on the event type.
	EVENT_TYPE ccode = LogicalEvent(event, ID_TABLE);
	switch (ccode)
	{
	case S_INITIALIZE:
		ccode = UIW_WINDOW::Event(event);
		if (virtualRecord)
		{
			virtualRecord->parent = this;
			virtualRecord->Event(event);
		}
		break;

	case S_CREATE:
		currentRecord = topRecord = 0;
		if (tableRecord)
		{
			UI_EVENT dataEvent(S_SET_DATA);
			dataEvent.rawCode = currentRecord;
			dataEvent.data = GetRecord(currentRecord);
			tableRecord->Event(dataEvent);
		}
		// Continue.

	case S_CHANGED:
		{
		UIW_WINDOW::Event(event);
		if (virtualRecord)
		{
			virtualRecord->Event(event);

			if (ccode == S_CREATE)
				UIW_TABLE::Event(UI_EVENT(S_CHANGED));
		}
		}
		break;

	case S_NON_CURRENT:
		if (current && FlagSet(Current()->woStatus, WOS_CURRENT))
			Current()->Event(UI_EVENT(S_NON_CURRENT, 0, trueRegion));
		UI_WINDOW_OBJECT::Event(event);
		break;

	case S_REDISPLAY:
		if (screenID && Inherited(ID_TABLE_HEADER) && parent)
		{
			Root(TRUE)->Event(UI_EVENT(S_CHANGED));
			parent->Event(UI_EVENT(RedisplayType(), 0, trueRegion));
		}
		else
			UIW_WINDOW::Event(event);
		break;


	case L_PGUP:
	case L_PGDN:
		if (tableRecord && tableRecord->Event(event) == S_UNKNOWN)
		{
			int recordHeight = virtualRecord->trueRegion.Height();
			int deltaRows = (ccode == L_PGUP ?
				-vScrollInfo.showing : vScrollInfo.showing) / recordHeight;
			int newRecord = currentRecord + (deltaRows * columns);
			if (newRecord < 0)
				newRecord = currentRecord % columns;
			if (newRecord > records - 1)
				newRecord = records - columns + currentRecord % columns;
			if (currentRecord != newRecord)
				SetCurrent(newRecord);
		}
		break;

	case L_LEFT:
	case L_RIGHT:
	case L_UP:
	case L_DOWN:
		if (tableRecord && tableRecord->Event(event) == S_UNKNOWN)
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
	case L_PREVIOUS:
		if (tableRecord && tableRecord->Event(event) == S_UNKNOWN)
		{
			int column = currentRecord % columns;
			int row = currentRecord / columns;

			if (ccode == L_PREVIOUS && column > 0)
				--column;
			else if (ccode == L_PREVIOUS && columns > 1)
				column = columns - 1;
			else if (ccode == L_NEXT && column < columns - 1)
				++column;
			else if (ccode == L_NEXT && columns > 1)
				column = 0;
			else
				break;
			SetCurrent(row * columns + column);
		}
		break;

	case S_ADD_OBJECT:
	case S_SUBTRACT_OBJECT:
		if (event.windowObject->Inherited(ID_TABLE_HEADER))
			ccode = UIW_WINDOW::Event(event);
		else
			tableRecord->Event(event);
		break;

	case L_BEGIN_SELECT:
		if (!FlagSet(woFlags, WOF_NON_SELECTABLE) &&
			!FlagSet(woAdvancedFlags, WOAF_NON_CURRENT))
		{
			UI_REGION client = clipList.First()->region;
			if (client.Overlap(event.position) && tableRecord)
			{
				UI_POSITION position = event.position;
				int recordWidth = tableRecord->trueRegion.Width();
				int recordHeight = tableRecord->trueRegion.Height();
				position.column -= (client.left + scroll.left);
				position.line -= (client.top + scroll.top);
				int newCurrent = position.line / recordHeight *
					columns + position.column / recordWidth + topRecord;

				if (newCurrent < records && newCurrent != currentRecord)
				{
					woStatus |= WOS_INTERNAL_ACTION;
					SetCurrent(newCurrent);
					tableRecord->Event(event);
					woStatus &= ~WOS_INTERNAL_ACTION;
				}
				if (!FlagSet(tableRecord->woStatus, WOS_CURRENT))
					tableRecord->Event(UI_EVENT(S_CURRENT, 0, tableRecord->trueRegion));
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

EVENT_TYPE UIW_TABLE::ScrollEvent(UI_EVENT &event)
{
	EVENT_TYPE ccode = LogicalEvent(event);
	switch (ccode)
	{
		case S_SCROLLRANGE:
			if (tableRecord)
			{
				UI_REGION client = clipList.First()->region;

				int recordWidth = tableRecord->trueRegion.Width();
				int recordHeight = tableRecord->trueRegion.Height();

				if (recordHeight < client.Height())
				{
					vScrollInfo.delta = 1;
					vScrollInfo.showing = (client.Height() + recordHeight -1)
						/ recordHeight;
					vScrollInfo.maximum = MaxValue(0, records/columns - vScrollInfo.showing + 1);
					vScrollInfo.current = topRecord / columns;
					scroll.top = 0;
				}
				else
				{
					vScrollInfo.delta = display->cellHeight;
					vScrollInfo.showing = client.Height() / display->cellHeight;
					ZIL_UINT32 maximum = (records / columns) * recordHeight;
					maximum = maximum / display->cellHeight - vScrollInfo.showing;
					vScrollInfo.maximum = MaxValue(0, (int)maximum);
				}

				if (recordWidth < client.Width())
				{
					hScrollInfo.delta = 1;
					hScrollInfo.showing = (client.Width() + recordWidth - 1)
						/ recordWidth;
					hScrollInfo.maximum = MaxValue(0, columns - hScrollInfo.showing + 1);
					hScrollInfo.current = topRecord % columns;
					scroll.left = 0;
				}
				else
				{
					hScrollInfo.delta = display->cellWidth;
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
				int recordWidth = tableRecord->trueRegion.Width();
				UI_REGION client = clipList.First()->region;
				int cellWidth = (recordWidth < client.Width()) ? recordWidth :
					display->cellWidth;

				if (recordWidth != cellWidth) // Scrolling by cellWidths
				{
					if (column < 0)
						scrollDelta = column * tableRecord->trueRegion.Width() / cellWidth;
					else if (column * recordWidth / cellWidth > hScrollInfo.current
						+ hScrollInfo.showing - 1)
						scrollDelta = column * recordWidth / cellWidth -
							hScrollInfo.current;
				}
				else // Scrolling by records
				{
					if (column < 0)
						scrollDelta = column;
					else if (column > hScrollInfo.showing - 1)
						scrollDelta = column - hScrollInfo.showing + 1;
				}

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

		case S_VSCROLL:
			{
				UI_REGION client = clipList.First()->region;
				int delta = event.scroll.delta;
				int recordHeight = tableRecord->trueRegion.Height();
				int cellHeight = (recordHeight < client.Height()) ? recordHeight :
					display->cellHeight;

				if (cellHeight != recordHeight)
				{
					if (vScrollInfo.current + delta < 0)
						delta = - vScrollInfo.current;
					else if (vScrollInfo.current + delta > vScrollInfo.maximum)
						delta = vScrollInfo.maximum - vScrollInfo.current;
				}
				else
				{
					int firstRow = topRecord / columns;
					if ((firstRow + delta) < 0)
						delta = - firstRow;
					else if (delta + firstRow > vScrollInfo.maximum)
						delta = vScrollInfo.maximum - firstRow;
				}

				if (delta)
				{
					UI_EVENT tEvent = event;
					tEvent.scroll.delta = delta;

					vScrollInfo.current += delta;

					if (vScroll)
						vScroll->Event(tEvent);
					if (rowHeader)
						rowHeader->Event(tEvent);

					if (cellHeight != recordHeight)
					{
						scroll.top -= delta;
						if (-scroll.top / cellHeight > vScrollInfo.maximum)
							scroll.top = -vScrollInfo.maximum * cellHeight;
					}
					else
					{
						topRecord += delta * columns;
					}


					if (tableRecord)
					{
						if (cellHeight == recordHeight)
						{
							tableRecord->relative.top -= delta * cellHeight;
							tableRecord->relative.bottom -= delta * cellHeight;
						}
						tableRecord->Event(S_CHANGED);
					}

					if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
					{
						UI_REGION updateRegion = clipList.First()->region;
						eventManager->DeviceState(E_CURSOR, D_OFF);
						Event(UI_EVENT(RedisplayType(), 0, updateRegion));
					}
				}
			}
			break;

		case S_HSCROLL:
			{
				UI_REGION client = clipList.First()->region;
				int delta = event.scroll.delta;
				int recordWidth = tableRecord->trueRegion.Width();
				int cellWidth = (recordWidth < client.Width()) ? recordWidth :
					display->cellWidth;

				if (cellWidth != recordWidth)
				{
					if (hScrollInfo.current + delta < 0)
						delta = - hScrollInfo.current;
					else if (hScrollInfo.current + delta > hScrollInfo.maximum)
						delta = hScrollInfo.maximum - hScrollInfo.current;
				}
				else
				{
					int firstColumn = topRecord % columns;
					if ((firstColumn + delta) < 0)
						delta = - firstColumn;
					else if (delta + firstColumn > hScrollInfo.maximum)
						delta = hScrollInfo.maximum - firstColumn;
				}

				if (delta)
				{
					UI_EVENT tEvent = event;
					tEvent.scroll.delta = delta;
					if (hScroll)
						hScroll->Event(tEvent);
					if (columnHeader)
						columnHeader->Event(tEvent);

					hScrollInfo.current += delta;

					if (recordWidth != cellWidth)
					{
						scroll.left -= delta * display->cellWidth;
						if (-scroll.left / cellWidth > hScrollInfo.maximum)
							scroll.left = -hScrollInfo.maximum * cellWidth;
					}
					else
					{
						topRecord += delta;
					}

					if (tableRecord)
					{
						if (cellWidth == recordWidth)
						{
							tableRecord->relative.left -= delta * cellWidth;
							tableRecord->relative.right -= delta * cellWidth;
						}
						tableRecord->Event(S_CHANGED);
					}

					if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
					{
						UI_REGION updateRegion = clipList.First()->region;
						eventManager->DeviceState(E_CURSOR, D_OFF);
						Event(UI_EVENT(RedisplayType(), 0, updateRegion));
					}
				}
			}
			break;

		default:
			ccode = UIW_WINDOW::ScrollEvent(event);
	}
	return ccode;
}

void UIW_TABLE::SetCurrent(int recordNum)
{
	eventManager->DeviceState(E_CURSOR, D_OFF);

	currentRecord = recordNum;

	int scrolled = FALSE;

	// Don't do any scrolling if current selection made by mouse.
	if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
	{
		woStatus |= WOS_INTERNAL_ACTION;  // Don't draw until finished scrolling
		int hCurrent = hScrollInfo.current;
		int vCurrent = vScrollInfo.current;

		UI_EVENT scrollEvent(S_VSCROLL_CHECK);
		ScrollEvent(scrollEvent);
		scrollEvent.type = S_HSCROLL_CHECK;
		ScrollEvent(scrollEvent);
		if (vScrollInfo.current != vCurrent || hScrollInfo.current != hCurrent)
			scrolled = TRUE;
		woStatus &= ~WOS_INTERNAL_ACTION;  
	}

	UI_EVENT recordEvent;
	recordEvent.type = S_NON_CURRENT;
	recordEvent.region = tableRecord->trueRegion;
	tableRecord->Event(recordEvent);

	int column = (recordNum  - topRecord) % columns;
	int row = (recordNum - topRecord) / columns;
	UI_REGION newRegion;
	newRegion.left = column * tableRecord->trueRegion.Width();
	newRegion.top = row * tableRecord->trueRegion.Height() +
		(FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL) ? 0 : display->preSpace);
	newRegion.right = newRegion.left + tableRecord->relative.Width() - 1;
	newRegion.bottom = newRegion.top + tableRecord->relative.Height() - 1;
	tableRecord->relative = newRegion;
	tableRecord->Event(UI_EVENT(S_CHANGED));

	UI_EVENT dataEvent(S_SET_DATA);
	dataEvent.rawCode = currentRecord;
	dataEvent.data = GetRecord(currentRecord);
	tableRecord->Event(dataEvent);

	// Scroll partially visaible records into view
	if (!FlagSet(woStatus, WOS_INTERNAL_ACTION))
	{
		woStatus |= WOS_INTERNAL_ACTION;  // Don't draw until finished scrolling
		UI_REGION client = clipList.First()->region;
		if (tableRecord->trueRegion.top < client.top)
		{
			int recordHeight = tableRecord->trueRegion.Height();
			int cellHeight = (recordHeight < client.Height()) ? recordHeight :
				display->cellHeight;
			scrolled = TRUE;
			UI_EVENT scrollEvent(S_VSCROLL);
			scrollEvent.scroll.delta = (tableRecord->trueRegion.top - client.top -
				cellHeight + 1) / cellHeight;
			Event(scrollEvent);
		}
		else if (tableRecord->trueRegion.bottom > client.bottom &&
			tableRecord->trueRegion.top > client.top &&
			tableRecord->trueRegion.Height() < client.Height())
		{
			int recordHeight = tableRecord->trueRegion.Height();
			int cellHeight = (recordHeight < client.Height()) ? recordHeight :
				display->cellHeight;
			scrolled = TRUE;
			UI_EVENT scrollEvent(S_VSCROLL);
			scrollEvent.scroll.delta = (tableRecord->trueRegion.bottom - client.bottom +
				cellHeight - 1) / cellHeight;
			Event(scrollEvent);
		}
		if (tableRecord->trueRegion.left < client.left)
		{
			int recordWidth = tableRecord->trueRegion.Width();
			int cellWidth = (recordWidth < client.Width()) ? recordWidth :
				display->cellWidth;
			scrolled = TRUE;
			UI_EVENT scrollEvent(S_HSCROLL);
			scrollEvent.scroll.delta = (tableRecord->trueRegion.left - client.left -
				cellWidth + 1) / cellWidth;
			Event(scrollEvent);
		}
		else if (tableRecord->trueRegion.right > client.right &&
			tableRecord->trueRegion.left > client.left && tableRecord->trueRegion.Width() <= client.Width())
		{
			int recordWidth = tableRecord->trueRegion.Width();
			int cellWidth = (recordWidth < client.Width()) ? recordWidth :
				display->cellWidth;
			scrolled = TRUE;
			UI_EVENT scrollEvent(S_HSCROLL);
			scrollEvent.scroll.delta = (tableRecord->trueRegion.right - client.right + cellWidth - 1)
				/ cellWidth;
			Event(scrollEvent);
		}
		woStatus &= ~WOS_INTERNAL_ACTION;  
	}

	if (scrolled)
	{
		UI_REGION updateRegion = clipList.First()->region;
		eventManager->DeviceState(E_CURSOR, D_OFF);
		Event(UI_EVENT(RedisplayType(), 0, updateRegion));

	}
	
	if	((!FlagSet(tableRecord->woStatus, WOS_CURRENT) &&
		!FlagSet(woStatus, WOS_INTERNAL_ACTION)) || scrolled)
	{
		recordEvent.type = S_CURRENT;
		recordEvent.region = tableRecord->trueRegion;
		tableRecord->Event(recordEvent);
	}
}

void UIW_TABLE::RegionMax(UI_WINDOW_OBJECT *object)
{
	UIW_WINDOW::RegionMax(object);

	if (!display->isText && !FlagSet(object->woFlags, WOF_NON_FIELD_REGION))
	{
		if (!FlagSet(woFlags, WOF_MINICELL | WOF_PIXEL))
		{
			object->trueRegion.top -= display->preSpace;
			object->trueRegion.bottom -=display->preSpace;
		}
		object->trueRegion.right += FlagSet(object->woFlags, WOF_BORDER) ? 4 : 2;
		object->trueRegion.bottom += FlagSet(object->woFlags, WOF_BORDER) ? 4 : 2;
		if (Inherited(ID_TABLE_HEADER) && FlagSet(object->woFlags, WOF_BORDER))
		{
			THF_FLAGS thFlags;
			Information(I_GET_FLAGS, &thFlags, ID_TABLE_HEADER);
			if (FlagSet(thFlags, THF_COLUMN_HEADER))
				object->trueRegion.bottom -= 2;
			else if (FlagSet(thFlags, THF_ROW_HEADER))
				object->trueRegion.right -= 2;
		}
	}
}

// ----- OS Specific Functions ----------------------------------------------

void UIW_TABLE::OSUpdateSettings(ZIL_OBJECTID objectID)
{
	// See if the field needs to be re-computed.
	if (objectID == ID_TABLE && FlagSet(woStatus, WOS_REDISPLAY))
	{
		UI_EVENT event(S_INITIALIZE, 0);
		Event(event);
		event.type = S_CREATE;
		Event(event);
		event.type = S_REDISPLAY;
		Event(event);
	}
}
