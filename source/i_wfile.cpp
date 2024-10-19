//	Zinc Interface Library - I_WFILE.CPP
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


#include <stdio.h>
#include <stdlib.h>
#if defined(_MSC_VER)
#	include<sys/types.h>
#endif
#include "ui_env.hpp"
#if defined(ZIL_MACINTOSH)
#	include <Aliases.h>
#	include <errno.h>
#	if defined(__MWERKS__)
#		include <unix.h>	// For O_CREAT.
#	else
#		include <fcntl.h>	// For O_CREAT.
#	endif
#else
#	include <sys/stat.h>
#	if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#		include <dir.h>
#	elif defined(__ZTC__) | defined(_MSC_VER) | defined(__WATCOMC__) || defined(__HIGHC__) || defined(__IBMCPP__) || defined(__GLOCKENSPIEL__)
#		include <direct.h>
#	endif
#endif
#if defined(ZIL_POSIX)
#	include <fcntl.h>
#	include <unistd.h>
#elif !defined(ZIL_MACINTOSH)
#	include <io.h>
#endif
#include "ui_gen.hpp"

#if defined(ZIL_POSIX)
/*
 * The following was provided by Ken Thompson of AT&T Bell Laboratories,
 * <ken@research.att.com>, on Tue, 8 Sep 92 03:22:07 EDT, to the X/Open
 * Joint Internationalization Group.  Some minor formatting changes have
 * been made by Glenn Adams, <glenn@metis.com>.
 *
 * -------------------------------------------------------------------------
 *  File System Safe Universal Character Set Transformation Format (FSS-UTF)
 * -------------------------------------------------------------------------
 *
 * With the approval of ISO/IEC 10646 (Unicode) as an international
 * standard and the anticipated wide spread use of this universal coded
 * character set (UCS), it is necessary for historically ASCII based
 * operating systems to devise ways to cope with representation and
 * handling of the large number of characters that are possible to be
 * encoded by this new standard.
 *
 * There are several challenges presented by UCS which must be dealt with
 * by historical operating systems and the C-language programming
 * environment.  The most significant of these challenges is the encoding
 * scheme used by UCS. More precisely, the challenge is the marrying of
 * the UCS standard with existing programming languages and existing
 * operating systems and utilities.
 *
 * The challenges of the programming languages and the UCS standard are
 * being dealt with by other activities in the industry.  However, we are
 * still faced with the handling of UCS by historical operating systems
 * and utilities.  Prominent among the operating system UCS handling
 * concerns is the representation of the data within the file system.  An
 * underlying assumption is that there is an absolute requirement to
 * maintain the existing operating system software investment while at
 * the same time taking advantage of the use the large number of
 * characters provided by the UCS.
 *
 * UCS provides the capability to encode multi-lingual text within a
 * single coded character set.  However, UCS and its UTF variant do not
 * protect null bytes and/or the ASCII slash ("/") making these character
 * encodings incompatible with existing Unix implementations.  The
 * following proposal provides a Unix compatible transformation format of
 * UCS such that Unix systems can support multi-lingual text in a single
 * encoding.  This transformation format encoding is intended to be used
 * as a file code.  This transformation format encoding of UCS is
 * intended as an intermediate step towards full UCS support.  However,
 * since nearly all Unix implementations face the same obstacles in
 * supporting UCS, this proposal is intended to provide a common and
 * compatible encoding during this transition stage.
 *
 * Goal/Objective
 * --------------
 *
 * With the assumption that most, if not all, of the issues surrounding
 * the handling and storing of UCS in historical operating system file
 * systems are understood, the objective is to define a UCS
 * transformation format which also meets the requirement of being usable
 * on a historical operating system file system in a non-disruptive
 * manner.  The intent is that UCS will be the process code for the
 * transformation format, which is usable as a file code.
 *
 * Criteria for the Transformation Format
 * --------------------------------------
 *
 * Below are the guidelines that were used in defining the UCS
 * transformation format:
 *
 *	1) Compatibility with historical file systems:
 *
 *	Historical file systems disallow the null byte and the ASCII
 *	slash character as a part of the file name.
 *
 *	2) Compatibility with existing programs:
 *
 *	The existing model for multibyte processing is that ASCII does
 *	not occur anywhere in a multibyte encoding.  There should be
 *	no ASCII code values for any part of a transformation format
 *	representation of a character that was not in the ASCII
 *	character set in the UCS representation of the character.
 *
 *	3) Ease of conversion from/to UCS.
 *
 *	4) The first byte should indicate the number of bytes to
 *	follow in a multibyte sequence.
 *
 *	5) The transformation format should not be extravagant in
 *	terms of number of bytes used for encoding.
 *
 *	6) It should be possible to find the start of a character
 *	efficiently starting from an arbitrary location in a byte
 *	stream.
 *
 * Proposed FSS-UTF
 * ----------------
 *
 * The proposed UCS transformation format encodes UCS values in the range
 * [0,0x7fffffff] using multibyte characters of lengths 1, 2, 3, 4, 5,
 * and 6 bytes.  For all encodings of more than one byte, the initial
 * byte determines the number of bytes used and the high-order bit in
 * each byte is set.  Every byte that does not start 10xxxxxx is the
 * start of a UCS character sequence.
 *
 * An easy way to remember this transformation format is to note that the
 * number of high-order 1's in the first byte signifies the number of
 * bytes in the multibyte character:
 *
 * Bits  Hex MinValue  Hex MaxValue  Byte Sequence in Binary
 *   7  00000000 0000007f 0vvvvvvv
 *  11  00000080 000007FF 110vvvvv 10vvvvvv
 *  16  00000800 0000FFFF 1110vvvv 10vvvvvv 10vvvvvv
 *  21  00010000 001FFFFF 11110vvv 10vvvvvv 10vvvvvv 10vvvvvv
 *  26  00200000 03FFFFFF 111110vv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
 *  31  04000000 7FFFFFFF 1111110v 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv 10vvvvvv
 *
 * The UCS value is just the concatenation of the v bits in the multibyte
 * encoding.  When there are multiple ways to encode a value, for example
 * UCS 0, only the shortest encoding is legal.
 *
 * Below are sample implementations of the C standard wctomb() and
 * mbtowc() functions which demonstrate the algorithms for converting
 * from UCS to the transformation format and converting from the
 * transformation format to UCS. The sample implementations include error
 * checks, some of which may not be necessary for conformance:
 *
 */

