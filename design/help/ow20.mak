# ----- General Definitions -------------------------------------------------
VERSION=ow20
pname=help
exename=hlpedit
datname=p_help

template_lib_objs = &
	$(pname)$(oext) &
	$(pname)1$(oext) &
	import$(oext) &
	export$(oext)

template_dep_libs = &
	$(prfx)help.lib &
	$(prfx)storage.lib &
	$(prfx)direct.lib &
	$(prfx)stredit.lib &
	$(prfx)service.lib

oext=.o32
prfx=
d32_lib_objs=$+$(template_lib_objs)$-
d32_dep_libs=$+$(template_dep_libs)$-

oext=.obw
prfx=w
win_lib_objs=$+$(template_lib_objs)$-
win_dep_libs=$+$(template_dep_libs)$-

oext=.obn
prfx=n
wnt_lib_objs=$+$(template_lib_objs)$-
wnt_dep_libs=$+$(template_dep_libs)$-

oext=.ob9
prfx=9
w32_lib_objs=$+$(template_lib_objs)$-
w32_dep_libs=$+$(template_dep_libs)$-

oext=.obo
prfx=o
os2_lib_objs=$+$(template_lib_objs)$-
os2_dep_libs=$+$(template_dep_libs)$-

.EXTENSIONS:
.EXTENSIONS: .o32 .obw .obn .ob9 .obo .cpp

CXX16=wpp -zq
CXX=wpp386 -zq
LINK=wlink op q,map
LIBRARIAN=wlib -q
RC=wrc -q -r

CXX_OPTS=-w4 -j -oaxt -I. -I"../include" -I"../../include"
LINK_OPTS=libpath '../lib/$(VERSION)' libpath '../../lib/$(VERSION)'
LIB_OPTS=-pa -n

# ----- DOS extender compiler options ---------------------------------------
D32_CXX_OPTS=-bt=dos
#D32_LINK_OPTS=SYSTEM dos4g OP stack=20000
D32_LINK_OPTS=SYSTEM causeway OP stack=20000
D32_OBJS=
# --- Use the next line for UI_WCC_DISPLAY ---
D32_LIBS=d32_zil.lib d32_wcc.lib
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
#D32_LIBS=d32_zil.lib d32_gfx.lib wc_32gfx.lib

# ----- Windows compiler options --------------------------------------------
WIN_CXX_OPTS=-zW -ml -zc -zt=100
WIN_LINK_OPTS=SYSTEM windows OP heapsize=16k OP stack=30k
WIN_RC_OPTS=-bt=windows
WIN_OBJS=
WIN_LIBS=win_zil.lib commdlg.lib

# ----- Windows NT (and WIN32s) compiler options ----------------------------
WNT_CXX_OPTS=-bt=nt
WNT_LINK_OPTS=SYSTEM nt_win
WNT_RC_OPTS=-bt=nt
WNT_OBJS=
WNT_LIBS=wnt_zil.lib

# ----- Windows 32 (and WIN32s) compiler options ----------------------------
W32_CXX_OPTS=-bt=nt -DZIL_WIN32
W32_LINK_OPTS=SYSTEM nt_win
W32_RC_OPTS=-bt=nt
W32_OBJS=
W32_LIBS=w32_zil.lib

# ----- OS/2 compiler options -----------------------------------------------
OS2_CXX_OPTS=-bt=os2
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=96000
OS2_RC_OPTS=-bt=os2
OS2_OBJS=
OS2_LIBS=os2_zil.lib

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

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
	rm -f foo *.bak *.bk? *.ob? *.rbj *.map *.exe *.tc tc*.* *.dsk *.dpr
	rm -f *.sav *.sv? *.cfg *.$$$$$$ *.lib *.fil *.res *.sym *.err *.zip *.ovl
	rm -f *.o16 *.o32 *.p16 *.p32 __tmp.rsp
	rm -f ../lib/$(VERSION)/$(pname).lib
	rm -f ../lib/$(VERSION)/w$(pname).lib
	rm -f ../lib/$(VERSION)/n$(pname).lib
	rm -f ../lib/$(VERSION)/9$(pname).lib
	rm -f ../lib/$(VERSION)/o$(pname).lib
	rm -f ../include/$(pname).hpp
	rm -f ../../bin/$(datname).znc
	rm -f ../../bin/$(exename).exe
	rm -f ../../bin/w$(exename).exe
	rm -f ../../bin/n$(exename).exe
	rm -f ../../bin/9$(exename).exe
	rm -f ../../bin/o$(exename).exe

copy_out: .PROCEDURE .EXPLICIT
	%copy $@ ../lib/$(VERSION)
	%copy $(pname).hpp ../include
	%copy $(datname).dat ../../bin/$(datname).znc

# ----- DOS extender --------------------------------------------------------
dos32: $(exename).exe .SYMBOLIC

$(exename).exe: main.o32 $(pname).lib
	$(LINK) $(LINK_OPTS) $(D32_LINK_OPTS) N $@ F main.o32 L {$(d32_dep_libs) $(D32_LIBS)}
	%copy $@ ../../bin

make_dos32_modules: $(pname).lib .SYMBOLIC

$(pname).lib : $(d32_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- Windows -------------------------------------------------------------
windows: w$(exename).exe .SYMBOLIC

w$(exename).exe: main.obw w$(pname).lib
	$(LINK) $(LINK_OPTS) $(WIN_LINK_OPTS) N $@ F main.obw L {$(win_dep_libs) $(WIN_LIBS)}
	%copy $@ ../../bin

make_windows_modules: w$(pname).lib .SYMBOLIC

w$(pname).lib : $(win_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- Windows NT (and WIN32s) ---------------------------------------------
winnt: n$(exename).exe .SYMBOLIC

n$(exename).exe: main.obn n$(pname).lib
	$(LINK) $(LINK_OPTS) $(WNT_LINK_OPTS) N $@ F main.obn L {$(wnt_dep_libs) $(WNT_LIBS)}
	%copy $@ ../../bin

make_winnt_modules: n$(pname).lib .SYMBOLIC

n$(pname).lib : $(wnt_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- Windows 32 (and WIN32s) ---------------------------------------------
win32: 9$(exename).exe .SYMBOLIC

9$(exename).exe: main.ob9 9$(pname).lib
	$(LINK) $(LINK_OPTS) $(W32_LINK_OPTS) N $@ F main.ob9 L {$(w32_dep_libs) $(W32_LIBS)}
	%copy $@ ../../bin

make_win32_modules: 9$(pname).lib .SYMBOLIC

9$(pname).lib : $(w32_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- OS/2 ----------------------------------------------------------------
os2: o$(exename).exe .SYMBOLIC

o$(exename).exe: main.obo o$(pname).lib
	$(LINK) $(LINK_OPTS) $(OS2_LINK_OPTS) N $@ F main.obo L {$(os2_dep_libs) $(OS2_LIBS)}
	%copy $@ ../../bin

make_os2_modules: o$(pname).lib .SYMBOLIC

o$(pname).lib : $(os2_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out
