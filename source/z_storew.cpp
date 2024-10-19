//	Zinc Interface Library - Z_STOREW.CPP
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
#include <time.h>
#if defined(__MWERKS__) && !defined(__BCPLUSPLUS__)
#	include <unix.h>	// For O_BINARY, etc.
#else
#	include <fcntl.h>
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
#if defined(ZIL_MSDOS) || defined(ZIL_MSWINDOWS) || defined(ZIL_OS2) || defined(__DVX__)
#	include <io.h>
#	if !defined(__IBMCPP__) && !defined(__GLOCKENSPIEL__)
#		include <dos.h>
#	endif
#endif
#include "z_store.hpp"

extern struct ZINC_SIGNATURE _signature;

// --- support -------------------------------------------------------------

#if defined(TESTING)
	// Hard abort for testing errors in Storage.  This isn't used
	// in the production version of ZAF.
	static void FileAbort(int i)
	{
		printf("error abort: %d\n", i);
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
		printf("error abort: %d\n", i);
		perror("");
		abort();
	}
#else
#	define FileAbortAll(x)
#endif

// Write some data out at a certain position.  This is here mainly to link
// the seek and write together.
int ZIL_STORAGE::WriteAt(long pos, void *buf, int len)
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
	errno = FSWrite(fd, &count, (BUFFTYPE *)buf);
	if (errno != noErr)
#else
	if (write(fd, (BUFFTYPE *)buf, len) != len)
#endif
	{
		FileAbort(errno);
		return (errno);
	}
	return (0);
}

// --- ZIL_STORAGE privates -------------------------------------------------

// Return all spaced associated with an inode to the free list.
// Return TRUE on error.
int ZIL_STORAGE::DeallocateSpace(inode *ientry)
{
	int i = 0;
	for (; i < LENGTHOF(ientry->direct); i++)
	{
		FreeData(ientry->direct[i]);
		ientry->direct[i] = 0;
	}
	if (ientry->sIndirect != 0)
	{
		ZIL_DISK_ADDRESS *iblk1 = (ZIL_DISK_ADDRESS *)ReadData(ientry->sIndirect);
		if (iblk1 == ZIL_NULLP(ZIL_DISK_ADDRESS))
			return (TRUE);
		for (i=0; i < DISK_ADDR_PER_DATA_BLOCK; i++)
		{
			SwapDiskAddr(&iblk1[i]);
			FreeData(iblk1[i]);
		}
		FreeData(ientry->sIndirect);
		ientry->sIndirect = 0;
	}
	if (ientry->dIndirect != 0)
	{
		ZIL_DISK_ADDRESS *iblk1 = (ZIL_DISK_ADDRESS *)ReadData(ientry->dIndirect);
		if (iblk1 == ZIL_NULLP(ZIL_DISK_ADDRESS))
			return (TRUE);
		for (i=0; i < DISK_ADDR_PER_DATA_BLOCK; i++)
		{
			if (iblk1[i] == 0)
				continue;
			SwapDiskAddr(&iblk1[i]);
			ZIL_DISK_ADDRESS *iblk2 = (ZIL_DISK_ADDRESS *)ReadData(iblk1[i]);
			if (iblk2 == ZIL_NULLP(ZIL_DISK_ADDRESS))
				return (TRUE);
			for (int j=0; j < DISK_ADDR_PER_DATA_BLOCK; j++)
			{
				SwapDiskAddr(&iblk2[j]);
				FreeData(iblk2[j]);
			}
			FreeData(iblk1[i]);
		}
		FreeData(ientry->dIndirect);
		ientry->dIndirect = 0;
	}
	return (FALSE);
}

// Add blknum to data freelist.  The structure is a linear list of blocks
// that contain 127 pointers to other free blocks.  This allows us to free
// 128 blocks while cacheing only one block.
void ZIL_STORAGE::FreeData(ZIL_UINT16 blknum)
{
	if (blknum == 0)
		return;
	freeListBlock *flblk = (freeListBlock *)ReadData(blknum);
	if (flblk == ZIL_NULLP(freeListBlock))
		return;
	// Clear out any data in block so that we don't think the old data
	// is really pointers to free blocks.
	int i = 0;
	for (; i < DISK_ADDR_PER_DATA_BLOCK-1; i++)
		flblk->freeBlocks[i] = 0;
	flblk->next = 0;
	// Free list is empty, just point at this block.
	if (sb->freeDataList == 0)
	{
		sb->freeDataList = blknum;
	}
	else
	{
		freeListBlock *flist = (freeListBlock *)ReadData(sb->freeDataList);
		if (flist == ZIL_NULLP(freeListBlock))
			return;
		// Find a place to add this block
		for (i=0; i < DISK_ADDR_PER_DATA_BLOCK-1; i++)
			if (flist->freeBlocks[i] == 0)
				break;
		if (i >= DISK_ADDR_PER_DATA_BLOCK-1)
		{
			// Append to front of list.
			flblk->next = sb->freeDataList;
			SwapDiskAddr(&flblk->next);
			sb->freeDataList = blknum;
		}
		else
		{
			// Insert into tree.
			flist->freeBlocks[i] = blknum;
			SwapDiskAddr(&flist->freeBlocks[i]);
		}
		WriteData(flist, i < DISK_ADDR_PER_DATA_BLOCK-1);
	}
	WriteData(flblk, TRUE);
}

