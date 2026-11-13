# Changes in vscpl1drv-can232

Every change that goes into a release is listed here, newest release first.
Add each change under **Unreleased** when it is made. When a release is
tagged, rename **Unreleased** to the version and date, and start a new
**Unreleased** section above it.

## Unreleased

- Fixed tests (991f1aa)
- Harden Codacy SARIF upload polling (beb9b66)
- Fix Codacy SARIF uploads for >20 runs (772e082)
- Use safe temporary Codacy SARIF files (11e6b13)
- Clean up temporary Codacy SARIF files (e88a638)
- Handle empty Codacy SARIF chunks (f4ea7e5)
- Split Codacy SARIF uploads into chunks (e20f9a8)
- Fix Codacy SARIF rewrite permissions (0743347)
- Restrict Windows CI workflow token permissions (2cbaf2f)
- Pin Windows CI runner to VS2022 image (9edabf9)
- Fix Windows CI CMake generator selection (ca6a777)
- Normalize Codacy SARIF runs before upload (a556824)
- Fix Codacy SARIF upload category conflict (712a0ce)
- Initial plan (0ccda0c)
- New version schema (42338f3)
- cpack(nsis): remove invalid PRODUCT_VERSION define (4f508b8)
- ci(nsis): simplify CPack NSIS config and always print NSIS log (77e696b)
- cpack(nsis): fix install root escaping and dump NSIS logs (a511736)
- cmake(win): use relative install dir for CPack NSIS (783c4af)
- ci(windows): surface NSIS cpack failures and verify makensis (1ec52d3)
- ci(windows): fail when NSIS package is missing (2469e38)
- ci(windows): publish NSIS installer in artifacts (2035e0f)
- ci: make NSIS detection non-blocking in windows workflow (dcb6d89)
- ci: relax NSIS installer verification in windows workflow (73b0384)
- ci: ensure windows NSIS installer generation (17f6235)
- ci: fix linux install prefix and macos workflow syntax (e8f48cc)
- ci: add platform-specific packaging workflows (5d9ba2f)
- chore: remove autotools files and update CI/build config (2f59e7e)
- Fix static analysis: null safety, buffer bounds, sys/file.h, strncat CWE-120 (39ce8a6)
- added build docs (678df8b)
- Fixed security issues (3d8443e)
- Fix macOS Comm API compatibility with can232 driver (d3dc8c1)
- Fix Makefile paths: use third-party/vscp; add canal_macro.h wrapper; fix com_macos/com_win32 (fe4555b)
- Fix: Implement complete Windows and macOS serial communication APIs (c93a71b)
- Fix: Add platform-specific serial communication (macOS & Windows COM) (84f1008)
- Fix: Add Windows COM driver implementation (com_win32) (118a7b3)
- Fix: Update GitHub Actions to Node.js 24 compatible versions (5341d35)
- Fix: macOS CMakeLists source file paths (e5380ef)
- Fix: Windows CI workflow path handling issues (f59063a)
- Fix: Add build-time patch for VSCP iconst typo in com.cpp (58802e1)
- Updated upload artifact to valid version (510c839)
- Changed to cmake build system (9c2de26)
- Fixed compiler warning (89a4811)
- Updated copyright year (0e8fe77)
- Updated copyright (26c70e3)

## v1.1.1 - 2020-05-28

- Released 1.1.1 (c272553)
- fix receive data from can after send frame (fb3275c)
- Fixed some warnings (e0d9414)
