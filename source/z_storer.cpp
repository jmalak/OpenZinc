//	Zinc Interface Library - Z_STORER.CPP
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


#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__MWERKS__)
#	include <unix.h>
#else
#   include <fcntl.h>
#endif
#include <errno.h>
#if !defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
}
#endif
#if defined(macintosh) || defined(__MWERKS__)
#	define S_IREAD  0
#	define S_IWRITE 0
#else
extern "C"
{
#	include <sys/types.h>
#	include <sys/stat.h>
}
#endif
#if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#	include <mem.h>
#	include <dir.h>
#	include <direct.h>
#elif (defined(__ZTC__) && !defined(macintosh)) || defined(_MSC_VER) || (defined(__WATCOMC__) && !defined(__QNX__)) || defined(__HIGHC__) || defined(__IBMCPP__) || defined(__GLOCKENSPIEL__)
#	include <direct.h>
#endif
#if defined(__GNUC__)
#	define SEEK_SET	0
#	define SEEK_CUR	1
#	define SEEK_END	2
#endif
#if defined(__ZTC__) && !defined(macintosh)
#	define _dos_getdrive dos_getdrive
#	define _dos_setdrive dos_setdrive
#endif
#if defined(_MSC_VER) && defined(DOS386)
#	include <pldos32.h>
#endif
#include "ui_gen.hpp"
#if defined(ZIL_POSIX)
#ifdef __WATCOMC__
#	if !defined(__QNX__)
#		include <sys/param.h>
#	endif
#	include <unistd.h>
#else
extern "C"
{
#	if defined(ZIL_NEXTSTEP)
#		include <libc.h>
#	endif
#	if !defined(__QNX__)
#		include <sys/param.h>
#		define O_BINARY	0
#	endif
#	include <unistd.h>
}
#endif
#endif
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
#	include <io.h>
#	if !defined(__IBMCPP__) && !defined(__GLOCKENSPIEL__)
#		include <dos.h>
#	endif
#endif
#include "z_store.hpp"

struct ZINC_SIGNATURE _signature =
#if defined(ZIL_UNICODE)
	{ "Zinc Data File Version 4.5\032", MAJOR_VERSION, MINOR_VERSION, MAGIC_NUMBER };
#else
	{ "Zinc Data File Version 4.0\032", MAJOR_VERSION, MINOR_VERSION, MAGIC_NUMBER };
#endif

// --- support ----------------------------------------------------------------

#if defined(TESTING)
	// Hard abort for testing errors in Storage.  This isn't used
	// in the production version of ZAF.
	static void FileAbort(int i)
	{
		printf("read error abort: %d\n", i);
		perror("");
		abort();
	}
#else
#	define FileAbort(x)
#endif

#if defined(TESTING_ALL)
	// Ditto above.
	static void FileAbortAll(int i)
	{
		printf("read error abort: %d\n", i);
		perror("");
		abort();
	}
#else
#	define FileAbortAll(x)
#endif