// Allocate a data block either from freelist or at the end of the file.
// See FreeData for other details of the data structure.
ZIL_DISK_ADDRESS ZIL_STORAGE::AllocData(void)
{
	ZIL_DISK_ADDRESS blkptr;
	if (FlagSet(flags, UIS_COPY))
	{
		static ZIL_DISK_ADDRESS nextBlock = 0;
		nextBlock++;
		return nextBlock;
	}
	if (sb->freeDataList == 0)
	{
		// extend the file by a block
#if defined(ZIL_MACINTOSH)
		long pos;
		GetEOF(fd, &pos);
		errno = SetFPos(fd, fsFromLEOF, 0L);
		if (errno != noErr)
#else
		long pos = lseek(fd, 0L, SEEK_END);
		if (pos < 0L)
#endif
		{
			FileAbort(errno);
			storageError = errno;
			return (0);
		}
		blkptr = (ZIL_UINT16)(pos / BYTES_PER_BLOCK);
		if (pos % BYTES_PER_BLOCK != 0)
			blkptr++;
		char *buff = new char[BYTES_PER_BLOCK];
		// zero the block
		memset(buff, 0, BYTES_PER_BLOCK);
#if defined(ZIL_MACINTOSH)
		long i = BYTES_PER_BLOCK;
		errno = FSWrite(fd, &i, (BUFFTYPE *)buff);
#else
		int i = write(fd, (BUFFTYPE *)buff, BYTES_PER_BLOCK);
#endif
		delete []buff;
		if (i != BYTES_PER_BLOCK)
		{
			FileAbort(errno);
			storageError = errno;
			return (0);
		}
	}
	else
	{
		freeListBlock *flblk = (freeListBlock *)ReadData(sb->freeDataList);
		if (flblk == ZIL_NULLP(freeListBlock))
			return (0);
		int i = 0;
		for (; i < DISK_ADDR_PER_DATA_BLOCK-1; i++)
			if (flblk->freeBlocks[i] != 0)
				break;
		if (i < DISK_ADDR_PER_DATA_BLOCK-1)
		{
			blkptr = flblk->freeBlocks[i];
			flblk->freeBlocks[i] = 0;
			SwapDiskAddr(&blkptr);
		}
		else
		{
			blkptr = sb->freeDataList;
			sb->freeDataList = flblk->next;
			SwapDiskAddr(&sb->freeDataList);
		}
		WriteData(flblk, i < DISK_ADDR_PER_DATA_BLOCK-1);
	}
	return (blkptr);
}

// Append another block of inodes to the file and make them free.
// Inum is the number of the last free inode.
ZIL_DISK_ADDRESS ZIL_STORAGE::AppendInode(ZIL_INODE_NUMBER inum)
{
	inode *ipage = new inode[INODES_PER_INODE_BLOCK];
	inode *tmp = ipage;
	// zero out the inodes.
	int i = 0;
	for (; i < INODES_PER_INODE_BLOCK; i++, tmp++)
	{
		tmp->size = 0;
		tmp->createTime = 0;
		tmp->modifyTime = 0;
		for (int j=0; j < LENGTHOF(tmp->direct); j++)
			tmp->direct[j] = 0;
		tmp->sIndirect = 0;
		tmp->dIndirect = 0;
		tmp->useCount = 0;
		if (inum == 0xFFFFFFFFL)
		{
			SET_FRAGMENT_WITH_INUM(tmp->fragmentIndex, tmp->fragmentBlock,
					       0);
		}
		else
		{
			SET_FRAGMENT_WITH_INUM(tmp->fragmentIndex, tmp->fragmentBlock,
					       inum + i + 1);
		}
		SwapInode(tmp);
	}
	// Add them to the end of the file.
#if defined(ZIL_MACINTOSH)
	long pos;
	GetEOF(fd, &pos);
	errno = SetFPos(fd, fsFromLEOF, 0L);
	if (errno != noErr)
#else
	long pos = lseek(fd, 0L, SEEK_END);
	if (pos < 0L)
#endif
		return (0);
	if (pos % BYTES_PER_BLOCK != 0)
	{
#if defined(ZIL_MACINTOSH)
		errno = SetFPos(fd, fsFromMark, BYTES_PER_BLOCK - (pos % BYTES_PER_BLOCK));
		if (errno == noErr)
			errno = GetFPos(fd, &pos);
		if (errno != noErr)
#else
		pos = lseek(fd, BYTES_PER_BLOCK - (pos % BYTES_PER_BLOCK), SEEK_CUR);
		if (pos < 0L)
#endif
			return (0);
	}
#if defined(ZIL_MACINTOSH)
	long count = BYTES_PER_INODE_BLOCK;
	errno = FSWrite(fd, &count, (BUFFTYPE *)ipage);
	i = (int)count;
#else
	i = write(fd, (BUFFTYPE *)ipage, BYTES_PER_INODE_BLOCK);
#endif
	delete []ipage;
	if (i != BYTES_PER_INODE_BLOCK)
		return (0);
	// Add them to the free list.
	if (inum && inum != 0xFFFFFFFFL)
		SET_FRAGMENT_WITH_INUM(sb->freeInodeListLo, sb->freeInodeListHi,
				    inum);
	return ((ZIL_UINT16)(pos / BYTES_PER_BLOCK));
}

// Allocate an Inode.  Inodes are linearly linked in the free list.
// 0 is used as the error marker, except for inode 0 which is only allocated
// once.  That place doesn't check for errors.  This is safe.
ZIL_INODE_NUMBER ZIL_STORAGE::AllocInode(void)
{

	ZIL_INODE_NUMBER i;
	SET_INUM_WITH_FRAGMENT(i, sb->freeInodeListLo, sb->freeInodeListHi);
	inode ibuf;
	if (RWInode(i, &ibuf, ZIL_IREAD) < 0)
		return (0);
	if (FlagSet(flags, UIS_COPY))
	{
		sb->freeInodeListLo++;
	}
	else
	{
		sb->freeInodeListLo = ibuf.fragmentIndex;
		sb->freeInodeListHi = ibuf.fragmentBlock;
	}
	return (i);
}

