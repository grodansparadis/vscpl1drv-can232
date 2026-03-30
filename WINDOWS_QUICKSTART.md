# Quick Start - Windows Build Guide

> **For detailed information, see [CMAKE_WINDOWS.md](CMAKE_WINDOWS.md)**

## One-Command Setup (Windows)

### Using Visual Studio
```cmd
setup.bat
cmake --build build --config Release
```

### Or
```cmd
setup.bat
REM Then open build\can232drv.sln in Visual Studio and build
```

## Prerequisites

### Minimum Requirements
- **CMake 3.10+** - Download from [cmake.org](https://cmake.org/download/)
- **C++ Compiler** - One of:
  - Visual Studio Community (free) with C++ support
  - MinGW
  - Clang
- **Git** - For submodule management

### Installation (Windows)

#### 1. Install CMake
```
Download from: https://cmake.org/download/
During installation: ✓ "Add CMake to system PATH"
```

#### 2. Install Visual Studio (Recommended)
- **Free Community Edition** - https://www.visualstudio.com/downloads/
- Select "Desktop development with C++"

## Building

### Option 1: Visual Studio (Recommended for Beginners)

```cmd
REM 1. Initialize and configure
setup.bat

REM 2. Open in Visual Studio
REM   - Visual Studio opens automatically, or
REM   - Open build\can232drv.sln manually from Visual Studio

REM 3. Build in Visual Studio IDE
REM   - Select Release configuration
REM   - Press Ctrl+Shift+B or Build > Build Solution
```

### Option 2: Command Line (All Platforms)

```cmd
REM Initialize
setup.bat

REM Build Release version
cmake --build build --config Release --parallel

REM Build Debug version
cmake --build build --config Debug
```

### Option 3: Using CMake Presets

```cmd
REM Windows MSVC Release (recommended)
cmake --preset windows-msvc-release
cmake --build --preset windows-release

REM Or Windows Debug
cmake --preset windows-msvc-debug
cmake --build --preset windows-debug
```

## Common Tasks

### Build Different Configurations
```cmd
REM Release (optimized, smaller)
cmake --build build --config Release

REM Debug (with debugging symbols, slower)
cmake --build build --config Debug
```

### Parallel Build (Faster)
```cmd
cmake --build build --config Release --parallel 8
```

### Install DLL
```cmd
REM To default VSCP location (requires Admin)
cmake --install build --config Release

REM To custom location
cmake --install build --config Release --prefix "C:\custom\path"
```

### Clean and Rebuild
```cmd
REM Delete build directory
rmdir /s /q build

REM Start fresh
setup.bat
cmake --build build --config Release
```

## Troubleshooting

### Error: "cmake: command not found"
**Solution**: CMake not in PATH
- Reinstall CMake, checking "Add to PATH"
- Or use full path: `"C:\Program Files\CMake\bin\cmake" --version`

### Error: "No suitable video mode found"
**Solution**: Ignore - this is just a graphics warning, not related to compilation

### Build fails with MSVC
**Solution**: Install Visual Studio C++ tools
1. Open Visual Studio Installer
2. Click "Modify"
3. Select "Desktop development with C++"
4. Install

### "Git submodule" errors
**Solution**:
```cmd
git submodule update --init --recursive
rmdir /s /q build
setup.bat
```

### Can't find Visual Studio
**Solution**: Either
- Install Visual Studio Community (https://www.visualstudio.com)
- Or use MinGW: edit setup.bat and set `CMAKE_GENERATOR=MinGW Makefiles`

## Visual Studio Tips

### Opening the Solution
1. Navigate to `build/` folder
2. Double-click `can232drv.sln`
3. Visual Studio opens automatically

### Building in IDE
- Press **Ctrl+Shift+B** (Build Solution)
- Or: Build > Build Solution menu
- Select Release/Debug config in dropdown

### Debugging
- Set breakpoints by clicking in left margin
- Press **F5** to start debugging
- Use Debug > Windows for various debug panels

## Configuration Paths

### Default Install Locations
```
C:\Program Files\VSCP\drivers\level1\can232drv.dll
```

### Build Output Locations
```
build\Release\can232drv.dll          (Release DLL)
build\Debug\can232drv.dll            (Debug DLL)
build\Release\can232drv.pdb          (Debug symbols)
build\lib\libcan232drv.a             (Static library - optional)
```

## Available Compilers

### MSVC (Recommended)
- Best integration with Visual Studio
- Stable and well-tested
- Recommended for production builds

### MinGW
```cmd
cmake .. -G "MinGW Makefiles"
cmake --build build --config Release
```

### Clang
```cmd
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER=clang++
cmake --build build --config Release
```

## Next Steps

After successful build:
1. Test the DLL with VSCP daemon
2. Verify CAN adapter detection
3. Test CAN message communication

## For More Information

- **Windows Details**: [CMAKE_WINDOWS.md](CMAKE_WINDOWS.md)
- **General CMake Info**: [CMAKE_BUILD.md](CMAKE_BUILD.md)
- **Quick Reference**: [QUICK_START.md](QUICK_START.md) (Linux)
- **Full Migration Info**: [CMAKE_CONVERSION.md](CMAKE_CONVERSION.md)

## Getting Help

If build fails:
1. Check [CMAKE_WINDOWS.md](CMAKE_WINDOWS.md) Troubleshooting section
2. Ensure all prerequisites are installed
3. Run setup.bat to verify configuration
4. Check error messages carefully - they usually indicate what's missing

---

**Windows CMake build is now ready!** 🎉
