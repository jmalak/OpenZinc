//	Zinc Interface Library - M_DSP.CPP
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


extern "C"
{
#include <stdio.h>
#include <stdlib.h>
}
#include "ui_dsp.hpp"

#if defined(ZIL_XKEY)
#	include <X11/StringDefs.h>
#endif

static int _virtualCount = 0;
static UI_REGION _virtualRegion;

// AppData and resources try to read user-modifiable resources specific
//	to zinc programs from the standard X resource files.

typedef struct {
	char *small_font;
	char *dialog_font;
	char *system_font;
} AppData;

#define XtNzincSmallFont	"zincSmallFont"
#define XtCZincSmallFont	"ZincSmallFont"
#define XtNzincDialogFont	"zincDialogFont"
#define XtCZincDialogFont	"ZincDialogFont"
#define XtNzincSystemFont	"zincSystemFont"
#define XtCZincSystemFont	"ZincSystemFont"

#if defined(ZIL_UNICODE)
static XtResource resources[] = {
	{
		XtNzincSmallFont,
		XtCZincSmallFont,
		XtRString,
		sizeof(char *),
		XtOffsetOf(AppData, small_font),
		XtRString,
		(XtPointer)"-*-*-*-r-normal--*-*-*"
	},
	{
		XtNzincDialogFont,
		XtCZincDialogFont,
		XtRString,
		sizeof(char *),
		XtOffsetOf(AppData, dialog_font),
		XtRString,
		(XtPointer)"-*-*-*-r-normal--*-*-*"
	},
	{
		XtNzincSystemFont,
		XtCZincSystemFont,
		XtRString,
		sizeof(char *),
		XtOffsetOf(AppData, system_font),
		XtRString,
		(XtPointer)"-*-*-*-r-normal--*-*-*"
	},
};
#else
static XtResource resources[] = {
	{
		XtNzincSmallFont,
		XtCZincSmallFont,
		XtRString,
		sizeof(char *),
		XtOffsetOf(AppData, small_font),
		XtRString,
		(XtPointer)"-*-helvetica-medium-r-*--12-*-iso8859-1"
	},
	{
		XtNzincDialogFont,
		XtCZincDialogFont,
		XtRString,
		sizeof(char *),
		XtOffsetOf(AppData, dialog_font),
		XtRString,
		(XtPointer)"-*-helvetica-bold-r-*--14-*-iso8859-1"
	},
	{
		XtNzincSystemFont,
		XtCZincSystemFont,
		XtRString,
		sizeof(char *),
		XtOffsetOf(AppData, system_font),
		XtRString,
		(XtPointer)"-*-helvetica-medium-r-*--14-*-iso8859-1"
	},
};
#endif


void NoOp(Widget, XEvent *, String *, Cardinal *)
{
}

// We cancel the action of some movement keys so we can handle them rather
// than letting Motif do it.
static XtActionsRec zincActions[] = {
	{"NoOp", (XtActionProc)NoOp}
};

// ----- UI_XT_DISPLAY -----------------------------------------------------

static ZIL_ICHAR _operatingSystem[] = { 'P','O','S','I','X',0 };
static ZIL_ICHAR _windowingSystem[] = { 'M','o','t','i','f', 0 };
static ZIL_ICHAR _codeSet[20] = { 'I','S','O','8','8','5','9','-','1',0 };
static ZIL_ICHAR _Mstr[] = { 'M', 0 };
static ZIL_ICHAR _MIQstr[] = { 'M', 'I', 'Q', 'q', 'z', 'g', 0 };

#if defined(ZIL_UNICODE)
static ZIL_ICHAR shiftJIS[] = { 'I', 'B', 'M', '_', '9', '3', '2', 0 };
static ZIL_ICHAR eucJIS[] =   { 'E', 'U', 'C', '_', 'J', 'I', 'S', 0 };
static ZIL_ICHAR big5[] =     { 'I', 'B', 'M', '_', '9', '5', '0', 0 };
static ZIL_ICHAR ksc5601[] =  { 'I', 'B', 'M', '_', '9', '4', '9', 0 };
static ZIL_ICHAR gb2312[] =   { 'I', 'B', 'M', '_', '1', '3', '8', '1', 0 };

static struct
{
	char *osMap;
	ZIL_ICHAR *Map;
} fontMap[] = {
#	if defined(__DECCXX)
	{ "ja_JP",		eucJIS },
	{ "ja_JP.SJIS",		shiftJIS },
	{ "ja_JP.deckanji",	eucJIS },
	{ "ja_JP.eucJP",	eucJIS },
	{ "ja_JP.sdeckanji",	shiftJIS },
	{ "ko_KR",		ksc5601 },
	{ "ko_KR.deckorean",	ksc5601 },
	{ "ko_KR.eucKR",	ksc5601 },
	{ "zh_CN",		gb2312 },
	{ "zh_CN.dechanzi",	gb2312 },
	{ "zh_CN.dechanzi@pinyin",	gb2312 },
	{ "zh_CN.dechanzi@radical",	gb2312 },
	{ "zh_CN.dechanzi@stroke",	gb2312 },
	{ "zh_HK.big5",		big5 },
	{ "zh_HK.dechanyu",	big5 },
	{ "zh_HK.dechanzi",	gb2312 },
	{ "zh_HK.eucTW",	big5 },
	{ "zh_TW",		big5 },
	{ "zh_TW.big5",		big5 },
	{ "zh_TW.dechanyu",	big5 },
	{ "zh_TW.eucTW",	big5 },
	{ "zh_TW.big5@chuyin",	big5 },
	{ "zh_TW.big5@radical",	big5 },
	{ "zh_TW.big5@stroke",	big5 },
	{ "zh_TW.dechanyu@chuyin",	big5 },
	{ "zh_TW.dechanyu@radical",	big5 },
	{ "zh_TW.dechanyu@stroke",	big5 },
	{ "zh_TW.eucTW@chuyin",	big5 },
	{ "zh_TW.eucTW@radical",big5 },
	{ "zh_TW.eucTW@stroke",	big5 },
#	elif defined(_IBM_RS6000)
	{ "ko_KR",		ksc5601 },
	{ "ko_KR.IBM-eucKR",	ksc5601 },
	{ "zh_TW",		big5 },
	{ "zh_TW.IBM-eucTW",	big5 },
	{ "ja_JP",		eucJIS },
	{ "Ja_JP",		eucJIS },
	{ "ja_JP.IBM-eucJP",	eucJIS },
	{ "Ja_JP.IBM-932",	shiftJIS },
#	elif defined(sun)
	{ "ja",			eucJIS },
	{ "ja_JP",		eucJIS },
	{ "japan",		eucJIS },
	{ "japanese",	eucJIS },
	{ "ja_JP.EUC",	eucJIS },
	{ "ja_JP.SJIS",	shiftJIS },
	{ "ko",		ksc5601 },
	{ "ko_KR",		ksc5601 },
	{ "korean",		ksc5601 },
	{ "ko_KR.EUC",	ksc5601 },
	{ "zh",		gb2312 },
	{ "chinese",		gb2312 },
	{ "zh_CN.EUC",	gb2312 },
	{ "zh_TW",		big5 },
	{ "tchinese",	big5 },
	{ "zh_TW.EUC",	big5 },
#	else
???? This table needs to be built for your platform.
#	endif
	{ ZIL_NULLP(char), _codeSet }
};