// Read or Write an inode depending on the direction.
int ZIL_STORAGE::RWInode(ZIL_INODE_NUMBER inum, inode *ientry, int direction)
{
	if (FlagSet(flags, UIS_COPY))
	{
		if (copyInodes == ZIL_NULLP(inode))
			copyInodes = new inode[INODES_PER_INODE_BLOCK];
		if (direction == ZIL_IWRITE)
			copyInodes[(int)inum] = *ientry;
		else
			*ientry = copyInodes[(int)inum];
		return 0;
	}
	long pos = FindInode(inum);
	if (direction == ZIL_IWRITE)
	{
		SwapInode(ientry);
		if ((storageError = WriteAt(pos, ientry, sizeof(*ientry))) != 0)
			return -1;
	}
	else
	{
		if ((storageError = ReadAt(pos, ientry, sizeof(*ientry))) != 0)
			return -1;
	}
	SwapInode(ientry);
	return (0);
}

// --- ZIL_STORAGE publics --------------------------------------------------
ZIL_STORAGE::ZIL_STORAGE(void) :
	ZIL_STORAGE_READ_ONLY(), copyInodes(ZIL_NULLP(inode))
{
}

ZIL_STORAGE::ZIL_STORAGE(const ZIL_ICHAR *name, UIS_FLAGS pflags) :
	ZIL_STORAGE_READ_ONLY(), copyInodes(ZIL_NULLP(inode))
{
	flags = pflags;
	StripFullPath(name, pname, fname);
	// If this is an old file, find it in the searchPath.
	if (!FlagSet(flags, UIS_CREATE))
	{
		ZIL_ICHAR tmp[ZIL_MAXPATHLEN];
		int notFound = SearchPath(tmp);
		if (FlagSet(flags, UIS_OPENCREATE))
		{
			flags |= UIS_READWRITE;
			if (notFound)
				flags |= UIS_CREATE;
		}
	}
	else
		MakeFullPath(pname);
	// open the file
	if (FlagSet(flags, UIS_COPY))
	{
		tname[0] = 0;
		// do nothing;
	}
	else if (FlagSet(flags, UIS_READWRITE))
	{
		// A temp file for read/write backup.
		strcpy(tname, pname);
		TempName(tname);
#if defined(ZIL_MACINTOSH)
		// Use file creator and type for the designer.
		OSType tempFileCreator = ZIL_INTERNATIONAL::fileCreator;
		OSType tempFileType = ZIL_INTERNATIONAL::fileType;
		ZIL_INTERNATIONAL::fileCreator = postFileCreator;
		ZIL_INTERNATIONAL::fileType = postFileType;
#endif

		fd = open(tname, O_BINARY|O_CREAT|O_TRUNC|O_RDWR, S_IREAD|S_IWRITE);

#if defined(ZIL_MACINTOSH)
		ZIL_INTERNATIONAL::fileCreator = tempFileCreator;
		ZIL_INTERNATIONAL::fileType = tempFileType;
#endif
	}
	else
	{
		// The real file for read-only.
		ZIL_ICHAR tmp[ZIL_MAXPATHLEN];
		tname[0] = '\0';
		AppendFullPath(tmp, pname, fname);
		fd = open(tmp, O_BINARY|O_RDONLY);
	}
	if (fd < 0 && !FlagSet(flags, UIS_COPY))
	{
		FileAbortAll(errno);
		storageError = errno;
		return;
	}
	sb = new superBlock;
	if (FlagSet(flags, UIS_CREATE))
	{
		// new file, write a super block
		memset(sb, 0, sizeof(*sb));
		sb->signature = _signature;
		TimeStamp(&sb->createTime);
		sb->revision = 0;
		sb->unused = 0;
		if (!FlagSet(flags, UIS_COPY) && !FlagSet(flags, UIS_TEMPORARY) && Flush() < 0)
			return;
	}
	else if (FlagSet(flags, UIS_READWRITE))
	{
		// old file, copy the file across to the working file
		ZIL_ICHAR tmp[ZIL_MAXPATHLEN];
		AppendFullPath(tmp, pname, fname);
		int fd1 = open(tmp, O_BINARY|O_RDONLY);
		char *buff = new char[COPY_BYTES];
#if defined(ZIL_MACINTOSH)
		SetFPos(fd, fsFromStart, 0L);
		long count = COPY_BYTES;
		while ((errno = FSRead(fd1, &count, (BUFFTYPE *)buff)) == noErr ||
			(errno == eofErr && count > 0L))
		{
			FSWrite(fd, &count, (BUFFTYPE *)buff);
			count = COPY_BYTES;
		}
		delete []buff;
		FSClose(fd1);
#else
		lseek(fd, 0L, SEEK_SET);
		int i;
		while ((i = read(fd1, (BUFFTYPE *)buff, COPY_BYTES)) > 0)
			write(fd, (BUFFTYPE *)buff, i);
		delete []buff;
		close(fd1);
#endif
	}
	if (!FlagSet(flags, UIS_COPY) && !FlagSet(flags, UIS_TEMPORARY))
	{
		storageError = ReadAt(0L, sb, sizeof(*sb));
		SwapSuperBlock(sb);
	}
	// Validate that this is a Zinc file.
	ValidateZincFile();
	CheckStorageErrorNull();
	SetUpLocalData();
	CheckStorageErrorNull();
	if (FlagSet(flags, UIS_CREATE))
	{
		// Create a root directory...
		if (MkDir(_rootStorageDirectory) < 0)
			return;
	}
	else
	{
		ChDirRoot();
		CheckStorageErrorNull();
	}
	storageError = 0;
}

