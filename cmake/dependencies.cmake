# dependencies.cmake
#
# Third-party sources, fetched at configure time with FetchContent. They used
# to be git submodules in third-party/; each is pinned to the same commit.
# Every download is checked against its SHA256.
#
# To use a local checkout instead, set FETCHCONTENT_SOURCE_DIR_<NAME>, for
# example -DFETCHCONTENT_SOURCE_DIR_TP_VSCP=/path/to/vscp.
# FETCHCONTENT_FULLY_DISCONNECTED=ON reuses downloaded sources offline.
#
# Sets:
#   THIRD_PARTY_VSCP_DIR               grodansparadis/vscp
#

include_guard(GLOBAL)
include(FetchContent)

set(_tp_options)
if(NOT CMAKE_VERSION VERSION_LESS "3.18")
    # Populate the sources only; the projects are not added to the build.
    list(APPEND _tp_options SOURCE_SUBDIR "__sources_only__")
endif()
if(NOT CMAKE_VERSION VERSION_LESS "3.24")
    list(APPEND _tp_options DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
endif()

# grodansparadis/vscp @ de6f056154b9572b83764d3e4c84292c17a66171
FetchContent_Declare(
    tp_vscp
    URL https://codeload.github.com/grodansparadis/vscp/tar.gz/de6f056154b9572b83764d3e4c84292c17a66171
    URL_HASH SHA256=81147e018a92c8984d3169e449f57ca8c350cfcd3c558f78a61f815acec34ec9
    ${_tp_options}
)

set(_tp_names tp_vscp)
if(CMAKE_VERSION VERSION_LESS "3.18")
    foreach(_tp IN LISTS _tp_names)
        FetchContent_GetProperties(${_tp})
        if(NOT ${_tp}_POPULATED)
            FetchContent_Populate(${_tp})
        endif()
    endforeach()
else()
    FetchContent_MakeAvailable(${_tp_names})
endif()

set(THIRD_PARTY_VSCP_DIR "${tp_vscp_SOURCE_DIR}")