// Variation of the example from Programmers Supplement for Release 5.
static XIMStyle ChooseBetterStyle(XIMStyle style1, XIMStyle style2)
{
	// Set to what are the allowed preedit and status flags
	XIMStyle preedit = XIMPreeditNothing | XIMPreeditNone;
	XIMStyle status =  XIMStatusNothing | XIMStatusNone;

	if (style1 == 0)
		return style2;
	if (style2 == 0)
		return style1;
	if ((style1 & (preedit | status)) == (style2 & (preedit | status)))
		return style1;
    
	XIMStyle s1 = style1 & preedit;
	XIMStyle s2 = style2 & preedit;
	if (s1 != s2)
	{
		if ((s1 | s2) & XIMPreeditCallbacks)
			return (s1 & XIMPreeditCallbacks)?style1:style2;
		if ((s1 | s2) & XIMPreeditPosition)
			return (s1 & XIMPreeditPosition)?style1:style2;
		if ((s1 | s2) & XIMPreeditArea)
			return (s1 & XIMPreeditArea)?style1:style2;
		if ((s1 | s2) & XIMPreeditNothing)
			return (s1 & XIMPreeditNothing)?style1:style2;
	}
	else
	{
		s2 = style1 & status;
		s2 = style2 & status;
		if ((s1 | s2) & XIMStatusCallbacks)
			return (s1 & XIMStatusCallbacks)?style1:style2;
		if ((s1 | s2) & XIMStatusArea)
			return (s1 & XIMStatusArea)?style1:style2;
		if ((s1 | s2) & XIMStatusNothing)
			return (s1 & XIMStatusNothing)?style1:style2;
	}
	return 0;
}
#endif



UI_XT_DISPLAY::UI_XT_DISPLAY(int *argc, char **argv, char *_appClass,
	XrmOptionDescList options, Cardinal numOptions, String *fallbackResources) :
	UI_DISPLAY(FALSE, _operatingSystem, _windowingSystem)
{
#if (ZIL_X11 > 4) && !(defined(sun) && defined(i386))
		// Get X11R5 ready for i18n.
		XtSetLanguageProc(ZIL_NULLH(XtAppContext), ZIL_NULLH(XtLanguageProc),
			ZIL_NULLH(XtPointer));
#endif

	topShell = XtAppInitialize(&appContext, _appClass, options, numOptions,
#if (ZIL_XT > 4)
			argc,
#else
D,kfjhm			(Cardinal *)argc,
#endif
#if !defined(__DVX__)
			argv, fallbackResources, ZIL_NULLP(Arg), 0);
#else
			argv, ZIL_NULLP(String), ZIL_NULLP(Arg), 0);
#endif
	if (!topShell)
		return;

	appClass = _appClass;
	xDisplay = XtDisplay(topShell);
	xScreen = XtScreen(topShell);
	xScreenNumber = DefaultScreen(xDisplay);
	XGCValues gcValues;
	gcValues.subwindow_mode = ClipByChildren;
	xGC = XCreateGC(xDisplay, RootWindow(xDisplay, xScreenNumber),
		GCSubwindowMode, &gcValues);
	columns = DisplayWidth(xDisplay, xScreenNumber);
	lines = DisplayHeight(xDisplay, xScreenNumber);

	XtAppAddActions(appContext, zincActions, XtNumber(zincActions));
#if defined(DEBUG)
	int ErrHandler(Display *disp, XErrorEvent *xEvent);
	void XtErrHandler(String msg);
	int WarnHandler(String msg);
	XSetErrorHandler(ErrHandler);
	XtSetErrorHandler(XtErrHandler);
	XtSetWarningHandler((XtErrorHandler)WarnHandler);
#endif

	AppData app_data;
	XtGetApplicationResources(topShell, (XtPointer)&app_data, resources,
		XtNumber(resources), ZIL_NULLP(Arg), 0);

