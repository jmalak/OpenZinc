//	Zinc Interface Library - DIRECT.HPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#if !defined(ZAF_DIRECT_HPP)
#define ZAF_DIRECT_HPP
#ifndef ZAF_SERVICE_HPP
#	include <service.hpp>
#endif

// ----- General include directives for all environments --------------------
#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES
#define INCL_DOSERRORS
#if !defined(__MWERKS__) && !(__BORLANDC__ >= 0x0500)
extern "C"
{
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if !defined(__MWERKS__) && !(__BORLANDC__ >= 0x0500)
}
#endif

// ----- Compiler specific files needed for the ZAF_DIRECTORY_SERVICE class -
#if defined(__BCPLUSPLUS__) || defined(__TCPLUSPLUS__)
#	include <dos.h>
#	include <dir.h>
#	include <direct.h>
#	include <errno.h>
#elif defined(__ZTC__) && !defined(ZIL_MACINTOSH)
#	include <dos.h>
#	include <cerror.h>
#	include <direct.h>
#	define _dos_getdrive dos_getdrive
#	define _dos_setdrive dos_setdrive
#elif defined(_MSC_VER)
#	if !defined(ZIL_WINNT)
#		include <dos.h>
#		if defined(DOS386)
#		include <pldos32.h>
#		endif
#	endif
#	if defined(DOSX286)
#		define	_int86x	int86x
#		define	_int86	int86
#		define	_REGS	REGS
#	endif
#	include <direct.h>
#elif defined(__WATCOMC__) && !defined (__QNX__)
#	include <io.h>
#	include <dos.h>
#	include <direct.h>
#elif defined(__HIGHC__) || defined(__IBMCPP__)
#	include <direct.h>
#elif defined(ZIL_POSIX)
extern "C"
{
#	include <fcntl.h>
#	include <unistd.h>
#	if defined(ZIL_NEXTSTEP)
#		include <sys/dirent.h>
#		include <bsd/sys/dir.h>
#	else
#		include <dirent.h>
#	endif
#	include <sys/types.h>
#	include <sys/stat.h>
#	if !defined (__QNX__)
#		include <sys/param.h>
#	endif
}
#	if defined(_IBM_RS6000) || defined(_SUNOS4)
#		include <time.h>
#	endif
#endif

typedef unsigned DEVICE_CLASSIFICATION;
const DEVICE_CLASSIFICATION DEVICE_UNKNOWN			= 0x0000;
const DEVICE_CLASSIFICATION DEVICE_DRIVE			= 0x1000;
const DEVICE_CLASSIFICATION DEVICE_DRIVE_FIXED		= 0x1001;
const DEVICE_CLASSIFICATION DEVICE_DRIVE_REMOVABLE	= 0x1002;
const DEVICE_CLASSIFICATION DEVICE_DRIVE_REMOTE		= 0x1004;
const DEVICE_CLASSIFICATION DEVICE_DIRECTORY		= 0x2000;
const DEVICE_CLASSIFICATION DEVICE_FILE				= 0x4000;

// --------------------------------------------------------------------------
// ----- ZNC_DEVICE_INFO ----------------------------------------------------
// --------------------------------------------------------------------------

class ZIL_EXPORT_CLASS ZNC_DEVICE_INFO
{
	friend class ZIL_EXPORT_CLASS ZAF_DIRECTORY_SERVICE;
public:
	DEVICE_CLASSIFICATION classification;
	ZIL_ICHAR name[32];
	ZIL_BIGNUM size;
	ZIL_DATE date;
	ZIL_TIME time;

	int Inherited(DEVICE_CLASSIFICATION _classification)
		{ return (((classification & _classification) == _classification) ? TRUE : FALSE) ; }

private:
	int reserved;
#if defined(ZIL_POSIX)
#	if defined(ZIL_NEXTSTEP)
		struct direct *_reserved;
#	else
		struct dirent *_reserved;
#	endif
#elif defined(ZIL_WINNT)
	WIN32_FIND_DATA _reserved;
#elif defined(ZIL_OS2)
	FILEFINDBUF3 _reserved;
#elif defined(ZIL_MACINTOSH)
	CInfoPBRec _reserved;
#elif defined(__BCPLUSPLUS__) | defined(__TCPLUSPLUS__)
	struct ffblk _reserved;
#elif defined(_MSC_VER) || defined(__WATCOMC__)
	struct _find_t _reserved;
#elif defined(__ZTC__)
	struct FIND *_reserved;
#endif
};

// --------------------------------------------------------------------------
// ----- ZAF_DIRECTORY_SERVICE ----------------------------------------------
// --------------------------------------------------------------------------

#if defined(ZIL_MACINTOSH) || defined(ZIL_NEXTSTEP) || defined(ZIL_OS2) || defined(ZIL_MSWINDOWS) || defined(ZIL_WINNT)
class ZIL_EXPORT_CLASS ZAF_DIRECTORY_SERVICE : public UI_WINDOW_OBJECT
#else
class ZIL_EXPORT_CLASS ZAF_DIRECTORY_SERVICE : public UIW_WINDOW
#endif
{
public:
	static ZIL_ICHAR _className[];
	static UI_ITEM *_userTable;
	static UI_ITEM *_objectTable;
	static ZIL_STORAGE_READ_ONLY *_storage;

	ZAF_DIRECTORY_SERVICE(void);
	~ZAF_DIRECTORY_SERVICE(void);
	virtual ZIL_ICHAR *ClassName(void) { return (_className); }
	virtual EVENT_TYPE Event(const UI_EVENT &event);
	virtual void *Information(INFO_REQUEST request, void *data, ZIL_OBJECTID objectID = ID_DEFAULT);
	int DeviceInfo(ZIL_ICHAR *pathName, ZNC_DEVICE_INFO &info);
	ZIL_ICHAR *DeviceName(ZIL_ICHAR *deviceName);
	ZIL_ICHAR *PathName(ZIL_ICHAR *pathName);

private:
	int originalDrive;
	int currentDrive;
	ZIL_ICHAR originalPath[ZIL_MAXPATHLEN];
	ZIL_ICHAR currentPath[ZIL_MAXPATHLEN];
	ZIL_ICHAR requestedFile[ZIL_MAXPATHLEN];

	EVENT_TYPE response;
	UI_WINDOW_OBJECT *requestor;
	ZIL_LANGUAGE msgTable;

	static void Close(void);
	static int DriveError(int drive);
#	if defined(ZIL_MACINTOSH) || defined(ZIL_NEXTSTEP)
	static int GetDrive(void);
	static int SetDrive(int drive);
	static int SetPath(ZIL_ICHAR *pathName);
#	else
	static int FindFirst(const ZIL_ICHAR *pathName, ZNC_DEVICE_INFO &info, DEVICE_CLASSIFICATION classification);
	static int FindNext(ZNC_DEVICE_INFO &info);
	static int GetDrive(void);
	static int GetPath(ZIL_ICHAR *pathName);
	static void Open(void);
	static int SetDrive(int drive);
	static int SetPath(ZIL_ICHAR *pathName);

	void DirectoryUpdate(void);
	void DriveUpdate(void);
	void FileUpdate(void);
#	endif
};

#endif

