//	Zinc Interface Library - G_MACH.CPP
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


#include "ui_gen.hpp"

#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))

#if defined(ZIL_HARDWARE) && defined(ZIL_MSDOS)
// From our friends at:
//	XLsoft International                   +1 714 453 2781 Voice
//	15375 Barranca Pky Ste A-204           +1 714 453 8811 FAX
//	Irvine, CA  92718-2204                 76350,1457 Compuserve
//	MHS      MAIL@CSERVE {76350,1457}
//	Internet 76350.1457@compuserve.com
//	X.400    (c=US;a=CompuServe;p=CSMAIL;d=ID:76350.1457)
#	include <ctype.h>
#	include <dos.h>
#	if defined(DOSX286)
#		include <phapi.h>
#	elif defined(DOS16M)
#		include <dos16.h>
#	endif

void ZIL_INTERNATIONAL::MachineName(void)
// Currently we only support ATs and NECs.  This sets the name of
// the machine.
{
	static int isAT;
	static int bAlreadyChecked = FALSE;

	if (!bAlreadyChecked)
	{
#	if defined(DOSX286)
		unsigned short sel;
		DosMapRealSeg(0xFFFF, 16, &sel);
		unsigned char *cpBIOSDate = (unsigned char *)MK_FP(sel, 0x5);
#	elif defined(DOS16PP) || defined(DOS32PP)
		unsigned char *cpBIOSDate = (unsigned char *)MK_FP(__SegF000, 0xFFF5);
#	elif defined(DOS16M)
		LPVOID pmptr;
		pmptr = d16MapRealAddress(MK_FP(0xFFFF, 0x0005));
		d16SetSegLimit(FP_SEG(pmptr),16);
		unsigned char *cpBIOSDate = (unsigned char *)pmptr;
#	elif defined(DOS386)
		???? this is not written for 386 mode yet;
#	else
		unsigned char *cpBIOSDate = (unsigned char *)0xF000FFF5L;
#	endif
		bAlreadyChecked = TRUE;
		isAT = (isdigit(*(cpBIOSDate + 0))
			&& isdigit(*(cpBIOSDate + 1))
			&& *(cpBIOSDate + 2) == '/'
			&& isdigit(*(cpBIOSDate + 3))
			&& isdigit(*(cpBIOSDate + 4))
			&& *(cpBIOSDate + 5) == '/'
			&& isdigit(*(cpBIOSDate + 6))
			&& isdigit(*(cpBIOSDate + 7)));
#	if defined(DOSX286)
		DosFreeSeg(sel);
# 	elif defined(DOS16M)
		d16FreeDesc(FP_SEG(pmptr));
#	endif
	}
	extern void ATI_SetFunctions(void);
	extern void NECI_SetFunctions(void);
	static void (*machines[])(void) =
	{
		{ ATI_SetFunctions },	// This is the default
		{ NECI_SetFunctions },
	};
	int i = (isAT ? 0 : 1);
	machines[i]();
	strcpy(machineName, _textDriverName);
}
#else

void ZIL_INTERNATIONAL::MachineName(void)
{
#if defined(ZIL_MSDOS)
	strcpy(machineName, _textDriverName);
#else
	static ZIL_ICHAR name[] =
	{
#	if defined(ZIL_MSWINDOWS)
		'M','S','W','i','n','d','o','w','s',0
#	elif defined(ZIL_OS2)
		'O','S','/','2',0
#	elif defined(ZIL_MOTIF)
		'M','o','t','i','f',0
#	elif defined(ZIL_CURSES)
		'C','u','r','s','e','s',0
#	elif defined(ZIL_MACINTOSH)
		'M','a','c','i','n','t','o','s','h',0
#	elif defined(ZIL_NEXTSTEP)
		'N','E','X','T','S','T','E','P',0
#	else
		'U','n','k','n','o','w','n',0
#	endif
	};
	strcpy(machineName, name);
#endif
}
#endif
