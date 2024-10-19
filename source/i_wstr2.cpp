//	Zinc Interface Library - I_WSTR2.CPP
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

int ZIL_INTERNATIONAL::strxfrm(ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n)
{
#if defined (ZIL_COLLATE)
	// This algorithm is taken from CSA Z243.4.1-1992
	// Canadian Alphanumeric Ordering Standard for Character Sets of
	// CSA Standard CAN/CSA-Z243.4 (ISO 8859-1).
	static struct
	{
		ZIL_ICHAR ch;
		ZIL_ICHAR val;
	} ZIL_FARDATA table2[] = {
		{ 0x20, 32 },
		{ 0x21, 40 },
		{ 0x22, 56 },
		{ 0x23, 82 },
		{ 0x24, 76 },
		{ 0x25, 83 },
		{ 0x26, 87 },
		{ 0x27, 53 },
		{ 0x28, 61 },
		{ 0x29, 63 },
		{ 0x2A, 79 },
		{ 0x2B, 85 },
		{ 0x2C, 37 },
		{ 0x2D, 36 },
		{ 0x2E, 45 },
		{ 0x2F, 44 },
		{ 0x3A, 39 },
		{ 0x3B, 38 },
		{ 0x3C, 91 },
		{ 0x3D, 93 },
		{ 0x3E, 95 },
		{ 0x3F, 42 },
		{ 0x40, 73 },
		{ 0x5B, 65 },
		{ 0x5C, 80 },
		{ 0x5D, 66 },
		{ 0x5E, 48 },
		{ 0x5F, 33 },
		{ 0x60, 47 },
		{ 0x7B, 67 },
		{ 0x7C, 97 },
		{ 0x7D, 68 },
		{ 0x7E, 50 },
		{ 0xA1, 41 },
		{ 0xA2, 75 },
		{ 0xA3, 77 },
		{ 0xA4, 74 },
		{ 0xA5, 78 },
		{ 0xA6, 98 },
		{ 0xA7, 69 },
		{ 0xA8, 49 },
		{ 0xA9, 71 },
		{ 0xAB, 59 },
		{ 0xAC, 96 },
		{ 0xAD, 35 },
		{ 0xAE, 72 },
		{ 0xAF, 34 },
		{ 0xB0, 99 },
		{ 0xB1, 87 },
		{ 0xB4, 46 },
		{ 0xB5, 100 },
		{ 0xB6, 70 },
		{ 0xB7, 51 },
		{ 0xB8, 52 },
		{ 0xBB, 60 },
		{ 0xBF, 43 },
		{ 0xD7, 89 },
		{ 0xF7, 88 },
	};
	static struct
	{
		ZIL_ICHAR ch;
		ZIL_ICHAR col2;
		ZIL_ICHAR col3;
		ZIL_ICHAR col4;
	} ZIL_FARDATA table1[] =
	{
		{ 0x30,  48, 15,  7 },
		{ 0x31,  49, 15,  7 },
		{ 0x32,  50, 15,  7 },
		{ 0x33,  51, 15,  7 },
		{ 0x34,  52, 15,  7 },
		{ 0x35,  53, 15,  7 },
		{ 0x36,  54, 15,  7 },
		{ 0x37,  55, 15,  7 },
		{ 0x38,  56, 15,  7 },
		{ 0x39,  57, 15,  7 },
		{ 0x41,  97, 15, 10 },
		{ 0x42,  98, 15, 10 },
		{ 0x43,  99, 15, 10 },
		{ 0x44, 100, 15, 10 },
		{ 0x45, 101, 15, 10 },
		{ 0x46, 102, 15, 10 },
		{ 0x47, 103, 15, 10 },
		{ 0x48, 104, 15, 10 },
		{ 0x49, 105, 15, 10 },
		{ 0x4A, 106, 15, 10 },
		{ 0x4B, 107, 15, 10 },
		{ 0x4C, 108, 15, 10 },
		{ 0x4D, 109, 15, 10 },
		{ 0x4E, 110, 15, 10 },
		{ 0x4F, 111, 15, 10 },
		{ 0x50, 112, 15, 10 },
		{ 0x51, 113, 15, 10 },
		{ 0x52, 114, 15, 10 },
		{ 0x53, 115, 15, 10 },
		{ 0x54, 116, 15, 10 },
		{ 0x55, 117, 15, 10 },
		{ 0x56, 118, 15, 10 },
		{ 0x57, 119, 15, 10 },
		{ 0x58, 120, 15, 10 },
		{ 0x59, 121, 15, 10 },
		{ 0x5A, 122, 15, 10 },
		{ 0x61,  97, 15,  7 },
		{ 0x62,  98, 15,  7 },
		{ 0x63,  99, 15,  7 },
		{ 0x64, 100, 15,  7 },
		{ 0x65, 101, 15,  7 },
		{ 0x66, 102, 15,  7 },
		{ 0x67, 103, 15,  7 },
		{ 0x68, 104, 15,  7 },
		{ 0x69, 105, 15,  7 },
		{ 0x6A, 106, 15,  7 },
		{ 0x6B, 107, 15,  7 },
		{ 0x6C, 108, 15,  7 },
		{ 0x6D, 109, 15,  7 },
		{ 0x6E, 110, 15,  7 },
		{ 0x6F, 111, 15,  7 },
		{ 0x70, 112, 15,  7 },
		{ 0x71, 113, 15,  7 },
		{ 0x72, 114, 15,  7 },
		{ 0x73, 115, 15,  7 },
		{ 0x74, 116, 15,  7 },
		{ 0x75, 117, 15,  7 },
		{ 0x76, 118, 15,  7 },
		{ 0x77, 119, 15,  7 },
		{ 0x78, 120, 15,  7 },
		{ 0x79, 121, 15,  7 },
		{ 0x7A, 122, 15,  7 },
		{ 0xA0,  32, 15,  7 },
		{ 0xAA,  97, 16,  9 },
		{ 0xB2,  50, 15,  9 },
		{ 0xB3,  51, 15,  9 },
		{ 0xB9,  49, 15,  9 },
		{ 0xBA, 111, 16,  9 },
		{ 0xBC,  48, 20,  7 },
		{ 0xBD,  48, 22,  7 },
		{ 0xBE,  48, 24,  7 },
		{ 0xC0,  97, 19, 10 },
		{ 0xC1,  97, 18, 10 },
		{ 0xC2,  97, 20, 10 },
		{ 0xC3,  97, 23, 10 },
		{ 0xC4,  97, 22, 10 },
		{ 0xC5,  97, 21, 10 },
		{ 0xC6,  97, 17, 10 },
		{ 0xC7,  99, 25, 10 },
		{ 0xC8, 101, 19, 10 },
		{ 0xC9, 101, 18, 10 },
		{ 0xCA, 101, 20, 10 },
		{ 0xCB, 101, 22, 10 },
		{ 0xCC, 105, 19, 10 },
		{ 0xCD, 105, 18, 10 },
		{ 0xCE, 105, 20, 10 },
		{ 0xCF, 105, 22, 10 },
		{ 0xD0, 100, 16, 10 },
		{ 0xD1, 110, 23, 10 },
		{ 0xD2, 111, 19, 10 },
		{ 0xD3, 111, 18, 10 },
		{ 0xD4, 111, 20, 10 },
		{ 0xD5, 111, 23, 10 },
		{ 0xD6, 111, 22, 10 },
		{ 0xD8, 111, 24, 10 },
		{ 0xD9, 117, 19, 10 },
		{ 0xDA, 117, 18, 10 },
		{ 0xDB, 117, 20, 10 },
		{ 0xDC, 117, 22, 10 },
		{ 0xDD, 121, 18, 10 },
		{ 0xDE, 116, 16, 10 },
		{ 0xDF, 115, 17,  7 },
		{ 0xE0,  97, 19,  7 },
		{ 0xE1,  97, 18,  7 },
		{ 0xE2,  97, 20,  7 },
		{ 0xE3,  97, 23,  7 },
		{ 0xE4,  97, 22,  7 },
		{ 0xE5,  97, 21,  7 },
		{ 0xE6,  97, 17,  7 },
		{ 0xE7,  99, 25,  7 },
		{ 0xE8, 101, 19,  7 },
		{ 0xE9, 101, 18,  7 },
		{ 0xEA, 101, 20,  7 },
		{ 0xEB, 101, 22,  7 },
		{ 0xEC, 105, 19,  7 },
		{ 0xED, 105, 18,  7 },
		{ 0xEE, 105, 20,  7 },
		{ 0xEF, 105, 22,  7 },
		{ 0xF0, 100, 16,  7 },
		{ 0xF1, 110, 23,  7 },
		{ 0xF2, 111, 19,  7 },
		{ 0xF3, 111, 18,  7 },
		{ 0xF4, 111, 20,  7 },
		{ 0xF5, 111, 23,  7 },
		{ 0xF6, 111, 22,  7 },
		{ 0xF8, 111, 24,  7 },
		{ 0xF9, 117, 19,  7 },
		{ 0xFA, 117, 18,  7 },
		{ 0xFB, 117, 20,  7 },
		{ 0xFC, 117, 22,  7 },
		{ 0xFD, 121, 18,  7 },
		{ 0xFE, 116, 20,  7 },
		{ 0xFF, 121, 22,  7 },
	},
	ZIL_FARDATA table1b[] =
	{
		{ 0xC6, 101, 17, 10 },
		{ 0xDE, 104, 16, 10 },
		{ 0xDF, 115, 17,  7 },
		{ 0xE6, 101, 17,  7 },
		{ 0xFE, 104, 16,  7 },
	};

	int i = strlen(s2) + 1;
	ZIL_ICHAR *cpo = new ZIL_ICHAR[2*i];
	ZIL_ICHAR *cso = new ZIL_ICHAR[2*i];
	ZIL_ICHAR *cto = new ZIL_ICHAR[2*i];
	ZIL_ICHAR *cqo = new ZIL_ICHAR[2*i];
	int cpopos = 0, csopos = 2*i, ctopos = 0, cqopos = 0;

	cso[--csopos] = 0;
	for (i=0; s2[i]; i++)
	{
		for (int j=0; j < LENGTHOF(table2); j++)
			if (table2[j].ch == s2[i])
			{
				cqo[cqopos++] = i + 1;
				cqo[cqopos++] = table2[j].val;
				break;
			}
		if (j == LENGTHOF(table2))
		{
			for (j=0; j < LENGTHOF(table1); j++)
				if (table1[j].ch == s2[i])
				{
					cpo[cpopos++] = table1[j].col2;
					cso[--csopos] = table1[j].col3;
					cto[ctopos++] = table1[j].col4;
					break;
				}
			for (j=0; j < LENGTHOF(table1b); j++)
				if (table1b[j].ch == s2[i])
				{
					cpo[cpopos++] = table1b[j].col2;
					cso[--csopos] = table1b[j].col3;
					cto[ctopos++] = table1b[j].col4;
					break;
				}
		}
	}
	cpo[cpopos] = cto[ctopos] = cqo[cqopos] = 0;
	int len = strlen(cpo)+strlen(&cso[csopos])+strlen(cto)+strlen(cqo);
	if (len < n)
	{
		strcpy(s1, cpo);
		strcat(s1, &cso[csopos]);
		strcat(s1, cto);
		strcat(s1, cqo);
	}
	delete cpo;
	delete cso;
	delete cto;
	delete cqo;
	return (len);
#else
	char *tmp2 = MapText(s2, ZIL_NULLP(char), FALSE);
	char *tmp1 = new char[n];
	int i = ::strxfrm(tmp1, tmp2, n);
	UnMapText(tmp1, s1, n);
	delete tmp1;
	return (i);
#endif
}

