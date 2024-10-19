setlocal
pushd source
wmake -f ow20.mak -h clean
popd
endlocal
setlocal
pushd design
wmake -f ow20.mak -h clean
popd
endlocal