#if (ZIL_MOTIF <= 1001)
	// set up first three fonts
	fontTable[FNT_SMALL_FONT].fontStruct
		= XLoadQueryFont(xDisplay, app_data.small_font);
	fontTable[FNT_SMALL_FONT].fontList =
		XmFontListCreate(fontTable[FNT_SMALL_FONT].fontStruct,
		XmSTRING_DEFAULT_CHARSET);
	if (fontTable[FNT_SMALL_FONT].fontStruct == 0) {
		fprintf(stderr, "Error: Unable to load FNT_SMALL_FONT:\n   %s\n",
			app_data.small_font);
		abort();
	}

	fontTable[FNT_DIALOG_FONT].fontStruct
		= XLoadQueryFont(xDisplay, app_data.dialog_font);
	fontTable[FNT_DIALOG_FONT].fontList =
		XmFontListCreate(fontTable[FNT_DIALOG_FONT].fontStruct,
		XmSTRING_DEFAULT_CHARSET);
	if (fontTable[FNT_DIALOG_FONT].fontStruct == 0) {
		fprintf(stderr, "Error: Unable to load FNT_DIALOG_FONT:\n   %s\n",
			app_data.dialog_font);
		abort();
	}

	fontTable[FNT_SYSTEM_FONT].fontStruct
		= XLoadQueryFont(xDisplay, app_data.system_font);
	fontTable[FNT_SYSTEM_FONT].fontList =
		XmFontListCreate(fontTable[FNT_SYSTEM_FONT].fontStruct,
		XmSTRING_DEFAULT_CHARSET);
	if (fontTable[FNT_SYSTEM_FONT].fontStruct == 0) {
		fprintf(stderr, "Error: Unable to load FNT_SYSTEM_FONT:\n   %s\n",
			app_data.system_font);
		abort();
	}

	strcpy(codeSet, _codeSet);
	cellWidth = TextWidth(_Mstr) * 3 / 4;
	cellHeight = TextHeight(_MIQstr)
		+ 2 * 2		// borderWidth
		+ 2 * 2		// shadowWidth
		+ 2 * 3;	// marginWidth
#else
#	if defined(ZIL_UNICODE)
#		define fontType XmFONT_IS_FONTSET
#		define GetFont(a)	(a).fontSet
#		define SetUpFont(a)	GetFont(a) = \
			(XFontSet)XmFontListEntryGetFont(entry, &type);
	XmFontListEntry entry;
	XmFontType type;
	entry = XmFontListEntryLoad(xDisplay, app_data.small_font,
		fontType, XmFONTLIST_DEFAULT_TAG);
	fontTable[FNT_SMALL_FONT].fontList =
		XmFontListAppendEntry(NULL, entry);
	SetUpFont(fontTable[FNT_SMALL_FONT]);
	XmFontListEntryFree(&entry);
	if (GetFont(fontTable[FNT_SMALL_FONT]) == 0) {
		fprintf(stderr, "Error: Unable to load FNT_SMALL_FONT:\n   %s\n",
			app_data.small_font);
		abort();
	}

	entry = XmFontListEntryLoad(xDisplay, app_data.dialog_font,
		fontType, XmFONTLIST_DEFAULT_TAG);
	fontTable[FNT_DIALOG_FONT].fontList =
		XmFontListAppendEntry(NULL, entry);
	SetUpFont(fontTable[FNT_DIALOG_FONT]);
	XmFontListEntryFree(&entry);
	if (GetFont(fontTable[FNT_DIALOG_FONT]) == 0) {
		fprintf(stderr, "Error: Unable to load FNT_DIALOG_FONT:\n   %s\n",
			app_data.dialog_font);
		abort();
	}

	entry = XmFontListEntryLoad(xDisplay, app_data.system_font,
		fontType, XmFONTLIST_DEFAULT_TAG);
	fontTable[FNT_SYSTEM_FONT].fontList =
		XmFontListAppendEntry(NULL, entry);
	SetUpFont(fontTable[FNT_SYSTEM_FONT]);
	XmFontListEntryFree(&entry);
	if (GetFont(fontTable[FNT_SYSTEM_FONT]) == 0) {
		fprintf(stderr, "Error: Unable to load FNT_SYSTEM_FONT:\n   %s\n",
			app_data.system_font);
		abort();
	}

	char *locale = XLocaleOfFontSet(fontTable[FNT_DIALOG_FONT].fontSet);
	for (int i=0; ; i++)
		if (fontMap[i].osMap == ZIL_NULLP(char) ||
		    ::strcmp(fontMap[i].osMap, locale) == 0)
		{
			strcpy(codeSet, fontMap[i].Map);
			break;
		}
	XFontSetExtents *tmp = XExtentsOfFontSet(fontTable[FNT_DIALOG_FONT].fontSet);
	cellWidth = (tmp->max_logical_extent.width * 5 + 7) / 8;
	int tmpWidth = TextWidth(_Mstr);
	if (tmpWidth > cellWidth) cellWidth = tmpWidth;

	cellHeight = tmp->max_logical_extent.height;
	int tmpHeight = TextHeight(_MIQstr);
	if (tmpHeight > cellHeight) cellHeight = tmpHeight;
	cellHeight += 2 * 2		// borderWidth
		+ 2 * 2		// shadowWidth
		+ 2 * 3;	// marginWidth

	// Bind in input method
	XIM im = XOpenIM(xDisplay, NULL, NULL, NULL);
	if (im == NULL)
	{
		(void)fprintf(stderr, "Couldn't open input method\n");
		exit(1);
	}
	XIMStyles *styles;
	XGetIMValues(im, XNQueryInputStyle, &styles, NULL);
	XIMStyle best = 0;
	for(i=0; i < styles->count_styles; i++)
		best = ChooseBetterStyle(styles->supported_styles[i], best);
	if (best == 0) {
		(void)fprintf(stderr, "Can't support requested interaction style.\n");
		exit(1);
	}
	XVaNestedList list =
		XVaCreateNestedList(0, XNFontSet,
				    GetFont(fontTable[FNT_SMALL_FONT]), NULL);
	ic = XCreateIC(im, XNInputStyle, best, 
		   XNPreeditAttributes, list, XNStatusAttributes, list, NULL);
	XFree(list);
	if (ic == NULL)
	{
		(void) fprintf(stderr, "Couldn't create input context\n");
		exit(1);
	}
	// XGetICValues(ic, XNFilterEvents, &im_event_mask, NULL);
	// XSelectInput(xDisplay, win, ExposureMask | KeyPressMask
	// 	     | StructureNotifyMask | im_event_mask);
	XSetICFocus(ic);
