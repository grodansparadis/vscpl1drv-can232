cmake_minimum_required(VERSION 3.10)

# Extract version from VERSION.m4
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION.m4" VERSION_FILE)
string(REGEX MATCH "major_version\],  *\\[([0-9]+)" _ "${VERSION_FILE}")
set(MAJOR_VERSION "${CMAKE_MATCH_1}")
string(REGEX MATCH "minor_version\],  *\\[([0-9]+)" _ "${VERSION_FILE}")
set(MINOR_VERSION "${CMAKE_MATCH_1}")
string(REGEX MATCH "release_version\], *\\[([0-9]+)" _ "${VERSION_FILE}")
set(RELEASE_VERSION "${CMAKE_MATCH_1}")
string(REGEX MATCH "build_version\],   *\\[([0-9]+)" _ "${VERSION_FILE}")
set(BUILD_VERSION "${CMAKE_MATCH_1}")

set(PACKAGE_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE_VERSION}")
set(FULL_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE_VERSION}.${BUILD_VERSION}")

project(vscpl1drv-can232 
        VERSION ${PACKAGE_VERSION}
        DESCRIPTION "VSCP Level I driver for CAN232 devices"
        LANGUAGES C CXX)

# Check for VSCP submodule
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third-party/vscp/src/common/dllist.c")
    message(FATAL_ERROR
        "VSCP submodule is required but not initialized.\n"
        "Please run the following command from the project root:\n"
        "  git submodule update --init --recursive\n"
        "Or use the setup script:\n"
        "  ./setup.sh")
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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VSCP Level I driver for CAN232 devices")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "vscpl1drv-can232")
set(CPACK_STRIP_FILES TRUE)

# Platform-specific packaging
if(WIN32)
    # Windows-specific packaging
    set(CPACK_GENERATOR "ZIP;NSIS")
    
    # NSIS installer configuration
    set(CPACK_NSIS_PACKAGE_NAME "VSCP CAN232 Driver")
    set(CPACK_NSIS_DISPLAY_NAME "VSCP CAN232 Driver ${FULL_VERSION}")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES\\VSCP\\drivers\\level1")
    
    # NSIS Installer URL and contact information
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/vscp/vscpl1drv-can232")
    set(CPACK_NSIS_HELP_LINK "https://github.com/vscp/vscpl1drv-can232/issues")
    set(CPACK_NSIS_CONTACT "https://github.com/vscp")
    
    # NSIS installer options
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    
    # Use custom NSIS script if available
    if(EXISTS "${CMAKE_SOURCE_DIR}/win32/vscpl1drv-can232.nsi")
        set(CPACK_NSIS_INSTALLER_ICON "${CMAKE_SOURCE_DIR}/win32/vscpl1drv-can232.nsi")
    endif()
    
    # NSIS installation instructions
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "")
    set(CPACK_NSIS_DEFINES "PRODUCT_VERSION=${FULL_VERSION}")
    
    # Add registry entries for better Windows integration
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS 
        "WriteRegStr HKCU 'Software\\\\VSCP\\\\CAN232Driver' 'InstallPath' '$INSTDIR'")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS 
        "DeleteRegKey HKCU 'Software\\\\VSCP\\\\CAN232Driver'")
elseif(APPLE)
    # macOS-specific packaging
    set(CPACK_GENERATOR "DragDrop;TGZ;ProductBuild")
    
    # DragDrop (DMG) configuration
    set(CPACK_DMG_VOLUME_NAME "VSCP CAN232 Driver")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_DMG_COMPRESSION_LEVEL 9)
    
    # ProductBuild (PKG) configuration
    set(CPACK_PRODUCTBUILD_IDENTIFIER "com.vscp.can232driver")
    set(CPACK_PRODUCTBUILD_INSTALL_LOCATION "/usr/local/vscp/drivers/level1")
else()
    # Linux-specific packaging
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "VSCP Project")
    set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libvscp1 (>= 1.0.0)")
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
    # RPM packaging
    set(CPACK_RPM_PACKAGE_REQUIRES "libvscp1 >= 1.0.0")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_FILE_NAME "RPM-DEFAULT")
endif()

# Apply build-time patches for VSCP library issues
# Fix typo in VSCP com.cpp: iconst -> const (upstream issue)
if(NOT EXISTS "${CMAKE_BINARY_DIR}/.vscp_patched")
    if(EXISTS "${CMAKE_SOURCE_DIR}/third-party/vscp/src/common/com.cpp")
        file(READ "${CMAKE_SOURCE_DIR}/third-party/vscp/src/common/com.cpp" COM_CPP_CONTENT)
        string(REPLACE "iconst char" "const char" COM_CPP_CONTENT "${COM_CPP_CONTENT}")
        file(WRITE "${CMAKE_SOURCE_DIR}/third-party/vscp/src/common/com.cpp" "${COM_CPP_CONTENT}")
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
    message(STATUS "2. Create DMG: cpack -G DragDrop")
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
