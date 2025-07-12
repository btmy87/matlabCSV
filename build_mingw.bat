:: simple build script
@echo off

where g++
if not errorlevel 1 goto have_gcc

set path=%path%;%MW_MINGW64_LOC%\bin
set path=%path%;%MW_MINGW64_LOC%\opt\bin
set path=%path%;%MW_MINGW64_LOC%\x86_64-w64-mingw32\bin

:have_gcc

set MEXNAME=mexcsv
set MATLABROOT=C:\apps\MATLAB\R2025a\extern
:: cpp options from running mex with -n option
set CLOPTS=-c -m64 -fexceptions -fno-omit-frame-pointer -O2 -fwrapv -DNDEBUG 
set CLOPTS=%CLOPTS% -DMATLAB_DEFAULT_RELEASE=R2017b  -DUSE_MEX_CMD   
set CLOPTS=%CLOPTS% -DMATLAB_MEX_FILE

set CLOPTS=%CLOPTS% -I %MATLABROOT%\include
set CLOPTS=%CLOPTS% -I %MW_MINGW64_LOC%\include
set CLOPTS=%CLOPTS% -I %MW_MINGW64_LOC%\x86_64-w64-mingw32\include

set LINKOPTS=-m64 -Wl,--no-undefined -shared -static -s
set LINKOPTS=%LINKOPTS% -Wl,"%MATLABROOT%\lib\win64\mingw64\exportsmexfileversion.def"
set LINKOPTS=%LINKOPTS% -Wl,--format=binary -Wl,--format=default 

set LINKLIBS=-L"%MATLABROOT%\lib\win64\mingw64" 
set LINKLIBS=%LINKLIBS% -L"%MW_MINGW64_LOC%\x86_64-w64-mingw32\lib"
set LINKLIBS=%LINKLIBS% -llibmx -llibmex -llibmat -lm -llibmwlapack -llibmwblas
set LINKLIBS=%LINKLIBS% -llibMatlabDataArray -llibMatlabEngine
set LINKLIBS=%LINKLIBS% -lpsapi

@echo on
g++ %CLOPTS% %MEXNAME%.cpp -o %MEXNAME%.obj
g++ %CLOPTS% "%MATLABROOT%\version\cpp_mexapi_version.cpp" -o cpp_mexapi_version.obj

g++ %LINKOPTS%  %MEXNAME%.obj cpp_mexapi_version.obj -o %MEXNAME%.mexw64 %LINKLIBS%
del %MEXNAME%.exp %MEXNAME%.lib %MEXNAME%.ilk %MEXNAME%.obj cpp_mexapi_version.obj