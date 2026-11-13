# vscpl1drv-can232 - VSCP Level I driver for CAN232

[![Linux CI](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/linux-ci.yml/badge.svg)](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/linux-ci.yml)
[![Windows CI](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/windows-ci.yml)
[![macOS CI](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/macos-ci.yml/badge.svg)](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/macos-ci.yml)
[![Raspberry Pi armhf CI](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/rpi-armhf-ci.yml/badge.svg)](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/rpi-armhf-ci.yml)
[![Raspberry Pi arm64 CI](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/rpi-arm64-ci.yml/badge.svg)](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/rpi-arm64-ci.yml)
[![Codacy Analysis](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/codacy.yml/badge.svg)](https://github.com/grodansparadis/vscpl1drv-can232/actions/workflows/codacy.yml)

![VSCP Logo](https://github.com/grodansparadis/vscp-logo/blob/master/logo_100.png)

VSCP Level I (CANAL) driver for the [Lawicel CAN232](https://www.can232.com/) serial CAN adapter. The driver lets VSCP software (and any CANAL-compatible application) talk to a CAN bus through a CAN232 adapter on Linux, Windows, macOS and Raspberry Pi.

## Versioning

Releases use calendar versioning: `YYYY.MM.patch`. CMake generates the year and month
in UTC at configure time; the patch starts at `0` with the first commit of each month
and increments with subsequent commits, so pushes automatically update package versions.

## Building

The project is built with CMake (3.16+). The VSCP library sources are downloaded
by CMake at configure time (see `cmake/dependencies.cmake`), so the first configure
needs network access:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Run the calendar-versioning tests with `python3 tests/test_versioning.py` (requires
Git, CMake and a C/C++ toolchain; no network access is needed).

Packages (DEB/RPM/TGZ on Linux, NSIS/ZIP on Windows, TGZ on macOS) are created with:

```bash
cd build && cpack
```

See [CMAKE_BUILD.md](CMAKE_BUILD.md) for the complete build guide, and
[CMAKE_WINDOWS.md](CMAKE_WINDOWS.md) / [CMAKE_MACOS.md](CMAKE_MACOS.md) for
platform-specific notes.

## CI artifacts

Every push builds installable packages for:

| Platform | Workflow | Artifacts |
|----------|----------|-----------|
| Linux x64 | [linux-ci.yml](.github/workflows/linux-ci.yml) | DEB, RPM, TGZ |
| Windows x64 | [windows-ci.yml](.github/workflows/windows-ci.yml) | NSIS installer, ZIP |
| macOS (universal) | [macos-ci.yml](.github/workflows/macos-ci.yml) | TGZ + Homebrew formula |
| Raspberry Pi 32-bit | [rpi-armhf-ci.yml](.github/workflows/rpi-armhf-ci.yml) | DEB, TGZ |
| Raspberry Pi 64-bit | [rpi-arm64-ci.yml](.github/workflows/rpi-arm64-ci.yml) | DEB, TGZ |

## License

MIT License. See [COPYRIGHT](linux/doc-pak/COPYRIGHT) for details.

----

Copyright © 2000-2026 Ake Hedman and contributors, [the VSCP project](https://www.vscp.org)
