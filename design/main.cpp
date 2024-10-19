#include <direct.hpp>
#include <storage.hpp>
#include <stredit.hpp>
#include <help.hpp>
#include <image.hpp>
#include <message.hpp>
#include <i18n.hpp>
#include <window.hpp>

#if defined(ZIL_LINUX)
const int LARGE_COUR_FONT = 3;	// Placement for the new font.
const int SMALL_COUR_FONT = 4;	// Placement for the new font.
void LoadLargeCourierFont(UI_DISPLAY *_display);	// Function to load the new font.
void LoadSmallCourierFont(UI_DISPLAY *_display);	// Function to load the new font.
#endif

// ----- UI_APPLICATION -----------------------------------------------------

int UI_APPLICATION::Main(void)
{
	UI_APPLICATION::LinkMain();

#if defined(ZIL_LINUX)
	LoadLargeCourierFont(display);
	LoadSmallCourierFont(display);
#endif

	// Create the service manager (level 0).
	_serviceManager = new ZAF_SERVICE_MANAGER;

	// Create and add base services (level 1).
	*_serviceManager
		+ new ZAF_DIRECTORY_SERVICE
		+ new ZAF_STORAGE_SERVICE(argc, argv)
		+ new ZAF_STRING_EDITOR(WOAF_LOCKED, WOS_MINIMIZED | WOS_REDISPLAY);

	// Create and add high level services (level 2).
	*_serviceManager
		+ new ZAF_IMAGE_EDITOR(WOAF_LOCKED, WOS_MINIMIZED | WOS_REDISPLAY)
		+ new ZAF_HELP_EDITOR(WOAF_LOCKED, WOS_MINIMIZED | WOS_REDISPLAY)
		+ new ZAF_MESSAGE_EDITOR(WOAF_LOCKED, WOS_MINIMIZED | WOS_REDISPLAY)
		+ new ZAF_I18N_EDITOR(WOAF_LOCKED, WOS_MINIMIZED | WOS_REDISPLAY)
		+ new ZAF_WINDOW_EDITOR(WOAF_NO_FLAGS, WOS_REDISPLAY);
	_serviceManager->UpdateToolsMenus();

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete _serviceManager;
	return (0);
}

#if defined(ZIL_LINUX)
void LoadLargeCourierFont(UI_DISPLAY *_display)
{
	UI_XT_DISPLAY *display = (UI_XT_DISPLAY *)_display;
	typedef struct
	{
		char *largeCourFontList;
	} AppData;
	
	#define XmNlargeCourFontList	"largeCourFontList"
	#define XmCLargeCourFontList	"LargeCourFontList"

	static XtResource resources[] =
	{
		XmNlargeCourFontList,
		XmCLargeCourFontList,
		XmRString,
		sizeof(char *),
		XtOffsetOf(AppData, largeCourFontList),
		XmRString,
		// If font string is not found in X resource file then this font will be used.
		(XtPointer)"-*-*-bold-r-*--14-*-iso8859-1"
	};

	AppData appData;
	XtGetApplicationResources(display->topShell, (XtPointer)&appData,
		resources, XtNumber(resources), ZIL_NULLP(Arg), 0);

	display->fontTable[LARGE_COUR_FONT].fontStruct
		= XLoadQueryFont(display->xDisplay, appData.largeCourFontList);

	display->fontTable[LARGE_COUR_FONT].fontList =
		XmFontListCreate(display->fontTable[LARGE_COUR_FONT].fontStruct,
			XmSTRING_DEFAULT_CHARSET);

}

void LoadSmallCourierFont(UI_DISPLAY *_display)
{
	UI_XT_DISPLAY *display = (UI_XT_DISPLAY *)_display;
	typedef struct
	{
		char *smallCourFontList;
	} AppData;
	
	#define XmNsmallCourFontList	"smallCourFontList"
	#define XmCSmallCourFontList	"SmallCourFontList"

	static XtResource resources[] =
	{
		XmNsmallCourFontList,
		XmCSmallCourFontList,
		XmRString,
		sizeof(char *),
		XtOffsetOf(AppData, smallCourFontList),
		XmRString,
		// If font string is not found in X resource file then this font will be used.
		(XtPointer)"-*-*-bold-r-*--12-*-iso8859-1"
	};

	AppData appData;
	XtGetApplicationResources(display->topShell, (XtPointer)&appData,
		resources, XtNumber(resources), ZIL_NULLP(Arg), 0);

	display->fontTable[SMALL_COUR_FONT].fontStruct
		= XLoadQueryFont(display->xDisplay, appData.smallCourFontList);

	display->fontTable[SMALL_COUR_FONT].fontList =
		XmFontListCreate(display->fontTable[SMALL_COUR_FONT].fontStruct,
			XmSTRING_DEFAULT_CHARSET);

}
#endif
