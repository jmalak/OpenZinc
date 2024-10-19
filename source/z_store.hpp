//	Zinc Interface Library - Z_STORE.HPP
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


#define DOCACHEPTR	20

extern ZIL_ICHAR *_zilBak;
extern ZIL_ICHAR *_parentDirectory;
extern ZIL_ICHAR *_currentDirectory;
extern ZIL_ICHAR *_rootStorageDirectory;

const ZIL_UINT16 MAGIC_NUMBER = 0x05AF;
const int NEW_INC = 5;

#if defined(ZIL_UNICODE)
	const ZIL_UINT8 MAJOR_VERSION = 4;
	const ZIL_UINT8 MINOR_VERSION = 5;
#else
	const ZIL_UINT8 MAJOR_VERSION = 4;
	const ZIL_UINT8 MINOR_VERSION = 0;
#endif

#define LENGTHOF(x)	(sizeof(x)/sizeof((x)[0]))
#define SET_FRAGMENT_WITH_INUM(index, block, inum) \
		(index) = (ZIL_UINT16)((inum) & 0xFFFF), \
		(block) = (ZIL_UINT16)(((inum) >> 16) & 0xFFFF);
#define SET_INUM_WITH_FRAGMENT(inum, index, block) \
		(inum) = ((ZIL_INODE_NUMBER)(block) << 16) | (index);

struct ZINC_SIGNATURE
{
	char copyrightNotice[64];
  	ZIL_UINT8 majorVersion;
	ZIL_UINT8 minorVersion;
	ZIL_UINT16 magicNumber;
};

struct inode
{
	// Note: fragments are currently unsupported
	ZIL_INT32 size;		// Size of the object
	ZIL_UINT32 createTime;	// Create time of the object
	ZIL_UINT32 modifyTime;	// Last modify time of the object
	ZIL_DISK_ADDRESS direct[21];	// pointers to the first 21 blocks of object
	ZIL_DISK_ADDRESS sIndirect;	// pointer to a block of 128? pointers to blocks of object
	ZIL_DISK_ADDRESS dIndirect;	// pointer to a block of 128? pointers to blocks of 128? pointers to blocks of object
	ZIL_DISK_ADDRESS fragmentBlock;	// this is used as a free list pointer
	ZIL_UINT16 useCount;	// if this is 0 then this inode is unused
	ZIL_UINT16 fragmentIndex;	// this is also used as the free list
};

const int BYTES_PER_DATA_BLOCK = 256;
const int DISK_ADDR_PER_DATA_BLOCK = (BYTES_PER_DATA_BLOCK / sizeof(ZIL_DISK_ADDRESS));
const int BYTES_PER_INODE_BLOCK = 1024;
const int DISK_ADDR_PER_INODE_BLOCK = (BYTES_PER_INODE_BLOCK / sizeof(ZIL_DISK_ADDRESS));
const int INODES_PER_INODE_BLOCK = (BYTES_PER_INODE_BLOCK / sizeof(struct inode));
const int BYTES_PER_BLOCK = (BYTES_PER_DATA_BLOCK);
const int COPY_BYTES = 4096;
const int INODE_INDIRECT_SIZE = (DISK_ADDR_PER_INODE_BLOCK*sizeof(ZIL_DISK_ADDRESS));

struct superBlock
{
	struct ZINC_SIGNATURE signature;
	ZIL_UINT32 createTime;			// Create time for file
	ZIL_UINT32 modifyTime;			// Last modify time for file
	ZIL_UINT32 unused;
	ZIL_UINT16 freeInodeListHi;			// Upper half of free inode list
	ZIL_UINT16 revision;
	ZIL_UINT16 freeInodeListLo;			// Lower half of free inode list
	ZIL_DISK_ADDRESS freeDataList;
	ZIL_DISK_ADDRESS inodeDirect[82];		// 82 pointers to inode blocks
	ZIL_DISK_ADDRESS inodeSIndirect;		// pointer to block of 128? pointers to inode blocks
	ZIL_DISK_ADDRESS inodeDIndirect;		// pointer to block of 128? pointers to blocks of 128? pointers to inode blocks
};

struct freeListBlock
{
	ZIL_DISK_ADDRESS freeBlocks[DISK_ADDR_PER_DATA_BLOCK-1];	// Address of free data blocks
	ZIL_DISK_ADDRESS next;				// the next one of these blocks
};

struct openObject
{
	int openCount;		// Number of times the file is open
	int modified;		// if the modifyTime needs updating
	ZIL_INODE_NUMBER inum;		// Inode number
	struct inode inode;	// Inode of this file
	ZIL_UINT16 revision;
	ZIL_UINT16 country;
};

struct cacheData
{
	ZIL_UINT16 pos;			// position of data buffer
	ZIL_DISK_ADDRESS blknum;		// absolute block number on disk
	ZIL_UINT8 dirty;			// 1 if block needs to be written
	ZIL_UINT8 used;			// 1 if this block is being used
};
	
#if defined(ZIL_BIGENDIAN)
	extern void SwapDiskAddr(ZIL_DISK_ADDRESS *datum);
	extern void SwapUZIL_INT16(ZIL_UINT16 *datum);
	extern void SwapInode(inode *di);
	extern void SwapSuperBlock(superBlock *ds);
#else
#	define SwapDiskAddr(x)
#	define SwapUZIL_INT16(x)
#	define SwapInode(x)
#	define SwapSuperBlock(x)
#endif

#if defined(ZIL_OLDIO)
#	define	BUFFTYPE	char
#	define	NAMETYPE	(char *)
#else
#	define	BUFFTYPE	void
#	define	NAMETYPE
#endif

#if defined(TESTING)
#	define CheckStorageError(x)	if (storageError != 0) FileAbort(-1);
#	define CheckStorageErrorNull()	if (storageError != 0) FileAbort(-1);
#	define CheckObjectError(x)	if (objectError != 0) FileAbort(-1);
#	define CheckObjectErrorNull()	if (objectError != 0) FileAbort(-1);
#else
#	define CheckStorageError(x)	if (storageError != 0) return x;
#	define CheckStorageErrorNull()	if (storageError != 0) return;
#	define CheckObjectError(x)	if (objectError != 0) return x;
#	define CheckObjectErrorNull()	if (objectError != 0) return;
#endif
