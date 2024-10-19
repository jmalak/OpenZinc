//	Zinc Application Framework - ODIB.HPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

//	NOTE: This is a private image file.

// --- Declaration of structures found in OS/2 DIB files --------------------

const ZIL_UINT16 BFT_BMAP			= 0x4D42; // 'BM'
const ZIL_UINT16 BFT_ICON			= 0x4349; // 'IC'
const ZIL_UINT16 BFT_POINTER		= 0x5450; // 'PT'
const ZIL_UINT16 BFT_COLORICON		= 0x4943; // 'CI'
const ZIL_UINT16 BFT_COLORPOINTER	= 0x5043; // 'CP'

struct OS2_RGBTRIPLE
{
	ZIL_UINT8	rgbBlue;
	ZIL_UINT8	rgbGreen;
	ZIL_UINT8	rgbRed;
};

class ODIB_RGB_TRIPLE
{
public:
	static	OS2_RGBTRIPLE _zincRGBValue[];

	ZIL_UINT8	rgbBlue;
	ZIL_UINT8	rgbGreen;
	ZIL_UINT8	rgbRed;

	ODIB_RGB_TRIPLE(void);
	ODIB_RGB_TRIPLE(OS2_RGBTRIPLE &rgbTriple);
	ODIB_RGB_TRIPLE(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);

	ZIL_UINT8 GetZincColor(void);
};

class ODIB_FILE_HEADER
{
public:
	static	ZIL_UINT32 _size;

	ZIL_UINT16	usType;
	ZIL_UINT32	cbSize;
	ZIL_INT16	xHotspot;
	ZIL_INT16	yHotspot;
	ZIL_UINT32	offBits;

	ODIB_FILE_HEADER(void);
	ODIB_FILE_HEADER(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);
};

class ODIB_INFO_HEADER
{
public:
	static	ZIL_UINT32 _size1;
	static	ZIL_UINT32 _size2;
	static	int _encodingRatio;

	ZIL_UINT32	cbFix;
	union
	{
		ZIL_UINT16	c1x;
		ZIL_UINT32	c2x;
	};
	union
	{
		ZIL_UINT16	c1y;
		ZIL_UINT32	c2y;
	};
	ZIL_UINT16	cPlanes;
	ZIL_UINT16	cBitCount;
	ZIL_UINT32	ulCompression;
	ZIL_UINT32	cbImage;
	ZIL_UINT32	cxResolution;
	ZIL_UINT32	cyResolution;
	ZIL_UINT32	cclrUsed;
	ZIL_UINT32	cclrImportant;
	ZIL_UINT16	usUnits;
	ZIL_UINT16	usReserved;
	ZIL_UINT16	usRecording;
	ZIL_UINT16	usRendering;
	ZIL_UINT32	cSize1;
	ZIL_UINT32	cSize2;
	ZIL_UINT32	ulColorEncoding;
	ZIL_UINT32	ulIdentifier;

	ODIB_INFO_HEADER(void);
	ODIB_INFO_HEADER(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);
};

class ODIB_ARRAY_FILE_HEADER
{
public:
	static	ZIL_UINT32 _size;

	ZIL_UINT16 	usType;
	ZIL_UINT32 	cbSize;
	ZIL_UINT32 	offNext;
	ZIL_UINT16 	cxDisplay;
	ZIL_UINT16	cyDisplay;

	ODIB_ARRAY_FILE_HEADER(void);
	ODIB_ARRAY_FILE_HEADER(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);
};