#	else
	memset(fontTable, 0, sizeof(fontTable));
 	// Allocate default fonts from the Motif font list.
	XmFontList fontList[ZIL_MAXFONTS];
	memset(fontList, 0, sizeof(fontList));
	XtVaGetValues(topShell,
		XmNtextFontList, &fontList[FNT_SMALL_FONT],
		XmNlabelFontList, &fontList[FNT_DIALOG_FONT],
		XmNdefaultFontList, &fontList[FNT_SYSTEM_FONT],
		0);
	int i;
	for (i = 0; i < ZIL_MAXFONTS; i++)
		if (fontList[i])
		{
			// Walk the font list for individual font entries.
			XmFontContext context;
			Boolean fontContext = XmFontListInitFontContext(&context, fontList[i]);
			while (fontContext)
			{
				// Get the next logical entry.
				XmFontType type;
				XmFontListEntry entry = XmFontListNextEntry(context);
				if (!entry)
					break;

				// Sift out the font structure.
				XtPointer pointer = XmFontListEntryGetFont(entry, &type);
				if (type == XmFONT_IS_FONT)
				{
					XFontStruct *font = (XFontStruct *)pointer;

					// Set the default xGC.
					fontTable[i].fontStruct = font;
					fontTable[i].fontList = fontList[i];
					if (i == FNT_DIALOG_FONT)
						XSetFont(xDisplay, xGC, font->fid);

					break;
				}
			}
			XmFontListFreeFontContext(context);
		}
	strcpy(codeSet, _codeSet);
	cellWidth = TextWidth(_Mstr) * 3 / 4;
	cellHeight = TextHeight(_MIQstr)
		+ 2 * 2		// borderWidth
		+ 2 * 2		// shadowWidth
		+ 2 * 3;	// marginWidth
#	endif
#endif
	preSpace = 2;
	postSpace = 2;
	// initialize the stipple for interleave file
#	define stipple_width 4
#	define stipple_height 4
	static char stipple_bits[] = {
   		0x09, 0x06, 0x06, 0x09};
	interleaveStipple = XCreateBitmapFromData(xDisplay,
		RootWindow(xDisplay, xScreenNumber), stipple_bits,
		stipple_width, stipple_height);
	XSetStipple(xDisplay, xGC, interleaveStipple);
	installed = TRUE;

	// Create the GC for "xor" drawing.  Really invert rather than xor.
	XGCValues gcv;
	XtGCMask  xorMask =
		GCFunction | GCSubwindowMode | GCPlaneMask;
	gcv.function = GXinvert;
	gcv.subwindow_mode = IncludeInferiors;
	gcv.plane_mask = BlackPixelOfScreen(xScreen) ^ WhitePixelOfScreen(xScreen);
	xorGC = XCreateGC(xDisplay, RootWindow(xDisplay, xScreenNumber),
		xorMask, &gcv);
}

UI_XT_DISPLAY::~UI_XT_DISPLAY(void)
{
	// XCreateGC called in constructor.
	XFreeGC(xDisplay, xGC);
	XFreeGC(xDisplay, xorGC);

#if defined(ZIL_UNICODE)
	XCloseIM(XIMOfIC(ic));
#endif
}

void UI_XT_DISPLAY::Bitmap(ZIL_SCREENID screenID, int column, int line,
	int bitmapWidth, int bitmapHeight, const ZIL_UINT8 *bitmapArray,
	const UI_PALETTE *palette, const UI_REGION *clipRegion,
	ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
colorBitmap = colorBitmap; monoBitmap = monoBitmap;
	VirtualGet(screenID, column, line, column + bitmapWidth,
		line + bitmapHeight);

	if (clipRegion)
	{
		XRectangle rect;
		rect.x = clipRegion->left - _virtualRegion.left;
		rect.y = clipRegion->top - _virtualRegion.top;
		rect.width = clipRegion->right - clipRegion->left;
		rect.height = clipRegion->bottom - clipRegion->top;
		XSetClipRectangles(xDisplay, xGC, 0, 0, &rect, 1, YXBanded);
	}

	ZIL_COLOR background;
#	if defined(ZIL_MOTIF)
	Arg args[2];
	XtSetArg(args[0], XtNbackground, &background);
	XtGetValues(screenID, args, 1);
#	elif defined(ZIL_XKEY)
	background = WhitePixel(xDisplay, xScreenNumber);
#	endif

	ZIL_UINT8 *pixel = (ZIL_UINT8 *)bitmapArray;
	for (int y = 0; y < bitmapHeight; y++)
	{
		for (int x = 0; x < bitmapWidth; x++)
		{
			ZIL_COLOR color;
			if (palette)
				color = MapColor(&palette[*pixel], TRUE);
			else if (*pixel <= MAX_COLORMAP_INDEX)
				color = MapColor(&colorMap[*pixel], TRUE);
			else
				color = background;
			pixel++;
			if (color != background)
			{
				XSetForeground(xDisplay, xGC, color);
				XDrawPoint(xDisplay, XtWindow(screenID), xGC,
					column + x - _virtualRegion.left,
					line + y - _virtualRegion.top);
			}
		}
	}

	XSetClipMask(xDisplay, xGC, None);

	// Un-virtualize the display.
	VirtualPut(screenID);
}