#if defined(ZIL_BIGENDIAN)
// Data is always stored as if on a little endian machine (ala Intel x86
// family).  This was done since the slowest machines we support are based
// on the 4.77 mhz 8088.  Big endian machines swap all their data around.
// These routines are macroed out for little endian machines.
void SwapDiskAddr(ZIL_DISK_ADDRESS *datum)
{
	union
	{
		ZIL_DISK_ADDRESS a;
		ZIL_UINT8 b[sizeof(ZIL_DISK_ADDRESS)];
	} tmp;
	ZIL_UINT8 tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

void SwapUZIL_INT16(ZIL_UINT16 *datum)
{
	union
	{
		ZIL_UINT16 a;
		ZIL_UINT8 b[sizeof(ZIL_UINT16)];
	} tmp;
	ZIL_UINT8 tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

void SwapInt32(ZIL_INT32 *datum)
{
	union
	{
		ZIL_INT32 a;
		ZIL_UINT8 b[sizeof(ZIL_INT32)];
	} tmp;
	ZIL_UINT8 tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

void SwapUZIL_INT32(ZIL_UINT32 *datum)
{
	union
	{
		ZIL_UINT32 a;
		ZIL_UINT8 b[sizeof(ZIL_UINT32)];
	} tmp;
	ZIL_UINT8 tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

void SwapInumt(ZIL_INODE_NUMBER *datum)
{
	union
	{
		ZIL_INODE_NUMBER a;
		ZIL_UINT8 b[sizeof(ZIL_INODE_NUMBER)];
	} tmp;
	ZIL_UINT8 tmpb;

	tmp.a = *datum;
	for (int j=0; j < sizeof(*datum) / 2; j++)
	{
		tmpb = tmp.b[j];
		tmp.b[j] = tmp.b[sizeof(*datum)-1-j];
		tmp.b[sizeof(*datum)-1-j] = tmpb;
	}
	*datum = tmp.a;
}

void SwapInode(inode *di)
{
	SwapInt32(&di->size);
	SwapUZIL_INT32(&di->createTime);
	SwapUZIL_INT32(&di->modifyTime);
	for (int i=0; i < LENGTHOF(di->direct); i++)
		SwapDiskAddr(&di->direct[i]);
	SwapDiskAddr(&di->sIndirect);
	SwapDiskAddr(&di->dIndirect);
	SwapUZIL_INT16(&di->useCount);
	SwapDiskAddr(&di->fragmentBlock);
	SwapUZIL_INT16(&di->fragmentIndex);
}

void SwapSuperBlock(superBlock *ds)
{
	SwapDiskAddr(&ds->signature.magicNumber);
	SwapUZIL_INT32(&ds->createTime);
	SwapUZIL_INT32(&ds->modifyTime);
	SwapUZIL_INT16(&ds->revision);
	SwapUZIL_INT16(&ds->freeInodeListHi);
	SwapUZIL_INT16(&ds->freeInodeListLo);
	SwapDiskAddr(&ds->freeDataList);
	for (int i=0; i < LENGTHOF(ds->inodeDirect); i++)
		SwapDiskAddr(&ds->inodeDirect[i]);
	SwapDiskAddr(&ds->inodeSIndirect);
	SwapDiskAddr(&ds->inodeDIndirect);
}
#endif

// Takes a possible partial path name in tmppath and converts it to a
// fully qualified path name starting from root and including a drive.
// It doesn't strip out redundant ".."s or "."s.
void ZIL_STORAGE_READ_ONLY::MakeFullPath(ZIL_ICHAR *tmppath)
{
	// Check for a null string;
	if (!tmppath)
		return;

#if defined(ZIL_POSIX)
	if (tmppath[0] != defaultCharMap->dirSepStr[0])
	{
		ZIL_ICHAR save[ZIL_MAXPATHLEN];
		strcpy(save, tmppath);
		getcwd(tmppath, ZIL_MAXPATHLEN);
		AppendFullPath(tmppath, tmppath, save);
	}
#elif defined(ZIL_MACINTOSH)
	// A full path may be a qualified partial path, still usable by the Toolbox.
	if (!strchr(tmppath, defaultCharMap->dirSepStr[0]))
		strcat(tmppath, defaultCharMap->dirSepStr);
#else
	if (tmppath[0] == defaultCharMap->dirSepStr[0])
	{
		memmove((char *)&tmppath[2], (const char *)&tmppath[0], (strlen(tmppath)+1) * sizeof(tmppath[0]));
		tmppath[1] = ':';
#if defined(ZIL_WINNT)
		unsigned drive;
		ZIL_ICHAR currentDrive[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDrive);
		drive = ToUpper(currentDrive[0]) - 'A' + 1;
		tmppath[0] = (char)(drive - 1) + 'A';
#elif defined(ZIL_OS2)
		ULONG pDrive, lDrive;
		DosQueryCurrentDisk(&pDrive, &lDrive);
		tmppath[0] = pDrive - 1 + 'A';
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(__DVX__)
		unsigned drive;
		_dos_getdrive(&drive);
		tmppath[0] = (char)(drive - 1) + 'A';
#else
		????;   This is an error;
#endif
	}
	else if (tmppath[0] && tmppath[1] == ':' && tmppath[2] != defaultCharMap->dirSepStr[0])
	{
		ZIL_ICHAR save[ZIL_MAXPATHLEN];
		strcpy(save, &tmppath[2]);
#if defined(ZIL_WINNT)
		ZIL_ICHAR currentDrive[MAX_PATH], newDrive[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, currentDrive);
		newDrive[0] = tmppath[0];
		newDrive[1] = ':';
		newDrive[2] = 0;
		SetCurrentDirectory(newDrive);
		GetCurrentDirectory(ZIL_MAXPATHLEN, &tmppath[2]);
		SetCurrentDirectory(currentDrive);
#elif defined(ZIL_OS2)
		ULONG drive, xxx;
		DosQueryCurrentDisk(&drive, &xxx);
		DosSetDefaultDisk(tmppath[0] - 'a' + 1);
		getcwd(&tmppath[2], CCHMAXPATH);
		DosSetDefaultDisk(drive);
#elif defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(__DVX__)
		unsigned drive, xxx;
		_dos_getdrive(&drive);
		_dos_setdrive(tmppath[0] - 'A' + 1, &xxx);
		getcwd(tmppath, ZIL_MAXPATHLEN);
		_dos_setdrive(drive, &xxx);
#else
		????; This is an error;
#endif
		AppendFullPath(tmppath, tmppath, save);
	}
	else if (!(tmppath[0] && tmppath[1] == ':' && tmppath[2] == defaultCharMap->dirSepStr[0]))
	{
		ZIL_ICHAR save[ZIL_MAXPATHLEN];
		strcpy(save, tmppath);
		getcwd(tmppath, ZIL_MAXPATHLEN);
		AppendFullPath(tmppath, tmppath, save);
	}
#endif
}

// --- ZIL_STORAGE_READ_ONLY statics -------------------------------------------

// Build a pathname from parts (hiding operating system specifics).
void ZIL_STORAGE_READ_ONLY::AppendFullPath(ZIL_ICHAR *fullPath,
	const ZIL_ICHAR *pathName, const ZIL_ICHAR *fileName,
	const ZIL_ICHAR *extension)
{
	if (pathName != fullPath)
	{
#if defined(ZIL_MACINTOSH)
		if (!pathName[0] || strchr(pathName, defaultCharMap->dirSepStr[0]))
			strcpy(fullPath, pathName);
		else
		{
			// Return only a partial path beginning from the current working directory.
			strcpy(fullPath, defaultCharMap->dirSepStr);
			strcat(fullPath, pathName);
		}
#else
		strcpy(fullPath, pathName);
#endif
	}
	int i = strlen(fullPath);
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
	if (i && fullPath[i-1] != defaultCharMap->dirSepStr[0] && fullPath[i-1] != ':')
#else
	if (i && fullPath[i-1] != defaultCharMap->dirSepStr[0])
#endif
		strcat(fullPath, defaultCharMap->dirSepStr);
	if (fileName)
		strcat(fullPath, fileName);
	if (extension)
		ChangeExtension(fullPath, extension);
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
	strupr(fullPath);
#endif
}

// Replace an extension.  Assumes the extension is separated from the rest
// of the filename with a single period.
void ZIL_STORAGE_READ_ONLY::ChangeExtension(ZIL_ICHAR *pathName,
	const ZIL_ICHAR *newExtension)
{
	ZIL_ICHAR *oldExtension = strrchr(pathName, '.');
	if (oldExtension)
		*oldExtension = '\0';
	if (newExtension)
		strcat(pathName, newExtension);
}

// Implements the following TRUTH table:
// createStorage =	TRUE	FALSE
// invalid path		FALSE	FALSE
// valid path/no file	TRUE	FALSE
// valid path/file RW	TRUE	TRUE
// valid path/file RO	FALSE	TRUE
// valid path/protected	FALSE	FALSE
// without destroying the file possibly referenced by name.
int ZIL_STORAGE_READ_ONLY::ValidName(const ZIL_ICHAR *name, int createStorage)
{
#if defined(__WATCOMC__)
	int fd = open(NAMETYPE name, O_RDWR);
	if (fd >= 0)
	{
		close(fd);
		return (TRUE);		// valid path/file RW
	}
	fd = open(NAMETYPE name, O_RDONLY);
	if (fd >= 0)
	{
		close(fd);
		return (!createStorage);
	}
	if (!createStorage)
		return (FALSE);
	fd = open(NAMETYPE name, O_RDWR|O_CREAT, S_IREAD|S_IWRITE);
	if (fd < 0)
		return (errno == EEXIST);
	close(fd);
	unlink(NAMETYPE name);
	return (TRUE);
#elif defined(ZIL_MACINTOSH)
	Str255 fName;
	MapText(name, (char *)fName, FALSE);
	c2pstr((char *)fName);
	errno = Create(fName, 0, '\?\?\?\?', 'TEXT');
	if (errno != noErr)
	{
		if (errno != dupFNErr)
			return (FALSE);			// invalid path
		HParamBlockRec paramBlock;
		paramBlock.fileParam.ioNamePtr = fName;
		paramBlock.fileParam.ioVRefNum = 0;
		paramBlock.ioParam.ioPermssn = fsRdWrPerm;
		paramBlock.ioParam.ioMisc = nil;
		paramBlock.fileParam.ioDirID = 0;
		errno = PBHOpen(&paramBlock, FALSE);
		if (errno == noErr)
		{
			FSClose(paramBlock.ioParam.ioRefNum);
			return (TRUE);			// valid path/file RW
		}
		paramBlock.ioParam.ioPermssn = fsRdPerm;
		errno = PBHOpen(&paramBlock, FALSE);
		if (errno != noErr)
			return (FALSE);			// valid path/file protected
		FSClose(paramBlock.ioParam.ioRefNum);
		return (!createStorage);	// valid path/file RO
	}
	FSDelete(fName, 0);
	return (createStorage);			// valid path/no file
#else
	int fd = open(NAMETYPE name, O_RDWR|O_CREAT|O_EXCL, S_IREAD|S_IWRITE);
	if (fd < 0)
	{
		if (errno != EEXIST)
			return (FALSE);		// invalid path
		fd = open(NAMETYPE name, O_RDWR);
		if (fd >= 0)
		{
			close(fd);
			return (TRUE);		// valid path/file RW
		}
		fd = open(NAMETYPE name, O_RDONLY);
		if (fd < 0)
			return (FALSE);		// valid path/file protected
		close(fd);
		return (!createStorage);	// valid path/file RO
	}
	close(fd);
	unlink(NAMETYPE name);
	return (createStorage);			// valid path/no file
#endif
}

// Split a pathname into parts (hiding operating specific parts).
void ZIL_STORAGE_READ_ONLY::StripFullPath(const ZIL_ICHAR *fullPath,
	ZIL_ICHAR *pathName, ZIL_ICHAR *fileName, ZIL_ICHAR *objectName,
	ZIL_ICHAR *objectPathName)
{
	// Determine the path/file split area.
	const ZIL_ICHAR *endPath = strrchr(fullPath, defaultCharMap->dirSepStr[0]);
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	if (!endPath)
		endPath = strrchr(fullPath, ':');
#endif
	if (!endPath)
		endPath = fullPath;
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2)
	if (*endPath == ':')
		endPath++;
	if (*endPath == defaultCharMap->dirSepStr[0] && endPath-fullPath == 2 && endPath[-1] == ':')
		endPath++;
#endif
	// Determine the file/object split area.
	const ZIL_ICHAR *endFile = strchr(endPath, ZIL_DIRECTORY_SEPARATOR);
	if (!endFile)
		endFile = &endPath[strlen(endPath)];

	// find the end of the object path name
	const ZIL_ICHAR *endOPath = strrchr(endFile, ZIL_DIRECTORY_SEPARATOR);
	if (!endOPath)
		endOPath = &endFile[strlen(endFile)];

	// Construct the path name.
	if (pathName)
	{
		int i = (int)(endPath - fullPath);
		strncpy(pathName, fullPath, i);
		pathName[i] = '\0';
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
		strupr(pathName);
#endif
	}
	if (*endPath == defaultCharMap->dirSepStr[0])
		endPath++;

	// Construct the file name.
	if (fileName)
	{
		int i = (int)(endFile - endPath);
		strncpy(fileName, endPath, i);
		fileName[i] = '\0';
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS)
		strupr(fileName);
#endif
	}

	// Construct the object path name.
	if (objectPathName)
	{
		int i = (int)(endOPath - endFile);
		strncpy(objectPathName, endFile, i);
		objectPathName[i] = '\0';
	}
	if (*endOPath == ZIL_DIRECTORY_SEPARATOR)
		endOPath++;

	// Construct the object name.
	if (objectName)
		strcpy(objectName, endOPath);
}

// Create a name for a temporary file that is readable, writable, and doesn't
// destroy any other file.
void ZIL_STORAGE_READ_ONLY::TempName(ZIL_ICHAR *tempname)
{
#if defined(ZIL_POSIX) || (defined(ZIL_OS2) && defined(__IBMCPP__))
	tmpnam(tempname);
#else
	ZIL_ICHAR tmppath[ZIL_MAXPATHLEN];
	tmpnam(tmppath);
	MakeFullPath(tempname);
	AppendFullPath(tempname, tempname, tmppath);
#endif
}

// --- ZIL_STORAGE_READ_ONLY privates ------------------------------------------

ZIL_DISK_ADDRESS ZIL_STORAGE_READ_ONLY::AllocData(void)
{
	abort();
#ifndef __WATCOMC__
	return (0);
#endif
}

ZIL_DISK_ADDRESS ZIL_STORAGE_READ_ONLY::AppendInode(ZIL_INODE_NUMBER)
{
	abort();
#ifndef __WATCOMC__
	return (0);
#endif
}

// Check to see if an inode (inum) is already in the open object list.
int ZIL_STORAGE_READ_ONLY::CheckOpen(ZIL_INODE_NUMBER inum)
{
	for (int i=0; i < openLen; i++)
		if (openObjects[i].openCount && openObjects[i].inum == inum)
			return (i);
#if defined(ZIL_MACINTOSH)
	storageError = permErr;
#else
	storageError = EACCES;
#endif
	return (-1);
}

ZIL_STORAGE_OBJECT_READ_ONLY *ZIL_STORAGE_READ_ONLY::CreateObject(void)
{
	return new ZIL_STORAGE_OBJECT_READ_ONLY();
}

// Read or Write an inode depending on the direction.
long ZIL_STORAGE_READ_ONLY::FindInode(ZIL_INODE_NUMBER inum)
{
	ZIL_DISK_ADDRESS inumblk;
	long pos;

	ZIL_DISK_ADDRESS iindex = (ZIL_UINT16)(inum / INODES_PER_INODE_BLOCK);
	ZIL_DISK_ADDRESS inumbyt = (ZIL_UINT16)(inum % INODES_PER_INODE_BLOCK);

	// First look through the direct blocks.
	if (iindex < LENGTHOF(sb->inodeDirect))
	{
		if (sb->inodeDirect[iindex] == 0)
			sb->inodeDirect[iindex] = AppendInode(inum);
		inumblk = sb->inodeDirect[iindex];
	}
	else
	{
		// Now look in the indirect blocks
		iindex -= LENGTHOF(sb->inodeDirect);
		// try to do the first stage of double indirect blocks
		ZIL_DISK_ADDRESS *iblk = new ZIL_DISK_ADDRESS[DISK_ADDR_PER_INODE_BLOCK];
		if (iindex >= DISK_ADDR_PER_INODE_BLOCK)
		{
			iindex -= DISK_ADDR_PER_INODE_BLOCK;
			if (iindex >= DISK_ADDR_PER_INODE_BLOCK)
			{
				delete []iblk;
#if defined(ZIL_MACINTOSH)
				FileAbort(rfNumErr);
				storageError = rfNumErr;
#else
				FileAbort(EINVAL);
				storageError = EINVAL;
#endif
				return (-1);
			}
			if (sb->inodeDIndirect == 0)
				sb->inodeDIndirect = AppendInode(0xFFFFFFFFL);
			pos = sb->inodeDIndirect * (long)BYTES_PER_BLOCK;
			storageError = ReadAt(pos, iblk,
					      INODE_INDIRECT_SIZE);
			ZIL_DISK_ADDRESS blkind = iindex % DISK_ADDR_PER_INODE_BLOCK;
			ZIL_DISK_ADDRESS blknum = iindex / DISK_ADDR_PER_INODE_BLOCK;
			iindex = iblk[blknum];
			if (storageError == 0 && iindex == 0)
			{
				iindex = iblk[blknum] = AppendInode(0xFFFFFFFFL);
				SwapDiskAddr(&iblk[blknum]);
				storageError = WriteAt(pos, iblk,
						       INODE_INDIRECT_SIZE);
			}
			else
				SwapDiskAddr(&iindex);
			pos = iindex * (long)BYTES_PER_BLOCK;
			iindex = blkind;
			if (storageError != 0)
			{
				delete []iblk;
				return (-1);
			}
		}
		else
		{
			// Allocate an single indirect block.
			if (sb->inodeSIndirect == 0)
				sb->inodeSIndirect = AppendInode(0xFFFFFFFFL);
			pos = sb->inodeSIndirect * (long)BYTES_PER_BLOCK;
		}
		// We are either reading an single indirect block or the
		// second stage of a double indirect block.
		storageError = ReadAt(pos, iblk, INODE_INDIRECT_SIZE);
		inumblk = iblk[iindex];
		if (storageError == 0 && inumblk == 0)
		{
			inumblk = iblk[iindex] = AppendInode(inum);
			SwapDiskAddr(&iblk[iindex]);
			storageError = WriteAt(pos, iblk,
					       INODE_INDIRECT_SIZE);
		}
		else
			SwapDiskAddr(&inumblk);
		delete []iblk;
		if (storageError != 0)
			return (-1);
	}
	// Actually perform the read or write.
	return ((long)inumblk*BYTES_PER_BLOCK + inumbyt*sizeof(inode));
}

// In our list of open objects, try to find an empty hole.  Allocate more
// space if we can't.
int ZIL_STORAGE_READ_ONLY::FindSlot(void)
{
	int i = 0;
	for (; i < openLen; i++)
		if (openObjects[i].openCount <= 0)
			break;
	// No room, allocate more space.
	if (i >= openLen)
	{
		openObject *nopen;

		nopen = new openObject[openLen+NEW_INC];
		if (nopen == ZIL_NULLP(openObject))
		{
#if defined(ZIL_MACINTOSH)
			FileAbort(ioErr);
			storageError = ioErr;
#else
			FileAbort(ENOMEM);
			storageError = ENOMEM;
#endif
			return (-1);
		}
		for (i=0; i < openLen; i++)
			nopen[i] = openObjects[i];
		for (i=openLen; i < openLen+NEW_INC; i++)
			nopen[i].openCount = 0;
		delete []openObjects;
		openObjects = nopen;
		i = openLen;
		openLen += NEW_INC;
	}
	return (i);
}

// See if a data block is already in the cache.
int ZIL_STORAGE_READ_ONLY::InMemory(ZIL_DISK_ADDRESS blknum)
{
	if (blknum == 0)
		return (FALSE);
	if (FlagSet(flags, UIS_COPY))
		return (TRUE);
	for (int i=0; i < cacheLen; i++)
		if (cd[i].blknum == blknum)
			return (TRUE);
	return (FALSE);
}

ZIL_STORAGE_READ_ONLY::~ZIL_STORAGE_READ_ONLY(void)
{
	// If the file has been really opened.
	if (fd >= 0)
	{
#if defined(ZIL_MACINTOSH)
		FSClose(fd);
#else
		close(fd);
#endif
	}
	if (currentDirectory)
		delete currentDirectory;
	// Deleting currentDirectory depends on openObjects existing.
	delete []openObjects;
	delete sb;
	delete []cd;
#if defined(ZIL_DOSEXTENDED16)
	if (cache)
		delete [](cache - 2);
#else
	delete []cache;
#endif
}

// Read some data from a certain position.  This is here mainly to link
// the seek and read together.
int ZIL_STORAGE_READ_ONLY::ReadAt(long pos, void *buf, int len)
{
#if defined(ZIL_MACINTOSH)
	errno = SetFPos(fd, fsFromStart, pos);
	if (errno != noErr)
#else
	if (lseek(fd, pos, SEEK_SET) != pos)
#endif
	{
		FileAbort(errno);
		return (errno);
	}
#if defined(ZIL_MACINTOSH)
	long count = len;
	errno = FSRead(fd, &count, (BUFFTYPE *)buf);
	if (errno != noErr)
#else
	if (read(fd, (BUFFTYPE *)buf, len) != len)
#endif
	{
		FileAbort(errno);
		return (errno);
	}
	return (0);
}

// Read a data block.  First check to see if it is currently in the cache.
// If so, just return a pointer to that block.  Otherwise, find an unused
// cache block, flush it if necessary, read the data into it and return it.
void *ZIL_STORAGE_READ_ONLY::ReadData(ZIL_DISK_ADDRESS blknum)
{
	if (blknum == 0)
		return (ZIL_NULLP(void));
	int i = 0;
	for (; i < cacheLen; i++)
	{
		if (cd[i].blknum == blknum)
		{
			cacheData tmpcd = cd[i];
			tmpcd.used++;
			for (int j=i; j > 0; j--)
				cd[j] = cd[j-1];
			cd[0] = tmpcd;
			return (&cache[tmpcd.pos*BYTES_PER_BLOCK]);
		}
	}
	if (FlagSet(flags, UIS_COPY))
	{
		for (i=0; i < cacheLen; i++)
			if (cd[i].used == 0 && !cd[i].dirty)
				break;
	}
	else
	{
		for (i=0; i < cacheLen; i++)
			if (cd[i].used == 0)
				break;
	}
	if (i >= cacheLen)
	{
		// This is a fatal error, ran out of room.
#if defined(ZIL_MACINTOSH)
		storageError = ioErr;
#else
		storageError = ERANGE;
#endif
		abort();
	}
	cacheData tmpcd = cd[i];
	for (int j=i; j > 0; j--)
		cd[j] = cd[j-1];
	int cpos = tmpcd.pos * BYTES_PER_BLOCK;
	if (tmpcd.dirty)
	{
		long dpos = (long)tmpcd.blknum * BYTES_PER_BLOCK;
		if ((storageError = WriteAt(dpos, &cache[cpos], BYTES_PER_BLOCK)) != 0)
			return (ZIL_NULLP(void));
		tmpcd.dirty = 0;
	}
	tmpcd.blknum = blknum;
	tmpcd.used = 1;
	cd[0] = tmpcd;
	long dpos = (long)blknum * BYTES_PER_BLOCK;
	if (FlagSet(flags, UIS_COPY))
	{
		memset(&cache[cpos], 0, BYTES_PER_BLOCK);
	}
	else
	{
		if ((storageError = ReadAt(dpos, &cache[cpos], BYTES_PER_BLOCK)) != 0)
			return (ZIL_NULLP(void));
	}
	return (&cache[cpos]);
}

int ZIL_STORAGE_READ_ONLY::RWInode(ZIL_INODE_NUMBER inum, inode *ientry, int)
{
	long pos = FindInode(inum);
	if ((storageError = ReadAt(pos, ientry, sizeof(*ientry))) != 0)
		return -1;
	SwapInode(ientry);
	return (0);
}

// Walk a path, strip the last name, return both end directory and name
// inside that directory to operate on.
void ZIL_STORAGE_READ_ONLY::WalkPartialPath(const ZIL_ICHAR *pathname,
				 ZIL_STORAGE_OBJECT_READ_ONLY **parentdir,
				 const ZIL_ICHAR **filename)
{
	static ZIL_ICHAR currentDir[2] = { '.', 0 };

	*parentdir = WalkPath(pathname, TRUE);
	*filename = pathname + strlen(pathname);
	while (*filename != pathname && **filename != ZIL_DIRECTORY_SEPARATOR)
		(*filename)--;
	if (**filename == ZIL_DIRECTORY_SEPARATOR)
		(*filename)++;
	if (!**filename)
		*filename = currentDir;
	if (*parentdir == ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY))
		*parentdir = currentDirectory;
}

// Given a path name into a STORAGE file, walk down it.  Return errors
// on bogus directory names, and return a NULL.  If stripLast is TRUE then
// we don't try to move to the last piece of the path (it might be a file
// or we might want to manipulate it).
ZIL_STORAGE_OBJECT_READ_ONLY *ZIL_STORAGE_READ_ONLY::WalkPath(const ZIL_ICHAR *name, int stripLast)
{
	if (!name || ! *name)
		return ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	ZIL_STORAGE_OBJECT_READ_ONLY *startdir = currentDirectory;
	ZIL_ICHAR *tmpname = new ZIL_ICHAR[strlen(name)+2];
	ZIL_ICHAR *strt = tmpname;
	ZIL_ICHAR *stop = tmpname;
	strcpy(tmpname, name);
	tmpname[strlen(name)+1] = '\0';	// hard mark the end
	// if (stripLast) then don't try to get into the last part of the path
	// Strip it off.
	if (stripLast)
	{
		int i = strlen(tmpname);
		while (i && tmpname[i] != ZIL_DIRECTORY_SEPARATOR)
			i--;
		if (i == 0 && tmpname[0] == ZIL_DIRECTORY_SEPARATOR)
			i++;
		tmpname[i] = '\0';
		tmpname[i+1] = '\0';	// hard mark the end
	}
	// There was no path to traverse.
	if (!*strt)
	{
		delete []tmpname;
		return ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	}
	for (;;)
	{
		// strip out a directory name
		while (*stop && *stop != ZIL_DIRECTORY_SEPARATOR)
			stop++;
		*stop = '\0';
		if (stop == strt)
		{
			// empty name, just a ~, start from root
			if (openObjects[startdir->inodeIndex].inum != 0)
			{
				static directoryEntry dentry = { 0, 0, 0, 0, { 0 } };
				inode ientry;

				if (RWInode((ZIL_INODE_NUMBER)0, &ientry, ZIL_IREAD) < 0)
					break;
				if (startdir != currentDirectory)
					delete startdir;
				startdir = CreateObject();
				startdir->flags = currentDirectory->flags;
				startdir->OpenTheObject(*this, &dentry, &ientry,
							FALSE);
			}
			stop++;
			// That's all folks.
			if (! *stop)
			{
				delete []tmpname;
				return (startdir);
			}
			strt = stop;
		}
		// Empty name (~~ ???) error.
		else if (! *strt)
			break;
		// No directory, error.
		else if (!startdir->FindName(strt))
			break;
		// Have a name, and it exists.  Let's move there.
		else
		{
			ZIL_STORAGE_OBJECT_READ_ONLY *dir = CreateObject();
			directoryEntry dentry;
			inode ientry;

			startdir->Load(&dentry);
			dir->objectID = dentry.objectID;
			dir->stringID = strdup(dentry.stringID);
			dir->flags = startdir->flags;
			int i = CheckOpen(dentry.inum);
			if (i < 0)
			{
				storageError = 0;	// not an error yet
				if (RWInode(dentry.inum, &ientry, ZIL_IREAD) < 0)
				{
					delete dir;
					break;
				}
			}
			else
				ientry = openObjects[i].inode;
			dir->OpenTheObject(*this, &dentry, &ientry, FALSE);
			if (!(openObjects[dir->inodeIndex].inode.useCount &
			      ZIL_DIRECTORY_TAG))
			{
				delete dir;
				break;
			}
			if (currentDirectory != startdir)
				delete startdir;
			startdir = dir;
			stop++;
			// That's all folks.
			if (! *stop)
			{
				delete []tmpname;
				return (startdir);
			}
			strt = stop;
		}
	}
	// All errors come through here.
	if (currentDirectory != startdir)
		delete startdir;
	delete []tmpname;
	return ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
}

int ZIL_STORAGE_READ_ONLY::WriteAt(long, void *, int)
{
	abort();
#ifndef __WATCOMC__
	return (0);
#endif
}

void ZIL_STORAGE_READ_ONLY::WriteData(void *data, int modified)
{
	if ((char *)data < cache || (char *)data >= &cache[cacheLen*BYTES_PER_BLOCK])
	{
#if defined(ZIL_MACINTOSH)
		storageError = ioErr;
#else
		storageError = ERANGE;
#endif
		abort();
	}
	ZIL_UINT16 i = (ZIL_UINT16)(((char *)data - cache) / BYTES_PER_BLOCK);
	// This is probably always 0 (or 1)
	for (int j=0; j < cacheLen; j++)
		if (cd[j].pos == i)
		{
			// FlagSet is probably overkill.
			cd[j].dirty |= FlagSet(flags, UIS_READWRITE) && modified;
			cd[j].used = 0;
			return;
		}
//#if defined(ZIL_MACINTOSH)
//	storageError = ioErr;
//#else
//	storageError = ERANGE;
//#endif
#if !defined(ZIL_MSWINDOWS) && !defined(ZIL_MACINTOSH)
	puts("Fatal internal ZIL_STORAGE error.\n");
#endif
	abort();
}

int ZIL_STORAGE_READ_ONLY::SearchPath(ZIL_ICHAR *tmp)
{
	const ZIL_ICHAR *path = _blankString;
	int first = 1;
	while (path != ZIL_NULLP(ZIL_ICHAR))
	{
#if defined(applec) || defined(__MWERKS__) || defined(__ZTC__)
		// Dummy space because of some Zortech/Symantec problem
		ZINC_SIGNATURE _statb;
#else
		struct stat _statb;
#endif
		AppendFullPath(tmp, path, pname);
		AppendFullPath(tmp, tmp, fname);
	if (stat(tmp, &_statb) >= 0) break;
		path = searchPath ?
			(first ? searchPath->FirstPathName() : searchPath->NextPathName()) : ZIL_NULLP(ZIL_ICHAR);
		first = 0;
	}
	if (path != ZIL_NULLP(ZIL_ICHAR))
	{
		AppendFullPath(tmp, path, pname);
		strcpy(pname, tmp);
	}
	MakeFullPath(pname);
	return (path == ZIL_NULLP(ZIL_ICHAR));
}

void ZIL_STORAGE_READ_ONLY::ValidateZincFile(void)
{
#if defined(ZIL_3x_COMPAT)
	if (storageError == 0 && sb->signature.majorVersion == 3 &&
	    sb->signature.minorVersion == 0)
	{
		sb->freeInodeListHi = 0;
		sb->signature.minorVersion = 2;
		sb->signature.copyrightNotice[25] = '2';
	}
	if (storageError != 0 ||
	    sb->signature.magicNumber != MAGIC_NUMBER ||
	    sb->signature.majorVersion < 3)
	{
#if defined(ZIL_MACINTOSH)
		storageError = rfNumErr;
#else
		storageError = EINVAL;
#endif
		return;
	}
#	if defined(ZIL_UNICODE)
	if (sb->signature.majorVersion == 3)
	{
		sb->signature.minorVersion = 6;
		sb->signature.copyrightNotice[25] = '6';
	}
	else
	{
		sb->signature = _signature;
	}
#	endif
#else
	if (storageError != 0 ||
	    sb->signature.magicNumber != MAGIC_NUMBER ||
	    sb->signature.majorVersion < MAJOR_VERSION)
	{
#if defined(ZIL_MACINTOSH)
		storageError = rfNumErr;
#else
		storageError = EINVAL;
#endif
		return;
	}
#	if defined(ZIL_UNICODE)
		sb->signature = _signature;
#	endif
#endif
}

void ZIL_STORAGE_READ_ONLY::SetUpLocalData(void)
{
	sb->revision++;
	cacheLen = cacheSize;
#if defined(ZIL_DOSEXTENDED16)
	// The DOS Extender can butt this up against the end of a segment
	// thereby creating a non-ANSI program (we can_t compare a pointer
	// against the address following "cache" because it wraps around
	// to 0x0000).  This will fix it!!!
	cache = new char[cacheLen * BYTES_PER_DATA_BLOCK + 4] + 2;
#else
	cache = new char[cacheLen * BYTES_PER_DATA_BLOCK];
#endif
	cd = new cacheData[cacheLen];
	if (cache == ZIL_NULLP(char) || cd == ZIL_NULLP(cacheData))
	{
#if defined(ZIL_MACINTOSH)
		storageError = ioErr;
#else
		storageError = ENOMEM;
#endif
		return;
	}
	// Initialize the cache.
	int i = 0;
	for (; i < cacheLen; i++)
	{
		cd[i].pos = i;
		cd[i].blknum = 0;
		cd[i].dirty = 0;
		cd[i].used = 0;
	}
	// initialize the open file table to all unused
	openLen = NEW_INC;
	openObjects = new openObject[openLen];
	for (i=0; i < openLen; i++)
		openObjects[i].openCount = 0;
}

void ZIL_STORAGE_READ_ONLY::ChDirRoot(void)
{
	// or start up access in the root (chdir("~")).
	currentDirectory = CreateObject();
	currentDirectory->file = this;
	currentDirectory->inodeIndex = 0;
	currentDirectory->position = 0;
	currentDirectory->flags = flags;
	openObjects[0].inum = 0;
	openObjects[0].openCount = 1;
	openObjects[0].modified = 0;
	if (RWInode((ZIL_INODE_NUMBER)0, &openObjects[0].inode, ZIL_IREAD) < 0)
		return;
}

// --- ZIL_STORAGE_READ_ONLY publics -------------------------------------------

ZIL_STORAGE_READ_ONLY::ZIL_STORAGE_READ_ONLY(void) :
	storageError(0), openLen(0), openObjects(ZIL_NULLP(openObject)),
	currentDirectory(ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY)), fd(-1),
	sb(ZIL_NULLP(superBlock)), cacheLen(0), cd(ZIL_NULLP(cacheData)),
	cache(ZIL_NULLP(char)),
	flags(0)
{
}

