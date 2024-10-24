# ----- General Definitions -------------------------------------------------
VERSION=ow20
pname=direct
exename=test
datname=p_direct

template_lib_objs =

template_dep_libs = &
	$(prfx)direct.lib &
	$(prfx)service.lib

oext=.o32
prfx=
d32_lib_objs=$+d_$(pname)$(oext)$-
d32_dep_libs=$+$(template_dep_libs)$-

oext=.obw
prfx=w
win_lib_objs=$+w_$(pname)$(oext)$-
win_dep_libs=$+$(template_dep_libs)$-

oext=.obn
prfx=n
wnt_lib_objs=$+w_$(pname)$(oext)$-
wnt_dep_libs=$+$(template_dep_libs)$-

oext=.ob9
prfx=9
w32_lib_objs=$+w_$(pname)$(oext)$-
w32_dep_libs=$+$(template_dep_libs)$-

oext=.obo
prfx=o
os2_lib_objs=$+o_$(pname)$(oext)$-
os2_dep_libs=$+$(template_dep_libs)$-

oext=.oc
prfx=c
crs_lib_objs=$+m_$(pname)$(oext)$-
crs_dep_libs=$+$(template_dep_libs)$-

oext=.om
prfx=m
mtf_lib_objs=$+m_$(pname)$(oext)$-
mtf_dep_libs=$+$(template_dep_libs)$-

.EXTENSIONS:
.EXTENSIONS: .o32 .obw .obn .ob9 .obo .oc .om .cpp

CXX16=wpp -zq
CXX=wpp386 -zq
LINK=wlink op q,map
LIBRARIAN=wlib -q
RC=wrc -q

CXX_OPTS=-w4 -j -oaxt -I. -I"../include" -I"../../include"
LINK_OPTS=libpath '../lib/$(VERSION)' libpath '../../lib/$(VERSION)'
LIB_OPTS=-pa -n
RC_OPTS=-r

# ----- DOS extender compiler options ---------------------------------------
D32_CXX_OPTS=-bt=dos
#D32_LINK_OPTS=SYSTEM dos4g OP stack=20000
D32_LINK_OPTS=SYSTEM causeway OP stack=20000
D32_OBJS=
# --- Use the next line for UI_WCC_DISPLAY ---
D32_LIBS=d32_wcc.lib
# --- Use the next line for UI_GRAPHICS_DISPLAY ---
#D32_LIBS=d32_gfx.lib wc_32gfx.lib

# ----- Windows compiler options --------------------------------------------
WIN_CXX_OPTS=-zW -zc -ml -zt=100
WIN_LINK_OPTS=SYSTEM windows OP heapsize=16k OP stack=30k
WIN_RC_OPTS=-bt=windows
WIN_OBJS=
WIN_LIBS=commdlg.lib

# ----- Windows NT (and WIN32s) compiler options ----------------------------
WNT_CXX_OPTS=-bt=nt
WNT_LINK_OPTS=SYSTEM nt_win
WNT_RC_OPTS=-bt=nt
WNT_OBJS=
WNT_LIBS=

# ----- Windows 32 bit compiler options -------------------------------------
W32_CXX_OPTS=-bt=nt -DZIL_WIN32
W32_RC_OPTS=-bt=nt
W32_LINK_OPTS=SYSTEM nt_win
W32_OBJS=
W32_LIBS=

# ----- OS/2 compiler options -----------------------------------------------
OS2_CXX_OPTS=-bt=os2
OS2_LINK_OPTS=SYSTEM os2v2_pm OP ST=96000
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

.cpp.oc:
	$(CXX) $(CXX_OPTS) $(LNX_CXX_OPTS) -DZIL_CURSES -fo=$@ $<

.cpp.om:
	$(CXX) $(CXX_OPTS) $(LNX_CXX_OPTS) -DZIL_MOTIF -fo=$@ $<