void UI_XT_DISPLAY::BitmapArrayToHandle(ZIL_SCREENID screenID, int bitmapWidth,
	int bitmapHeight, const ZIL_UINT8 *bitmapArray, const UI_PALETTE *palette,
	ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *)
{
	int column, row;
	ZIL_COLOR background, foreground;
	Arg args;

	// get default background
	if (screenID)
	{
		XtSetArg(args, XtNbackground, &background);
		XtGetValues(screenID, &args, 1);
		XtSetArg(args, XtNforeground, &foreground);
		XtGetValues(screenID, &args, 1);
	}
	else
	{
		background = WhitePixel(xDisplay, xScreenNumber);
		background = BlackPixel(xDisplay, xScreenNumber);
	}

	*colorBitmap = XCreatePixmap(xDisplay, xScreen->root,
		bitmapWidth, bitmapHeight, XDefaultDepth(xDisplay, xScreenNumber));

	XSetFillStyle(xDisplay, xGC, FillSolid);

	// clear background
	XSetForeground(xDisplay, xGC, background);
	XFillRectangle(xDisplay, *colorBitmap, xGC, 0, 0,
		bitmapWidth, bitmapHeight);

	const ZIL_UINT8 *pixel = bitmapArray;
	for (row = 0; row < bitmapHeight; row++)
	{
		for (column = 0; column < bitmapWidth; column++)
		{
			ZIL_COLOR color;
			if (palette)
				color = MapColor(&palette[*pixel], TRUE);
			else if (*pixel <= MAX_COLORMAP_INDEX)
				color = MapColor(&colorMap[*pixel], TRUE);
			else
				color = background;
			pixel++;
			if (color != background)
			{
				XSetForeground(xDisplay, xGC, color);
				XDrawPoint(xDisplay, *colorBitmap, xGC,
					column, row);
			}
		}
	}
}

void UI_XT_DISPLAY::BitmapHandleToArray(ZIL_SCREENID, ZIL_BITMAP_HANDLE,
		ZIL_BITMAP_HANDLE, int *, int *,
		ZIL_UINT8 **)
{
}

void UI_XT_DISPLAY::DestroyBitmapHandle(ZIL_SCREENID, ZIL_BITMAP_HANDLE *colorBitmap, ZIL_BITMAP_HANDLE *monoBitmap)
{
	if (colorBitmap && *colorBitmap)
	{
		XFreePixmap(xDisplay, *colorBitmap);
		*colorBitmap = 0;
	}
	if (monoBitmap && *monoBitmap)
	{
		XFreePixmap(xDisplay, *monoBitmap);
		*monoBitmap = 0;
	}
}

void UI_XT_DISPLAY::IconArrayToHandle(ZIL_SCREENID, int,
	int, const ZIL_UINT8 *, const UI_PALETTE *, ZIL_ICON_HANDLE *)
{
}

void UI_XT_DISPLAY::IconHandleToArray(ZIL_SCREENID, ZIL_ICON_HANDLE,
		int *, int *, ZIL_UINT8 **)
{
}

void UI_XT_DISPLAY::DestroyIconHandle(ZIL_SCREENID, ZIL_ICON_HANDLE *icon)
{
	if (icon && *icon)
		XFreePixmap(xDisplay, *icon);
	*icon = 0;
}


//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//void UI_XT_DISPLAY::Ellipse(ZIL_SCREENID screenID, int x, int y, int startAngle,
//	int endAngle, int xRadius, int yRadius,const UI_PALETTE *palette, int fill, 
//	int xor, const UI_REGION *clipRegion)
void UI_XT_DISPLAY::Ellipse(ZIL_SCREENID screenID, int x, int y, int startAngle,
	int endAngle, int xRadius, int yRadius,const UI_PALETTE *palette, int fill, 
	int xorInt, const UI_REGION *clipRegion)
{
	VirtualGet(screenID, x, y, x + xRadius + xRadius, y + yRadius + yRadius);

	GC gc = xGC;

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//	if (xor)
	if (xorInt)
		gc = xorGC;

	endAngle -= startAngle;
	while (startAngle > 360)
		startAngle -= 360;
	while (startAngle < 0)
		startAngle += 360;
	while (endAngle > 360)
		endAngle -= 360;
	while (endAngle < 0)
		endAngle += 360;
	startAngle *= 64;
	endAngle *= 64;

	unsigned long foreground = MapColor(palette, TRUE);
	unsigned long background = MapColor(palette, FALSE);

	if (clipRegion)
	{
		XRectangle rect;
		rect.x = clipRegion->left - _virtualRegion.left;
		rect.y = clipRegion->top - _virtualRegion.top;
		rect.width = clipRegion->right - clipRegion->left;
		rect.height = clipRegion->bottom - clipRegion->top;
		XSetClipRectangles(xDisplay, gc, 0, 0, &rect, 1, YXBanded);
	}

	if (fill)
	{
		if (palette->fillPattern == PTN_INTERLEAVE_FILL)
		{
			XSetForeground(xDisplay, gc, foreground);
			XFillArc(xDisplay, XtWindow(screenID), gc,
				x - xRadius - _virtualRegion.left,
				y - yRadius - _virtualRegion.top,
				xRadius + xRadius, yRadius + yRadius, startAngle, endAngle);
			XSetFillStyle(xDisplay, gc, FillStippled);
		}
		XSetForeground(xDisplay, gc, background);
		XFillArc(xDisplay, XtWindow(screenID), gc,
			x - xRadius - _virtualRegion.left,
			y - yRadius - _virtualRegion.top,
			xRadius + xRadius, yRadius + yRadius, startAngle, endAngle);
		XSetFillStyle(xDisplay, gc, FillSolid);
	}
	XSetForeground(xDisplay, gc, foreground);
	XDrawArc(xDisplay, XtWindow(screenID), gc,
			x - xRadius - _virtualRegion.left,
			y - yRadius - _virtualRegion.top,
			xRadius + xRadius, yRadius + yRadius, startAngle, endAngle);

	XSetClipMask(xDisplay, gc, None);

	// Un-virtualize the display.
	VirtualPut(screenID);
}

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//void UI_XT_DISPLAY::Line(ZIL_SCREENID screenID, int column1, int line1,
//	int column2, int line2, const UI_PALETTE *palette, int width, int xor,
//	const UI_REGION *clipRegion)
void UI_XT_DISPLAY::Line(ZIL_SCREENID screenID, int column1, int line1,
	int column2, int line2, const UI_PALETTE *palette, int width, int xorInt,
	const UI_REGION *clipRegion)
{
	VirtualGet(screenID, MinValue(column1, column2), MinValue(line1, line2),
		MaxValue(column1, column2), MaxValue(line1, line2));

	GC gc = xGC;

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//	if (xor)
	if (xorInt)
		gc = xorGC;
	else
	{
		unsigned long color = MapColor(palette, TRUE);
		XSetForeground(xDisplay, gc, color);
	}

	if (clipRegion)
	{
		XRectangle rect;
		rect.x = clipRegion->left - _virtualRegion.left;
		rect.y = clipRegion->top - _virtualRegion.top;
		rect.width = clipRegion->right - clipRegion->left;
		rect.height = clipRegion->bottom - clipRegion->top;
		XSetClipRectangles(xDisplay, gc, 0, 0, &rect, 1, YXBanded);
	}
	if (width > 1)
		XSetLineAttributes(xDisplay, gc, width, LineSolid, CapButt, JoinBevel);

	if (width)
		XDrawLine(xDisplay, XtWindow(screenID), gc,
			column1 - _virtualRegion.left,
			line1 - _virtualRegion.top, column2 - _virtualRegion.left,
			line2 - _virtualRegion.top);

	if (width > 1)
		XSetLineAttributes(xDisplay, gc, 1, LineSolid, CapButt, JoinBevel);
	XSetClipMask(xDisplay, gc, None);

	// Un-virtualize the display.
	VirtualPut(screenID);
}

