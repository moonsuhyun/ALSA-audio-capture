cmake_minimum_required (VERSION 3.15)


# =============================================================================
# This project is based on C++ 11 / C 90 (Ancient C) for maximum compatibility.
# =============================================================================
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_STANDARD   90)

# ====================================================================
# Directory - related CMake environment variable
#   - AMI_DIR_INCLUDE : Directory for exported header
#   - AMI_DIR_SRC     : Directory for source file / internal header
#   - AMI_DIR         : Root directory
# ====================================================================
set (AMI_DIR_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set (AMI_DIR_SRC     ${CMAKE_CURRENT_SOURCE_DIR}/src)
set (AMI_DIR         ${CMAKE_CURRENT_SOURCE_DIR})

project (alsa-mic-input)

include (cmake/setup.cmake)

add_subdirectory (src)