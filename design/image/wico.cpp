//	Zinc Application Framework - WICO.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "wdib.hpp"

// ----- WICO_FILE_HEADER ---------------------------------------------------

ZIL_UINT32 WICO_FILE_HEADER::_size = 6;

WICO_FILE_HEADER::WICO_FILE_HEADER(void)
{
	reserved = iconType = iconCount = 0; // ZIL_UINT16 variables.
}

// ----- WICO_DIRECTORY -----------------------------------------------------

ZIL_UINT32 WICO_DIRECTORY::_size = 16;

WICO_DIRECTORY::WICO_DIRECTORY(void)
{
	width = height = colorCount = reserved1 = 0; // ZIL_UINT8 variables.
	hotspotX = hotspotY = 0; // ZIL_UINT16 variables.
	icoDIBSize = icoDIBOffset = 0; // ZIL_UINT32 variables.
}