ZIL_COLOR UI_XT_DISPLAY::MapColor(const UI_PALETTE *palette, int isForeground)
{

	XColor xcolor;
#define DivideColor(color) { \
	xcolor.red		= (unsigned short)((color) >> 16 << 8); \
	xcolor.green	= (unsigned short)((color) >> 8 << 8); \
	xcolor.blue		= (unsigned short)((color) >> 0 << 8); \
	xcolor.flags = 	DoRed | DoGreen | DoBlue; \
	}

	// Match the color request based on the ZIL_COLOR type in fillPattern.
	ZIL_COLOR color = isForeground ? palette->colorForeground : palette->colorBackground;
	if (palette->fillPattern == PTN_SYSTEM_COLOR)
		return (color);
	else if (palette->fillPattern == PTN_RGB_COLOR)
	{
		UI_PALETTE *tmpPalette = (UI_PALETTE *)palette;
		DivideColor(palette->colorForeground);
		XAllocColor(xDisplay, DefaultColormap(xDisplay,xScreenNumber), &xcolor);
		tmpPalette->colorForeground = xcolor.pixel;

		DivideColor(palette->colorBackground);
		XAllocColor(xDisplay, DefaultColormap(xDisplay,xScreenNumber), &xcolor);
		tmpPalette->colorBackground = xcolor.pixel;

		tmpPalette->fillPattern = PTN_SYSTEM_COLOR;
		return (isForeground ? palette->colorForeground
			: palette->colorBackground);
	}
	else if (color <= MAX_COLORMAP_INDEX)
	{
		// the color is 0 - 15; one of the EGA colors
		MapColor(&colorMap[(int)color], isForeground);
		return (isForeground ? colorMap[(ZIL_COLOR)color].colorForeground
			: colorMap[(ZIL_COLOR)color].colorBackground);
	}
	else
		return (RGB_BLACK);
}

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//void UI_XT_DISPLAY::Polygon(ZIL_SCREENID screenID, int numPoints,
//	const int *polygonPoints, const UI_PALETTE *palette, int fill, int xor,
//	const UI_REGION *clipRegion)
void UI_XT_DISPLAY::Polygon(ZIL_SCREENID screenID, int numPoints,
	const int *polygonPoints, const UI_PALETTE *palette, int fill, int xorInt,
	const UI_REGION *clipRegion)
{
	// Determine the maximum region that bounds the polygon.
	ZIL_UINT16 left = 0xFFFF, top = 0xFFFF;
	ZIL_UINT16 right = 0, bottom = 0;
	//line below added by jdh to conform to current "for" scoping
	int i;
	for (i = 0; i < numPoints; i++)
	{
		ZIL_INT16 column = polygonPoints[i*2];
		ZIL_INT16 line = polygonPoints[i*2+1];
		if (column < left)
			left = column;
		if (column > right)
			right = column;
		if (line < top)
			top = line;
		if (line > bottom)
			bottom = line;
	}

	// Virtualize the display.
	VirtualGet(screenID, left, top, right, bottom);

	GC gc = xGC;

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//	if (xor)
	if (xorInt)
		gc = xorGC;

	if (clipRegion)
	{
		XRectangle rect;
		rect.x = clipRegion->left - _virtualRegion.left;
		rect.y = clipRegion->top - _virtualRegion.top;
		rect.width = clipRegion->right - clipRegion->left;
		rect.height = clipRegion->bottom - clipRegion->top;
		XSetClipRectangles(xDisplay, gc, 0, 0, &rect, 1, YXBanded);
	}

	XPoint *xPolygon = new XPoint[numPoints];
	unsigned long foreground = MapColor(palette, TRUE);
	unsigned long background = MapColor(palette, FALSE);

	// Set up the polygon points.
	for (i = 0; i < numPoints; i++)
	{
		xPolygon[i].x = polygonPoints[i*2] - _virtualRegion.left;
		xPolygon[i].y = polygonPoints[i*2+1] - _virtualRegion.top;
	}

	if (fill)
	{
		if (palette->fillPattern == PTN_INTERLEAVE_FILL)
		{
			XSetForeground(xDisplay, gc, foreground);
			XFillPolygon(xDisplay, XtWindow(screenID), gc, xPolygon, numPoints,
				Complex, CoordModeOrigin);
			XSetFillStyle(xDisplay, gc, FillStippled);
		}
		XSetForeground(xDisplay, gc, background);
		XFillPolygon(xDisplay, XtWindow(screenID), gc, xPolygon, numPoints,
			Complex, CoordModeOrigin);
		XSetFillStyle(xDisplay, gc, FillSolid);
	}
	XSetForeground(xDisplay, gc, foreground);
	XDrawLines(xDisplay, XtWindow(screenID), gc, xPolygon, numPoints,
		CoordModeOrigin);

	XSetFillStyle(xDisplay, gc, FillSolid);
	XSetClipMask(xDisplay, gc, None);

	// Clean up the XPoint array.
	delete [] xPolygon;

	// Un-virtualize the display.
	VirtualPut(screenID);
}

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//void UI_XT_DISPLAY::Rectangle(ZIL_SCREENID screenID, int left, int top,
//	int right, int bottom, const UI_PALETTE *palette, int width, int fill,
//	int xor, const UI_REGION *clipRegion)
void UI_XT_DISPLAY::Rectangle(ZIL_SCREENID screenID, int left, int top,
	int right, int bottom, const UI_PALETTE *palette, int width, int fill,
	int xorInt, const UI_REGION *clipRegion)
{
	VirtualGet(screenID, left, top, right, bottom);

	GC gc = xGC;

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//	if (xor)
	if (xorInt)
		gc = xorGC;

	unsigned long foreground = MapColor(palette, TRUE);
	unsigned long background = MapColor(palette, FALSE);

	if (clipRegion)
	{
		XRectangle rect;
		rect.x = clipRegion->left - _virtualRegion.left;
		rect.y = clipRegion->top - _virtualRegion.top;
		rect.width = clipRegion->right - clipRegion->left;
		rect.height = clipRegion->bottom - clipRegion->top;
		XSetClipRectangles(xDisplay, gc, 0, 0, &rect, 1, YXBanded);
	}
	if (width > 1)
		XSetLineAttributes(xDisplay, gc, width, LineSolid, CapButt, JoinBevel);

	if (fill)
	{
		if (palette->fillPattern == PTN_INTERLEAVE_FILL)
		{
			XSetForeground(xDisplay, gc, foreground);
			XFillRectangle(xDisplay,
				screenID ? XtWindow(screenID) :
				RootWindow(xDisplay, xScreenNumber),
				gc, left - _virtualRegion.left, top - _virtualRegion.top,
				right - left + 1, bottom - top + 1);
				XSetFillStyle(xDisplay, gc, FillStippled);
			XSetFillStyle(xDisplay, gc, FillStippled);
		}
		XSetForeground(xDisplay, gc, background);
		XFillRectangle(xDisplay,
			screenID ? XtWindow(screenID) : RootWindow(xDisplay, xScreenNumber),
			gc, left - _virtualRegion.left, top - _virtualRegion.top,
			right - left + 1, bottom - top + 1);
		XSetFillStyle(xDisplay, gc, FillSolid);
	}

	if (width)
	{
		XSetForeground(xDisplay, gc, foreground);
		XDrawRectangle(xDisplay,
			screenID ? XtWindow(screenID) : RootWindow(xDisplay, xScreenNumber),
			gc, left - _virtualRegion.left, top - _virtualRegion.top,
			right - left, bottom - top);
	}
	
	if (width > 1)
		XSetLineAttributes(xDisplay, gc, 1, LineSolid, CapButt, JoinBevel);
	XSetClipMask(xDisplay, gc, None);

	// Un-virtualize the display.
	VirtualPut(screenID);
}