ZIL_STORAGE::~ZIL_STORAGE(void)
{
	// If the file has been really opened.
	if (copyInodes)
		delete []copyInodes;
	if (fd >= 0)
	{
		Flush();
#if defined(ZIL_MACINTOSH)
		FSClose(fd);
#else
		close(fd);
#endif
		fd = -1;
	}
	if (tname[0])
		unlink(tname);
}

int ZIL_STORAGE::Save(int revisions)
{
	ZIL_ICHAR tmp[ZIL_MAXPATHLEN];

	CheckStorageError(-1);
	if (FlagSet(flags, UIS_TEMPORARY))
		return (0);
	AppendFullPath(tmp, pname, fname);
	if (!FlagSet(flags, UIS_READWRITE))
	{
#if defined(ZIL_MACINTOSH)
		storageError = permErr;
#else
		storageError = EACCES;
#endif
		return (-1);
	}
	if (modified)
		TimeStamp(&sb->modifyTime);
	if (Flush() < 0)
		return (-1);
	// Only create the backup on the first time we save the file.
	if (firstTime)
	{
		ZIL_ICHAR tmp1[ZIL_MAXPATHLEN], tmp2[ZIL_MAXPATHLEN];

		AppendFullPath(tmp1, pname, fname, _zilBak);
		strcpy(tmp2, tmp1);
		tmp1[strlen(tmp1)-1] = revisions + '0';
		unlink(tmp1);
		for (int j=revisions-1; j > 0; j--)
		{
			tmp2[strlen(tmp2)-1] = j + '0';
			rename(tmp2, tmp1);
			tmp1[strlen(tmp1)-1] = j + '0';
		}
		if (revisions)
			rename(tmp, tmp1);
	}
#if defined(ZIL_POSIX)
	int fd1 = open(tmp, O_BINARY|O_CREAT|O_TRUNC|O_RDWR, 0664);
#else
#	if defined(ZIL_MACINTOSH)
	// Use file creator and type for the designer.
	OSType tempFileCreator = ZIL_INTERNATIONAL::fileCreator;
	OSType tempFileType = ZIL_INTERNATIONAL::fileType;
	ZIL_INTERNATIONAL::fileCreator = postFileCreator;
	ZIL_INTERNATIONAL::fileType = postFileType;
#	endif

	int fd1 = open(tmp, O_BINARY|O_CREAT|O_TRUNC|O_RDWR, S_IREAD|S_IWRITE);

#	if defined(ZIL_MACINTOSH)
	ZIL_INTERNATIONAL::fileCreator = tempFileCreator;
	ZIL_INTERNATIONAL::fileType = tempFileType;
#	endif
#endif
	char *buff = new char[COPY_BYTES];

#if defined(ZIL_MACINTOSH)
	SetFPos(fd, fsFromStart, 0L);
	long count = COPY_BYTES;
	while ((errno = FSRead(fd, &count, (BUFFTYPE *)buff)) == noErr ||
		(errno == eofErr && count > 0L))
	{
		OSErr wError = FSWrite(fd1, &count, (BUFFTYPE *)buff);
		if (wError != noErr)
		{
			delete buff;
			return (-1);
		}
		count = COPY_BYTES;
	}
	delete []buff;
	if (errno != noErr && errno != eofErr)
		return (-1);
	FSClose(fd1);
#else
	lseek(fd, 0L, SEEK_SET);
	int i;
	while ((i = read(fd, (BUFFTYPE *)buff, COPY_BYTES)) > 0)
		if (write(fd1, (BUFFTYPE *)buff, i) != i)
		{
			delete buff;
			return (-1);
		}
	delete []buff;
	if (i < 0)
		return (-1);
	close(fd1);
#endif
	firstTime = 0;
	return (0);
}

int ZIL_STORAGE::SaveAs(const ZIL_ICHAR *newName, int revisions)
{
	CheckStorageError(-1);
	if (!FlagSet(flags, UIS_READWRITE))
	{
#if defined(ZIL_MACINTOSH)
		storageError = permErr;
#else
		storageError = EACCES;
#endif
		return (-1);
	}
	if (modified)
		TimeStamp(&sb->modifyTime);
	if (Flush() < 0)
		return (-1);
	StripFullPath(newName, pname, fname);
	firstTime = 1;
	return (Save(revisions));
}

int ZIL_STORAGE::Flush(void)
{
	CheckStorageError(-1);
	if (!FlagSet(flags, UIS_READWRITE) || FlagSet(flags, UIS_TEMPORARY))
	{
#if defined(ZIL_MACINTOSH)
		storageError = permErr;
#else
		storageError = EACCES;
#endif
		return -1;
	}
	// Write out all dirty data blocks.
	int i = 0;
	for (; i < cacheLen; i++)
		if (cd[i].dirty)
		{
			long pos = (long)cd[i].blknum * BYTES_PER_BLOCK;
			if ((storageError = WriteAt(pos, &cache[cd[i].pos * BYTES_PER_BLOCK], BYTES_PER_BLOCK)) != 0)
				return (-1);
			cd[i].dirty = 0;
		}
	// Write out the possibly changed super block.
	SwapSuperBlock(sb);
	if ((storageError = WriteAt(0L, sb, sizeof(*sb))) != 0)
		return (-1);
	SwapSuperBlock(sb);
	// Write out all open inodes.
	for (i=0; i < openLen; i++)
		if (openObjects[i].openCount > 0)
			if (RWInode(openObjects[i].inum, &openObjects[i].inode, ZIL_IWRITE) < 0)
				return (-1);
	// ?? If necessary on DOS, flush the buffers
	return (0);
}

