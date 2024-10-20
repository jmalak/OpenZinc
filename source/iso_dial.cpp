//	Zinc Interface Library - ISO_DIAL.CPP
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


#include "gfx.h"
#if defined(DOS386)
#	undef _far
#	define _far
#endif

static unsigned short _far iso_dial_offset_table[] =
{
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0007, 0x000A, 0x0010, 0x0018, 0x001F, 0x0027, 0x0031,
	0x0035, 0x003A, 0x003F, 0x0048, 0x004F, 0x0054, 0x005B, 0x005F,
	0x0068, 0x0070, 0x0077, 0x007F, 0x0087, 0x008F, 0x0097, 0x009F,
	0x00A7, 0x00AF, 0x00B7, 0x00BA, 0x00BD, 0x00C4, 0x00CB, 0x00D2,
	0x00D9, 0x00E1, 0x00E9, 0x00F1, 0x00F9, 0x0101, 0x0108, 0x010F,
	0x0117, 0x011F, 0x0122, 0x0129, 0x0131, 0x0138, 0x0141, 0x0149,
	0x0151, 0x0159, 0x0163, 0x016B, 0x0173, 0x017C, 0x0184, 0x018D,
	0x0196, 0x019F, 0x01A8, 0x01B1, 0x01B6, 0x01BF, 0x01C4, 0x01CB,
	0x01D3, 0x01D7, 0x01DE, 0x01E5, 0x01EC, 0x01F3, 0x01FA, 0x0200,
	0x0207, 0x020E, 0x0211, 0x0217, 0x021E, 0x0221, 0x022A, 0x0231,
	0x0238, 0x023F, 0x0246, 0x024D, 0x0254, 0x0259, 0x0260, 0x0267,
	0x0272, 0x0278, 0x027F, 0x0285, 0x028B, 0x028E, 0x0294, 0x029C,
	0x02A5, 0x02A5, 0x02A5, 0x02A5, 0x02A5, 0x02A5, 0x02A5, 0x02A5,
	0x02A5, 0x02A5, 0x02A5, 0x02A5, 0x02A5, 0x02A5, 0x02A5, 0x02A5,
	0x02A5, 0x02A7, 0x02A7, 0x02A7, 0x02A7, 0x02A7, 0x02A7, 0x02A7,
	0x02A7, 0x02A7, 0x02A7, 0x02A7, 0x02A7, 0x02A7, 0x02A7, 0x02A7,
	0x02A7, 0x02AE, 0x02B1, 0x02BA, 0x02C3, 0x02CC, 0x02D5, 0x02D8,
	0x02E0, 0x02E7, 0x02F3, 0x02FA, 0x0302, 0x030A, 0x0311, 0x031D,
	0x0324, 0x032B, 0x0334, 0x033A, 0x0340, 0x0344, 0x034C, 0x0355,
	0x0359, 0x0360, 0x0365, 0x036C, 0x0374, 0x037F, 0x038A, 0x0396,
	0x039D, 0x03A5, 0x03AD, 0x03B5, 0x03BD, 0x03C5, 0x03CD, 0x03D9,
	0x03E1, 0x03E8, 0x03EF, 0x03F6, 0x03FD, 0x0401, 0x0405, 0x040B,
	0x0411, 0x041A, 0x0422, 0x042A, 0x0432, 0x043A, 0x0442, 0x044A,
	0x0452, 0x045C, 0x0464, 0x046C, 0x0474, 0x047C, 0x0485, 0x048E,
	0x0496, 0x049D, 0x04A4, 0x04AB, 0x04B3, 0x04BA, 0x04C1, 0x04CC,
	0x04D4, 0x04DB, 0x04E2, 0x04E9, 0x04F0, 0x04F5, 0x04FA, 0x0501,
	0x0508, 0x050F, 0x0517, 0x051E, 0x0525, 0x052C, 0x0534, 0x053B,
	0x0541, 0x0549, 0x0550, 0x0557, 0x055E, 0x0565, 0x056C, 0x0574,
	0x057B
};


