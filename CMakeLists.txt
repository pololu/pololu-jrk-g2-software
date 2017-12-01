cmake_minimum_required (VERSION 2.8.11)

# Fix behavior of CMAKE_CXX_STANDARD when targeting macOS.
if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

# Fix a warning on Mac OS X.
if (POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif ()

# Fix a warning
if (POLICY CMP0043)
  cmake_policy (SET CMP0043 NEW)
endif ()

# Don't use -rdynamic since it breaks causes musl static linking.
if (POLICY CMP0065)
  cmake_policy(SET CMP0065 NEW)
endif ()

project (jrk)

set (CLI_NAME "jrkcmd")
set (GUI_NAME "jrkgui")
set (DOCUMENTATION_URL "https://www.pololu.com/docs/0J71")

set (SOFTWARE_VERSION_MAJOR 1)
set (SOFTWARE_VERSION_MINOR 0)
set (SOFTWARE_VERSION_PATCH 0)

option (BUILD_SHARED_LIBS "Build a shared library" TRUE)
if (NOT BUILD_SHARED_LIBS)
  add_definitions (-DJRK_STATIC)
endif ()

set(ENABLE_GUI TRUE CACHE BOOL
  "True if you want to build the GUI, which depends on Qt 5.")

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Options are Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif ()

if (EXISTS "${CMAKE_SOURCE_DIR}/images/app.icns")
  set (POLOLU_BUILD TRUE)
endif ()
if (EXISTS "${CMAKE_SOURCE_DIR}/images/app.ico")
  set (POLOLU_BUILD TRUE)
endif ()

set (SOFTWARE_VERSION ${SOFTWARE_VERSION_MAJOR}.${SOFTWARE_VERSION_MINOR}.${SOFTWARE_VERSION_PATCH})

string(TIMESTAMP YEAR "%Y")

find_package(PkgConfig)

# Our C code uses features from the C99 standard.
macro(use_c99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(use_c99)

# Our C++ code uses features from the C++11 standard.
macro(use_cxx11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
    endif ()
  else ()
    set (CMAKE_CXX_STANDARD 11)
  endif ()
endmacro(use_cxx11)

# Put libraries and executables in the top level of the build directory
# so that the executables can find the libraries and it is easy to run
# everything.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (WIN32)
  # Enable correct behavior for the return value of vsnprintf.
  add_definitions (-D__USE_MINGW_ANSI_STDIO=1)

  # Enable functions only available in Windows Vista and later.
  add_definitions (-D_WIN32_WINNT=0x0600 -DNTDDI_VERSION=0x06000000)
endif ()


# Detect Linux.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set (LINUX TRUE)
endif ()

include_directories (
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_BINARY_DIR}/include"
)

configure_file (
  "include/config.h.in"
  "include/config.h"
)

file(WRITE "${CMAKE_BINARY_DIR}/version.txt" "${SOFTWARE_VERSION}")

# TODO: add_subdirectory (lib)
# TODO: add_subdirectory (cli)
# TODO: add_subdirectory (bootloader)

if (ENABLE_GUI)
  add_subdirectory (gui)
endif ()

# Install the header files into include/
install(FILES include/jrk.h include/jrk.hpp include/jrk_protocol.h
  DESTINATION "include/libpololu-jrk-${SOFTWARE_VERSION_MAJOR}")