# ----- Clean ---------------------------------------------------------------
clean: .SYMBOLIC
	rm -f foo *.bak *.bk? *.ob? *.rbj *.map *.exe *.tc tc*.* *.dsk *.dpr
	rm -f *.sav *.sv? *.cfg *.$$$$$$ *.lib *.fil *.res *.sym *.err *.zip *.ovl
	rm -f *.o16 *.o32 *.p16 .oc .om *.p32
	rm -f ../../bin/$(datname).znc
	rm -f ../../lib/$(VERSION)/$(pname).lib
	rm -f ../../lib/$(VERSION)/w$(pname).lib
	rm -f ../../lib/$(VERSION)/n$(pname).lib
	rm -f ../../lib/$(VERSION)/9$(pname).lib
	rm -f ../../lib/$(VERSION)/o$(pname).lib
	rm -f ../../lib/$(VERSION)/c$(pname).lib
	rm -f ../../lib/$(VERSION)/m$(pname).lib
	rm -f ../../include/$(pname).hpp

copy_out: .PROCEDURE .EXPLICIT
	%copy $@ ../../lib/$(VERSION)
	%copy $(pname).hpp ../../include
	%copy $(datname).dat ../../bin/$(datname).znc

# ----- DOS extender --------------------------------------------------------
dos32: $(exename).exe .SYMBOLIC

$(exename).exe: main.o32 $(pname).lib
	$(LINK) $(LINK_OPTS) $(D32_LINK_OPTS) N $@ F main.o32 L {$(d32_dep_libs) d32_zil.lib $(D32_LIBS)}

make_dos32_modules: $(pname).lib .SYMBOLIC

$(pname).lib : $(d32_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- Windows -------------------------------------------------------------
windows: w$(exename).exe .SYMBOLIC

w$(exename).exe: main.obw w$(pname).lib
	$(LINK) $(LINK_OPTS) $(WIN_LINK_OPTS) N $@ F main.obw L {$(win_dep_libs) win_zil.lib $(WIN_LIBS)}

make_windows_modules: w$(pname).lib .SYMBOLIC

w$(pname).lib : $(win_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- Windows NT (and WIN32s) ---------------------------------------------
winnt: n$(exename).exe .SYMBOLIC

n$(exename).exe: main.obn n$(pname).lib
	$(LINK) $(LINK_OPTS) $(WNT_LINK_OPTS) N $@ F main.obn L {$(wnt_dep_libs) wnt_zil.lib $(WNT_LIBS)}

make_winnt_modules: n$(pname).lib .SYMBOLIC

n$(pname).lib : $(wnt_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- Windows 32 (and WIN32s) ---------------------------------------------
win32: 9$(exename).exe .SYMBOLIC

9$(exename).exe: main.ob9 9$(pname).lib
	$(LINK) $(LINK_OPTS) $(W32_LINK_OPTS) N $@ F main.ob9 L {$(w32_dep_libs) w32_zil.lib $(W32_LIBS)}

make_win32_modules: 9$(pname).lib .SYMBOLIC

9$(pname).lib : $(w32_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- OS/2 ----------------------------------------------------------------
os2: o$(exename).exe .SYMBOLIC

o$(exename).exe: main.obo o$(pname).lib
	$(LINK) $(LINK_OPTS) $(OS2_LINK_OPTS) N $@ F main.obo L {$(os2_dep_libs) os2_zil.lib $(OS2_LIBS)}

make_os2_modules: o$(pname).lib .SYMBOLIC

o$(pname).lib : $(os2_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- Linux Curses ----------------------------------------------------------------
curses: c$(exename).exe .SYMBOLIC

c$(exename).exe: main.oc c$(pname).lib
	$(LINK) $(LINK_OPTS) $(LNX_LINK_OPTS) N $@ F main.oc L {$(crs_dep_libs) crs_zil.lib $(LNX_LIBS)}

make_curses_modules: c$(pname).lib .SYMBOLIC

c$(pname).lib : $(crs_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out

# ----- Linux Motif ----------------------------------------------------------------
motif: m$(exename).exe .SYMBOLIC

m$(exename).exe: main.om m$(pname).lib
	$(LINK) $(LINK_OPTS) $(LNX_LINK_OPTS) N $@ F main.om L {$(mtf_dep_libs) mtf_zil.lib $(LNX_LIBS)}

make_motif_modules: m$(pname).lib .SYMBOLIC

m$(pname).lib : $(mtf_lib_objs)
	$(LIBRARIAN) $(LIB_OPTS) $@ $<
	%make copy_out