ZIL_STORAGE_READ_ONLY::ZIL_STORAGE_READ_ONLY(const ZIL_ICHAR *name) :
	storageError(0), openLen(0), openObjects(ZIL_NULLP(openObject)),
	currentDirectory(ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY)), fd(-1),
	sb(ZIL_NULLP(superBlock)), cacheLen(0), cd(ZIL_NULLP(cacheData)),
	cache(ZIL_NULLP(char)),
	flags(UIS_READ)
{
	StripFullPath(name, pname, fname);
	// If this is an old file, find it in the searchPath.
	ZIL_ICHAR tmp[ZIL_MAXPATHLEN];
	SearchPath(tmp);
	// open the file
	// The real file for read-only.
	AppendFullPath(tmp, pname, fname);
	fd = open(tmp, O_BINARY|O_RDONLY);
	if (fd < 0)
	{
		FileAbortAll(errno);
		storageError = errno;
		return;
	}
	sb = new superBlock;
	storageError = ReadAt(0L, sb, sizeof(*sb));
	SwapSuperBlock(sb);
	// Validate that this is a Zinc file.
	ValidateZincFile();
	CheckStorageErrorNull();
	SetUpLocalData();
	CheckStorageErrorNull();
	ChDirRoot();
	CheckStorageErrorNull();
	storageError = 0;
}