typedef struct
{
	int     cmask;
	int     cval;
	int     shift;
	long    lmask;
	long    lval;
} Tab;

static Tab tab[] =
{
	0x80,  0x00,   0*6,    0x7F,           0,         /* 1 byte sequence */
	0xE0,  0xC0,   1*6,    0x7FF,          0x80,      /* 2 byte sequence */
	0xF0,  0xE0,   2*6,    0xFFFF,         0x800,     /* 3 byte sequence */
	0xF8,  0xF0,   3*6,    0x1FFFFF,       0x10000,   /* 4 byte sequence */
	0xFC,  0xF8,   4*6,    0x3FFFFFF,      0x200000,  /* 5 byte sequence */
	0xFE,  0xFC,   5*6,    0x7FFFFFFF,     0x4000000, /* 6 byte sequence */
	0,                                                /* end of table    */
};

static int _mbtowc ( ZIL_ICHAR *p, char *s, size_t n )
{
	long l;
	int c0, c, nc;
	Tab *t;

	if ( s == 0 )
		return 0;
	nc = 0;
	if ( n <= nc )
		return -1;
	c0 = *s & 0xff;
	l = c0;
	for ( t = tab; t->cmask; t++ )
	{
		nc++;
		if ( ( c0 & t->cmask ) == t->cval )
		{
			l &= t->lmask;
			if ( l < t->lval )
				return -1;
			*p = l;
			return nc;
		}
		if ( n <= nc )
			return -1;
		s++;
		c = ( *s ^ 0x80 ) & 0xFF;
		if ( c & 0xC0 )
			return -1;
		l = ( l << 6 ) | c;
	}
	return -1;
}

