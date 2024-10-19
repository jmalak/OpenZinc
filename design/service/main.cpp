//	Zinc Interface Library Designer - TEST.CPP
//	COPYRIGHT (C) 1990-1995.  All Rights Reserved.
//	Zinc Software Incorporated.  Pleasant Grove, Utah  USA

#include "service.hpp"

int UI_APPLICATION::Main(void)
{
	// Set up the initial services.
	UI_APPLICATION::LinkMain();

	// Initialize the services.
	_serviceManager = new ZAF_SERVICE_MANAGER;
	*windowManager + _serviceManager;
	windowManager->screenID = _serviceManager->screenID;

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete _serviceManager;
	return (0);
}

