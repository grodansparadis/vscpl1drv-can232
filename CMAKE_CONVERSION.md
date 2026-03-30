# CMake Conversion Complete ✓

The vscpl1drv-can232 project has been successfully converted from Autotools to CMake.

## What Was Done

### Build System Files Created
1. **CMakeLists.txt** - Main project configuration
2. **linux/CMakeLists.txt** - Linux-specific build rules
3. **CMakePresets.json** - IDE-friendly build presets
4. **.vscode/tasks.json** - VS Code task integration

### Documentation Created
1. **QUICK_START.md** - Fast getting started guide
2. **CMAKE_BUILD.md** - Comprehensive build documentation
3. **MIGRATION_GUIDE.md** - Detailed migration information
4. **CONVERSION_SUMMARY.md** - Technical conversion details
5. **THIS_DOCUMENT.md** - Overview and links

### Automation
1. **setup.sh** - One-command initialization script
2. **linux/version.h.in** - Version header template

### Configuration
1. **.gitignore-cmake** - CMake-specific ignore patterns
2. **CMakePresets.json** - Build presets for common scenarios

## File Structure

```
vscpl1drv-can232/
├── CMakeLists.txt           ← Main CMake configuration
├── CMakePresets.json        ← Build presets
├── setup.sh                 ← Setup script (executable)
├── QUICK_START.md           ← Start here!
├── CMAKE_BUILD.md           ← Build documentation
├── MIGRATION_GUIDE.md       ← Why CMake details
├── CONVERSION_SUMMARY.md    ← Technical change summary
├── linux/
│   ├── CMakeLists.txt       ← Linux build rules
│   ├── version.h.in         ← Version header template
│   ├── vscpl1drv-can232.cpp
│   └── ... (other sources)
├── .vscode/
│   └── tasks.json           ← Updated with CMake tasks
└── ... (other directories)
```

## Getting Started

### Prerequisites
- CMake 3.10 or later
- C++ compiler (gcc, clang)
- Git
- pandoc (optional, for man pages)

### Build in 2 Steps
```bash
# Initialize submodules and create build directory
bash setup.sh

# Build
cmake --build build
```

### Complete Build & Install
```bash
bash setup.sh
cmake --build build
sudo cmake --install build --prefix /var/lib/vscp
```

## Key Improvements Over Autotools

| Aspect | Autotools | CMake |
|--------|-----------|-------|
| Configuration | `./configure` (slow) | `cmake ..` (fast) |
| Build | `make` | `cmake --build .` |
| IDE Support | Limited | Native support |
| Out-of-source builds | Not default | Built-in |
| Windows support | Complex | Native |
| Build presets | Not available | CMakePresets.json |
| Installation | `make install` | `cmake --install .` |

## Important Notes

### VSCP Submodule Required
This project depends on the VSCP library as a Git submodule. The `setup.sh` script initializes it automatically, or do it manually:

```bash
git submodule update --init --recursive
```

### Build Directories
CMake uses out-of-source builds by default:
- `build/` - Debug build directory
- `build-release/` - Release build directory

Your source code remains clean and unmodified.

### VS Code Integration
1. Install CMake Tools extension (ms-vscode.cmake-tools)
2. Open the project - CMake will auto-detect
3. Use Ctrl+Shift+B to build (CMake: Build Debug is default)
4. Or use CMake panel in sidebar

## Documentation

- **[QUICK_START.md](QUICK_START.md)** - Fast reference for common tasks
- **[CMAKE_BUILD.md](CMAKE_BUILD.md)** - Complete build system guide
- **[MIGRATION_GUIDE.md](MIGRATION_GUIDE.md)** - Why and how we migrated
- **[CONVERSION_SUMMARY.md](CONVERSION_SUMMARY.md)** - Technical details of conversion

## Build Commands Reference

### Configuration
```bash
# Debug configuration
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug

# Release configuration
cmake -B build-release -S . -DCMAKE_BUILD_TYPE=Release

# Custom installation prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

### Building
```bash
# Build with all CPU cores
cmake --build build --parallel

# Build specific target
cmake --build build --target vscpl1drv-can232

# Verbose output
cmake --build build --verbose
```

### Installation
```bash
# Install to configured prefix
sudo cmake --install build

# Install with custom prefix override
sudo cmake --install build --prefix /var/lib/vscp

# Install with staging (DESTDIR)
cmake --install build --prefix /var/lib/vscp --config Release
DESTDIR=/tmp/staging cmake --install build
```

### Cleaning
```bash
# Remove single build directory
rm -rf build

# Clean all build directories
rm -rf build build-release build-tests
```

## Using CMake Presets

Presets provide reproducible configurations without command-line flags:

```bash
# List available presets
cmake --list-presets

# Configure using preset
cmake --preset=default       # Debug
cmake --preset=release       # Release
cmake --preset=with-tests    # Debug + tests

# Build using preset
cmake --build --preset=default
```

## CI/CD Pipeline Update

If you have CI/CD pipelines using Autotools:

**Before:**
```bash
./configure
make -j$(nproc)
sudo make install
```

**After:**
```bash
cmake -B build .
cmake --build build --parallel
sudo cmake --install build --prefix /var/lib/vscp
```

## Backward Compatibility

The original Autotools infrastructure remains intact:
- configure.ac
- Makefile.in
- m4/ directory
- autotools/ directory

These are not used by CMake but are preserved for reference. They can be removed in a future cleanup if desired.

## What's Next?

1. **Try the new build:**
   ```bash
   bash setup.sh && cmake --build build
   ```

2. **Install and test:**
   ```bash
   sudo cmake --install build --prefix /var/lib/vscp
   ```

3. **Update your documentation** to reference CMAKE_BUILD.md

4. **Update CI/CD pipelines** to use new CMake commands

5. **Remove Autotools files** (optional, after stabilization):
   ```bash
   rm -rf configure.ac m4/ autotools/ Makefile.in linux/Makefile.in
   ```

## Support & Troubleshooting

### Can't find cmake
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake
```

### Submodule error
```bash
git submodule update --init --recursive
```

### Clean rebuild needed
```bash
rm -rf build && mkdir build && cd build
cmake .. && cmake --build .
```

### Check CMake version
```bash
cmake --version
```

For detailed troubleshooting, see [CMAKE_BUILD.md](CMAKE_BUILD.md).

## Questions?

Refer to the appropriate documentation:
- **Getting started:** [QUICK_START.md](QUICK_START.md)
- **Build details:** [CMAKE_BUILD.md](CMAKE_BUILD.md)
- **Migration info:** [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md)
- **Technical details:** [CONVERSION_SUMMARY.md](CONVERSION_SUMMARY.md)

---

**Status:** ✅ CMake conversion complete and ready for use!

Generated: 2026-03-30
