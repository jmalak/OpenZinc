#include "storage.hpp"

int UI_APPLICATION::Main(void)
{
	// Initialize the services.
	UI_APPLICATION::LinkMain();
	_serviceManager = new ZAF_SERVICE_MANAGER;

#if defined(ZIL_MACINTOSH)
	// Fudge the command line until we start using directory services.
	ZIL_ICHAR arg1[] = { 'B','r','o','w','s','e',0 };
	ZIL_ICHAR arg2[] = { 'B','r','o','w','s','e','.','D','a','t',0 };
	ZIL_ICHAR *arg1p = arg1, *arg2p = arg2;
	ZIL_ICHAR *myArgs[] =
	{
		arg1p,
		arg2p,
		0
	};
	ZAF_STORAGE_SERVICE *storageService = new ZAF_STORAGE_SERVICE(2, myArgs);
#else
	ZAF_STORAGE_SERVICE *storageService = new ZAF_STORAGE_SERVICE(argc, argv);
#endif

	ZIL_STORAGE *storage = ZIL_NULLP(ZIL_STORAGE);
	if (storageService->Information(I_GET_STORAGE, &storage))
	{
		extern ZIL_ICHAR *_windowDirectory;
		storage->ChDir(_windowDirectory);
		EVENT_TYPE ccode = OPT_RESOURCE_LOAD;
		storageService->Information(I_SET_REQUEST, &ccode);
		storageService->Information(I_SET_TEXT, argv[1]);
	}
	storageService->Information(I_ACTIVATE_SERVICE, ZIL_NULLP(void));

	// Process user responses.
	UI_APPLICATION::Control();

	// Clean up.
	delete _serviceManager;
	return (0);
}