static unsigned char _far iso_dial_data_table[] =
{

	// Scan Line #1.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// Scan Line #2.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x3C, 0x6C, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x07, 0xE7, 0x80, 0x07, 0x9C, 0x00, 0x00, 0x00, 0x00,
	0x60, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x01, 0x80, 0xC1, 0xC3, 0xB3,
	0x61, 0xC0, 0x00, 0x00, 0x18, 0x18, 0xE3, 0x66, 0x33, 0x9B, 0x00, 0x1D,
	0x98, 0x06, 0x0E, 0x1D, 0x9B, 0x00, 0x00, 0x06, 0x01, 0x83, 0x86, 0xC0,
	0xC0, 0x00, 0x01, 0x80, 0xC3, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x06, 0x03,
	0x0C, 0x00, 0xC1, 0x8C, 0x00, 0x00, 0x00, 0xC0, 0x61, 0x80, 0x00, 0x00,
	0x00, 0x30, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00,

	// Scan Line #3.
	0x01, 0xB6, 0x6C, 0x30, 0x00, 0x70, 0x31, 0xB0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x7C, 0x30, 0xF8, 0xF8, 0x19, 0xFC, 0xF9, 0xFC, 0xF8, 0xF8, 0x00,
	0x60, 0x18, 0x1E, 0x3E, 0x1C, 0x7E, 0x3E, 0x7C, 0x7E, 0xFC, 0xF9, 0x8D,
	0x83, 0x63, 0x60, 0xC3, 0x63, 0x3E, 0x7E, 0x3E, 0x1F, 0x8F, 0x9F, 0xEC,
	0x6C, 0x36, 0x1B, 0x0D, 0x86, 0xFF, 0x78, 0x01, 0xE0, 0x00, 0x18, 0x03,
	0x00, 0x00, 0xC0, 0x00, 0x01, 0x83, 0x0D, 0x83, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xDB, 0x80, 0x00,
	0x04, 0x03, 0x03, 0x0F, 0x00, 0x0C, 0x36, 0x66, 0x6C, 0x7F, 0x0F, 0x00,
	0x00, 0x00, 0x1F, 0xC0, 0x0C, 0xC0, 0x0D, 0xB6, 0x60, 0x00, 0x00, 0x00,
	0xE3, 0xC0, 0x06, 0x00, 0xC0, 0x36, 0x00, 0xC0, 0xC1, 0x83, 0x66, 0xE3,
	0x63, 0x61, 0xFF, 0x3E, 0x0C, 0x31, 0xB3, 0x63, 0x66, 0xDB, 0x3E, 0x37,
	0x0C, 0x0C, 0x1B, 0x37, 0x1B, 0x00, 0x00, 0x63, 0x03, 0x06, 0xC6, 0xC1,
	0x87, 0x80, 0x00, 0xC1, 0x87, 0x8E, 0xD9, 0x9B, 0x00, 0x00, 0x03, 0x06,
	0x1E, 0x66, 0x63, 0x1E, 0x66, 0x7C, 0xEC, 0x60, 0xC3, 0xC7, 0x6C, 0xC0,
	0x00, 0x18, 0x30, 0xF3, 0x30, 0xC0, 0x0C, 0xC0,

	// Scan Line #4.
	0x01, 0xB6, 0x6C, 0x31, 0xC0, 0xD8, 0x33, 0x18, 0x00, 0x00, 0x00, 0x00,
	0x06, 0xC6, 0x71, 0x8D, 0x8C, 0x39, 0x81, 0x8D, 0x8D, 0x8D, 0x8C, 0x00,
	0xC0, 0x0C, 0x33, 0x63, 0x36, 0x63, 0x63, 0x66, 0x60, 0xC1, 0x8D, 0x8D,
	0x83, 0x66, 0x60, 0xE7, 0x73, 0x63, 0x63, 0x63, 0x18, 0xD8, 0xC3, 0x0C,
	0x6C, 0x36, 0x1B, 0x0D, 0x86, 0x03, 0x63, 0x00, 0x63, 0x00, 0x18, 0x03,
	0x00, 0x00, 0xC0, 0x0C, 0x01, 0x83, 0x0D, 0x83, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x18, 0xC7, 0x60,
	0x04, 0x03, 0x3F, 0x19, 0x98, 0x6C, 0x36, 0x30, 0x00, 0xC1, 0x81, 0x80,
	0x00, 0x00, 0x30, 0x60, 0x0C, 0xC3, 0x03, 0x0C, 0x60, 0x07, 0xF0, 0x00,
	0x66, 0x60, 0x06, 0x30, 0xC6, 0x0C, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00,
	0x01, 0xC3, 0x70, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x01, 0xF0, 0x63, 0x0C, 0xDB, 0x99, 0x8E, 0x00, 0x00, 0x01, 0x8C,
	0x33, 0x66, 0x36, 0x33, 0x66, 0xC1, 0xB8, 0x31, 0x86, 0x6D, 0xCC, 0xCE,
	0x03, 0x0C, 0x61, 0x9B, 0x31, 0x8F, 0x0C, 0xC0,

	// Scan Line #5.
	0x01, 0xB6, 0x6C, 0x79, 0xCC, 0xD8, 0x36, 0x0C, 0xCC, 0x30, 0x00, 0x00,
	0x0C, 0xCE, 0xF0, 0x0C, 0x0C, 0x79, 0x81, 0x80, 0x0D, 0x8D, 0x8D, 0x81,
	0x80, 0x06, 0x33, 0x63, 0x63, 0x63, 0x60, 0x63, 0x60, 0xC1, 0x81, 0x8D,
	0x83, 0x6C, 0x60, 0xFF, 0x73, 0x63, 0x63, 0x63, 0x18, 0xD8, 0x03, 0x0C,
	0x6C, 0x36, 0x19, 0x98, 0xCC, 0x06, 0x61, 0x80, 0x67, 0x80, 0x18, 0x03,
	0x00, 0x00, 0xC0, 0x1E, 0x01, 0x80, 0x01, 0x83, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x18, 0xCD, 0xC0,
	0x04, 0x00, 0x67, 0x98, 0x0F, 0xC6, 0x66, 0x18, 0x01, 0x9E, 0xCF, 0x80,
	0x00, 0x00, 0x6F, 0x30, 0x07, 0x83, 0x06, 0x36, 0x60, 0x0D, 0xB0, 0x00,
	0x66, 0x60, 0x06, 0x60, 0xCC, 0x36, 0xC0, 0x01, 0xC1, 0xC1, 0xC1, 0xC1,
	0xC1, 0xC6, 0x30, 0x60, 0x7E, 0xFD, 0xFB, 0xF3, 0x63, 0x0C, 0x31, 0xB1,
	0x9F, 0x1F, 0x1F, 0x1F, 0x1F, 0x19, 0x99, 0xCC, 0x6C, 0x6C, 0x6C, 0x6C,
	0x33, 0xF3, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E,
	0x3E, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,

	// Scan Line #6.
	0x01, 0x80, 0xFE, 0xCC, 0x18, 0x70, 0x06, 0x0C, 0x78, 0x30, 0x00, 0x00,
	0x18, 0xDE, 0x30, 0x18, 0x0C, 0xD9, 0xF9, 0x80, 0x19, 0x8D, 0x8D, 0xB3,
	0x0F, 0xC3, 0x06, 0x6F, 0x63, 0x63, 0x60, 0x63, 0x60, 0xC1, 0x81, 0x8D,
	0x83, 0x78, 0x60, 0xDB, 0x7B, 0x63, 0x63, 0x63, 0x18, 0xD8, 0x03, 0x0C,
	0x6C, 0x36, 0x18, 0xF0, 0x78, 0x0C, 0x60, 0xC0, 0x6C, 0xC0, 0x0C, 0xF3,
	0xE3, 0xE7, 0xCF, 0x18, 0x7D, 0xF3, 0x0D, 0xB3, 0x7F, 0x3E, 0x3C, 0xF8,
	0xFB, 0xE3, 0xEF, 0x66, 0xCD, 0x99, 0xB6, 0xCD, 0xF3, 0x18, 0xC0, 0x01,
	0x84, 0x03, 0x6C, 0x18, 0x18, 0x63, 0xC6, 0x3C, 0x01, 0xB0, 0xD9, 0x8D,
	0x80, 0x00, 0x6D, 0xB0, 0x00, 0x1F, 0xEF, 0x9C, 0xC6, 0x6D, 0xB7, 0x00,
	0xF6, 0x6D, 0x86, 0xD8, 0xD8, 0x1D, 0xB0, 0xC3, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x66, 0x30, 0x60, 0x60, 0xC1, 0x83, 0x03, 0x63, 0x0C, 0x31, 0xB9,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0x8F, 0x33, 0x6C, 0x6C, 0x6C, 0x6C, 0x66,
	0x63, 0x1B, 0x19, 0xE3, 0xC7, 0x8F, 0x0F, 0x1E, 0x3F, 0xCC, 0x67, 0x8F,
	0x1E, 0x3C, 0x63, 0x0C, 0x18, 0x31, 0xF0, 0xF1, 0xE3, 0xC7, 0x87, 0x80,
	0x67, 0x66, 0xCD, 0x9B, 0x36, 0x67, 0xCC, 0xC0,

	// Scan Line #7.
	0x01, 0x80, 0x6C, 0xC0, 0x30, 0xF6, 0x06, 0x0D, 0xFE, 0xFC, 0x0F, 0xC0,
	0x30, 0xF6, 0x30, 0x30, 0x79, 0x98, 0x0D, 0xF8, 0x30, 0xF8, 0xFC, 0x36,
	0x00, 0x01, 0x8C, 0x6F, 0x7F, 0x7E, 0x60, 0x63, 0x7C, 0xF9, 0x9D, 0xFD,
	0x83, 0x70, 0x60, 0xC3, 0x6F, 0x63, 0x7E, 0x63, 0x1F, 0x8F, 0x83, 0x0C,
	0x6C, 0x36, 0x18, 0x60, 0x30, 0x18, 0x60, 0x60, 0x60, 0x00, 0x00, 0x1B,
	0x36, 0x0C, 0xD9, 0xBC, 0xCD, 0x9B, 0x0D, 0xE3, 0x6D, 0xB3, 0x66, 0xCD,
	0x9B, 0x36, 0x06, 0x66, 0xCD, 0x99, 0xB6, 0xCC, 0x36, 0x00, 0x60, 0x03,
	0xC4, 0x03, 0x6C, 0x3E, 0x18, 0x67, 0xE0, 0x66, 0x01, 0xB0, 0xCF, 0x9B,
	0x00, 0x3F, 0x6F, 0x30, 0x00, 0x03, 0x00, 0x00, 0x06, 0x6D, 0xB7, 0x00,
	0x03, 0xC6, 0xC7, 0xB8, 0xF7, 0x83, 0x70, 0xC6, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x37, 0xFE, 0x60, 0x60, 0xC1, 0x83, 0x03, 0x63, 0x0C, 0x7D, 0xBD,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0x86, 0x36, 0x6C, 0x6C, 0x6C, 0x6C, 0x63,
	0xC3, 0x1B, 0xF0, 0x30, 0x60, 0xC1, 0x81, 0x83, 0x06, 0x6C, 0x0C, 0xD9,
	0xB3, 0x66, 0x63, 0x0C, 0x18, 0x79, 0x99, 0x9B, 0x36, 0x6C, 0xCC, 0xDF,
	0x6F, 0x66, 0xCD, 0x9B, 0x36, 0x66, 0x6C, 0xC0,

	// Scan Line #8.
	0x01, 0x80, 0xFE, 0x78, 0x61, 0x9C, 0x06, 0x0C, 0x78, 0x30, 0x00, 0x00,
	0x60, 0xE6, 0x30, 0x60, 0x0D, 0xFC, 0x0D, 0x8C, 0x61, 0x8C, 0x0C, 0x03,
	0x0F, 0xC3, 0x0C, 0x6E, 0x63, 0x63, 0x60, 0x63, 0x60, 0xC1, 0x8D, 0x8D,
	0x83, 0x78, 0x60, 0xC3, 0x6F, 0x63, 0x60, 0x67, 0x19, 0x80, 0xC3, 0x0C,
	0x6C, 0x36, 0xD8, 0xF0, 0x30, 0x30, 0x60, 0x30, 0x60, 0x00, 0x00, 0xFB,
	0x36, 0x0C, 0xDF, 0x98, 0xCD, 0x9B, 0x0D, 0xC3, 0x6D, 0xB3, 0x66, 0xCD,
	0x9B, 0x03, 0xC6, 0x66, 0xCD, 0x99, 0x9C, 0xCC, 0x63, 0x18, 0xC0, 0x06,
	0x64, 0x03, 0x79, 0x98, 0x18, 0x61, 0x86, 0x66, 0x01, 0xB0, 0xC0, 0x36,
	0x00, 0x00, 0x6D, 0xB0, 0x00, 0x03, 0x00, 0x00, 0x06, 0x67, 0xB7, 0x00,
	0x00, 0x03, 0x63, 0x78, 0x6D, 0x86, 0xF1, 0x86, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x30, 0x60, 0x7C, 0xF9, 0xF3, 0xE3, 0x63, 0x0C, 0x31, 0xB7,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0x8F, 0x36, 0x6C, 0x6C, 0x6C, 0x6C, 0x61,
	0x83, 0x1B, 0x19, 0xF3, 0xE7, 0xCF, 0x8F, 0x9F, 0x3F, 0xEC, 0x0F, 0xDF,
	0xBF, 0x7E, 0x63, 0x0C, 0x18, 0xCD, 0x99, 0x9B, 0x36, 0x6C, 0xCC, 0xC0,
	0x7B, 0x66, 0xCD, 0x9B, 0x36, 0x66, 0x6C, 0xC0,

	// Scan Line #9.
	0x00, 0x00, 0x6C, 0x0C, 0xC1, 0x8C, 0x06, 0x0C, 0xCC, 0x30, 0x00, 0x00,
	0xC0, 0xC6, 0x30, 0xC0, 0x0C, 0x18, 0x0D, 0x8C, 0x61, 0x8C, 0x0D, 0x81,
	0x80, 0x06, 0x00, 0x60, 0x63, 0x63, 0x60, 0x63, 0x60, 0xC1, 0x8D, 0x8D,
	0xB3, 0x6C, 0x60, 0xC3, 0x67, 0x63, 0x60, 0x63, 0x18, 0xC0, 0xC3, 0x0C,
	0x66, 0x67, 0xF9, 0x98, 0x30, 0x60, 0x60, 0x18, 0x60, 0x00, 0x01, 0x9B,
	0x36, 0x0C, 0xD8, 0x18, 0xCD, 0x9B, 0x0D, 0xE3, 0x6D, 0xB3, 0x66, 0xCD,
	0x9B, 0x00, 0x66, 0x66, 0xCD, 0x99, 0x9C, 0xCC, 0xC3, 0x18, 0xC0, 0x0C,
	0x34, 0x03, 0x3F, 0x18, 0x0F, 0xC7, 0xE6, 0x3C, 0x01, 0x9E, 0xDF, 0x9B,
	0x3F, 0x80, 0x6D, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x06, 0x61, 0xB0, 0x00,
	0x07, 0xE6, 0xC6, 0xFC, 0xC3, 0x0D, 0xFB, 0x37, 0xF7, 0xF7, 0xF7, 0xF7,
	0xF7, 0xF6, 0x30, 0x63, 0x60, 0xC1, 0x83, 0x03, 0x63, 0x0C, 0x31, 0xB7,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0x99, 0x9C, 0xCC, 0x6C, 0x6C, 0x6C, 0x61,
	0x83, 0xF3, 0x1B, 0x36, 0x6C, 0xD9, 0x99, 0xB3, 0x66, 0x0C, 0x6C, 0x18,
	0x30, 0x60, 0x63, 0x0C, 0x18, 0xCD, 0x99, 0x9B, 0x36, 0x6C, 0xCC, 0xCE,
	0x73, 0x66, 0xCD, 0x9B, 0x36, 0x66, 0x6C, 0xC0,

	// Scan Line #10.
	0x01, 0x80, 0x6C, 0xCD, 0x9D, 0x8E, 0x03, 0x18, 0x00, 0x00, 0xE0, 0x1D,
	0x80, 0xC6, 0x31, 0x8D, 0x8C, 0x19, 0x8D, 0x8C, 0x61, 0x8D, 0x8D, 0xB0,
	0xC0, 0x0C, 0x0C, 0x60, 0x63, 0x63, 0x63, 0x66, 0x60, 0xC1, 0x8D, 0x8D,
	0xB3, 0x66, 0x60, 0xC3, 0x67, 0x63, 0x60, 0x63, 0x98, 0xD8, 0xC3, 0x0C,
	0x63, 0xC7, 0x3B, 0x0C, 0x30, 0xC0, 0x60, 0x0C, 0x60, 0x00, 0x01, 0x9B,
	0x36, 0x0C, 0xD9, 0x98, 0xCD, 0x9B, 0x0D, 0xB3, 0x6D, 0xB3, 0x66, 0xCD,
	0x9B, 0x00, 0x66, 0x66, 0x78, 0xFF, 0x36, 0xCD, 0x83, 0x18, 0xC0, 0x0C,
	0x34, 0x03, 0x30, 0x18, 0xD8, 0x61, 0x86, 0x18, 0x00, 0xC1, 0x80, 0x0D,
	0x81, 0x80, 0x30, 0x60, 0x00, 0x1F, 0xE0, 0x00, 0x07, 0xC1, 0xB0, 0x0C,
	0x00, 0x0D, 0x8C, 0x19, 0x86, 0x18, 0x33, 0x36, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x30, 0x3E, 0x60, 0xC1, 0x83, 0x03, 0x63, 0x0C, 0x33, 0x33,
	0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0x80, 0x1F, 0x8C, 0x6C, 0x6C, 0x6C, 0x61,
	0x83, 0x03, 0x73, 0x36, 0x6C, 0xD9, 0x99, 0xB3, 0x66, 0x67, 0xCC, 0xD9,
	0xB3, 0x66, 0x63, 0x0C, 0x18, 0xCD, 0x99, 0x9B, 0x36, 0x6C, 0xCC, 0xCE,
	0x3E, 0x66, 0xCD, 0x9B, 0x36, 0x67, 0xCC, 0xC0,

	// Scan Line #11.
	0x01, 0x80, 0x6C, 0x78, 0x1C, 0xFB, 0x01, 0xB0, 0x00, 0x00, 0xE0, 0x1C,
	0x00, 0x7C, 0xFD, 0xFC, 0xF8, 0x3C, 0xF8, 0xF8, 0x60, 0xF8, 0xF8, 0x30,
	0x60, 0x18, 0x0C, 0x3F, 0x63, 0x7E, 0x3E, 0x7C, 0x7E, 0xC0, 0xF9, 0x8D,
	0x9E, 0x63, 0x7E, 0xC3, 0x63, 0x3E, 0x60, 0x3E, 0xD8, 0xCF, 0x83, 0x07,
	0xC1, 0x86, 0x1B, 0x0C, 0x30, 0xFF, 0x78, 0x01, 0xE0, 0x00, 0x00, 0xFB,
	0xE3, 0xE7, 0xCF, 0x18, 0x7D, 0x9B, 0x0D, 0x9B, 0x6D, 0xB3, 0x3C, 0xF8,
	0xFB, 0x07, 0xC6, 0x3E, 0x30, 0x66, 0x36, 0x7D, 0xF1, 0xDB, 0x80, 0x0F,
	0xF4, 0x03, 0x60, 0x3F, 0x80, 0x01, 0x86, 0x0C, 0x00, 0x7F, 0x00, 0x00,
	0x01, 0x80, 0x1F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0xB0, 0x0C,
	0x00, 0x00, 0x00, 0x18, 0x0F, 0x80, 0x31, 0xE6, 0x36, 0x36, 0x36, 0x36,
	0x36, 0x36, 0x3F, 0x0C, 0x7E, 0xFD, 0xFB, 0xF3, 0x63, 0x0C, 0x3E, 0x31,
	0x9F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, 0x30, 0x07, 0xC7, 0xC7, 0xC7, 0xC1,
	0x87, 0x83, 0x01, 0xF3, 0xE7, 0xCF, 0x8F, 0x9F, 0x3F, 0xC1, 0x87, 0x8F,
	0x1E, 0x3C, 0x63, 0x0C, 0x18, 0x79, 0x98, 0xF1, 0xE3, 0xC7, 0x87, 0x80,
	0x60, 0x3E, 0x7C, 0xF9, 0xF3, 0xE6, 0x07, 0xC0,

	// Scan Line #12.
	0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0xC0,
	0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x66,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0x00, 0xC0,

	// Scan Line #13.
	0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0xC0,
	0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x07, 0xC0, 0x0F, 0x80
};


static FONT iso_dial =
{
	iso_dial_offset_table,
	iso_dial_data_table,

	176,		// The number of bytes in each row of the bitmap.
	 13,		// The number of rows in the bitmap.
	  0,		// The first character in the bitmap.
	255,		// The last character in the bitmap.
	 12,		// The widest character, in bits.
	 10,		// The distance from the baseline to the top of a character.
	 11,		// The distance from the top to the underline bar.
	  0 		// The flag for a ROM resident font.
};

FONT *GFX_iso_dial_font (void)
{
	return (&iso_dial);
}