static int _wctomb ( char *s, wchar_t wc )
{
	long l;
	int c, nc;
	Tab *t;

	if (s == 0 )
		return 0;
	l = wc;
	nc = 0;
	for ( t=tab; t->cmask; t++ )
	{
		nc++;
		if ( l <= t->lmask )
		{
			c = t->shift;
			*s = t->cval | ( l >> c );
			while ( c > 0 )
			{
				c -= 6;
				s++;
				*s = 0x80 | ( ( l >> c ) & 0x3F );
			}
			return nc;
		}
	}
	return -1;
}
#endif

ZIL_FILE_CHAR *ZIL_INTERNATIONAL::ConvertToFilename(const ZIL_ICHAR *src)
{
#if defined(ZIL_POSIX)
	ZIL_FILE_CHAR *dst = new ZIL_FILE_CHAR[sizeof(*src) * strlen(src) / sizeof(ZIL_FILE_CHAR)];
	int i = 0, j;
	while (*src)
	{
		j = _wctomb(&dst[i], *src);
		if (j >= 0)
			i += j;
		src++;
	}
	dst[i] = '\0';
	return (dst);
//#elif defined(ZIL_WINNT)
//	strcpy(dst, src);
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MACINTOSH)
	return MapText(src);
#else
	????;
#endif
}

void ZIL_INTERNATIONAL::ConvertFromFilename(ZIL_ICHAR *dst, const ZIL_FILE_CHAR *src)
{
#if defined(ZIL_POSIX)
	int i = 0, j;
	while (*src)
	{
		j = _mbtowc(dst, (ZIL_FILE_CHAR *)src, 6);
		if (j >= 0)
		{
			src += j;
			dst++;
		}
	}
	*dst = '\0';
//#elif defined(ZIL_WINNT)
//	strcpy(dst, src);
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(ZIL_MACINTOSH)
	UnMapText(src, dst);
#else
	????;
#endif
}

#if defined(ZIL_MACINTOSH)
int ZIL_INTERNATIONAL::chdir(const ZIL_ICHAR *)
{
	return (-1);
}
#else
int ZIL_INTERNATIONAL::chdir(const ZIL_ICHAR *pname)
{
	ZIL_FILE_CHAR *fpName = ConvertToFilename(pname);
	int i = ::chdir(fpName);
	delete fpName;
	return (i);
}
#endif

#if defined(ZIL_MACINTOSH)
ZIL_ICHAR *ZIL_INTERNATIONAL::getcwd(ZIL_ICHAR *, unsigned)
{
	return (ZIL_NULLP(ZIL_ICHAR));
}
#else
ZIL_ICHAR *ZIL_INTERNATIONAL::getcwd(ZIL_ICHAR *buf, unsigned size)
{
	ZIL_FILE_CHAR *fBuf = new ZIL_FILE_CHAR[size];
	::getcwd(fBuf, size);
	if (!buf)
		buf = new ZIL_ICHAR[size];
	ConvertFromFilename(buf, fBuf);
	delete fBuf;
	return (buf);
}
#endif

#if defined(ZIL_MACINTOSH)
ZIL_ICHAR *ZIL_INTERNATIONAL::getenv(const ZIL_ICHAR *)
{
	return (ZIL_NULLP(ZIL_ICHAR));
}
#else
ZIL_ICHAR *ZIL_INTERNATIONAL::getenv(const ZIL_ICHAR *name)
{
#	if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
	static ZIL_ICHAR ZIL_FARDATA env[128];
#	else
	static ZIL_ICHAR env[512];
#	endif

	ZIL_FILE_CHAR *fName = ConvertToFilename(name);
	ZIL_FILE_CHAR *fEnv = ::getenv(fName);
	delete fName;
	if (fEnv == ZIL_NULLP(ZIL_FILE_CHAR))
		return (ZIL_NULLP(ZIL_ICHAR));
	ConvertFromFilename(env, fEnv);
	return (env);
}
#endif

