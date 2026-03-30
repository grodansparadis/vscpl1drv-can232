# macOS-specific driver build

# Compiler flags for macOS
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -DCBC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__MACOSX__ -fPIC -D_REENTRANT")

# Warning flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# macOS specific settings
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_INSTALL_RPATH "@loader_path/../lib;@executable_path/../lib")

# Optimization and debugging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# Define VSCP include paths
set(VSCP_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/third-party/vscp/src/vscp/common
    ${CMAKE_SOURCE_DIR}/third-party/vscp/src/common
)

# Create compatibility headers for naming differences (underscores vs hyphens)
# VSCP uses hyphens (canal-macro.h) but driver code expects underscores (canal_macro.h)
file(WRITE ${CMAKE_BINARY_DIR}/canal_macro.h "#include \"canal-macro.h\"\n")

# Add build directory to include paths for compatibility headers
set(VSCP_COMPAT_INCLUDE_DIR ${CMAKE_BINARY_DIR})

# Add both VSCP paths and compatibility path
include_directories(${VSCP_INCLUDE_DIRS} ${VSCP_COMPAT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/linux .)

# Find required packages
find_package(Threads REQUIRED)

# Check for required libraries
find_library(M_LIBRARY m)

# Source files for the driver
set(CAN232DRV_SOURCES
    ${CMAKE_SOURCE_DIR}/linux/vscpl1drv-can232.cpp
    ${CMAKE_SOURCE_DIR}/linux/can232obj.cpp
    ${CMAKE_SOURCE_DIR}/third-party/vscp/src/common/dllist.c
    ${CMAKE_SOURCE_DIR}/third-party/vscp/src/common/com.cpp
)

set(CAN232DRV_HEADERS
    ${CMAKE_SOURCE_DIR}/linux/vscpl1drv-can232.h
    ${CMAKE_SOURCE_DIR}/linux/can232obj.h
)

# Create dynamic library (macOS dylib)
add_library(vscpl1drv-can232 SHARED ${CAN232DRV_SOURCES})

# Set include directories for the target
target_include_directories(vscpl1drv-can232 PRIVATE ${VSCP_INCLUDE_DIRS} ${VSCP_COMPAT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/linux .)

# Set version properties
set_target_properties(vscpl1drv-can232 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${MAJOR_VERSION}
    SUFFIX ".dylib")

# Link libraries
target_link_libraries(vscpl1drv-can232 PRIVATE
    Threads::Threads
    ${M_LIBRARY})

# Compiler definitions
target_compile_definitions(vscpl1drv-can232 PRIVATE
    MAJOR_VERSION=${MAJOR_VERSION}
    MINOR_VERSION=${MINOR_VERSION}
    RELEASE_VERSION=${RELEASE_VERSION}
    BUILD_VERSION=${BUILD_VERSION})

# Create static library
add_library(vscpl1drv-can232-static STATIC ${CAN232DRV_SOURCES})
set_target_properties(vscpl1drv-can232-static PROPERTIES
    OUTPUT_NAME vscpl1drv-can232
    VERSION ${PROJECT_VERSION})

# Set include directories for static target
target_include_directories(vscpl1drv-can232-static PRIVATE ${VSCP_INCLUDE_DIRS} ${VSCP_COMPAT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/linux .)

target_link_libraries(vscpl1drv-can232-static PRIVATE
    Threads::Threads
    ${M_LIBRARY})

target_compile_definitions(vscpl1drv-can232-static PRIVATE
    MAJOR_VERSION=${MAJOR_VERSION}
    MINOR_VERSION=${MINOR_VERSION}
    RELEASE_VERSION=${RELEASE_VERSION}
    BUILD_VERSION=${BUILD_VERSION})

# Optional: Build test executable
if(BUILD_TESTS)
    add_executable(test ${CMAKE_SOURCE_DIR}/linux/test.cpp ${CAN232DRV_SOURCES})
    target_include_directories(test PRIVATE ${VSCP_INCLUDE_DIRS} ${VSCP_COMPAT_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/linux .)
    target_compile_definitions(test PRIVATE
        MAJOR_VERSION=${MAJOR_VERSION}
        MINOR_VERSION=${MINOR_VERSION}
        RELEASE_VERSION=${RELEASE_VERSION}
        BUILD_VERSION=${BUILD_VERSION})
    target_link_libraries(test PRIVATE
        Threads::Threads
        ${M_LIBRARY})
endif()

# Installation
install(TARGETS vscpl1drv-can232
    LIBRARY DESTINATION drivers/level1)

install(TARGETS vscpl1drv-can232-static
    ARCHIVE DESTINATION drivers/level1)

install(FILES ${CAN232DRV_HEADERS}
    DESTINATION include)

# Install man page if pandoc is available
find_program(PANDOC pandoc)
if(PANDOC)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/man)
    add_custom_command(OUTPUT vscpl1drv-can232.1
        COMMAND ${PANDOC} -s -t man ${CMAKE_SOURCE_DIR}/man/vscpl1drv-can232.1.md -o ${CMAKE_BINARY_DIR}/man/vscpl1drv-can232.1
        DEPENDS ${CMAKE_SOURCE_DIR}/man/vscpl1drv-can232.1.md
        COMMENT "Generating man page")
    add_custom_target(man ALL DEPENDS vscpl1drv-can232.1)
    install(FILES ${CMAKE_BINARY_DIR}/man/vscpl1drv-can232.1
            DESTINATION share/man/man1)
endif()
