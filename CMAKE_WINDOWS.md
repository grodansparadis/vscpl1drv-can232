# CMake Build on Windows

This guide covers building vscpl1drv-can232 on Windows using CMake.

## Prerequisites

### Required Software
- **CMake 3.10 or later** - Download from https://cmake.org/download/
- **Visual Studio** with C++ support, OR
- **MinGW** or **Clang** for Windows

### Optional
- **Pandoc** - for generating man pages (optional)
- **Git** - for managing submodules

## Installation

### 1. Install CMake
- Download from: https://cmake.org/download/
- During installation, choose "Add CMake to the system PATH"

### 2. Install Visual Studio (Recommended)
- Community Edition (free) at https://www.visualstudio.com/downloads/
- Include "Desktop development with C++" workload

OR use MinGW:
```bash
# Using Chocolatey (if installed)
choco install mingw
```

## Building on Windows

### Quick Start (Recommended)

#### Option 1: Visual Studio IDE
```bash
# Initialize submodules
git submodule update --init --recursive

# Create and open Visual Studio project
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64

# Then open can232drv.sln in Visual Studio and build
```

#### Option 2: Command Line (MSVC)
```bash
# Initialize submodules
git submodule update --init --recursive

# Configure and build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

If you get an error like `Could not find toolchain file: .../vcpkg.cmake`, run:

```bash
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=""
```

This clears a broken externally-injected toolchain setting for the current configure command.

#### Option 3: Command Line (MinGW)
```bash
# Initialize submodules
git submodule update --init --recursive

# Configure and build
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release
```

### Using CMake GUI

1. Open CMake GUI
2. Set "Where is the source code" to the project root directory
3. Set "Where to build the binaries" to `build/`
4. Click "Configure"
5. Select your compiler (Visual Studio 17 2022 or MinGW Makefiles)
6. Click "Generate"
7. Click "Open Project" to open in Visual Studio, or use command line

## Build Options

### Debug Build
```bash
cmake --build build --config Debug
```

### Release Build
```bash
cmake --build build --config Release
```

### Parallel Build (Faster)
```bash
cmake --build build --config Release --parallel 8
```

## Installation

### To System Path (Requires Admin)
```bash
# Command Prompt (as Administrator)
cd build
cmake --install . --prefix "C:\Program Files\VSCP\drivers\level1"
```

### To Custom Location
```bash
cmake --install build --prefix "C:\custom\path\drivers\level1"
```

## Visual Studio Generation Options

CMake can generate projects for different Visual Studio versions:

```bash
# Visual Studio 2022 (Latest)
cmake .. -G "Visual Studio 17 2022"

# Visual Studio 2019
cmake .. -G "Visual Studio 16 2019"

# Visual Studio 2017
cmake .. -G "Visual Studio 15 2017"

# For 64-bit builds (explicit)
cmake .. -G "Visual Studio 17 2022" -A x64

# For 32-bit builds
cmake .. -G "Visual Studio 17 2022" -A Win32
```

## Output Files

Build output will be in:
```
build/
├── Can232drv.dll              (Debug)
├── Can232drv.pdb              (Debug symbols)
├── Release/
│   └── can232drv.dll          (Release)
└── [other build files]
```

## Troubleshooting

### "cmake: command not found"
**Solution**: CMake not in system PATH. Either:
- Add CMake to PATH in Environment Variables
- Use full path: `C:\Program Files\CMake\bin\cmake.exe` ...

### "Visual Studio not found"
**Solution**: 
```bash
# List available generators
cmake --help

# Explicitly specify compiler
cmake .. -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++
```

### "Could not find toolchain file: .../vcpkg.cmake"
**Cause**: `CMAKE_TOOLCHAIN_FILE` is set in your environment and points to a missing vcpkg path.

**Solution**:
```bash
# One-off configure (recommended)
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=""

