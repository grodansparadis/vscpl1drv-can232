@echo off
REM Setup script for vscpl1drv-can232 CMake build on Windows
REM This script initializes dependencies and prepares the build environment

setlocal enabledelayedexpansion

echo Setting up vscpl1drv-can232 CMake build environment (Windows)...
echo.

REM Check if we're in the right directory
if not exist "CMakeLists.txt" (
    echo Error: Please run this script from the project root directory
    exit /b 1
)

if not exist "VERSION.m4" (
    echo Error: VERSION.m4 not found. Are you in the right directory?
    exit /b 1
)

REM Initialize git submodules
echo Initializing VSCP submodule...
call git submodule update --init --recursive
if errorlevel 1 (
    echo Error: Failed to initialize submodules
    exit /b 1
)
echo [Ok] Submodule initialized
echo.

REM Check if CMake is available
where cmake >nul 2>nul
if errorlevel 1 (
    echo Error: CMake not found. Please install CMake from https://cmake.org/download/
    echo Make sure to add CMake to system PATH.
    exit /b 1
)
echo [Ok] CMake found: 
cmake --version | findstr /R "cmake version"
echo.

REM Create build directory
if not exist "build" (
    echo Creating build directory...
    mkdir build
    echo [Ok] Build directory created
) else (
    echo Build directory already exists
)
echo.

REM Detect Visual Studio version
echo Detecting Visual Studio...
where msbuild >nul 2>nul
if errorlevel 1 (
    echo Warning: Visual Studio not found. Will use MinGW if available.
    set CMAKE_GENERATOR=MinGW Makefiles
) else (
    REM Look for VS 2022
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        set CMAKE_GENERATOR=Visual Studio 17 2022
        echo [Ok] Visual Studio 2022 Community found
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
        set CMAKE_GENERATOR=Visual Studio 17 2022
        echo [Ok] Visual Studio 2022 Professional found
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        set CMAKE_GENERATOR=Visual Studio 16 2019
        echo [Ok] Visual Studio 2019 Community found
    ) else (
        set CMAKE_GENERATOR=MinGW Makefiles
        echo [Ok] Will use MinGW Makefiles
    )
)
echo.

REM Configure with CMake
echo Configuring with CMake...
echo Using generator: !CMAKE_GENERATOR!
cd build
if "!CMAKE_GENERATOR!"=="Visual Studio 17 2022" (
    cmake .. -G "!CMAKE_GENERATOR!" -A x64
) else if "!CMAKE_GENERATOR!"=="Visual Studio 16 2019" (
    cmake .. -G "!CMAKE_GENERATOR!" -A x64
) else (
    cmake ..
)

if errorlevel 1 (
    echo Error: CMake configuration failed
    exit /b 1
)
echo [Ok] Configuration complete
cd ..
echo.

echo Setup complete! You can now build with:
echo.
echo   Visual Studio IDE:
echo     - Open build\can232drv.sln in Visual Studio
echo     - Build using the IDE
echo.
echo   Command line:
echo     cmake --build build --config Release
echo.
echo To install:
echo   cmake --install build --prefix "C:\Program Files\VSCP\drivers\level1"
echo.
echo For more information, see CMAKE_WINDOWS.md
echo.