int ZIL_STORAGE::RenameObject(const ZIL_ICHAR *oldObject, const ZIL_ICHAR *newName)
{
	CheckStorageError(-1);
	if (currentDirectory->FindName(newName))
	{
		// If the newname does exist.
#if defined(ZIL_MACINTOSH)
		storageError = permErr;
#else
		storageError = EACCES;
#endif
		return (-1);
	}
	if (!currentDirectory->FindName(oldObject))
	{
		// If the oldname doesn't exist.
#if defined(ZIL_MACINTOSH)
		storageError = fnfErr;
#else
		storageError = ENOENT;
#endif
		return (-1);
	}
	long delentry = currentDirectory->position;

	directoryEntry oldentry;
	currentDirectory->Load(&oldentry);
	// if it is already open, stop.  We don't allow renaming open objects
	// (would it hurt anything if we did?).
	if (CheckOpen(oldentry.inum) >= 0)
		return (-1);

	// Delete the old directory entry.
	storageError = 0;
	openObject *o = &openObjects[currentDirectory->inodeIndex];
	long pos = currentDirectory->position;
	while (pos < o->inode.size)
	{
		char buff[256];
		currentDirectory->position = pos;
		int size = currentDirectory->ZIL_STORAGE_OBJECT_READ_ONLY::Load(buff, sizeof(buff[0]), sizeof(buff));
		pos = currentDirectory->position;

		currentDirectory->position = delentry;
		// This is really an ZIL_STORAGE_OBJECT, but looks like an
		// ZIL_STORAGE_OBJECT_READ_ONLY.  The virtual store would
		// normally handle this, but if you passed in an
		// ZIL_DELTA_STORAGE_OBJECT we would get the wrong Store().
		((ZIL_STORAGE_OBJECT *)currentDirectory)->ZIL_STORAGE_OBJECT::Store(buff, sizeof(buff[0]), size);
		delentry = currentDirectory->position;
	}
	// Append the new directory entry.
	strcpy(oldentry.stringID, newName);
	currentDirectory->Store(&oldentry);
	o->inode.size = currentDirectory->position;
	return (0);
}

int ZIL_STORAGE::DestroyObject(const ZIL_ICHAR *name)
{
#define DOEXIT(x)	if (parentdir != currentDirectory)	\
				delete parentdir;		\
			return (x)

	CheckStorageError(-1);
	// parentdir really comes back from WalkPartialPath() as
	// a ZIL_STORAGE_OBJECT!!!
	ZIL_STORAGE_OBJECT_READ_ONLY *parentdir;
	const ZIL_ICHAR *fname;
	WalkPartialPath(name, &parentdir, &fname);
	if (!parentdir->FindName(fname))
	{
		// if the object doesn't exist
#if defined(ZIL_MACINTOSH)
		FileAbortAll(fnfErr);
		storageError = fnfErr;
#else
		FileAbortAll(ENOENT);
		storageError = ENOENT;
#endif
		DOEXIT(-1);
	}
	long delentry = parentdir->position;
	directoryEntry dentry;
	parentdir->Load(&dentry);
	// If the object is already open, stop.  Unix semantics allows this
	// to happen and does the real delete when the object is closed.
	if (CheckOpen(dentry.inum) >= 0)
	{
		FileAbortAll(storageError);
		DOEXIT(-1);
	}
	inode ientry;
	if (RWInode(dentry.inum, &ientry, ZIL_IREAD) < 0)
	{
		FileAbortAll(storageError);
		DOEXIT(-1);
	}
	// If the object is a directory, stop.  We need to do more work.
	// See RmDir().
	if (ientry.useCount & ZIL_DIRECTORY_TAG)
	{
#if defined(ZIL_MACINTOSH)
		FileAbortAll(permErr);
		storageError = permErr;
#else
		FileAbortAll(EACCES);
		storageError = EACCES;
#endif
		DOEXIT(-1);
	}
	// if this isn't really a file return error.
	if (ientry.useCount <= 0)
	{
#if defined(ZIL_MACINTOSH)
		FileAbortAll(rfNumErr);
		storageError = rfNumErr;
#else
		FileAbortAll(EINVAL);
		storageError = EINVAL;
#endif
		DOEXIT(-1);
	}

	// Delete the entry out of the directory
	openObject *o = &openObjects[parentdir->inodeIndex];
	long pos = parentdir->position;

	while (pos < o->inode.size)
	{
		char buff[256];
		parentdir->position = pos;
		int size = parentdir->ZIL_STORAGE_OBJECT_READ_ONLY::Load(buff, sizeof(buff[0]), sizeof(buff));
		pos = parentdir->position;

		parentdir->position = delentry;
		// This is really an ZIL_STORAGE_OBJECT, but looks like an
		// ZIL_STORAGE_OBJECT_READ_ONLY.  The virtual store would
		// normally handle this, but if you passed in an
		// ZIL_DELTA_STORAGE_OBJECT we would get the wrong Store().
		((ZIL_STORAGE_OBJECT *)parentdir)->ZIL_STORAGE_OBJECT::Store(buff, sizeof(buff[0]), size);
		delentry = parentdir->position;
	}
	o->inode.size = parentdir->position = delentry;

	// If the inode is still being used somewhere else, just exit.
	ientry.useCount--;
	if (ientry.useCount > 0)
	{
		DOEXIT(RWInode(dentry.inum, &ientry, ZIL_IWRITE));
	}
	ientry.useCount = 0;

	// Deallocate the data space.
	if (DeallocateSpace(&ientry))
	{
		FileAbortAll(storageError);
		DOEXIT(-1);
	}

	// Free the inode.
	ientry.fragmentIndex = sb->freeInodeListLo;
	ientry.fragmentBlock = sb->freeInodeListHi;
	SET_FRAGMENT_WITH_INUM(sb->freeInodeListLo, sb->freeInodeListHi,
			    dentry.inum);
	if (RWInode(dentry.inum, &ientry, ZIL_IWRITE) < 0)
	{
		FileAbortAll(storageError);
		DOEXIT(-1);
	}
	DOEXIT(0);
#undef DOEXIT
}

