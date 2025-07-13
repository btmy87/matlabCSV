:: simple build script
@echo off
set MEXNAME=mexcsv
set MATLABROOT=C:\apps\MATLAB\R2025a\extern
:: cpp options from running mex with -n option
:: ommitted /GR, statically linking, /MT instead of MD
set CLOPTS=/nologo /c /Zc:__cplusplus /Zp8 /W3 /EHs
set CLOPTS=%CLOPTS% /MT /O2 /Oy- /DNDEBUG /Zi
::set CLOPTS=%CLOPTS% /MT /Od /DDEBUG /Zi
set CLOPTS=%CLOPTS% /DMATLAB_DEFAULT_RELEASE=R2017b  /DUSE_MEX_CMD   
set CLOPTS=%CLOPTS% /D_CRT_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_DEPRECATE 
set CLOPTS=%CLOPTS% /D_SECURE_SCL=0   /DMATLAB_MEX_FILE

set CLOPTS=%CLOPTS% /I%MATLABROOT%\include

set LINKOPTS=/nologo
set LINKOPTS=%LINKOPTS% /DLL
set LINKOPTS=%LINKOPTS% /OUT:mexListDlls.mexw64
set LINKOPTS=%LINKOPTS% /EXPORT:mexFunction
set LINKOPTS=%LINKOPTS% /EXPORT:mexfilerequiredapiversion
set LINKOPTS=%LINKOPTS% /DEBUG /OPT:NOREF /OPT:NOICF /INCREMENTAL:NO

set LINKLIBS=/LIBPATH:"%MATLABROOT%\lib\win64\microsoft"
set LINKLIBS=%LINKLIBS% libmex.lib libmx.lib libmat.lib 
set LINKLIBS=%LINKLIBS% libMatlabDataArray.lib libMatlabEngine.lib

@echo on
cl %CLOPTS% %MEXNAME%.cpp 
cl %CLOPTS% "%MATLABROOT%\version\cpp_mexapi_version.cpp" 
link %LINKOPTS% %MEXNAME%.obj cpp_mexapi_version.obj /out:%MEXNAME%.mexw64 %LINKLIBS%
del %MEXNAME%.exp %MEXNAME%.lib %MEXNAME%.ilk %MEXNAME%.obj cpp_mexapi_version.obj 