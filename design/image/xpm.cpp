//	Zinc Application Framework - XPM.CPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "image.hpp"
#include "xpm.hpp"

XPM_COLOR_MATCH _zincRGBValue[] =
{
	{ "0", 0x00, 0x00, 0x00, "c black" },	   		// 0 - BLACK
	{ "1", 0x00, 0x00, 0x80, "c blue" },	   		// 1 - BLUE
	{ "2", 0x00, 0x80, 0x00, "c green" },	   		// 2 - GREEN
	{ "3", 0x00, 0x80, 0x80, "c cyan" },	   		// 3 - CYAN
	{ "4", 0x80, 0x00, 0x00, "c red" },				// 4 - RED
	{ "5", 0x80, 0x00, 0x80, "c magenta" },			// 5 - MAGENTA
	{ "6", 0x80, 0x80, 0x00, "c brown" },	   		// 6 - BROWN
	{ "7", 0xC0, 0xC0, 0xC0, "c light_gray" }, 		// 7 - LIGHTGRAY
	{ "8", 0x80, 0x80, 0x80, "c dark_gray" },  		// 8 - DARKGRAY
	{ "9", 0x00, 0x00, 0xFF, "c light_blue" }, 		// 9 - LIGHTBLUE
	{ "A", 0x00, 0xFF, 0x00, "c light_green" },		// 10 - LIGHTGREEN
	{ "B", 0x00, 0xFF, 0xFF, "c light_cyan" }, 		// 11 - LIGHTCYAN
	{ "C", 0xFF, 0x00, 0x00, "c light_red" },  		// 12 - LIGHTRED
	{ "D", 0xFF, 0x00, 0xFF, "c light_magenta" },	// 13 - LIGHTMAGENTA
	{ "E", 0xFF, 0xFF, 0x00, "c light_yellow" },	// 14 - YELLOW
	{ "F", 0xFF, 0xFF, 0xFF, "c white" },			// 15 - WHITE

	{ " ", 0xAA, 0xAA, 0xAA, "c none" }				// FF - BACKGROUND
};