int ZIL_STORAGE::MkDir(const ZIL_ICHAR *name)
{
#define DOEXIT(x)	if (parentdir != currentDirectory)	\
				delete parentdir;		\
			return (x)

	CheckStorageError(-1);
	if (!FlagSet(flags, UIS_READWRITE))
		return (-1);

	directoryEntry dentry;
	const ZIL_ICHAR *fname;
#if defined(TESTING_ALL)
	memset(&dentry, 0, sizeof(dentry));
#endif
	ZIL_STORAGE_OBJECT_READ_ONLY *parentdir =
		ZIL_NULLP(ZIL_STORAGE_OBJECT_READ_ONLY);
	// this is ROOT, handle it special
	if (streq(name, _rootStorageDirectory) == 0)
	{
		dentry.inum = AllocInode();	// this will be a 0!!
		fname = name;
	}
	else
	{
		// Parentdir will really come back as a ZIL_STORAGE_OBJECT.
		WalkPartialPath(name, &parentdir, &fname);
		// if it does exist
		if (parentdir->FindName(fname))
		{
			FileAbortAll(EEXIST);
			DOEXIT(-1);
		}
		// allocate an inode
		if ((dentry.inum = AllocInode()) == 0)
		{
			FileAbort(storageError);
			DOEXIT(-1);
		}
	}
	// Really create the directory object
	dentry.objectID = 0;
	dentry.revision = 0;
	dentry.country = 0;

	inode ientry;
	ientry.useCount = 1;
	ientry.size = 0;
	int i = 0;
	for (; i < LENGTHOF(ientry.direct); i++)
		ientry.direct[i] = 0;
	ientry.sIndirect = 0;
	ientry.dIndirect = 0;
	ientry.fragmentBlock = 0;
	ientry.fragmentIndex = 0;
	TimeStamp(&ientry.createTime);
	modified = 0;
	sb->modifyTime = ientry.modifyTime = ientry.createTime;

	if ((i = FindSlot()) <  0)
	{
		DOEXIT(-1);
	}
	openObject *o = &openObjects[i];
	o->inum = dentry.inum;
	o->openCount = 1;
	o->inode = ientry;
	o->modified = 0;

	ZIL_STORAGE_OBJECT_READ_ONLY *dir = new ZIL_STORAGE_OBJECT();
	dir->inodeIndex = i;
	dir->file = this;
	dir->position = 0;
	dir->flags = UIS_READWRITE;
	if (currentDirectory == ZIL_NULLP(ZIL_STORAGE_OBJECT))
		currentDirectory = dir;

	// Write the "." (myself) entry
	strcpy(dentry.stringID, _currentDirectory);
	dir->Store(&dentry);
	// If this isn't the root directory
	if (dentry.inum != 0)
	{
		openObject *p = &openObjects[parentdir->inodeIndex];
		// Write my name in my parent
		strcpy(dentry.stringID, fname);
		// write name to parent directory
		parentdir->position = p->inode.size;
		parentdir->Store(&dentry);
		// mark a pointer to ..
		dentry.inum = p->inum;
		p->inode.useCount++;
	}
	o->inode.useCount++;
	// Point me at my parent
	strcpy(dentry.stringID, _parentDirectory);
	dir->Store(&dentry);
	o->inode.useCount |= ZIL_DIRECTORY_TAG;
	if (o->inum != 0)
		delete dir;
	DOEXIT(0);
#undef DOEXIT
}

int ZIL_STORAGE::RmDir(const ZIL_ICHAR *name)
{
	CheckStorageError(-1);

	// Parentdir will really come back as a ZIL_STORAGE_OBJECT.
	ZIL_STORAGE_OBJECT_READ_ONLY *parentdir;
	const ZIL_ICHAR *fname;
	WalkPartialPath(name, &parentdir, &fname);
	if (!parentdir->FindName(fname))
	{
		// if it doesn't exist
#if defined(ZIL_MACINTOSH)
		FileAbortAll(fnfErr);
#else
		FileAbortAll(ENOENT);
#endif
		return (-1);
	}

	directoryEntry dentry;
	parentdir->Load(&dentry);
	if (CheckOpen(dentry.inum) >= 0)
		return (-1);

	inode ientry;
	if (RWInode(dentry.inum, &ientry, ZIL_IREAD) < 0)
		return (-1);

	// Make sure this is really a directory and it is only used 2 times
	// i.e. empty (except for "." and "..")
	if (ientry.useCount != (ZIL_DIRECTORY_TAG|2))
	{
#if defined(ZIL_MACINTOSH)
		FileAbortAll(permErr);
#else
		FileAbortAll(EACCES);
#endif
		return (-1);
	}
	// and not currentDirectory
	if (dentry.inum == openObjects[currentDirectory->inodeIndex].inum)
	{
#if defined(ZIL_MACINTOSH)
		FileAbortAll(permErr);
#else
		FileAbortAll(EACCES);
#endif
		return (-1);
	}
	// deallocate it
	ientry.useCount &= ~ZIL_DIRECTORY_TAG;
	ientry.useCount--;

	// ????? what if useCount doesn't go to 0
	if (RWInode(dentry.inum, &ientry, ZIL_IWRITE) < 0)
		return -1;
	// Make sure we update the parent too!!!
	openObject *o = &openObjects[parentdir->inodeIndex];
	o->inode.useCount--;
	if (RWInode(o->inum, &o->inode, ZIL_IWRITE) < 0)
		return -1;
	if (parentdir != currentDirectory)
		delete parentdir;
	return (DestroyObject(name));
}

