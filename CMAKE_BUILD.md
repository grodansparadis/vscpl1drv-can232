# CMake Build System for vscpl1drv-can232

This project has been converted from an Autotools-based build system to CMake.

## Prerequisites

This project depends on the VSCP library. You need to initialize the VSCP submodule first:

```bash
git submodule update --init --recursive
```

This will download the VSCP library source code required to build the driver.

## Quick Start

### Initialize dependencies
```bash
git submodule update --init --recursive
```

### Build (Debug)
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Build (Release)
```bash
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### Install
```bash
cd build
sudo cmake --install . --prefix /var/lib/vscp
```

## Build Options

- `CMAKE_BUILD_TYPE`: Debug or Release (default: Debug)
- `CMAKE_INSTALL_PREFIX`: Installation prefix (default: /var/lib/vscp)
- `BUILD_TESTS`: Build test executables (default: OFF)
- `DESTDIR`: Set root directory for installation

## Examples

### Build with custom install prefix
```bash
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
cmake --build .
```

### Build with tests
```bash
cmake -DBUILD_TESTS=ON ..
cmake --build .
ctest
```

### Install with DESTDIR
```bash
cmake --install . --prefix /var/lib/vscp --config Debug
DESTDIR=/tmp/staging cmake --install .
```

## CMake Presets

You can use CMake presets for predefined configurations:

```bash
# Debug build
cmake --preset=default
cmake --build --preset=default

# Release build
cmake --preset=release
cmake --build --preset=release

# Debug with tests
cmake --preset=with-tests
cmake --build --preset=with-tests
```

## What Changed from Autotools

| Feature | Autotools | CMake |
|---------|-----------|-------|
| Configuration | `./configure` | `cmake ..` |
| Build | `make` | `cmake --build .` |
| Install | `make install` | `cmake --install .` |
| Version file | `configure.ac` + `VERSION.m4` | CMake parses `VERSION.m4` |
| Compiler flags | `Makefile.in` | `CMakeLists.txt` |

## Project Structure

```
.
├── CMakeLists.txt              # Main project configuration
├── CMakePresets.json           # Build presets for IDE integration
├── VERSION.m4                  # Version definitions
├── linux/
│   ├── CMakeLists.txt          # Linux-specific build configuration
│   ├── vscpl1drv-can232.cpp    # Main driver source
│   ├── vscpl1drv-can232.h      # Main driver header
│   ├── can232obj.cpp           # CAN driver object implementation
│   ├── can232obj.h             # CAN driver object header
│   └── test.cpp                # Test program
├── vscp/
│   └── src/                    # VSCP library sources
└── ...
```

## Packaging with CPack

CPack generates distributable packages for your platform:

### Linux Packages
```bash
cd build
cpack -G DEB                    # Create Debian package (.deb)
cpack -G RPM                    # Create RPM package (.rpm)
cpack -G TGZ                    # Create tarball (.tar.gz)
cpack                           # Create default packages (all generators)
```

### Windows Packages

#### Prerequisites for NSIS Installer
To build Windows NSIS installers, you need:
- **NSIS 3.x or later** installed on Windows
- Add NSIS installation directory to PATH (e.g., `C:\Program Files (x86)\NSIS`)
- or set `NSIS_PATH` in CMake cache: `cmake -DNSIS_PATH="C:\Program Files (x86)\NSIS" .. `

#### Create Windows Packages
```cmd
cd build
cpack -G ZIP                    # Create ZIP archive (no dependencies)
cpack -G NSIS                   # Create NSIS installer (.exe, requires NSIS)
cpack                           # Create all generators
```

#### Windows Package Details
- **ZIP Archive**: `vscpl1drv-can232-X.X.X-win64.zip` - Portable, no installation required
  - Unzip and copy files to desired location
  - Can be used for standalone deployment
  
- **NSIS Installer**: `vscpl1drv-can232-X.X.X-installer.exe` - Full Windows installer
  - Installs to `C:\Program Files\VSCP\drivers\level1` by default
  - Creates Start Menu shortcuts
  - Adds uninstaller
  - Registers in Windows Add/Remove Programs
  - Optional: Integrate with environment PATH

#### Custom NSIS Script
The installer is built using the custom NSIS script: [win32/vscpl1drv-can232.nsi](win32/vscpl1drv-can232.nsi)

Features:
- Installs DLL, import library, and debug symbols
- Includes header files
- Includes README and LICENSE
- Creates uninstaller
- Windows registry integration
- Start Menu shortcuts

### Package Output
Packages are created in the build directory:
- **Linux**: 
  - `vscpl1drv-can232-X.X.X-Linux.tar.gz` (tarball)
  - `vscpl1drv-can232_X.X.X_amd64.deb` (Debian package)
  - `vscpl1drv-can232-X.X.X-Linux.rpm` (RPM package, if rpmbuild installed)
  
- **Windows**: 
  - `vscpl1drv-can232-X.X.X-win64.zip` (portable ZIP archive)
  - `vscpl1drv-can232-X.X.X-installer.exe` (NSIS installer, if NSIS installed)

### Custom Package Configuration
You can customize packaging by setting CPack variables:
```bash
cpack -G DEB -D CPACK_DEBIAN_PACKAGE_MAINTAINER="Your Name"
```

## Notes

- The shared library is built as `libvscpl1drv-can232.so` with version information
- A static library variant is also built as `libvscpl1drv-can232.a`
- Man page generation requires `pandoc` to be installed
- All compiler flags (warnings, optimization, etc.) match the original Autotools configuration
- CPack configuration is defined in `CMakeLists.txt` and includes platform-specific generators
- DEB and RPM packages are only generated on Linux
- NSIS installer and ZIP archives are only generated on Windows