int ZIL_STORAGE_READ_ONLY::ChDir(const ZIL_ICHAR *name)
{
	CheckStorageError(-1);
	ZIL_STORAGE_OBJECT_READ_ONLY *startdir = WalkPath(name, FALSE);
	if (startdir == ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY))
		return (-1);
	if (startdir != currentDirectory)
		delete currentDirectory;
	currentDirectory = startdir;
	return (0);
}

int ZIL_STORAGE_READ_ONLY::GetCWD(ZIL_ICHAR *path, int pathLen)
{
	// To make the loop code easier, short circuit if we are already
	// in the root directory.
	if (openObjects[currentDirectory->inodeIndex].inum == 0)
	{
		if (pathLen < strlen(_rootStorageDirectory))
		{
#if defined(ZIL_MACINTOSH)
			FileAbort(ioErr);
			storageError = ioErr;
#else
			FileAbort(ERANGE);
			storageError = ERANGE;
#endif
			return (-1);
		}
		strcpy(path, _rootStorageDirectory);
		return (0);
	}

#define DOEXIT(x)	if (oldCurrentDirectory != currentDirectory)	\
				delete currentDirectory;		\
			currentDirectory = oldCurrentDirectory;		\
			return (x)

	ZIL_STORAGE_OBJECT_READ_ONLY *oldCurrentDirectory = currentDirectory;

	path[pathLen-1] = 0;
	int currentPathLen = pathLen - 1;

	// The alogrithm goes as such: Save our Inum, change to our parent,
	// search for our Inum in our parent's data, loop until we hit the
	// root directory (Inum == 0).  On the way, append our name and a
	// separator to the front of the path.
	ZIL_INODE_NUMBER searchInum;
	while ((searchInum = openObjects[currentDirectory->inodeIndex].inum) != 0)
	{
		ZIL_STORAGE_OBJECT_READ_ONLY *parentDir = WalkPath(_parentDirectory, FALSE);
		if (oldCurrentDirectory != currentDirectory)
			delete currentDirectory;
		currentDirectory = parentDir;

		directoryEntry dentry;
		currentDirectory->position = 0;
		long currentSize = openObjects[currentDirectory->inodeIndex].inode.size;
		while (currentDirectory->position < currentSize)
		{
			if (currentDirectory->Load(&dentry) < 0)
			{
#if defined(ZIL_MACINTOSH)
				FileAbort(rfNumErr);
				storageError = rfNumErr;
#else
				FileAbort(EINVAL);
				storageError = EINVAL;
#endif
				DOEXIT(-1);
			}
			if (dentry.inum == searchInum)
			{
				currentDirectory->position = 0;
				break;
			}
		}
		if (currentDirectory->position >= currentSize)
		{
#if defined(ZIL_MACINTOSH)
			FileAbort(rfNumErr);
			storageError = rfNumErr;
#else
			FileAbort(EINVAL);
			storageError = EINVAL;
#endif
			DOEXIT(-1);
		}
		currentPathLen -= strlen(dentry.stringID) + strlen(_rootStorageDirectory);
		if (currentPathLen < 0)
		{
#if defined(ZIL_MACINTOSH)
			FileAbort(ioErr);
			storageError = ioErr;
#else
			FileAbort(ERANGE);
			storageError = ERANGE;
#endif
			DOEXIT(-1);
		}
		strcpy(&path[currentPathLen], _rootStorageDirectory);
		strcpy(&path[currentPathLen+strlen(_rootStorageDirectory)], dentry.stringID);
	}
	memmove((char *)path, (char *)&path[currentPathLen], (pathLen-currentPathLen)*sizeof(ZIL_ICHAR));
	DOEXIT(0);
#undef DOEXIT
}

