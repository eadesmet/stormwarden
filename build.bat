@echo off

REM set D3dInclude="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include"
REM set D3dLib="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x64"
REM set LibFiles=user32.lib gdi32.lib winmm.lib kernel32.lib d3d11.lib d3dcompiler.lib
REM set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -FC -Z7
REM set CommonLinkerFlags= -incremental:no -opt:ref


set COMMON_COMPILER_FLAGS=/nologo /EHa- /GR- /fp:fast /Oi /W4 /Fm /FC

set DEBUG_FLAGS=/DDEBUG_BUILD /Od /MTd /Zi
set RELEASE_FLAGS =/O2

set COMPILER_FLAGS=%COMMON_COMPILER_FLAGS% %DEBUG_FLAGS%
REM set COMPILER_FLAGS=%COMMON_COMPILER_FLAGS% %RELEASE_FLAGS%

set LINKER_FLAGS=/INCREMENTAL:NO /opt:ref
set SYSTEM_LIBS=user32.lib gdi32.lib winmm.lib d3d11.lib d3dcompiler.lib

set BUILD_DIR=".\build"
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
pushd %BUILD_DIR%

cl %COMPILER_FLAGS% ..\code\win32.cpp /link %LINKER_FLAGS% %SYSTEM_LIBS%


popd
echo Build Complete.