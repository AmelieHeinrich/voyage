@echo off

set rootDir=%cd%
if not exist build (
    mkdir build
)

set debug=true

if %debug%==true (
    echo Compiling in debug mode.
    echo.

    set debugFlags = -DGAME_DEBUG
    set entryPoint = /link /subsystem:CONSOLE
    set link_path=/link /LIBPATH:%rootDir%/bin/dbg    
    xcopy "bin/dbg/assimp-vc143-mtd.dll" "build" 
) else (
    echo Compiling in release mode.
    echo.

    set debugFlags=-DNDEBUG -O2 -Oi -fp:fast
    set entryPoint = /link /subsystem:WINDOWS
    set link_path=/link /LIBPATH:%rootDir%/bin/dbg
    xcopy "bin/dbg/assimp-vc143-mtd.dll" "build" 
)

set output=game
set flags=-nologo -FC -Zi -WX -W4
set disabledWarnings=-wd4100 -wd4201 -wd4018 -wd4099 -wd4189 -wd4505 -wd4530 -wd4840 -wd4324 -wd4459 -wd4702 -wd4244 -wd4310 -wd4611
set source= %rootDir%/third_party/sol/*.c %rootDir%/sp/*.cpp %rootDir%/sp/video/*.cpp %rootDir%/sp/material/*.cpp %rootDir%/sp/player/*.cpp %rootDir%/sp/audio/*.cpp %rootDir%/sp/script/*.cpp %rootDir%/sp/entity/*.cpp
set links=user32.lib d3d11.lib d3dcompiler.lib dxgi.lib assimp-vc143-mtd.lib
set includeDirs= -I%rootDir%/third_party

pushd build
cl %disabledWarnings% %includeDirs% %debugFlags% %flags% -Fe%output% %source% /std:c++latest %links% %entryPoint% %link_path%
popd

echo.
echo Build finished.