ZIL_STORAGE_DIRECTORY *ZIL_STORAGE_READ_ONLY::OpenDir(const ZIL_ICHAR *name)
{
	ZIL_STORAGE_OBJECT_READ_ONLY *tmp =
		new ZIL_STORAGE_OBJECT_READ_ONLY(*this, name, 0);
	if (tmp->objectError != 0 ||
	    !FlagSet(tmp->file->openObjects[tmp->inodeIndex].inode.useCount, ZIL_DIRECTORY_TAG))
	{
		delete tmp;
		return (ZIL_NULLP(ZIL_STORAGE_DIRECTORY));
	}
	return (new ZIL_STORAGE_DIRECTORY(tmp));
}

ZIL_STATS_INFO *ZIL_STORAGE_READ_ONLY::Stats(void)
{
	static ZIL_STATS_INFO stats;
	stats.createTime = sb->createTime;
	stats.modifyTime = sb->modifyTime;
#if defined(ZIL_MACINTOSH)
	// Fix for time value since Apple uses 1904.
	stats.createTime += 31554100L * 66L + 249000L;
	stats.modifyTime += 31554100L * 66L + 249000L;

	GetEOF(fd, &stats.size);
#else
	stats.size = lseek(fd, 0L, SEEK_END);
#endif
	stats.useCount = 1;		// ??? Unix will change this
	stats.revision = sb->revision;
	stats.countryID = 0;
	stats.inum = 0;			// ??? Unix will change this
	return (&stats);
}

int ZIL_STORAGE_READ_ONLY::Version(void)
{
	return (100*sb->signature.majorVersion + sb->signature.minorVersion);
}

// --- ZIL_STORAGE_OBJECT_READ_ONLY protected --------------------------------

