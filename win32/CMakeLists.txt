# Windows-specific driver build for can232drv DLL

# Ensure we're on Windows
if(NOT WIN32)
    message(FATAL_ERROR "This CMakeLists.txt is for Windows builds only. Use linux/CMakeLists.txt on Linux.")
endif()

# Set MSVC runtime library to dynamic
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "DynamicMDd" CACHE STRING "MSVC Runtime Library" FORCE)
    # Use /MD for release, /MDd for debug (already set by above)
endif()

# Compiler flags for Windows
if(MSVC)
    # MSVC-specific flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /WX-")  # Warning level 3, don't treat warnings as errors
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3 /WX-")
    
    # Define macro for MSVC
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_WARNINGS)
else()
    # MinGW/Clang on Windows
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()

# Windows-specific defines
add_definitions(-DWIN32 -D_WINDOWS)

# Include directories - DLL version
include_directories(
    ../third-party/vscp/src/vscp/common
    ../third-party/vscp/src/common
    ../common
    .)

# Find required packages
find_package(Threads REQUIRED)

# Source files for common objects
set(COMMON_SOURCES
    ../common/dlldrvobj.cpp
    ../common/can232obj.cpp
    ../common/com_win32.cpp
    ../third-party/vscp/src/common/dllist.c
)

set(COMMON_HEADERS
    ../common/dlldrvobj.h
    ../common/can232obj.h
)

# Windows DLL main sources
set(CAN232DRV_SOURCES
    can232drv/can232drv.cpp
    ${COMMON_SOURCES}
)

# Create compatibility headers for naming differences
file(WRITE ${CMAKE_BINARY_DIR}/canal_macro.h "#include \"canal-macro.h\"\n")

# Add build directory to include paths for compatibility headers
set(VSCP_COMPAT_INCLUDE_DIR ${CMAKE_BINARY_DIR})

# Create DLL library
add_library(can232drv SHARED ${CAN232DRV_SOURCES})

# Set version properties for Windows DLL
set_target_properties(can232drv PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MAJOR_VERSION}
    PREFIX ""  # Remove 'lib' prefix on Windows
    SUFFIX ".dll"  # Ensure .dll extension
)

# Set include directories for DLL target
target_include_directories(can232drv PRIVATE
    ../third-party/vscp/src/vscp/common
    ../third-party/vscp/src/common
    ../common
    ${VSCP_COMPAT_INCLUDE_DIR}
    .)

# Link libraries for Windows
target_link_libraries(can232drv PRIVATE
    Threads::Threads
    ws2_32)  # Windows socket library

# Compiler definitions
target_compile_definitions(can232drv PRIVATE
    MAJOR_VERSION=${MAJOR_VERSION}
    MINOR_VERSION=${MINOR_VERSION}
    RELEASE_VERSION=${RELEASE_VERSION}
    BUILD_VERSION=${BUILD_VERSION}
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS)

# For MSVC, also create a static library version
if(MSVC)
    add_library(can232drv-static STATIC ${COMMON_SOURCES})
    
    set_target_properties(can232drv-static PROPERTIES
        OUTPUT_NAME can232drv
        VERSION ${PROJECT_VERSION})
    
    target_include_directories(can232drv-static PRIVATE
        ../third-party/vscp/src/vscp/common
        ../third-party/vscp/src/common
        ../common
        ${VSCP_COMPAT_INCLUDE_DIR}
        .)
    
    target_link_libraries(can232drv-static PRIVATE
        Threads::Threads
        ws2_32)
    
    target_compile_definitions(can232drv-static PRIVATE
        MAJOR_VERSION=${MAJOR_VERSION}
        MINOR_VERSION=${MINOR_VERSION}
        RELEASE_VERSION=${RELEASE_VERSION}
        BUILD_VERSION=${BUILD_VERSION}
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_WARNINGS)
endif()

# Installation for Windows
# Typical Windows installation path: Program Files/VSCP drivers
if(NOT VSCP_INSTALL_DIR)
    if(CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
        # If using default, set Windows-appropriate path
        set(VSCP_INSTALL_DIR "C:/Program Files/VSCP/drivers/level1" CACHE PATH "VSCP installation directory")
    else()
        set(VSCP_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/drivers/level1" CACHE PATH "VSCP installation directory")
    endif()
endif()

install(TARGETS can232drv
    LIBRARY DESTINATION "${VSCP_INSTALL_DIR}"
    RUNTIME DESTINATION "${VSCP_INSTALL_DIR}")

if(MSVC)
    install(TARGETS can232drv-static
        ARCHIVE DESTINATION "${VSCP_INSTALL_DIR}")
    
    install(FILES ${CMAKE_BINARY_DIR}/can232drv.pdb
            DESTINATION "${VSCP_INSTALL_DIR}"
            OPTIONAL)
endif()

# Install headers
install(FILES ${COMMON_HEADERS}
    DESTINATION include)

message(STATUS "Windows build configuration complete")
message(STATUS "DLL will be installed to: ${VSCP_INSTALL_DIR}")
