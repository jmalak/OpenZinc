#include <ui_win.hpp>

#if defined(ZIL_UNICODE)
# define COMCTLLIB		L"comctl32.dll"
#else
# define COMCTLLIB		"comctl32.dll"
#endif

void InitCommonControls(void)
{
	LoadModule(COMCTLLIB, ZIL_NULLP(void));
}