void ZIL_STORAGE_OBJECT_READ_ONLY::CreateBlkCache(void)
{
#if defined(DOCACHEPTR)
	// Partial initialization, others finish the work.
	cachedBlknum = new ZIL_DISK_ADDRESS[DOCACHEPTR];
	cachedBlkptr = new ZIL_DISK_ADDRESS[DOCACHEPTR];
	for (int i=0; i < DOCACHEPTR; i++)
	{
		cachedBlknum[i] = 0;
		cachedBlkptr[i] = 0;
	}
#endif
}

// --- ZIL_STORAGE_OBJECT_READ_ONLY publics -----------------------------------

ZIL_STORAGE_OBJECT_READ_ONLY::ZIL_STORAGE_OBJECT_READ_ONLY(void) :
	objectError(0), file(ZIL_NULLP(ZIL_STORAGE)), stringID(ZIL_NULLP(ZIL_ICHAR)),
	inodeIndex(-1), position(-1), cachedBlknum(ZIL_NULLP(ZIL_DISK_ADDRESS)),
	cachedBlkptr(ZIL_NULLP(ZIL_DISK_ADDRESS)), flags(UIS_READ)
{
	CreateBlkCache();
}

ZIL_STORAGE_OBJECT_READ_ONLY::ZIL_STORAGE_OBJECT_READ_ONLY(ZIL_STORAGE_READ_ONLY &pfile, const ZIL_ICHAR *name, ZIL_OBJECTID) :
	objectError(0), file(ZIL_NULLP(ZIL_STORAGE)), stringID(ZIL_NULLP(ZIL_ICHAR)),
	inodeIndex(-1), position(-1), cachedBlknum(ZIL_NULLP(ZIL_DISK_ADDRESS)),
	cachedBlkptr(ZIL_NULLP(ZIL_DISK_ADDRESS)), flags(UIS_READ)
{
#define DOEXIT	if (parentdir != pfile.currentDirectory)	\
			delete parentdir

	if (name == ZIL_NULLP(ZIL_ICHAR))
	{
#if defined(ZIL_MACINTOSH)
		FileAbort(rfNumErr);
		objectError = rfNumErr;
#else
		FileAbort(EINVAL);
		objectError = EINVAL;
#endif
		return;
	}

	ZIL_STORAGE_OBJECT_READ_ONLY *parentdir;
	const ZIL_ICHAR *fname;
	pfile.WalkPartialPath(name, &parentdir, &fname);
	int found = parentdir->FindName(fname);

	directoryEntry dentry;
	inode ientry;
	if (!found)
	{
		// If the object doesn't exist
#if defined(ZIL_MACINTOSH)
		FileAbort(fnfErr);
		objectError = fnfErr;
#else
		FileAbort(ENOENT);
		objectError = ENOENT;
#endif
		DOEXIT;
		return;
	}
	// If the object does exist
	parentdir->Load(&dentry);
	objectID = dentry.objectID;
	stringID = strdup(dentry.stringID);
	int i = pfile.CheckOpen(dentry.inum);
	if (i < 0) {
		pfile.storageError = 0;		// not an error yet
		if (pfile.RWInode(dentry.inum, &ientry, ZIL_IREAD) < 0)
		{
			objectError = pfile.storageError;
			pfile.storageError = 0;
			DOEXIT;
			return;
		}
	}
	else
		ientry = pfile.openObjects[i].inode;
	if (ientry.useCount == 0)
	{
#if defined(ZIL_MACINTOSH)
		FileAbort(fnfErr);
		objectError = fnfErr;
#else
		FileAbort(ENOENT);
		objectError = ENOENT;
#endif
		DOEXIT;
		return;
	}
	CreateBlkCache();
	OpenTheObject(pfile, &dentry, &ientry, FALSE);
	DOEXIT;
#undef DOEXIT
}

ZIL_STORAGE_OBJECT_READ_ONLY::~ZIL_STORAGE_OBJECT_READ_ONLY(void)
{
#if defined(DOCACHEPTR)
	delete []cachedBlknum;
	delete []cachedBlkptr;
#endif
	delete stringID;
	openObject *o = CheckObject();
	if (o)
		o->openCount--;
}

ZIL_STATS_INFO *ZIL_STORAGE_OBJECT_READ_ONLY::Stats(void)
{
	openObject *o = CheckObject();
	if (o == ZIL_NULLP(openObject))
		return (ZIL_NULLP(ZIL_STATS_INFO));

	static ZIL_STATS_INFO stats;
	stats.createTime = o->inode.createTime;
	stats.modifyTime = o->inode.modifyTime;
#if defined(ZIL_MACINTOSH)
	// Fix for time value since Apple uses 1904.
	stats.createTime += 31554100L * 66L + 249000L;
	stats.modifyTime += 31554100L * 66L + 249000L;
#endif
	stats.size = o->inode.size;
	stats.useCount = o->inode.useCount;
	stats.revision = o->revision;
	stats.countryID = o->country;
	stats.inum = o->inum;
	return (&stats);
}

// --- Loads ------------------------------------------------------------------

#if defined(ZIL_BIGENDIAN)

int ZIL_STORAGE_OBJECT_READ_ONLY::Load(ZIL_INT16 *value)
{
	ZIL_UINT8 buff[sizeof(*value)];

	int i = RWData(buff, sizeof(*value), ZIL_IREAD);
	if (i > 0)
	{
		*value = 0;
		for (int j=0; j < sizeof(*value); j++)
			*value |= (buff[j] << 8*j);
	}
	return (i);
}

int ZIL_STORAGE_OBJECT_READ_ONLY::Load(ZIL_UINT16 *value)
{
	ZIL_UINT8 buff[sizeof(*value)];

	int i = RWData(buff, sizeof(*value), ZIL_IREAD);
	if (i > 0)
	{
		*value = 0;
		for (int j=0; j < sizeof(*value); j++)
			*value |= (buff[j] << 8*j);
	}
	return (i);
}

int ZIL_STORAGE_OBJECT_READ_ONLY::Load(ZIL_INT32 *value)
{
	ZIL_UINT8 buff[sizeof(*value)];

	int i = RWData(buff, sizeof(*value), ZIL_IREAD);
	if (i > 0)
	{
		*value = 0;
		for (int j=0; j < sizeof(*value); j++)
			*value |= (buff[j] << 8*j);
	}
	return (i);
}

int ZIL_STORAGE_OBJECT_READ_ONLY::Load(ZIL_UINT32 *value)
{
	ZIL_UINT8 buff[sizeof(*value)];

	int i = RWData(buff, sizeof(*value), ZIL_IREAD);
	if (i  > 0)
	{
		*value = 0;
		for (int j=0; j < sizeof(*value); j++)
			*value |= (buff[j] << 8*j);
	}
	return (i);
}
#endif

int ZIL_STORAGE_OBJECT_READ_ONLY::Load(ZIL_ICHAR *string, int len)
{
	CheckObjectError(-1);
	*string = '\0';
	ZIL_UINT16 size;
	int cnt1 = ZIL_STORAGE_OBJECT_READ_ONLY::Load(&size);
	if (cnt1 != sizeof(size))
		return (cnt1);
	// Unicode support can load both Unicode and ISO 8859-1 strings
	// without data loss.  The upper 128 characters of OEM strings
	// are bogus in Unicode mode.  ISO and OEM support can load the
	// first 256 or 128 Unicode characters respectivly.  Both loose
	// information outside this range.
	int doUni = (size & ZIL_UNICODE_FLAG);
	size &= ~ZIL_UNICODE_FLAG;
	if (size > len)
	{
		position -= cnt1;
		return (-1);
	}
	int cnt2 = 0;
	if (size)
#if defined(ZIL_UNICODE)
		if (doUni)		// Byte Swap each short
		{
			for (int i = 0; i < size; i++)
			{
				ZIL_UINT16 c;
				cnt2 += ZIL_STORAGE_OBJECT_READ_ONLY::Load(&c);
				string[i] = c;
			}
		}
		else
		{
			for (int i = 0; i < size; i++)
			{
				ZIL_UINT8 tmp;
				cnt2 += ZIL_STORAGE_OBJECT_READ_ONLY::Load(&tmp);
				string[i] = tmp;
			}
		}
#else
		if (doUni)
			for (int i = 0; i < size; i++)
			{
				ZIL_UINT16 tmp;
				cnt2 += ZIL_STORAGE_OBJECT_READ_ONLY::Load(&tmp);
				string[i] = (char)tmp;
			}
		else
			cnt2 += RWData(string, size, ZIL_IREAD);
#endif
	string[size] = '\0';
	return (cnt2 != (size * sizeof(string[0])) ? cnt2 : cnt1 + cnt2);
}

