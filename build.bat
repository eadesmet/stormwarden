@echo off

REM set d3d_libs=d3d11.lib d3dcompiler.lib dxguid.lib dxgi.lib

REM NOTE(Eric): Changed "/DEBUG:fastlink" to "/DEBUG:FULL" for debugging
REM NOTE(Eric): Also had to add -PDB:... to get debugging to work

set application_name=app
set build_options= -DBUILD_WIN32=1
set compile_flags= -nologo /Od /Zi /FC /I ../source/
set common_link_flags= opengl32.lib -opt:ref -incremental:no /DEBUG:FULL
set platform_link_flags= gdi32.lib user32.lib winmm.lib %common_link_flags%

REM NOTE(Eric): Shader Compilation
pushd source\shaders

start /b /wait "" "fxc.exe"  /nologo /T vs_5_0 /E vs /Od /WX /Zpc /Ges /Fh d3d11_vshader.h /Vn d3d11_vshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv shaders.hlsl

start /b /wait "" "fxc.exe"  /nologo /T ps_5_0 /E ps /Od /WX /Zpc /Ges /Fh d3d11_pshader.h /Vn d3d11_pshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv shaders.hlsl
popd

if not exist build mkdir build
pushd build
start /b /wait "" "cl.exe"  %build_options% %compile_flags% ../source/win32/win32_main.c /link %platform_link_flags% /out:%application_name%.exe -PDB:win32.pdb
start /b /wait "" "cl.exe"  %build_options% %compile_flags% ../source/app.c /LD /link %common_link_flags% /out:%application_name%.dll -PDB:game.pdb
popd