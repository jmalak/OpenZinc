setlocal
@set OZINCPATH=%CD%\INCLUDE
@set LIB=.;%CD%\LIB\ow20
pushd source
wmake -f ow20.mak -h clean
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
popd
endlocal
