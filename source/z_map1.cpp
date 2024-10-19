//	Zinc Interface Library - Z_MAP1.CPP
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


#include "ui_win.hpp"

extern UI_PALETTE_MAP *_normalPaletteMapTable;
// ----- UI_PALETTE_MAP -----------------------------------------------------
#if defined(ZIL_NEXTSTEP)
// This is necessary since NEXTSTEP uses id.
UI_PALETTE *UI_PALETTE_MAP::MapPalette(UI_PALETTE_MAP *mapTable,
	ZIL_LOGICAL_PALETTE logicalPalette, ZIL_OBJECTID id1, ZIL_OBJECTID id2,
	ZIL_OBJECTID id3, ZIL_OBJECTID id4, ZIL_OBJECTID id5)
{
	int level = 10;
	if (!mapTable)
		mapTable = _normalPaletteMapTable;

	UI_PALETTE *palette = &mapTable[0].palette;

	
	int idArray[5];
	idArray[0] = id1; idArray[1] = id2; idArray[2] = id3; idArray[3] = id4; idArray[4] = id5;

	// Try to find a matching palette.
	for (UI_PALETTE_MAP *map = mapTable; map->objectID != ID_END; map++)
		if (map->objectID == id1 && FlagsSet(logicalPalette, map->logicalPalette))
			return (&map->palette);
		else
		{
			for (int i = 0; i < 5; i++)
				if (map->objectID == idArray[i] &&
					map->logicalPalette == PM_ANY && level > 2 * i + 1)
				{
					level = 2 * i + 1;
					palette = &map->palette;
					break;
				}
				else if (map->objectID == idArray[i] &&
					FlagsSet(logicalPalette, map->logicalPalette) && level > 2 * i)
				{
					level = 2 * i;
					palette = &map->palette;
					break;
				}
		}
	// Return the logical match palette.
	return (palette);
}
#else

UI_PALETTE *UI_PALETTE_MAP::MapPalette(UI_PALETTE_MAP *mapTable,
	ZIL_LOGICAL_PALETTE logicalPalette, ZIL_OBJECTID id1, ZIL_OBJECTID id2,
	ZIL_OBJECTID id3, ZIL_OBJECTID id4, ZIL_OBJECTID id5)
{
	int level = 10;
	if (!mapTable)
		mapTable = _normalPaletteMapTable;

	UI_PALETTE *palette = &mapTable[0].palette;
	int id[5];
	id[0] = id1; id[1] = id2; id[2] = id3; id[3] = id4; id[4] = id5;

	// Try to find a matching palette.
	for (UI_PALETTE_MAP *map = mapTable; map->objectID != ID_END; map++)
		if (map->objectID == id1 && FlagsSet(logicalPalette, map->logicalPalette))
			return (&map->palette);
		else
		{
			for (int i = 0; i < 5; i++)
				if (map->objectID == id[i] &&
					map->logicalPalette == PM_ANY && level > 2 * i + 1)
				{
					level = 2 * i + 1;
					palette = &map->palette;
					break;
				}
				else if (map->objectID == id[i] &&
					FlagsSet(logicalPalette, map->logicalPalette) && level > 2 * i)
				{
					level = 2 * i;
					palette = &map->palette;
					break;
				}
		}

	// Return the logical match palette.
	return (palette);
}

#endif