void UI_XT_DISPLAY::RectangleXORDiff(const UI_REGION &oldRegion,
	const UI_REGION &newRegion, ZIL_SCREENID screenID, const UI_REGION *)
{
	VirtualGet(screenID, MinValue(oldRegion.left, newRegion.left),
		MinValue(oldRegion.top, newRegion.top), MaxValue(oldRegion.right, newRegion.right),
		MinValue(oldRegion.bottom, newRegion.bottom));

	// Draw the two rectangles.
	Window window = screenID == ID_SCREEN ?
		RootWindow(xDisplay, xScreenNumber) : XtWindow(screenID);
	XDrawRectangle(xDisplay, window, xorGC,
		oldRegion.left - _virtualRegion.left,
		oldRegion.top - _virtualRegion.top,
		oldRegion.right - oldRegion.left - _virtualRegion.left,
		oldRegion.bottom - oldRegion.top - _virtualRegion.top);
	XDrawRectangle(xDisplay, window, xorGC,
		newRegion.left - _virtualRegion.left,
		newRegion.top - _virtualRegion.top,
		newRegion.right - newRegion.left - _virtualRegion.left,
		newRegion.bottom - newRegion.top - _virtualRegion.top);

	// Un-virtualize the display.
	VirtualPut(screenID);
}

void UI_XT_DISPLAY::RegionDefine(ZIL_SCREENID, int, int, int, int)
{
}


