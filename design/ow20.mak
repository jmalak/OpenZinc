# DESIGN makefile
pname=design

template_dep_libs= &
	$(prfx)window.lib &
	$(prfx)file.lib &
	$(prfx)i18n.lib &
	$(prfx)help.lib &
	$(prfx)message.lib &
	$(prfx)image.lib &
	$(prfx)storage.lib &
	$(prfx)direct.lib &
	$(prfx)stredit.lib &
	$(prfx)service.lib

prfx=
d32_dep_libs=$+$(template_dep_libs)$-

prfx=w
win_dep_libs=$+$(template_dep_libs)$-

prfx=n
wnt_dep_libs=$+$(template_dep_libs)$-

prfx=9
w32_dep_libs=$+$(template_dep_libs)$-

prfx=o
os2_dep_libs=$+$(template_dep_libs)$-

prfx=c
crs_dep_libs=$+$(template_dep_libs)$-

prfx=m
mtf_dep_libs=$+$(template_dep_libs)$-

.EXTENSIONS:
.EXTENSIONS: .lib .rew .ren .re9 .reo .o32 .obw .obn .ob9 .obo .oc .om .rc .cpp .c

# Compiler and linker: (Add -d2 to CPP_OPTS and DEBUG ALL to LINK_OPTS for debug.)

CXX16=wpp -zq
CXX=wpp386 -zq
LINK=wlink op q,map
LIBRARIAN=wlib -q
RC=wrc -q

CXX_OPTS=-w4 -j -oaxt
LIB_OPTS=-pa -n
RC_OPTS=-r

# ----- DOS extender compiler options ---------------------------------------
D32_CXX_OPTS=-bt=dos -dDOS386 -w4 -j -oaxt
D32_LINK_OPTS=SYSTEM dos4g OP stack=48000 DISA 1124
D32_OBJS=
# --- Use the next line for UI_WCC_DISPLAY ---
D32_LIBS=d32_wcc.lib
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
#D32_LIBS=d32_gfx.lib wc_32gfx.lib

# ----- Windows compiler options --------------------------------------------
WIN_CXX_OPTS=-zW -ml -zc -zt=100
WIN_LINK_OPTS=SYS windows OP heapsize=16k OP stack=30k OP res=wdesign.rew
WIN_RC_OPTS=-bt=windows
WIN_OBJS=
WIN_LIBS=commdlg.lib

# ----- Windows NT (and WIN32s) compiler options ----------------------------
WNT_CXX_OPTS=-bt=nt
WNT_LINK_OPTS=SYSTEM nt_win op res=wdesign.ren
WNT_RC_OPTS=-bt=nt
WNT_OBJS=
WNT_LIBS=

# ----- Windows 32 (and WIN32s) compiler options ----------------------------
W32_CXX_OPTS=-bt=nt -DZIL_WIN32
W32_LINK_OPTS=SYSTEM nt_win op res=wdesign.re9
W32_RC_OPTS=-bt=nt
W32_OBJS=
W32_LIBS=

# ----- OS/2 compiler options -----------------------------------------------
OS2_CXX_OPTS=-bt=os2
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=96000 op res=odesign.reo
OS2_RC_OPTS=-bt=os2
OS2_OBJS=
OS2_LIBS=

# ----- Linux compiler options -----------------------------------------------
LNX_CXX_OPTS=-bt=linux
LNX_LINK_OPTS=SYSTEM linux
LNX_OBJS=
LNX_LIBS=

.cpp.o32:
	$(CXX) $(CXX_OPTS) $(D32_CXX_OPTS) -fo=$@ $<

.cpp.obw:
	$(CXX16) $(CXX_OPTS) $(WIN_CXX_OPTS) -fo=$@ $<

.cpp.obn:
	$(CXX) $(CXX_OPTS) $(WNT_CXX_OPTS) -fo=$@ $<

.cpp.ob9:
	$(CXX) $(CXX_OPTS) $(W32_CXX_OPTS) -fo=$@ $<

.cpp.obo:
	$(CXX) $(CXX_OPTS) $(OS2_CXX_OPTS) -fo=$@ $<

.cpp.om:
	$(CXX) $(CXX_OPTS) $(LNX_CXX_OPTS) -fo=$@ $<

.cpp.oc:
	$(CXX) $(CXX_OPTS) $(LNX_CXX_OPTS) -fo=$@ $<

.rc.rew:
	$(RC) $(RC_OPTS) $(WIN_RC_OPTS) $< -fo=$@

.rc.ren:
	$(RC) $(RC_OPTS) $(WNT_RC_OPTS) $< -fo=$@

.rc.re9:
	$(RC) $(RC_OPTS) $(W32_RC_OPTS) $< -fo=$@

