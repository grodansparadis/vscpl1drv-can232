cmake_minimum_required(VERSION 3.16)

# Calendar versioning: YYYY.MM.patch; the first commit in each month is patch 0.
string(TIMESTAMP MAJOR_VERSION "%Y" UTC)
string(TIMESTAMP MINOR_VERSION "%m" UTC)
string(TIMESTAMP MONTH_START "%Y-%m-01" UTC)

find_package(Git QUIET)
set(RELEASE_VERSION "0")
if(GIT_EXECUTABLE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-list --count "--since=${MONTH_START}T00:00:00Z" HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE MONTH_COMMIT_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    if(MONTH_COMMIT_COUNT MATCHES "^[0-9]+$" AND MONTH_COMMIT_COUNT GREATER 0)
        math(EXPR RELEASE_VERSION "${MONTH_COMMIT_COUNT} - 1")
    endif()
endif()
set(BUILD_VERSION "0")

set(PACKAGE_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE_VERSION}")
set(FULL_VERSION "${PACKAGE_VERSION}")

project(vscpl1drv-can232 
        VERSION ${PACKAGE_VERSION}
        DESCRIPTION "VSCP Level I driver for CAN232 devices"
        LANGUAGES C CXX)

# Third-party sources (formerly git submodules)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake)

# Check that the VSCP sources were fetched
if(NOT EXISTS "${THIRD_PARTY_VSCP_DIR}/src/common/dllist.c")
    message(FATAL_ERROR "VSCP sources not found in ${THIRD_PARTY_VSCP_DIR}")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set installation prefix based on platform
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    if(APPLE)
        set(CMAKE_INSTALL_PREFIX "/usr/local/vscp")
    elseif(WIN32)
        set(CMAKE_INSTALL_PREFIX "C:/Program Files/VSCP")
    else()
        set(CMAKE_INSTALL_PREFIX "/var/lib/vscp")
    endif()
endif()

# Allow DESTDIR override
if(NOT DEFINED DESTDIR)
    set(DESTDIR "")
endif()

# Build configuration
set(VSCP_INSTALL_DIR "${DESTDIR}${CMAKE_INSTALL_PREFIX}" CACHE PATH "VSCP installation directory")

# CPack Configuration
set(CPACK_PACKAGE_NAME "vscpl1drv-can232")
set(CPACK_PACKAGE_VENDOR "VSCP")
set(CPACK_PACKAGE_VERSION "${FULL_VERSION}")
# Generate a .sha256 digest file next to every package
set(CPACK_PACKAGE_CHECKSUM "SHA256")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VSCP Level I driver for CAN232 devices")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
else()
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CMAKE_BUILD.md")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
elseif(EXISTS "${THIRD_PARTY_VSCP_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${THIRD_PARTY_VSCP_DIR}/LICENSE")
endif()
set(CPACK_PACKAGE_INSTALL_DIRECTORY "vscpl1drv-can232")
set(CPACK_STRIP_FILES TRUE)

# Platform-specific packaging
if(WIN32)
    # Windows-specific packaging
    set(CPACK_GENERATOR "ZIP;NSIS")
    
    # NSIS installer configuration
    set(CPACK_NSIS_PACKAGE_NAME "VSCP CAN232 Driver")
    set(CPACK_NSIS_DISPLAY_NAME "VSCP CAN232 Driver ${FULL_VERSION}")
    # Keep install root macro simple; package subdirectory is handled by CPACK_PACKAGE_INSTALL_DIRECTORY.
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "VSCP/drivers/level1/vscpl1drv-can232")
    
    # NSIS Installer URL and contact information
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/vscp/vscpl1drv-can232")
    set(CPACK_NSIS_HELP_LINK "https://github.com/vscp/vscpl1drv-can232/issues")
    set(CPACK_NSIS_CONTACT "https://github.com/vscp")
    
    # NSIS installer options
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    
    # NSIS installation instructions
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "")
    
    # Keep NSIS customization minimal for CI reliability.