// --- ZIL_STORAGE_OBJECT privates ------------------------------------------

// --- ZIL_STORAGE_OBJECT publics -------------------------------------------

ZIL_STORAGE_OBJECT::ZIL_STORAGE_OBJECT(void) :
	ZIL_STORAGE_OBJECT_READ_ONLY()
{
}

ZIL_STORAGE_OBJECT::ZIL_STORAGE_OBJECT(ZIL_STORAGE &pfile, const ZIL_ICHAR *name,
				     ZIL_OBJECTID nobjectID, UIS_FLAGS pflags) :
	ZIL_STORAGE_OBJECT_READ_ONLY()
{
	flags = pflags;
#define DOEXIT	if (parentdir != pfile.currentDirectory)	\
			delete parentdir;			\
		return

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
	if (!FlagSet(pfile.flags, UIS_READWRITE))
	{
		flags |= UIS_READ;
		flags &= ~UIS_READWRITE;
	}

	// Parentdir will really come back as a ZIL_STORAGE_OBJECT.
	ZIL_STORAGE_OBJECT_READ_ONLY *parentdir;
	const ZIL_ICHAR *fname;
	pfile.WalkPartialPath(name, &parentdir, &fname);
	int found = parentdir->FindName(fname);
	if (FlagSet(flags, UIS_CREATE))
	{
		if (found)
			((ZIL_STORAGE *)parentdir->file)->DestroyObject(fname);
		found = FALSE;
	}

	directoryEntry dentry;
	inode ientry;
	if (!found)
	{
		// If the object doesn't exist
		if (!FlagSet(flags, UIS_CREATE) &&
		    !FlagSet(flags, UIS_OPENCREATE))
		{
#if defined(ZIL_MACINTOSH)
			FileAbortAll(fnfErr);
			objectError = fnfErr;
#else
			FileAbortAll(ENOENT);
			objectError = ENOENT;
#endif
			DOEXIT;
		}
		// We ARE going to be able to write this object!!!
		flags |= UIS_READWRITE;
#if defined(TESTING_ALL)
		memset(&dentry, 0, sizeof(dentry));
#endif
		if ((dentry.inum = pfile.AllocInode()) == 0)
		{
			FileAbort(pfile.storageError);
			objectError = pfile.storageError;
			pfile.storageError = 0;
			DOEXIT;
		}
		objectID = dentry.objectID = nobjectID;
		stringID = strdup(fname);
		strcpy(dentry.stringID, fname);
		dentry.revision = 0;
		dentry.country = 0;
		parentdir->Store(&dentry);

		ientry.useCount = 1;
		ientry.size = 0;
		for (int i=0; i < LENGTHOF(ientry.direct); i++)
			ientry.direct[i] = 0;
		ientry.sIndirect = 0;
		ientry.dIndirect = 0;
		ientry.fragmentBlock = 0;
		ientry.fragmentIndex = 0;
	}
	else	// FindName()
	{
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
		}
	}
	if (FlagSet(flags, UIS_CREATE))
	{
		flags |= UIS_READWRITE;
		TimeStamp(&ientry.createTime);
		pfile.modified = 0;
		pfile.sb->modifyTime = ientry.modifyTime =
			ientry.createTime;
	}
	OpenTheObject(pfile, &dentry, &ientry, FlagSet(flags, UIS_CREATE));
	DOEXIT;
#undef DOEXIT
}

ZIL_STORAGE_OBJECT::~ZIL_STORAGE_OBJECT(void)
{
	if (file && FlagSet(file->flags, UIS_COPY))
		return;
	openObject *o = CheckObject();
	if (o != ZIL_NULLP(openObject) && o->openCount <= 1 && FlagSet(flags, UIS_READWRITE))
	{
		if (o->modified)
			TimeStamp(&o->inode.modifyTime);
		if (file->RWInode(o->inum, &o->inode, ZIL_IWRITE) < 0)
		{
 			objectError = file->storageError;
			file->storageError = 0;
		}
	}
}

void ZIL_STORAGE_OBJECT::SetCTime(ZIL_INT32 val)
{
	CheckObjectErrorNull();
	openObject *o = CheckObject();
	if (o == ZIL_NULLP(openObject))
		return;
	TimeStamp(&file->sb->modifyTime);
	o->inode.createTime = val;
	o->modified = file->modified = 0;
}

void ZIL_STORAGE_OBJECT::SetMTime(ZIL_INT32 val)
{
	CheckObjectErrorNull();
	openObject *o = CheckObject();
	if (o == ZIL_NULLP(openObject))
		return;
	TimeStamp(&file->sb->modifyTime);
	o->inode.modifyTime = val;
	o->modified = file->modified = 0;
}

void ZIL_STORAGE_OBJECT::Touch(void)
{
	CheckObjectErrorNull();
	openObject *o = CheckObject();
	if (o == ZIL_NULLP(openObject))
		return;
	TimeStamp(&o->inode.modifyTime);
	file->sb->modifyTime = o->inode.modifyTime;
	o->modified = file->modified = 0;
}

// --- Stores -----------------------------------------------------------------

#if defined(ZIL_BIGENDIAN)

int ZIL_STORAGE_OBJECT::Store(ZIL_INT16 value)
{
	ZIL_UINT8 buff[sizeof(value)];

	for (int j=0; j < sizeof(value); j++)
		buff[j] = (ZIL_UINT8)((value >> 8*j) & 0xff);
	return (RWData(buff, sizeof(value), ZIL_IWRITE));
}