void UI_XT_DISPLAY::RegionMove(const UI_REGION &oldRegion, int newColumn,
	int newLine, ZIL_SCREENID oldScreenID, ZIL_SCREENID newScreenID)
{
	XImage *image = XGetImage(xDisplay, XtWindow(oldScreenID), oldRegion.left,
		oldRegion.top, oldRegion.Width(), oldRegion.Height(), 0xFF, XYPixmap);
	if (image)
	{
		XGCValues gcValues;
		gcValues.subwindow_mode = IncludeInferiors;
		XChangeGC(xDisplay, xGC, GCSubwindowMode, &gcValues);
		XPutImage(xDisplay, XtWindow(newScreenID), xGC, image, 0, 0,
			newColumn, newLine, oldRegion.Width(), oldRegion.Height());
		XDestroyImage(image);
		gcValues.subwindow_mode = ClipByChildren;
		XChangeGC(xDisplay, xGC, GCSubwindowMode, &gcValues);
	}
}

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//void UI_XT_DISPLAY::Text(ZIL_SCREENID screenID, int left, int top,
//	const ZIL_ICHAR *text, const UI_PALETTE *palette, int length, int fill,
//	int xor, const UI_REGION *clipRegion, ZIL_LOGICAL_FONT logicalFont)
void UI_XT_DISPLAY::Text(ZIL_SCREENID screenID, int left, int top,
	const ZIL_ICHAR *text, const UI_PALETTE *palette, int length, int fill,
	int xorInt, const UI_REGION *clipRegion, ZIL_LOGICAL_FONT logicalFont)
{
	// Make sure there is a valid string.
	if (!text || text[0] == '\0')
		return;

	// Set up the fill line.
	if (length < 0)
		length = strlen(text);
	ZIL_ICHAR *fillLine = new ZIL_ICHAR[length+1];
	strncpy(fillLine, text, length);
	fillLine[length] = '\0';

	// Virtualize the display.
	int right = left + TextWidth(fillLine, screenID, logicalFont) - 1;
	int bottom = top + TextHeight(fillLine, screenID, logicalFont) - 1;
	VirtualGet(screenID, left, top, right, bottom);

	GC gc = xGC;

//statement below altered by jdh, compiler objects to use of xor 
//as identifier. xorInt has been substituted
//	if (xor)
	if (xorInt)
		gc = xorGC;

	if (clipRegion)
	{
		XRectangle rect;
		rect.x = clipRegion->left - _virtualRegion.left;
		rect.y = clipRegion->top - _virtualRegion.top;
		rect.width = clipRegion->right - clipRegion->left;
		rect.height = clipRegion->bottom - clipRegion->top;
		XSetClipRectangles(xDisplay, gc, 0, 0, &rect, 1, YXBanded);
	}

	logicalFont &= 0x0FFF;

	unsigned long foreground = MapColor(palette, TRUE);
	unsigned long background = MapColor(palette, FALSE);

	if (fill)
	{
		XSetForeground(xDisplay, gc, background);
		XFillRectangle(xDisplay, XtWindow(screenID), gc,
			left - _virtualRegion.left, top - _virtualRegion.top,
			right - left, bottom - top);
	}
	XSetForeground(xDisplay, gc, foreground);

#if defined(ZIL_UNICODE)
	char *mb = MapText(fillLine);
	wchar_t *wText = new wchar_t[strlen(fillLine) + 1];
	::mbstowcs(wText, mb, strlen(fillLine) + 1);
	XwcDrawString(xDisplay, XtWindow(screenID),
		      fontTable[logicalFont].fontSet,
		      gc, left - _virtualRegion.left,
		      top - _virtualRegion.top + (bottom - top),
		      wText, length);
	delete mb;
	delete wText;
#else
	XSetFont(xDisplay, gc, fontTable[logicalFont].fontStruct->fid);
	XDrawString(xDisplay, XtWindow(screenID),
		gc, left - _virtualRegion.left,
		top - _virtualRegion.top + fontTable[logicalFont].fontStruct->ascent,
		fillLine, length);
#endif
	delete fillLine;

	XSetClipMask(xDisplay, gc, None);

	// Un-virtualize the display.
	VirtualPut(screenID);
}

int UI_XT_DISPLAY::TextHeight(const ZIL_ICHAR *text, ZIL_SCREENID,
	ZIL_LOGICAL_FONT logicalFont)
{
	logicalFont &= 0x0FFF;

	if (!text || !*text)
		return 0;

#if defined(ZIL_UNICODE)
	char *displayText = MapText(text);
	XRectangle logical, tmp1;
	XmbTextExtents(fontTable[logicalFont].fontSet, displayText,
		::strlen(displayText), &tmp1, &logical);
	delete displayText;
	return (logical.height);
#else
	int direction, ascent, descent;
	XCharStruct temp;

	XTextExtents(fontTable[logicalFont].fontStruct, text,
		strlen(text), &direction, &ascent, &descent, &temp);
	return(ascent + descent + 1);
#endif
}

int UI_XT_DISPLAY::TextWidth(const ZIL_ICHAR *text, ZIL_SCREENID,
	ZIL_LOGICAL_FONT logicalFont)
{	
	logicalFont &= 0x0FFF;

	if (!text || !*text)
		return 0;

#if defined(ZIL_UNICODE)
	char *displayText = MapText(text);
	XRectangle logical, tmp1;
	XFontSet tmp = fontTable[logicalFont].fontSet;
	XmbTextExtents(tmp, displayText,
		::strlen(displayText), &tmp1, &logical);
	delete displayText;
	return (logical.width);
#else
	int width = XTextWidth(fontTable[logicalFont].fontStruct, text,
		strlen(text));
	return width;
#endif
}

int UI_XT_DISPLAY::VirtualGet(ZIL_SCREENID, int left, int top,
	int right, int bottom)
{
	if (--_virtualCount == -1)
	{
		_virtualRegion.left = left;
		_virtualRegion.top = top;
		_virtualRegion.right = right;
		_virtualRegion.bottom = bottom;
	}
	return (TRUE);
}

int UI_XT_DISPLAY::VirtualPut(ZIL_SCREENID)
{
	_virtualCount++;
//	if (_virtualCount == 0)
//		XmUpdateDisplay(screenID);
	return (TRUE);
}


#if defined(DEBUG)
int ErrHandler(Display *disp, XErrorEvent *xEvent)
{
	char msg[80];
	XGetErrorText(disp, xEvent->error_code, msg, 80);
	fprintf(stderr, "X error: %s\n", msg);
	return 0;
}
void XtErrHandler(String msg)
{
	fprintf(stderr, "Xt error: %s\n", msg);
	exit(1);
}
int WarnHandler(String msg)
{
	fprintf(stderr, "X warning: %s\n", msg);
	return 0;
}
#endif
