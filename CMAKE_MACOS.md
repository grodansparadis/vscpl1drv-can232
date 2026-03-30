# CMake Build on macOS

This guide covers building vscpl1drv-can232 on macOS using CMake.

## Prerequisites

### Required Software
- **macOS 10.13 or later** (Big Sur, Monterey, Ventura, Sonoma, or later recommended)
- **CMake 3.10 or later** - Install via Homebrew: `brew install cmake`
- **Xcode Command Line Tools** or **Xcode IDE**
- **Git** - Usually included with Xcode or `brew install git`

### Optional
- **Xcode IDE** - For IDE integration (free from App Store)
- **Homebrew** - Package manager for macOS: https://brew.sh
- **Pandoc** - for generating man pages: `brew install pandoc`

## Installation

### Using Homebrew (Recommended)

```bash
# Install CMake
brew install cmake

# Install optional dependencies
brew install pandoc

# For Xcode integration (if needed)
xcode-select --install
```

### Alternative: Download CMake Directly

Download from https://cmake.org/download/ and follow the macOS installation instructions.

## Building on macOS

### Quick Start

#### Option 1: Command Line (Recommended)
```bash
# Initialize submodules
git submodule update --init --recursive

# Debug build
mkdir build && cd build
cmake ..
cmake --build .

# Release build
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

#### Option 2: Using CMake Presets
```bash
# Debug build
cmake --preset=macos-debug
cmake --build --preset=macos-debug

# Release build
cmake --preset=macos-release
cmake --build --preset=macos-release
```

#### Option 3: Xcode IDE Integration
```bash
# Generate Xcode project
mkdir build-xcode && cd build-xcode
cmake .. -G Xcode

# Or use the preset
cmake --preset=macos-xcode

# Then open in Xcode
open vscpl1drv-can232.xcodeproj
```

### Build Options

#### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

#### Release Build (Optimized)
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

#### Specify Architecture
```bash
# Intel (x86_64)
cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 ..

# Apple Silicon (ARM64)
cmake -DCMAKE_OSX_ARCHITECTURES=arm64 ..

# Universal binary (Intel + Apple Silicon)
cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ..
```

#### Specify Deployment Target
```bash
# Minimum macOS version to support
cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 ..
```

### Parallel Build (Faster)
```bash
cmake --build build --parallel $(sysctl -n hw.ncpu)
```

## Installation

### To System Path (May require sudo)
```bash
cd build
cmake --install . --prefix /usr/local/vscp
```

### To User Home Directory
```bash
cd build
cmake --install . --prefix $HOME/.local/vscp
```

### To Custom Location
```bash
cmake --install build --prefix $HOME/vscp-drivers
```

### Using DESTDIR (For packaging)
```bash
cmake --install build --prefix /usr/local/vscp --config Release
DESTDIR=/tmp/staging cmake --install build --prefix /usr/local/vscp
```

## Building Packages

### DMG (Disk Image)
The most common distribution format for macOS applications:

```bash
cd build
cmake --build . --config Release
cpack -G DragDrop
```

Output: `vscpl1drv-can232-X.X.X-Darwin.dmg`

**Features:**
- Mountable disk image
- Professional distribution format
- Double-click to mount and install

### PKG (Apple Installer)
```bash
cd build
cmake --build . --config Release
cpack -G ProductBuild
```

Output: `vscpl1drv-can232-X.X.X-Darwin.pkg`

**Features:**
- Standard macOS installer
- Can be distributed via Apple notarization
- Integrates with System Preferences

### TAR.GZ (Portable Archive)
```bash
cd build
cpack -G TGZ
```

Output: `vscpl1drv-can232-X.X.X-Darwin.tar.gz`

**Features:**
- Platform-independent format
- No installation required
- Good for development/testing

## Output Files

Build output will be in:
```
build/
├── lib/
│   ├── libvscpl1drv-can232.dylib       (Shared library)
│   ├── libvscpl1drv-can232.a           (Static library)
│   ├── libvscpl1drv-can232.dylib.X     (Versioned)
│   └── libvscpl1drv-can232.1.dylib     (Major version)
├── vscpl1drv-can232.1                  (Man page)
└── [other build files]
```

## Architecture Support

### Current Machine Type
```bash
# Check your architecture
uname -m

