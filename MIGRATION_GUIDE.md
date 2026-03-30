# Migration Guide: Autotools to CMake

This document explains the migration from Autotools-based build system to CMake for vscpl1drv-can232.

## Why CMake?

- **Modern Standard**: CMake is the de-facto standard for C/C++ projects today
- **Better IDE Integration**: Native support in Visual Studio Code, Visual Studio, CLion, and more
- **Cleaner Configuration**: Easier to read and maintain compared to shell scripts
- **Cross-Platform**: Better Windows build support
- **CMake Presets**: IDE-friendly build configurations

## File Mapping

### Old Autotools Files → New CMake Files

| Autotools | CMake | Purpose |
|-----------|-------|---------|
| `configure.ac` | `CMakeLists.txt` | Main project configuration |
| `linux/Makefile.in` | `linux/CMakeLists.txt` | Linux-specific build rules |
| `VERSION.m4` | CMake version parsing | Version information |
| `./configure && make` | `cmake .. && cmake --build .` | Build commands |

## Key Changes

### Version Management
**Before (Autotools):**
- Version defined in `VERSION.m4` using m4 macros
- Substituted into `Makefile.in` by configure script

**After (CMake):**
- CMake parses `VERSION.m4` directly
- Version accessible to all build targets
- No separate configuration step needed per version change

### Compiler Flags
**Before:**
```makefile
CXXFLAGS = -D__LINUX__ -fPIC -D_REENTRANT @CXXFLAGS@
```

**After:**
```cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__LINUX__ -fPIC -D_REENTRANT")
```

### Library Building
**Before:**
```makefile
$(LIB_PLUS_VER): $(CAN232DRV_OBJECTS)
	$(CXX) -Wl,-soname,$(LIB_SONAME) -o $@ ...
```

**After:**
```cmake
add_library(vscpl1drv-can232 SHARED ${CAN232DRV_SOURCES})
set_target_properties(vscpl1drv-can232 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MAJOR_VERSION})
```

### Installation
**Before:**
```makefile
install:
	$(INSTALL_DIR) $(DESTDIR)$(libdir)
	$(INSTALL_PROGRAM) $(LIB_PLUS_VER) $(DESTDIR)$(libdir)
```

**After:**
```cmake
install(TARGETS vscpl1drv-can232
    LIBRARY DESTINATION drivers/level1)
```

## Build System Comparison

### Configuration
```bash
# Autotools
./configure
./configure --prefix=/custom/path
./configure --enable-debug

# CMake
cmake ..
cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### Building
```bash
# Autotools
make
make -j4

# CMake
cmake --build .
cmake --build . --parallel 4
```

### Installation
```bash
# Both support DESTDIR
sudo make install DESTDIR=/staging
sudo cmake --install . --prefix /var/lib/vscp
```

## Build Output Structure

### Before (Autotools)
```
project/
  ├── linux/
  │   ├── .deps/
  │   ├── *.o (object files in source dir)
  │   └── vscpl1drv-can232.so*
```

### After (CMake)
```
project/
  ├── build/                    (out-of-source)
  │   ├── CMakeCache.txt
  │   ├── CMakeFiles/
  │   ├── lib/
  │   │   ├── libvscpl1drv-can232.so*
  │   │   └── libvscpl1drv-can232.a
  │   └── Makefile
  ├── CMakeLists.txt
  └── linux/
      ├── CMakeLists.txt
      └── *.cpp (sources stay clean)
```

## IDE Integration

### VS Code
- Install CMake Tools extension: ms-vscode.cmake-tools
- Automatically detects CMakeLists.txt
- Use CMakePresets.json for preset configurations
- Building: Ctrl+Shift+B (if task configured)

### Command Line
```bash
# Configure with presets
cmake --preset=default
cmake --build --preset=default

# Or manually
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build --parallel
```

## Dependency Management

### VSCP Submodule
The project depends on the VSCP library as a git submodule.

**Initialize:**
```bash
git submodule update --init --recursive
```

This is now **required** before building with CMake.

## Troubleshooting

### CMake not found
Install CMake 3.10 or later:
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake
```

### VSCP submodule initialization error
```bash
cd /path/to/vscpl1drv-can232
git submodule update --init --recursive
```

### Clean rebuild
```bash
rm -rf build && mkdir build && cd build
cmake .. && cmake --build .
```

## Running Tests

### Build with tests enabled
```bash
cmake -DBUILD_TESTS=ON ..
cmake --build .
```

### Run tests
```bash
ctest
ctest --verbose
```

## Advantages Over Autotools

1. **Faster**: No need to run configure for every clean build
2. **CMake Presets**: IDE-friendly, reproducible configurations
3. **Better Error Messages**: Clearer what's wrong and how to fix it
4. **Modern Syntax**: More readable and maintainable
5. **Out-of-Source Builds**: Keep source directory clean
6. **Generator Support**: Works with Ninja, NMake, Visual Studio, etc.

## Backward Compatibility

The Autotools build system files remain in the repository for reference:
- `configure.ac`
- `Makefile.in`
- `linux/Makefile.in`
- `m4/` directory

They can be removed in future cleanup if desired.
