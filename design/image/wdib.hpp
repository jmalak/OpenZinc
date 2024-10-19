//	Zinc Application Framework - WDIB.HPP
//	COPYRIGHT (C) 1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

//	NOTE: This is a private image file.

// --- Declaration of structures found in Windows DIB files -----------------

const ZIL_UINT16 BFT_BMAP			= 0x4D42; // 'BM'
const ZIL_UINT16 BFT_ICON			= 0x4349; // 'IC'
const ZIL_UINT16 BFT_POINTER		= 0x5450; // 'PT'
const ZIL_UINT16 BFT_COLORICON		= 0x4943; // 'CI'
const ZIL_UINT16 BFT_COLORPOINTER	= 0x5043; // 'CP'

struct WIN_RGBQUAD
{
	ZIL_UINT8	rgbBlue;
	ZIL_UINT8	rgbGreen;
	ZIL_UINT8	rgbRed;
	ZIL_UINT8	rgbReserved;
};

class WDIB_RGB_QUAD
{
public:
	static	WIN_RGBQUAD _zincRGBValue[];

	ZIL_UINT8	rgbBlue;
	ZIL_UINT8	rgbGreen;
	ZIL_UINT8	rgbRed;
	ZIL_UINT8	rgbReserved;

	WDIB_RGB_QUAD(void);
	WDIB_RGB_QUAD(WIN_RGBQUAD &rgbQuad);
	WDIB_RGB_QUAD(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);

	ZIL_UINT8 GetZincColor(void);
};

class WDIB_FILE_HEADER
{
public:
	static	ZIL_UINT32 _size;

	ZIL_UINT16 	bfType;
	ZIL_UINT32 	bfSize;
	ZIL_UINT16 	bfReserved1;
	ZIL_UINT16 	bfReserved2;
	ZIL_UINT32 	bfOffBits;

	WDIB_FILE_HEADER(void);
	WDIB_FILE_HEADER(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);
};

#if !defined(BI_RGB)
const int BI_RGB = 0;
#endif

class WDIB_INFO_HEADER
{
public:
	static	ZIL_UINT32 _size;
	static	int _encodingRatio;

	ZIL_UINT32	biSize;
	ZIL_UINT32	biWidth;
	ZIL_UINT32	biHeight;
	ZIL_UINT16	biPlanes;
	ZIL_UINT16	biBitCount;
	ZIL_UINT32	biCompression;
	ZIL_UINT32	biSizeImage;
	ZIL_UINT32	biXPelsPerMeter;
	ZIL_UINT32	biYPelsPerMeter;
	ZIL_UINT32	biClrUsed;
	ZIL_UINT32	biClrImportant;

	WDIB_INFO_HEADER(void);
	WDIB_INFO_HEADER(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);
};

class WICO_FILE_HEADER
{
public:
	static	ZIL_UINT32 _size;

	ZIL_UINT16	reserved;
	ZIL_UINT16	iconType;
	ZIL_UINT16	iconCount;

	WICO_FILE_HEADER(void);
	WICO_FILE_HEADER(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);
};

class WICO_DIRECTORY
{
public:
	static	ZIL_UINT32 _size;

	ZIL_UINT8	width;
	ZIL_UINT8	height;
	ZIL_UINT8	colorCount;
	ZIL_UINT8	reserved1;
	ZIL_UINT16	hotspotX;
	ZIL_UINT16	hotspotY;
	ZIL_UINT32	icoDIBSize;
	ZIL_UINT32	icoDIBOffset;

	WICO_DIRECTORY(void);
	WICO_DIRECTORY(ZIL_FILE *file);

	ZIL_FILE::Z_ERROR Load(ZIL_FILE *file);
	ZIL_FILE::Z_ERROR Store(ZIL_FILE *file);
};