# Optional: clear environment variable for current shell session
set CMAKE_TOOLCHAIN_FILE=
```

In PowerShell, clear for current session with:
```powershell
Remove-Item Env:CMAKE_TOOLCHAIN_FILE -ErrorAction SilentlyContinue
```

### "VSCP submodule is missing"
**Solution**:
```bash
git submodule update --init --recursive
```

### "Missing header files"
**Solution**: Ensure submodules are initialized:
```bash
git submodule update --init --recursive
rm -rf build
mkdir build && cd build
cmake ..
```

### Build fails with MSVC
**Solution**: Make sure C++ development tools are installed:
1. Open Visual Studio Installer
2. Modify your Visual Studio installation
3. Select "Desktop development with C++"
4. Install

## Compiler-Specific Notes

### Visual Studio (MSVC)
- Automatically configured by CMake
- Supports Debug/Release configurations
- Creates `.pdb` files for debugging

### MinGW
- Make sure MinGW is in system PATH
- Use CMake generator: `MinGW Makefiles`
- Good free alternative to MSVC

### Clang (Windows)
```bash
cmake .. -G "Ninja" -DCMAKE_CXX_COMPILER=clang++
cmake .. -G "Unix Makefiles" -DCMAKE_CXX_COMPILER=clang++
```

## Cross-Platform Building

### From Linux to Windows (MinGW)
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=windows-toolchain.cmake
```

Note: You'll need to set up MinGW cross-compilation tools first.

## Installation Paths

### Default Locations

**Windows VSCP System**:
```
C:\Program Files\VSCP\drivers\level1\can232drv.dll
C:\Program Files\VSCP\drivers\level1\can232drv.lib
```

**Development Build**:
```
build\Release\can232drv.dll
build\lib\libcan232drv.a (static library)
```

## Development

### Debugging in Visual Studio

1. Build Debug configuration: `cmake --build build --config Debug`
2. Open `build\can232drv.sln` in Visual Studio
3. Set breakpoints and debug

## Using Visual Studio's CMake Support

Visual Studio 2019+ has built-in CMake support:
1. File → Open → Folder (select project root)
2. Visual Studio automatically configures CMake
3. Build using toolbar

## Creating Windows Installer (NSIS)

### Prerequisites for NSIS
1. Download NSIS from: https://nsis.sourceforge.io/
2. Install NSIS 3.x or later
3. During installation, note the installation directory (default: `C:\Program Files (x86)\NSIS`)
4. Add NSIS to your system PATH:
   - Right-click "This PC" → Properties
   - Advanced system settings → Environment Variables
   - Add NSIS bin directory to PATH

### Building the Installer

After building the project:

```bash
# Configure and build Release
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release

# Create NSIS installer
cpack -G NSIS

# Create ZIP archive (doesn't require NSIS)
cpack -G ZIP
```

### Output
- **ZIP Archive**: `vscpl1drv-can232-X.X.X-win64.zip`
  - Portable, no installation required
  - Extract and use directly
  - Good for portable/USB deployments

- **NSIS Installer**: `vscpl1drv-can232-X.X.X-installer.exe`
  - Full Windows installer with uninstall support
  - Installs to `C:\Program Files\VSCP\drivers\level1`
  - Creates Start Menu shortcuts
  - Registers in Windows Add/Remove Programs
  - Includes DLL, import library, headers, and documentation

### Installer Customization

The NSIS configuration is in [win32/vscpl1drv-can232.nsi](../win32/vscpl1drv-can232.nsi).

To customize the installer:
- Edit `win32/vscpl1drv-can232.nsi`
- Modify installation directory, icons, or registry entries
- Rebuild using `cpack -G NSIS`

### Troubleshooting NSIS

If CPack fails with "NSIS not found":
```bash
# Explicitly set NSIS path
cmake .. -DNSIS_PATH="C:\Program Files (x86)\NSIS"
cpack -G NSIS
```

Or use ZIP instead (no NSIS required):
```bash
cpack -G ZIP
```

## Best Practices

1. **Out-of-source builds**: Always use a `build/` directory
2. **Separate Debug/Release**: Use different build directories
3. **Always initialize submodules**: `git submodule update --init --recursive`
4. **Use presets for consistency**: `cmake --preset` (see CMakePresets.json)

## Next Steps

After successful build:
1. Test the DLL with VSCP daemon
2. Verify COM port connections work
3. Test CAN communication with actual device

## Getting Help

Check the main documentation:
- [CMAKE_BUILD.md](../CMAKE_BUILD.md) - General CMake info
- [QUICK_START.md](../QUICK_START.md) - Quick reference
- [CMAKE_CONVERSION.md](../CMAKE_CONVERSION.md) - Conversion details
