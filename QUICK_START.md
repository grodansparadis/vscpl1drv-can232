# Quick Start - Building with CMake

> **Note:** The project has been converted from Autotools to CMake. For detailed information, see [CMAKE_BUILD.md](CMAKE_BUILD.md).

## One-Command Setup

```bash
bash ./setup.sh
cmake --build build
```

That's it! Your build is ready in `build/` directory.

## Common Tasks

### Build Debug Version
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Build Release Version
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

### Clean All
```bash
rm -rf build build-release
```

## VS Code Integration

1. Install **CMake Tools** extension: `ms-vscode.cmake-tools`
2. Press `Ctrl+Shift+B` to build (configured as default task)
3. Or use **CMake** in the left panel

## Troubleshooting

### Error: "VSCP submodule is required but not initialized"
```bash
git submodule update --init --recursive
rmdir build && mkdir build && cd build
cmake ..
```

### Clean Rebuild
```bash
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
```

### Use Build Presets
```bash
cmake --preset=debug      # Configure with debug preset
cmake --build --preset=debug  # Build with debug preset
```

## What Changed?

- **Old:** `./configure && make && sudo make install`
- **New:** `cmake .. && cmake --build . && sudo cmake --install .`

See [MIGRATION_GUIDE.md](MIGRATION_GUIDE.md) and [CONVERSION_SUMMARY.md](CONVERSION_SUMMARY.md) for details.
