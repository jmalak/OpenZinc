//	Zinc Interface Library - Z_MAP2.CPP
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


#define INCL_WININPUT
#include "ui_win.hpp"

// ----- UI_EVENT_MAP -------------------------------------------------------

EVENT_TYPE UI_EVENT_MAP::MapEvent(UI_EVENT_MAP *mapTable,
	const UI_EVENT &event, ZIL_OBJECTID id1, ZIL_OBJECTID id2, ZIL_OBJECTID id3,
	ZIL_OBJECTID id4, ZIL_OBJECTID id5)
{
#if defined(ZIL_MSDOS) || defined(ZIL_CURSES)
	// See if the type is already mapped.
	EVENT_TYPE logicalValue = event.type;
	EVENT_TYPE eventType = event.type;
	if (logicalValue > E_DEVICE_LAST)
		return (logicalValue);
	ZIL_RAW_CODE eventRawCode = event.rawCode;
#elif defined(ZIL_MSWINDOWS)
	// See if the type is already mapped.
	EVENT_TYPE logicalValue = event.type;
	EVENT_TYPE eventType = event.message.message;
	ZIL_RAW_CODE eventRawCode;
	if (logicalValue != E_MSWINDOWS)
		return (logicalValue);
	else if (eventType >= WM_KEYFIRST && eventType <= WM_KEYLAST)
		eventRawCode = event.key.value;
	else if (eventType >= WM_MOUSEFIRST && eventType <= WM_MOUSELAST)
		eventRawCode = event.rawCode;
	else
		switch (eventType)
		{
		case WM_ACTIVATE:
		case WM_ACTIVATEAPP:
		case WM_ENABLE:
		case WM_MENUCHAR:
		case WM_SYSCHAR:
			eventRawCode = event.message.wParam;
			break;

		case WM_SYSCOMMAND:
			eventRawCode = event.message.wParam & 0xFFF0;
			break;

		default:
			eventRawCode = 0;
			break;
		}
#elif defined(ZIL_OS2)
	// See if the type is already mapped.
	EVENT_TYPE logicalValue = event.type;
	EVENT_TYPE eventType = event.message.msg;
	ZIL_RAW_CODE eventRawCode = 0;
	if (logicalValue != E_OS2)
		return (logicalValue);
	else if (eventType == WM_CHAR)
		eventRawCode = event.rawCode;
	else if (eventType >= WM_MOUSEFIRST && eventType <= WM_MOUSELAST)
		eventRawCode = event.rawCode;
#elif defined(ZIL_XT)
	// See if the type is already mapped.
	EVENT_TYPE logicalValue = event.type;
	EVENT_TYPE eventType = event.message.type;
	if (logicalValue != E_XT)
		return (logicalValue);
	ZIL_RAW_CODE eventRawCode = event.rawCode;
#elif defined(ZIL_MACINTOSH)
	// See if the type is already mapped.
	EVENT_TYPE eventType = event.message.what;
	EVENT_TYPE logicalValue = event.type;
	if (logicalValue != E_MACINTOSH)
		return (logicalValue);
	ZIL_RAW_CODE eventRawCode = event.rawCode;
#elif defined(ZIL_NEXTSTEP)
	// See if the type is already mapped.
	EVENT_TYPE eventType = event.message.type;
	EVENT_TYPE logicalValue = event.type;
	if (logicalValue != E_NEXTSTEP)
		return (logicalValue);
	ZIL_RAW_CODE eventRawCode = event.rawCode;
#endif
#if defined(ZIL_NEXTSTEP)
	// This is because NEXTSTEP uses id.
	int level = 4;
	ZIL_OBJECTID idArray[4];
	idArray[0] = id2; idArray[1] = id3; idArray[2] = id4; idArray[3] = id5;

	// Try to find a matching event.
	for (UI_EVENT_MAP *map = mapTable; map->objectID != ID_END; map++)
		if (map->rawCode == eventRawCode && map->eventType == eventType &&
			map->modifiers == event.modifiers)
		{
			if (map->objectID == id1)
				return (map->logicalValue);
			for (int i = 0; i < 4; i++)
				if (map->objectID == idArray[i] && level > i)
				{
					level = i;
					logicalValue = map->logicalValue;
					break;
				}
		}

	// Return the logical match value.
	return (logicalValue);
#else
	int level = 4;
	ZIL_OBJECTID id[4];
	id[0] = id2; id[1] = id3; id[2] = id4; id[3] = id5;

	// Try to find a matching event.
	for (UI_EVENT_MAP *map = mapTable; map->objectID != ID_END; map++)
		if (map->rawCode == eventRawCode && map->eventType == eventType &&
			map->modifiers == event.modifiers)
		{
			if (map->objectID == id1)
				return (map->logicalValue);
			for (int i = 0; i < 4; i++)
				if (map->objectID == id[i] && level > i)
				{
					level = i;
					logicalValue = map->logicalValue;
					break;
				}
		}

	// Return the logical match value.
	return (logicalValue);
#endif
}
