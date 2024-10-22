setlocal
pushd source
wmake -f ow20.mak -h clean
@set INCLUDE=.;%WATCOM%\H
wmake -f ow20.mak -h dos32
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\WIN
wmake -f ow20.mak -h windows
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\NT
wmake -f ow20.mak -h winnt
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\NT
wmake -f ow20.mak -h win32
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\OS2
wmake -f ow20.mak -h os2
rem @set INCLUDE=.;%WATCOM%\LH
rem wmake -f ow20.mak -h curses
rem wmake -f ow20.mak -h motif
popd
endlocal
setlocal
@set OZINCPATH=%CD%\INCLUDE
@set LIB=.;%CD%\LIB\ow20
pushd design
@set INCLUDE=%OLD_INCLUDE%
wmake -f ow20.mak -h clean
@set INCLUDE=.;%WATCOM%\H;%OZINCPATH%
wmake -f ow20.mak -h make_dos32_modules
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\WIN;%OZINCPATH%
wmake -f ow20.mak -h make_windows_modules
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\NT;%OZINCPATH%
wmake -f ow20.mak -h make_winnt_modules
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\NT;%OZINCPATH%
wmake -f ow20.mak -h make_win32_modules
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\OS2;%OZINCPATH%
wmake -f ow20.mak -h make_os2_modules
rem @set INCLUDE=.;%WATCOM%\LH;%OZINCPATH%
rem wmake -f ow20.mak -h make_curses_modules
rem wmake -f ow20.mak -h make_motif_modules
rem
@set INCLUDE=.;%WATCOM%\H;%OZINCPATH%
wmake -f ow20.mak -h dos32
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\WIN;%OZINCPATH%
wmake -f ow20.mak -h windows
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\NT;%OZINCPATH%
wmake -f ow20.mak -h winnt
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\NT;%OZINCPATH%
wmake -f ow20.mak -h win32
@set INCLUDE=.;%WATCOM%\H;%WATCOM%\H\OS2;%OZINCPATH%
wmake -f ow20.mak -h os2
rem @set INCLUDE=.;%WATCOM%\LH;%OZINCPATH%
rem wmake -f ow20.mak -h curses
rem wmake -f ow20.mak -h motif
popd
endlocal