#if !defined(__ZTC__) || defined(macintosh)
int ZIL_INTERNATIONAL::strcoll(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
{
#if defined(ZIL_COLLATE)
	int max1 = 2*3*strlen(s1);
	int max2 = 2*3*strlen(s2);
	ZIL_ICHAR *cmp1 = new ZIL_ICHAR[max1];
	ZIL_ICHAR *cmp2 = new ZIL_ICHAR[max2];

	strxfrm(cmp1, s1, max1);
	strxfrm(cmp2, s2, max2);
	int i = streq(cmp1, cmp2);
	delete cmp1;
	delete cmp2;
	return (i);
#else
	char *cmp1 = MapText(s1, ZIL_NULLP(char), FALSE);
	char *cmp2 = MapText(s2);
#	if defined(ZIL_MSDOS) && defined(_MSC_VER) && defined(DOS386)
		int i = ::strcmp(cmp1, cmp2);   // Microsoft & PharlapTNT strcoll bug.
#	elif defined(ZIL_MACINTOSH) && defined(__MWERKS__)
		int i = ::strcmp(cmp1, cmp2);   // Metrowerks hanging strcoll bug.
#	else
		int i = ::strcoll(cmp1, cmp2);
#	endif
	delete cmp2;
	return (i);
#endif
}
#endif

ZIL_ICHAR *ZIL_INTERNATIONAL::strncat(ZIL_ICHAR *s1, const ZIL_ICHAR *s2, int n)
{
	int i = 0;
	for (; s1[i]; i++)
		;
	for (int j=0; n && s2[j]; i++, j++, n--)
		s1[i] = s2[j];
	s1[i] = 0;
	return (s1);
}

int ZIL_INTERNATIONAL::strcspn(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
{
	const ZIL_ICHAR *val;
	int pos = strlen(s1);

	for (int i = 0; s2[i]; i++)
	{
		if ((val = strchr(s1, s2[i])) != ZIL_NULLP(ZIL_ICHAR))
		{
			if (val - s1 < pos)
				pos = (int)(val - s1);
		}
	}
	return (pos);
}

ZIL_ICHAR *ZIL_INTERNATIONAL::strpbrk(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
{
	ZIL_ICHAR *val;
	ZIL_ICHAR *pos = ZIL_NULLP(ZIL_ICHAR);

	for (int i = 0; s2[i]; i++)
	{
		if ((val = strchr(s1, s2[i])) != ZIL_NULLP(ZIL_ICHAR))
		{
			if (pos == ZIL_NULLP(ZIL_ICHAR) || val < pos)
				pos = val;
		}
	}
	return (pos);
}

int ZIL_INTERNATIONAL::strspn(const ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
{
	const ZIL_ICHAR *val;
	int pos = strlen(s1);

	for (int i = 0; s2[i]; i++)
	{
		if ((val = strrchr(s1, s2[i])) != ZIL_NULLP(ZIL_ICHAR))
		{
			if (val - s1 < pos)
				pos = (int)(val - s1);
		}
	}
	return (pos);
}

ZIL_ICHAR *ZIL_INTERNATIONAL::strtok(ZIL_ICHAR *s1, const ZIL_ICHAR *s2)
{
	static ZIL_ICHAR *_s1;
	int pos;

	if (s1 != ZIL_NULLP(ZIL_ICHAR))
		_s1 = s1;
	s1 = _s1;
	if (! *_s1)
		return ZIL_NULLP(ZIL_ICHAR);
	pos = strcspn(_s1, s2);
	if (pos == 0)
	{
		s1 = _s1;
		_s1 += strlen(_s1);
	}
	else
	{
		_s1 += pos;
		*_s1++ = 0;
	}
	return (s1);
}

