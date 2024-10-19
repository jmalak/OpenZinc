#include "stredit.hpp"

// ----- UI_APPLICATION -----------------------------------------------------

int UI_APPLICATION::Main(void)
{
	UI_APPLICATION::LinkMain();
	_serviceManager = new ZAF_SERVICE_MANAGER;

	// Create and add the support editors.
	ZAF_STRING_EDITOR *stringEditor = new ZAF_STRING_EDITOR(WOAF_NO_FLAGS, WOS_REDISPLAY);
	stringEditor->Information(I_ACTIVATE_SERVICE, ZIL_NULLP(void));

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	return (0);
}