int ZIL_STORAGE_OBJECT_READ_ONLY::Load(ZIL_ICHAR **string)
{
	CheckObjectError(-1);
	ZIL_UINT16 size;
	int cnt1 = ZIL_STORAGE_OBJECT_READ_ONLY::Load(&size);
	int cnt2 = 0;
	int doUni = (size & ZIL_UNICODE_FLAG);
	size &= ~ZIL_UNICODE_FLAG;
	if (cnt1 == sizeof(size) && size)
	{
		// See Unicode related comments in previous function.
		*string = new ZIL_ICHAR[size + 1];
#if defined(ZIL_UNICODE)
		if (doUni)		// Byte Swap each short
		{
			for (int i = 0; i < size; i++)
			{
				ZIL_UINT16 c;
				cnt2 += ZIL_STORAGE_OBJECT_READ_ONLY::Load(&c);
				(*string)[i] = c;
			}
		}
		else
		{
			for (int i = 0; i < size; i++)
			{
				ZIL_UINT8 tmp;
				cnt2 += ZIL_STORAGE_OBJECT_READ_ONLY::Load(&tmp);
				(*string)[i] = tmp;
			}
		}
#else
		if (doUni)
			for (int i = 0; i < size; i++)
			{
				ZIL_UINT16 tmp;
				cnt2 += ZIL_STORAGE_OBJECT_READ_ONLY::Load(&tmp);
				(*string)[i] = (char)tmp;
			}
		else
			cnt2 += RWData(*string, size, ZIL_IREAD);
#endif
		(*string)[size] = '\0';
	}
	else
		*string = ZIL_NULLP(ZIL_ICHAR);
	return (cnt2 != (size * sizeof((*string)[0])) ? cnt2 : cnt1 + cnt2);
}

int ZIL_STORAGE_OBJECT_READ_ONLY::Store(void *, int, int)
{
	abort();
#ifndef __WATCOMC__
	return (0);
#endif
}

// --- ZIL_STORAGE_OBJECT_READ_ONLY privates -----------------------------------

// Find an object's information in its files open-object array.
// Return errors if it doesn't exist.
openObject *ZIL_STORAGE_OBJECT_READ_ONLY::CheckObject(void)
{
	if (file == ZIL_NULLP(ZIL_STORAGE))
	{
#if defined(ZIL_MACINTOSH)
		FileAbort(fnfErr);
		objectError = fnfErr;
#else
		FileAbort(ENOENT);
		objectError = ENOENT;
#endif
		return (ZIL_NULLP(openObject));
	}
	if (inodeIndex < 0 || inodeIndex >= file->openLen)
	{
#if defined(ZIL_MACINTOSH)
		FileAbort(rfNumErr);
		objectError = rfNumErr;
#else
		FileAbort(EBADF);
		objectError = EBADF;
#endif
		return (ZIL_NULLP(openObject));
	}
	openObject *o = &file->openObjects[inodeIndex];
	if (o->openCount < 0)
	{
#if defined(ZIL_MACINTOSH)
		FileAbort(rfNumErr);
		objectError = rfNumErr;
#else
		FileAbort(EBADF);
		objectError = EBADF;
#endif
		return (ZIL_NULLP(openObject));
	}
	return (o);
}

// Find a name in a directory object.
int ZIL_STORAGE_OBJECT_READ_ONLY::FindName(const ZIL_ICHAR *name)
{
	// Assume this is a directory
	position = 0;
	while (position < file->openObjects[inodeIndex].inode.size)
	{
		long i = position;
		directoryEntry dentry;
		if (Load(&dentry) < 0)
			return (FALSE);
		if (streq(name, dentry.stringID) == 0)
		{
			position = i;
			return (TRUE);
		}
	}
	return (FALSE);
}

// Given a (relative) block number in an object, return the (absolute)
// block number in the file.
ZIL_DISK_ADDRESS ZIL_STORAGE_OBJECT_READ_ONLY::GetBlockPtr(ZIL_DISK_ADDRESS blknum)
{
#if defined(DOCACHEPTR)
	// Return a cached value.
	int i = 0;
	for (; i < DOCACHEPTR; i++)
		if (cachedBlkptr[i] != 0 && blknum == cachedBlknum[i])
		{
			ZIL_DISK_ADDRESS tmpNum = cachedBlknum[i];
			ZIL_DISK_ADDRESS tmpPtr = cachedBlkptr[i];
			// Make it most recently used (MRU).
			for (int j=i; j > 0; j--)
			{
				cachedBlknum[j] = cachedBlknum[j-1];
				cachedBlkptr[j] = cachedBlkptr[j-1];
			}
			cachedBlknum[0] = tmpNum;
			cachedBlkptr[0] = tmpPtr;
			return (tmpPtr);
		}
	// Make space for the current value (MRU algorithm).
	for (i=DOCACHEPTR-1; i > 0; i--)
	{
		cachedBlknum[i] = cachedBlknum[i-1];
		cachedBlkptr[i] = cachedBlkptr[i-1];
	}
	cachedBlknum[0] = blknum;
#endif
	// Find direct blocks.  (Maybe we shouldn't cache them...)
	inode *inode = &file->openObjects[inodeIndex].inode;
	if (blknum < LENGTHOF(inode->direct))
	{
		if (inode->direct[blknum] == 0)
			inode->direct[blknum] = file->AllocData();
#if defined(DOCACHEPTR)
		cachedBlkptr[0] = inode->direct[blknum];
		return (cachedBlkptr[0]);
#else
		return (inode->direct[blknum]);
#endif
	}
	blknum -= LENGTHOF(inode->direct);
	// single indirect blocks
	if (blknum < DISK_ADDR_PER_DATA_BLOCK)
	{
		if (inode->sIndirect == 0)
			inode->sIndirect = file->AllocData();
		ZIL_DISK_ADDRESS *iblk = (ZIL_DISK_ADDRESS *)file->ReadData(inode->sIndirect);
		if (iblk == ZIL_NULLP(ZIL_DISK_ADDRESS))
			return (0);
		ZIL_DISK_ADDRESS retval = iblk[blknum];
		int modified = (retval == 0);
		if (modified)
		{
			retval = iblk[blknum] = file->AllocData();
			SwapDiskAddr(&iblk[blknum]);
		}
		else
			SwapDiskAddr(&retval);
		file->WriteData(iblk, modified);
#if defined(DOCACHEPTR)
		cachedBlkptr[0] = retval;
#endif
		return (retval);
	}
	blknum -= DISK_ADDR_PER_DATA_BLOCK;
	// double indirect blocks
	unsigned int blkptr = blknum % DISK_ADDR_PER_DATA_BLOCK;
	blknum /= DISK_ADDR_PER_DATA_BLOCK;
	if (blknum < DISK_ADDR_PER_DATA_BLOCK)
	{
		if (inode->dIndirect == 0)
			inode->dIndirect = file->AllocData();
		ZIL_DISK_ADDRESS *iblk = (ZIL_DISK_ADDRESS *)file->ReadData(inode->dIndirect);
		if (iblk == ZIL_NULLP(ZIL_DISK_ADDRESS))
			return (0);
		ZIL_DISK_ADDRESS retval = iblk[blknum];
		int modified = (retval == 0);
		if (modified)
		{
			retval = iblk[blknum] = file->AllocData();
			SwapDiskAddr(&iblk[blknum]);
		}
		else
			SwapDiskAddr(&retval);
		file->WriteData(iblk, modified);
		iblk = (ZIL_DISK_ADDRESS *)file->ReadData(retval);
		if (iblk == ZIL_NULLP(ZIL_DISK_ADDRESS))
			return (0);
		retval = iblk[blkptr];
		modified = (retval == 0);
		if (modified)
		{
			retval = iblk[blkptr] = file->AllocData();
			SwapDiskAddr(&iblk[blkptr]);
		}
		else
			SwapDiskAddr(&retval);
		file->WriteData(iblk, modified);
#if defined(DOCACHEPTR)
		cachedBlkptr[0] = retval;
#endif
		return (retval);
	}
	return (0);		// error
}

// Utility routine for Storage to use.
// Don't change this without changing ZIL_STORAGE_DIRECTORY::ReadDir().
int ZIL_STORAGE_OBJECT_READ_ONLY::Load(directoryEntry *dirent)
{
	CheckObjectError(-1);
	ZIL_UINT16 sinum;
	int i = ZIL_STORAGE_OBJECT_READ_ONLY::Load(&sinum);
	if (i < 0)
		return (-1);
	int j;
	if (sinum == 0xffff)
	{
		if ((j = ZIL_STORAGE_OBJECT_READ_ONLY::Load(&dirent->inum)) < 0)
			return (-1);
		i += j;
	}
	else
		dirent->inum = sinum;
	if ((j = ZIL_STORAGE_OBJECT_READ_ONLY::Load(&dirent->objectID)) < 0)
		return (-1);
	i += j;
	if ((j = ZIL_STORAGE_OBJECT_READ_ONLY::Load(&dirent->revision)) < 0)
		return (-1);
	i += j;
	if ((j = ZIL_STORAGE_OBJECT_READ_ONLY::Load(&dirent->country)) < 0)
		return (-1);
	i += j;
	if ((j = ZIL_STORAGE_OBJECT_READ_ONLY::Load(dirent->stringID, sizeof(dirent->stringID))) < 0)
		return (-1);
	return (i + j);
}

