#include <direct.hpp>
#include <storage.hpp>
#include <stredit.hpp>
#include "help.hpp"

// ----- UI_APPLICATION -----------------------------------------------------

int UI_APPLICATION::Main(void)
{
	UI_APPLICATION::LinkMain();

	// Create the service manager (level 0).
	_serviceManager = new ZAF_SERVICE_MANAGER;

	// Create and add base services (level 1).
	*_serviceManager
		+ new ZAF_DIRECTORY_SERVICE
		+ new ZAF_STORAGE_SERVICE(argc, argv)
		+ new ZAF_STRING_EDITOR(WOAF_LOCKED, WOS_MINIMIZED | WOS_REDISPLAY);

	// Create and add high level services (level 2).
	*_serviceManager
		+ new ZAF_HELP_EDITOR(WOAF_NO_FLAGS, WOS_REDISPLAY);

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete _serviceManager;
	return (0);
}