# x86_64 = Intel
# arm64 = Apple Silicon (M1, M2, M3, etc.)
```

### Build for Different Architectures

#### Intel (x86_64)
```bash
cmake -DCMAKE_OSX_ARCHITECTURES=x86_64 ..
cmake --build .
```

#### Apple Silicon (arm64)
```bash
cmake -DCMAKE_OSX_ARCHITECTURES=arm64 ..
cmake --build .
```

#### Universal Binary (Both Architectures)
```bash
cmake -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" ..
cmake --build .
```

## Troubleshooting

### "CMake not found"
**Solution**: 
```bash
# Install via Homebrew
brew install cmake

# Or add to PATH if installed manually
export PATH="/Applications/CMake.app/Contents/bin:$PATH"
```

### "VSCP submodule is missing"
**Solution**:
```bash
git submodule update --init --recursive
```

### "Xcode Command Line Tools not installed"
**Solution**:
```bash
xcode-select --install
```

### Build fails with architecture mismatch
**Solution**: Build for your current architecture
```bash
rm -rf build
mkdir build && cd build
cmake -DCMAKE_OSX_ARCHITECTURES=$(uname -m) ..
cmake --build .
```

### Library not found at runtime
**Solution**: Check the library search path
```bash
# View library dependencies
otool -L build/lib/libvscpl1drv-can232.dylib

# Reset the install name path
install_name_tool -change /old/path /usr/local/lib/libvscpl1drv-can232.dylib build/lib/libvscpl1drv-can232.dylib
```

## Compiler-Specific Notes

### Clang (Default on macOS)
- Automatically used by CMake on macOS
- Part of Xcode or Command Line Tools
- Excellent optimization support

### GCC (via Homebrew)
```bash
# Install GCC
brew install gcc

# Use GCC for build
cmake -DCMAKE_C_COMPILER=gcc-13 -DCMAKE_CXX_COMPILER=g++-13 ..
```

## Using Xcode IDE

### Native Xcode Integration
Visual Studio Code with CMake extension can auto-detect and configure:

1. Open VSCode
2. Install CMake extension (ms-vscode.cmake-tools)
3. Project auto-configures with CMake
4. Build with Cmd+Shift+B or via CMake status area

### Manual Xcode Project Generation
```bash
mkdir build-xcode && cd build-xcode
cmake .. -G Xcode
open vscpl1drv-can232.xcodeproj
```

Then in Xcode:
- Select scheme (Debug/Release)
- Build with Cmd+B
- Debug with Cmd+Shift+D

## Code Signing and Notarization

For distributing your built library, you may need to sign and notarize:

### Code Sign
```bash
codesign -s - build/lib/libvscpl1drv-can232.dylib
```

### Verify Signature
```bash
codesign -v build/lib/libvscpl1drv-can232.dylib
```

## Best Practices

1. **Use presets for consistency**: `cmake --preset=macos-release`
2. **Separate build directories**: Keep debug and release builds separate
3. **Always initialize submodules**: `git submodule update --init --recursive`
4. **Universal binaries for distribution**: Build for both arm64 and x86_64
5. **Check dependencies before installing**: `otool -L libname.dylib`
6. **Use Homebrew for dependencies**: Easier to maintain and update

## Performance Optimization

For optimized builds on Apple Silicon:

```bash
cmake -DCMAKE_BUILD_TYPE=Release \
       -DCMAKE_OSX_ARCHITECTURES=arm64 \
       -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 \
       ..
cmake --build . --parallel $(sysctl -n hw.ncpu)
```

## Additional Resources

- CMake Documentation: https://cmake.org/cmake/help/latest/
- Xcode Documentation: https://developer.apple.com/documentation/xcode/
- macOS Architecture Information: https://developer.apple.com/design/human-interface-guidelines/macos/
- Homebrew Packages: https://formulae.brew.sh/
