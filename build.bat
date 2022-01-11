@echo off

set rootDir=%cd%
mkdir build

set debug=false

if %debug%==true (
    echo Compiling in debug mode.
    echo.

    set debugFlags=-DGAME_DEBUG
) else (
    echo Compiling in release mode.
    echo.

    set debugFlags=-DNDEBUG -O2 -Oi -fp:fast
)

set output=game
set flags=-nologo -FC -Zi -WX -W4
set disabledWarnings=-wd4100 -wd4201 -wd4018 -wd4099 -wd4189 -wd4505
set source= %rootDir%/sp/*.cpp
set links=user32.lib d3d11.lib d3dcompiler.lib dxgi.lib

pushd build
cl %disabledWarnings% %includeDirs% %debugFlags% %flags% -Fe%output% %source% %links%
popd

echo.
echo Build finished.