.rc.reo:
	$(RC) $(RC_OPTS) $(OS2_RC_OPTS) $< -fo=$@

# ----- Usage --------------------------------------------------------------
usage: .SYMBOLIC
	@echo ...........
	@echo ...........
	@echo To make the Zinc designer for Watcom C++ type:
	@echo wmake -h -f ow20.mak dos32	(makes the DOS designer)
	@echo wmake -h -f ow20.mak windows	(makes the 16-bit Windows designer)
	@echo wmake -h -f ow20.mak winnt	(makes the Windows NT designer)
	@echo wmake -h -f ow20.mak win32	(makes the Windows WIN32 designer)
	@echo wmake -h -f ow20.mak os2		(makes the OS/2 designer)
	@echo wmake -h -f ow20.mak curses	(makes the Linux Curses designer)
	@echo wmake -h -f ow20.mak motif	(makes the Linux Motif designer)
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules
	rm -f foo *.bak *.bk? *.ob? *.rbj *.map *.exe *.tc tc*.* *.dsk *.dpr
	rm -f *.sav *.sv? *.cfg *.$$$$$$ *.lib *.fil *.re? *.sym *.err *.zip *.ovl
	rm -f *.o16 *.o32 *.p16 *.p32
	rm -f ../bin/$(pname).exe
	rm -f ../bin/w$(pname).exe
	rm -f ../bin/n$(pname).exe
	rm -f ../bin/9$(pname).exe
	rm -f ../bin/o$(pname).exe

# ----- DOS extender --------------------------------------------------------
dos32: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules
	%make $(pname).exe

$(pname).exe: main.o32
	$(LINK) $(D32_LINK_OPTS) N $@ F main.o32 L {$(d32_dep_libs) d32_zil.lib $(D32_LIBS)}
	%copy $@ ../bin

make_dos32_modules: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules

# ----- Windows -------------------------------------------------------------
windows: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules
	%make w$(pname).exe

w$(pname).exe: main.obw wdesign.rew
	$(LINK) $(WIN_LINK_OPTS) N $@ F main.obw L {$(win_dep_libs) win_zil.lib $(WIN_LIBS)}
	%copy $@ ../bin

make_windows_modules: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules

# ----- Windows NT (and WIN32s) ---------------------------------------------
winnt: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules
	%make n$(pname).exe

n$(pname).exe: main.obn wdesign.ren
	$(LINK) $(WNT_LINK_OPTS) N $@ F main.obn L {$(wnt_dep_libs) wnt_zil.lib $(WNT_LIBS)}
	%copy $@ ../bin

make_winnt_modules: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules

# ----- 32 bit Windows ------------------------------------------------------
win32: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules
	%make 9$(pname).exe

9$(pname).exe: main.ob9 wdesign.re9
	$(LINK) $(W32_LINK_OPTS) N $@ F main.ob9 L {$(w32_dep_libs) w32_zil.lib $(W32_LIBS)}
	%copy $@ ../bin

make_win32_modules: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules

# ----- OS/2 ----------------------------------------------------------------
os2: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules
	%make o$(pname).exe

o$(pname).exe: main.obo odesign.reo
	$(LINK) $(OS2_LINK_OPTS) N $@ F main.obo L {$(os2_dep_libs) os2_zil.lib $(OS2_LIBS)}
	%copy $@ ../bin

make_os2_modules: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules

# ----- Linux Libraries and Programs (Curses) --------------------------------
curses: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules
	%make c$(pname).exe

c$(pname).exe: main.oc
	$(LINK) $(LNX_LINK_OPTS) N $@ F main.oc L {$(crs_dep_libs) crs_zil.lib $(LNX_LIBS)}
	%copy $@ ../bin

make_curses_modules: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules

# ----- Linux Libraries and Programs (Motif) --------------------------------
motif: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules
	%make m$(pname).exe

m$(pname).exe: main.om
	$(LINK) $(LNX_LINK_OPTS) N $@ F main.om L {$(mtf_dep_libs) mtf_zil.lib $(LNX_LIBS)}
	%copy $@ ../bin

make_motif_modules: .SYMBOLIC
        set MODULES_FUNC=$@
	%make make_modules

# ---------------------------------------------------------------------------

make_modules: .EXPLICIT .PROCEDURE
#	----- Level 1 Applications -----
	cd service
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
#	----- Level 2 Applications -----
	cd direct
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
	cd storage
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
	cd stredit
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
#	----- Level 3 Applications -----
	cd help
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
	cd message
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
	cd image
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
	cd i18n
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
	cd file
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
	cd window
	wmake -h -f ow20.mak $(%MODULES_FUNC)
	@cd ..
