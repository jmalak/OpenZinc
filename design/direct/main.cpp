#include "direct.hpp"

int UI_APPLICATION::Main(void)
{
	// Initialize the services.
	UI_APPLICATION::LinkMain();
	_serviceManager = new ZAF_SERVICE_MANAGER;

	// Make a request to the directory service.
	ZAF_DIRECTORY_SERVICE *directoryService = new ZAF_DIRECTORY_SERVICE;
	directoryService->Information(I_ACTIVATE_SERVICE, ZIL_NULLP(void));

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete _serviceManager;
	return (0);
}