#if defined(ZIL_MACINTOSH)
int ZIL_INTERNATIONAL::open(const ZIL_ICHAR *name, int oflag, unsigned)
{
	ZIL_FILE_CHAR *convertedName = ConvertToFilename(name);
	Str255 fName;
	::strcpy((char *)fName, convertedName);
	delete convertedName;
	c2pstr((char *)fName);
	if (oflag & O_CREAT)
	{
		errno = Create(fName, 0, fileCreator, fileType);
		if (errno != noErr && errno != dupFNErr)
			return (-1);
	}
	else
	{
		// We must resolve, if this is an alias.
		Boolean isFolder, isAlias;
		FSSpec theSpec;
		if (FSMakeFSSpec(0, 0L, fName, &theSpec) == noErr &&
			ResolveAliasFile(&theSpec, TRUE, &isFolder, &isAlias) == noErr &&
			isAlias)
		{
			GetPathNameFromDirID(fName, theSpec.parID, theSpec.vRefNum);
			pstrcat(fName, theSpec.name);
		}
	}
	SignedByte permission;
	short refNum;
#	if defined(__ZTC__)
	// Because Symantec says O_RDONLY is 0.
	if (!FlagSet(oflag, (O_WRONLY | O_RDWR)))
#	else
	if (FlagSet(oflag, O_RDONLY))
#	endif
		permission = fsRdPerm;
	else
		permission = fsRdWrPerm;

	errno = HOpenDF(0, 0L, fName, permission, &refNum);
	if (errno == noErr)
		return (refNum);
	else
	{
		if (oflag & O_CREAT)
			FSDelete(fName, 0);
		return (-1);
	}
}
#else
int ZIL_INTERNATIONAL::open(const ZIL_ICHAR *name, int oflag, unsigned mode)
{
	ZIL_FILE_CHAR *fName = ConvertToFilename(name);
	int i = ::open(fName, oflag, mode);
	delete fName;
	return (i);
}
#endif

int ZIL_INTERNATIONAL::rename(const ZIL_ICHAR *oldName, const ZIL_ICHAR *newName)
{
	ZIL_FILE_CHAR *fOld = ConvertToFilename(oldName);
	ZIL_FILE_CHAR *fNew = ConvertToFilename(newName);
#if defined(ZIL_MACINTOSH)
	Str255 fName, tName;
	::strcpy((char *)fName, fOld);
	::strcpy((char *)tName, fNew);
	delete fNew;
	delete fOld;
	c2pstr((char *)fName);
	c2pstr((char *)tName);
	errno = Rename(fName, 0, tName);
	if (errno == noErr)
		return (0);
	else
		return (-1);
#else
	int i = ::rename(fOld, fNew);
	delete fNew;
	delete fOld;
	return (i);
#endif
}

#if defined(ZIL_MACINTOSH)
int ZIL_INTERNATIONAL::stat(const ZIL_ICHAR *path, void *)
{
	ZIL_FILE_CHAR *fPath = ConvertToFilename(path);
	FILE *fp = fopen(fPath, "r");
	delete fPath;
	if (fp == ZIL_NULLP(FILE))
		return (-1);

	fclose(fp);
	return (0);
}
#else
int ZIL_INTERNATIONAL::stat(const ZIL_ICHAR *path, void *buff)
{
	ZIL_FILE_CHAR *fPath = ConvertToFilename(path);
#	if defined(__ZTC__)
	int i = ::stat(fPath, buff);
#	else
	int i = ::stat(fPath, (struct stat *)buff);
#	endif
	delete fPath;
	return (i);
}
#endif

ZIL_ICHAR *ZIL_INTERNATIONAL::tmpnam(ZIL_ICHAR *s)
{
	static ZIL_ICHAR ZIL_FARDATA buff[ZIL_MAXPATHLEN];
	ZIL_FILE_CHAR *fS = ::tmpnam(ZIL_NULLP(ZIL_FILE_CHAR));
	if (!s)
		s = buff;
	ConvertFromFilename(s, fS);
	return (s);
}

int ZIL_INTERNATIONAL::unlink(const ZIL_ICHAR *name)
{
	ZIL_FILE_CHAR *fName = ConvertToFilename(name);
#if defined(ZIL_MACINTOSH)
	Str255 pName;
	::strcpy((char *)pName, fName);
	c2pstr((char *)pName);
	errno = FSDelete(pName, 0);
	int i = (errno == noErr) ? 0 : -1;
#elif defined(__SC__)
	int i = ::remove(fName);
#else
	int i = ::unlink(fName);
#endif
	delete fName;
	return (i);
}
