#if !defined(WM_NOTIFY)
#	define WM_NOTIFY 0x004E
	typedef struct tagNMHDR
	{
		HWND hwndFrom;
		UINT idFrom;
		UINT code;
	} NMHDR;
#endif

#if !defined(TOOLBARCLASSNAME)
#	ifdef _WIN32
#		define TOOLBARCLASSNAMEW		L"ToolbarWindow32"
#		define TOOLBARCLASSNAMEA		"ToolbarWindow32"
#		ifdef  UNICODE
#			define TOOLBARCLASSNAME		TOOLBARCLASSNAMEW
#		else
#			define TOOLBARCLASSNAME		TOOLBARCLASSNAMEA
#		endif
#	else
#		define TOOLBARCLASSNAME			"ToolbarWindow"
#	endif
#endif

#if !defined(TRACKBAR_CLASS)
#	ifdef _WIN32
#		define TRACKBAR_CLASSA         "msctls_trackbar32"
#		define TRACKBAR_CLASSW         L"msctls_trackbar32"
#		ifdef UNICODE
#			define  TRACKBAR_CLASS         TRACKBAR_CLASSW
#		else
#			define  TRACKBAR_CLASS         TRACKBAR_CLASSA
#		endif
#	else
#		define TRACKBAR_CLASS          "msctls_trackbar"
#	endif
#	define TBS_AUTOTICKS           0x0001
#	define TBS_VERT                0x0002
#	define TBS_HORZ                0x0000
#	define TBS_TOP                 0x0004
#	define TBS_BOTTOM              0x0000
#	define TBS_LEFT                0x0004
#	define TBS_RIGHT               0x0000
#	define TBS_BOTH                0x0008
#	define TBS_NOTICKS             0x0010
#	define TBS_ENABLESELRANGE      0x0020
#	define TBS_FIXEDLENGTH         0x0040
#	define TBS_NOTHUMB             0x0080
#	define TBM_GETPOS              (WM_USER)
#	define TBM_GETRANGEMIN         (WM_USER+1)
#	define TBM_GETRANGEMAX         (WM_USER+2)
#	define TBM_GETTIC              (WM_USER+3)
#	define TBM_SETTIC              (WM_USER+4)
#	define TBM_SETPOS              (WM_USER+5)
#	define TBM_SETRANGE            (WM_USER+6)
#	define TBM_SETRANGEMIN         (WM_USER+7)
#	define TBM_SETRANGEMAX         (WM_USER+8)
#	define TBM_CLEARTICS           (WM_USER+9)
#	define TBM_SETSEL              (WM_USER+10)
#	define TBM_SETSELSTART         (WM_USER+11)
#	define TBM_SETSELEND           (WM_USER+12)
#	define TBM_GETPTICS            (WM_USER+14)
#	define TBM_GETTICPOS           (WM_USER+15)
#	define TBM_GETNUMTICS          (WM_USER+16)
#	define TBM_GETSELSTART         (WM_USER+17)
#	define TBM_GETSELEND           (WM_USER+18)
#	define TBM_CLEARSEL            (WM_USER+19)
#	define TBM_SETTICFREQ          (WM_USER+20)
#	define TBM_SETPAGESIZE         (WM_USER+21)
#	define TBM_GETPAGESIZE         (WM_USER+22)
#	define TBM_SETLINESIZE         (WM_USER+23)
#	define TBM_GETLINESIZE         (WM_USER+24)
#	define TBM_GETTHUMBRECT        (WM_USER+25)
#	define TBM_GETCHANNELRECT      (WM_USER+26)
#	define TBM_SETTHUMBLENGTH      (WM_USER+27)
#	define TBM_GETTHUMBLENGTH      (WM_USER+28)
#endif

#if !defined(UPDOWN_CLASS)
#	ifdef _WIN32
#		define UPDOWN_CLASSA           "msctls_updown32"
#		define UPDOWN_CLASSW           L"msctls_updown32"
#		ifdef UNICODE
#			define  UPDOWN_CLASS           UPDOWN_CLASSW
#		else
#			define  UPDOWN_CLASS           UPDOWN_CLASSA
#		endif
#	else
#		define UPDOWN_CLASS            "msctls_updown"
#	endif
#	define UDS_WRAP                0x0001
#	define UDS_SETBUDDYINT         0x0002
#	define UDS_ALIGNRIGHT          0x0004
#	define UDS_ALIGNLEFT           0x0008
#	define UDS_AUTOBUDDY           0x0010
#	define UDS_ARROWKEYS           0x0020
#	define UDS_HORZ                0x0040
#	define UDS_NOTHOUSANDS         0x0080
#	define UDM_SETRANGE            (WM_USER+101)
#	define UDM_GETRANGE            (WM_USER+102)
#	define UDM_SETPOS              (WM_USER+103)
#	define UDM_GETPOS              (WM_USER+104)
#	define UDM_SETBUDDY            (WM_USER+105)
#	define UDM_GETBUDDY            (WM_USER+106)
#	define UDM_SETACCEL            (WM_USER+107)
#	define UDM_GETACCEL            (WM_USER+108)
#	define UDM_SETBASE             (WM_USER+109)
#	define UDM_GETBASE             (WM_USER+110)
#endif


#if !defined(COLOR_3DDKSHADOW)
#	define COLOR_3DDKSHADOW			21
#endif
#if !defined(COLOR_3DLIGHT)
#	define COLOR_3DLIGHT			22
#endif
#if !defined(COLOR_3DSHADOW)
#	define COLOR_3DSHADOW			COLOR_BTNSHADOW
#endif
#if !defined(COLOR_3DHIGHLIGHT)
#	define COLOR_3DHIGHLIGHT		COLOR_BTNHIGHLIGHT
#endif


VOID WINAPI InitCommonControls(VOID);