// Do the common work to open up an object.
void ZIL_STORAGE_OBJECT_READ_ONLY::OpenTheObject(ZIL_STORAGE_READ_ONLY &pfile,
				      directoryEntry *dentry,
				      inode *ientry, int truncate)
{
	// if it is already open, dup the object
	int i = pfile.CheckOpen(dentry->inum);
	if (i >= 0)
		pfile.openObjects[i].openCount++;
	else
	{
		pfile.storageError = 0;		// not an error yet
		// find an empty slot
		if ((i = pfile.FindSlot()) < 0)
		{
#if defined(ZIL_MACINTOSH)
			objectError = ioErr;
#else
			objectError = ENOMEM;
#endif
			return;
		}
		pfile.openObjects[i].inum = dentry->inum;
		pfile.openObjects[i].openCount = 1;
		pfile.openObjects[i].inode = *ientry;
		pfile.openObjects[i].modified = 0;
		pfile.openObjects[i].revision = dentry->revision;
		pfile.openObjects[i].country = dentry->country;
	}
	// This next line may be changed sometime to get rid of space used.
	if (truncate)
		pfile.openObjects[i].inode.size = 0;
	inodeIndex = i;
	position = 0;
	file = &pfile;
}

// Read or write data from or to an object.  N is the number of bytes,
// direction is read or write.
int ZIL_STORAGE_OBJECT_READ_ONLY::RWData(void *_buffer, unsigned N, int direction)
{
	char *buffer = (char *)_buffer;

	CheckObjectError(-1);
	if (direction == ZIL_IWRITE && !FlagSet(flags, UIS_READWRITE))
		return -1;
	openObject *o = CheckObject();
	if (o == ZIL_NULLP(openObject))
		return (-1);
	if (N <= 0)
		return (0);
	// Work with the first partial block.
	unsigned pos = (unsigned)(position % BYTES_PER_DATA_BLOCK);
	unsigned cpy = BYTES_PER_DATA_BLOCK - pos;
	int i = (short)(position / BYTES_PER_DATA_BLOCK);
	unsigned cnt = 0;

	// Cut back on number of bytes to read.
	if (direction == ZIL_IREAD && o->inode.size < (long)(position + N))
		N = (short)(o->inode.size - position);
	if (cpy > N)
		cpy = N;
	// Work with blocks until done.
	while (cnt < N)
	{
		// Get the data block.
		ZIL_DISK_ADDRESS addr = GetBlockPtr(i);
		if (cpy == BYTES_PER_BLOCK && !file->InMemory(addr))
		{
			if (direction == ZIL_IWRITE)
				file->WriteAt(addr * (long)BYTES_PER_BLOCK, buffer, cpy);
			else
				file->ReadAt(addr * (long)BYTES_PER_BLOCK, buffer, cpy);
		}
		else
		{
			char *bblk = (char *)file->ReadData(addr);
			if (bblk == ZIL_NULLP(char))
				return (-1);
			// Read or write it.
			if (direction == ZIL_IWRITE)
				memcpy(&bblk[pos], buffer, cpy);
			else
				memcpy(buffer, &bblk[pos], cpy);
			// Mark it as un used (and possibly dirty).
			file->WriteData(bblk, direction == ZIL_IWRITE);
		}
		buffer += cpy;
		i++;
		cnt += cpy;
		pos = 0;
		cpy = N-cnt;
		if (cpy > BYTES_PER_DATA_BLOCK)
			cpy = BYTES_PER_DATA_BLOCK;
	}
	position += cnt;
	// Mark the file and object as having been modified.
	if (direction == ZIL_IWRITE)
	{
		if (o->inode.size < position)
			o->inode.size = position;
		o->modified = 1;
		file->modified = 1;
	}
	return (cnt);
}

int ZIL_STORAGE_OBJECT_READ_ONLY::Store(const directoryEntry *)
{
	abort();
#ifndef __WATCOMC__
	return (0);
#endif
}

// ---- ZIL_STORAGE_DIRECTORY --------------------------------------------------

ZIL_STORAGE_DIRECTORY::ZIL_STORAGE_DIRECTORY(ZIL_STORAGE_OBJECT_READ_ONLY *_directory)
{
	PartialConstruct(_directory);
}

ZIL_STORAGE_DIRECTORY::~ZIL_STORAGE_DIRECTORY(void)
{
	delete directoryBuffer;
	delete directory;
}

directoryEntry *ZIL_STORAGE_DIRECTORY::ReadDir(void)
{
	int j;
	if (position >= size)
		return (ZIL_NULLP(directoryEntry));

	ZIL_UINT16 oldInum = 0;
	for (j=0; j < sizeof(oldInum); j++)
		oldInum |= (directoryBuffer[position++] << 8*j);
	if (oldInum == 0xFFFF)
	{
		current.inum = 0;
		for (j=0; j < sizeof(current.inum); j++)
			current.inum |= (directoryBuffer[position++] << 8*j);
	}
	else
		current.inum = oldInum;

	current.objectID = 0;
	for (j=0; j < sizeof(current.objectID); j++)
		current.objectID |= (directoryBuffer[position++] << 8*j);

	current.revision = 0;
	for (j=0; j < sizeof(current.revision); j++)
		current.revision |= (directoryBuffer[position++] << 8*j);

	current.country = 0;
	for (j=0; j < sizeof(current.country); j++)
		current.country |= (directoryBuffer[position++] << 8*j);

	ZIL_UINT16 stringSize = 0;
	for (j=0; j < sizeof(stringSize); j++)
		stringSize |= (directoryBuffer[position++] << 8*j);

	int doUni = (stringSize & ZIL_UNICODE_FLAG);
	stringSize &= ~ZIL_UNICODE_FLAG;
	if (doUni)
	{
		for (int i = 0; i < stringSize; i++)
		{
#if defined(ZIL_UNICODE)
			current.stringID[i] = 0;
			for (j=0; j < sizeof(current.stringID[i]); j++)
				current.stringID[i] |= ((ZIL_UINT16)directoryBuffer[position++] << 8*j);
#else
			current.stringID[i] = (ZIL_UINT8)directoryBuffer[position++];
			position++;
#endif
		}
	}
	else
	{
		for (int i = 0; i < stringSize; i++)
			current.stringID[i] = (ZIL_UINT8)directoryBuffer[position++];
	}
	current.stringID[stringSize] = 0;

	return (&current);
}

void ZIL_STORAGE_DIRECTORY::RewindDir(void)
{
	delete directoryBuffer;
	PartialConstruct(directory);
}

void ZIL_STORAGE_DIRECTORY::PartialConstruct(ZIL_STORAGE_OBJECT_READ_ONLY *_directory)
{
	directory = _directory;
	openObject *o = directory->CheckObject();
	size = 0;
	if (o == ZIL_NULLP(openObject))
		return;
	size = (ZIL_UINT16)o->inode.size;
	directoryBuffer = new ZIL_UINT8[size];
	directory->RWData(directoryBuffer, size, ZIL_IREAD);
	position = 0;
}

// --- FindFirst -----------------------------------------------------------
// This allows only one Findfirst per file.  Use OpenDir/ReadDir.

static ZIL_ICHAR ZIL_FARDATA searchInfo[ZIL_MAXNAMELEN];
static const ZIL_ICHAR *searchPat;
static ZIL_UINT16 searchID;
static long patPosition;

ZIL_ICHAR *ZIL_STORAGE_READ_ONLY::FindFirstObject(const ZIL_ICHAR *pattern)
{
	CheckStorageError(0);
	patPosition = 0;
	searchPat = pattern;
	return (FindNextObject());
}

ZIL_ICHAR *ZIL_STORAGE_READ_ONLY::FindNextObject(void)
{
	directoryEntry dentry;

	CheckStorageError(0);
	currentDirectory->position = patPosition;
	do {
		if (currentDirectory->position >= openObjects[currentDirectory->inodeIndex].inode.size)
			return ZIL_NULLP(ZIL_ICHAR);
		currentDirectory->Load(&dentry);
	} while (WildStrcmp(dentry.stringID, searchPat) != 0);
	patPosition = currentDirectory->position;
	strcpy(searchInfo, dentry.stringID);
	return (searchInfo);
}

ZIL_ICHAR *ZIL_STORAGE_READ_ONLY::FindFirstID(ZIL_UINT16 id)
{
	CheckStorageError(0);
	patPosition = 0;
	searchID = id;
	return (FindNextID());
}

ZIL_ICHAR *ZIL_STORAGE_READ_ONLY::FindNextID(void)
{
	directoryEntry dentry;

	CheckStorageError(0);
	currentDirectory->position = patPosition;
	do {
		if (currentDirectory->position >= openObjects[currentDirectory->inodeIndex].inode.size)
			return ZIL_NULLP(ZIL_ICHAR);
		currentDirectory->Load(&dentry);
	} while (dentry.objectID != searchID);
	patPosition = currentDirectory->position;
	strcpy(searchInfo, dentry.stringID);
	return (searchInfo);
}

