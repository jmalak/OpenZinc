//      HELLO1.CPP (HELLO) - Simple "Hello, World!" in a window.
//      COPYRIGHT (C) 1990-1994.  All Rights Reserved.
//      Zinc Software Incorporated.  Pleasant Grove, Utah  USA
//  May be freely copied, used and distributed.

#include <ui_win.hpp>
#include <fg.h>

static ZIL_ICHAR title[] = { 'H','e','l','l','o',' ','W','o','r','l','d',' ','W','i','n','d','o','w',0 };
static ZIL_ICHAR text[] = { 'H','e','l','l','o',',',' ','W','o','r','l','d','!',0 };

unsigned int _x32_stack_size = 20000;

int UI_APPLICATION::Main(void)
{
	// The UI_APPLICATION constructor automatically initializes the 
	// display, eventManager, and windowManager variables.

	// This line fixes linkers that don't look for main in the .LIBs.
	UI_APPLICATION::LinkMain();

	//adjust mouse movement 
	fg_msm_motion( 0 );

	// Create the hello world window.
	UIW_WINDOW *window = new UIW_WINDOW(5, 5, 40, 10);

	ZIL_BIGNUM my_bignum( 13.25);
	ZIL_DATE my_date;
	
	// Add the window objects to the window.
	*window
		+ new UIW_BORDER
		+ new UIW_MAXIMIZE_BUTTON
		+ new UIW_MINIMIZE_BUTTON
		+ new UIW_SYSTEM_BUTTON(SYF_GENERIC)
		+ new UIW_TITLE(title)
		+ new UIW_STRING( 2, 6, 20, "Hello World")
		+ new UIW_BIGNUM( 2, 2, 10, &my_bignum, 0, 
				NMF_DECIMAL(2))
		+ new UIW_DATE( 2, 4, 20, &my_date);
		
	// Add the window to the window manager.
	*windowManager + window;

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	return (0);
}