elseif(APPLE)
    # macOS-specific packaging (Homebrew-friendly source/binary tarball)
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${FULL_VERSION}-macos")
else()
    # Linux-specific packaging
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "VSCP Project")
    set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libvscp1 (>= 1.0.0)")
    # RPM packaging
    set(CPACK_RPM_PACKAGE_REQUIRES "libvscp1 >= 1.0.0")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    # Use "x64" instead of "amd64"/"x86_64" in package file names
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
        set(CPACK_DEBIAN_FILE_NAME "${CPACK_PACKAGE_NAME}_${FULL_VERSION}_x64.deb")
        set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_NAME}-${FULL_VERSION}-Linux-x64.rpm")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${FULL_VERSION}-Linux-x64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
        set(CPACK_DEBIAN_FILE_NAME "${CPACK_PACKAGE_NAME}_${FULL_VERSION}_arm64.deb")
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "aarch64")
        set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_NAME}-${FULL_VERSION}-Linux-arm64.rpm")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${FULL_VERSION}-Linux-arm64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|armv6l|armv7l|armhf)$")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
        set(CPACK_DEBIAN_FILE_NAME "${CPACK_PACKAGE_NAME}_${FULL_VERSION}_armhf.deb")
        set(CPACK_RPM_PACKAGE_ARCHITECTURE "armv7hl")
        set(CPACK_RPM_FILE_NAME "${CPACK_PACKAGE_NAME}-${FULL_VERSION}-Linux-armhf.rpm")
        set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${FULL_VERSION}-Linux-armhf")
    else()
        set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
        set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")
    endif()
endif()

# Apply build-time patches for VSCP library issues
# Fix typo in VSCP com.cpp: iconst -> const (upstream issue)
if(NOT EXISTS "${CMAKE_BINARY_DIR}/.vscp_patched")
    if(EXISTS "${THIRD_PARTY_VSCP_DIR}/src/common/com.cpp")
        file(READ "${THIRD_PARTY_VSCP_DIR}/src/common/com.cpp" COM_CPP_CONTENT)
        string(REPLACE "iconst char" "const char" COM_CPP_CONTENT "${COM_CPP_CONTENT}")
        file(WRITE "${THIRD_PARTY_VSCP_DIR}/src/common/com.cpp" "${COM_CPP_CONTENT}")
        file(WRITE "${CMAKE_BINARY_DIR}/.vscp_patched" "patched")
    endif()
endif()

# Set output directory for libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific builds
if(WIN32)
    message(STATUS "Building for Windows platform")
    add_subdirectory(win32)
elseif(APPLE)
    message(STATUS "Building for macOS platform")
    add_subdirectory(mac)
else()
    message(STATUS "Building for Linux platform")
    add_subdirectory(linux)
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "           VSCP CAN232 Driver Configuration")
message(STATUS "--------------------------------------------")
message(STATUS "Version                  : ${FULL_VERSION}")
message(STATUS "System                   : ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++ Compiler             : ${CMAKE_CXX_COMPILER}")
message(STATUS "C Compiler               : ${CMAKE_C_COMPILER}")
message(STATUS "Build type               : ${CMAKE_BUILD_TYPE}")
message(STATUS "Installation prefix      : ${VSCP_INSTALL_DIR}")
if(WIN32)
    message(STATUS "")
    message(STATUS "Windows Configuration")
    message(STATUS "Generator              : ${CMAKE_GENERATOR}")
    if(MSVC)
        message(STATUS "MSVC Version           : ${MSVC_VERSION}")
        message(STATUS "Runtime Library        : ${CMAKE_MSVC_RUNTIME_LIBRARY}")
    endif()
    message(STATUS "")
    message(STATUS "Next steps:")
    message(STATUS "1. Run: cmake --build build --config Release")
    message(STATUS "2. Or open build\\can232drv.sln in Visual Studio")
    message(STATUS "3. For details, see: CMAKE_WINDOWS.md")
elseif(APPLE)
    message(STATUS "")
    message(STATUS "macOS Configuration")
    message(STATUS "Architecture           : ${CMAKE_OSX_ARCHITECTURES}")
    message(STATUS "Deployment Target      : ${CMAKE_OSX_DEPLOYMENT_TARGET}")
    message(STATUS "")
    message(STATUS "Next steps:")
    message(STATUS "1. Run: cmake --build build")
    message(STATUS "2. Create brew package tarball: cpack -G TGZ")
    message(STATUS "3. For details, see: CMAKE_MACOS.md")
else()
    message(STATUS "")
    message(STATUS "Linux Configuration")
    message(STATUS "")
    message(STATUS "Next steps:")
    message(STATUS "1. Run: cmake --build build")
    message(STATUS "2. For details, see: CMAKE_BUILD.md")
endif()
message(STATUS "")

# Include CPack for packaging
include(CPack)