int ZIL_STORAGE_OBJECT::Store(ZIL_UINT16 value)
{
	ZIL_UINT8 buff[sizeof(value)];

	for (int j=0; j < sizeof(value); j++)
		buff[j] = (ZIL_UINT8)((value >> 8*j) & 0xff);
	return (RWData(buff, sizeof(value), ZIL_IWRITE));
}

int ZIL_STORAGE_OBJECT::Store(ZIL_INT32 value)
{
	ZIL_UINT8 buff[sizeof(value)];

	for (int j=0; j < sizeof(value); j++)
		buff[j] = (ZIL_UINT8)((value >> 8*j) & 0xff);
	return (RWData(buff, sizeof(value), ZIL_IWRITE));
}

int ZIL_STORAGE_OBJECT::Store(ZIL_UINT32 value)
{
	ZIL_UINT8 buff[sizeof(value)];

	for (int j=0; j < sizeof(value); j++)
		buff[j] = (ZIL_UINT8)((value >> 8*j) & 0xff);
	return (RWData(buff, sizeof(value), ZIL_IWRITE));
}

#endif

// Utility routine for Storage to use.
int ZIL_STORAGE_OBJECT::Store(const directoryEntry *dirent)
{
	int i, j;

	CheckObjectError(-1);
	if (dirent->inum >= 0xFFFF)
	{
		if ((i = ZIL_STORAGE_OBJECT::Store((ZIL_UINT16)0xFFFF)) < 0)
			return -1;
		if ((j = ZIL_STORAGE_OBJECT::Store(dirent->inum)) < 0)
			return -1;
		i += j;
	}
	else
	{
		if ((i = ZIL_STORAGE_OBJECT::Store((ZIL_UINT16)dirent->inum)) < 0)
			return -1;
	}
	if ((j = ZIL_STORAGE_OBJECT::Store(dirent->objectID)) < 0)
		return -1;
	i += j;
	if ((j = ZIL_STORAGE_OBJECT::Store(dirent->revision)) < 0)
		return -1;
	i += j;
	if ((j = ZIL_STORAGE_OBJECT::Store(dirent->country)) < 0)
		return -1;
	i += j;
	if ((j = ZIL_STORAGE_OBJECT::Store(dirent->stringID)) < 0)
		return -1;
	return i + j;
}

int ZIL_STORAGE::Link(const ZIL_ICHAR *path1, const ZIL_ICHAR *path2)
{
	CheckStorageError(-1);
	// Dir will really come back as a ZIL_STORAGE_OBJECT.
	ZIL_STORAGE_OBJECT_READ_ONLY *dir;
	const ZIL_ICHAR *fname1;
	WalkPartialPath(path1, &dir, &fname1);
	if (dir == ZIL_NULLP(ZIL_STORAGE_OBJECT))
		return (-1);
	if (!dir->FindName(fname1))
	{
		// if it doesn't exist
		if (dir != currentDirectory)
			delete dir;
#if defined(ZIL_MACINTOSH)
		storageError = fnfErr;
#else
		storageError = ENOENT;
#endif
		return (-1);
	}
	// get the inode of fname1;
	directoryEntry dentry;
	dir->Load(&dentry);
	inode ientry;
	RWInode(dentry.inum, &ientry, ZIL_IREAD);
	if (dir != currentDirectory)
		delete dir;
	// make sure it isn't a directory;
	if ((ientry.useCount & ZIL_DIRECTORY_TAG) != 0)
	{
#if defined(ZIL_MACINTOSH)
		storageError = permErr;
#else
		storageError = EACCES;
#endif
		return (-1);
	}
	const ZIL_ICHAR *fname2;
	WalkPartialPath(path2, &dir, &fname2);
	if (dir == ZIL_NULLP(ZIL_STORAGE_OBJECT))
		return (-1);
	if (dir->FindName(fname2))
	{
		if (dir != currentDirectory)
			delete dir;
		// if it does exist
#if defined(ZIL_MACINTOSH)
		storageError = permErr;
#else
		storageError = EACCES;
#endif
		return (-1);
	}
	// create the entry fname2 in dir;
	strcpy(dentry.stringID, fname2);
	dir->Store(&dentry);
	if (dir != currentDirectory)
		delete dir;
	// update the inode to an extra entry
	ientry.useCount++;
	return (RWInode(dentry.inum, &ientry, ZIL_IWRITE));
}

int ZIL_STORAGE_OBJECT::Store(const ZIL_ICHAR *string)
{
	CheckObjectError(-1);
	ZIL_UINT16 size = (string == ZIL_NULLP(ZIL_ICHAR) ? 0 : strlen(string));
	// See Unicode comments in previous functions.
#if defined(ZIL_UNICODE)
	int cnt1 = ZIL_STORAGE_OBJECT::Store((ZIL_UINT16)(size | ZIL_UNICODE_FLAG));
	if (cnt1 != sizeof(size))
		return cnt1;
	int cnt2 = 0;
	// Must (possibly) byte swap the file
	for (int i = 0; i < size; i++)
	{	
		ZIL_UINT16 c = string[i];
		cnt2 += ZIL_STORAGE_OBJECT::Store(c);
	}
#else
	int cnt1 = ZIL_STORAGE_OBJECT::Store(size);
	if (cnt1 != sizeof(size))
		return cnt1;
	int cnt2 = 0;
	cnt2 += RWData((void *)string, size, ZIL_IWRITE);
#endif
	return (cnt2 != (size * sizeof(string[0])) ? cnt2 : cnt1 + cnt2);
}

ZIL_STORAGE_OBJECT_READ_ONLY *ZIL_STORAGE::CreateObject(void)
{
	return new ZIL_STORAGE_OBJECT();
}

