# Copyright 2005-2025 Lode Vandevenne
# Copyright 2025 Mark Callow
# SPDX-License-Identifier: Zlib

# This CMake project only makes the unit test, benchmark and pngdetail and showpng
# utilities. It does not make the PNG codec itself as shared or static library.
# That is because:
# LodePNG itself has only 1 source file (lodepng.cpp, can be renamed to
# lodepng.c) and is intended to be included as source file in other projects and
# their build system directly.

cmake_minimum_required(VERSION 3.28)
include(CMakePrintHelpers)

# Must be set before project() so the variable can affect vcpkg.
set(VCPKG_MANIFEST_FEATURES sdl2)

# TODO: Automatic version generation from repo tags.
project(lodepng
    VERSION 20250506
    DESCRIPTION "Tests of and utilities using lodepng PNG codec."
    HOMEPAGE_URL https://github.com/lvandeve/lodepng
    LANGUAGES C;CXX
)

option( LODEPNG_WERROR "Make all warnings in lodepng code, errors." OFF)

#include(CTest)  # Must be after project

# Global compile options
if(MSVC)
    # Can't find an equivalent to -ansi (-std=c90, -std=c++98) in modern MSVC.
    add_compile_options( /W4;$<$<BOOL:${LODEPNG_WERROR}>:/WX> )
    add_compile_options( $<IF:$<CONFIG:Debug>,/Gz,/O2> )
    # Enable UTF-8 support
    add_compile_options( $<$<C_COMPILER_ID:MSVC>:/utf-8> )
    add_compile_options( $<$<CXX_COMPILER_ID:MSVC>:/utf-8> )
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU"
       OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    # -W is --no-warnings in clang and modern gcc. But the GCC documentation at
    # https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html has conflicting
    # information that it is the same as -Wextra and "The older name is still
    # supported". Do you really want to use it?
    #
    # WHEN BUILDING WITH XCODE THERE ARE A LOT OF WARNINGS ABOUT CONVERSION FROM
    # size_t TO int IN lodepng_unittest.cpp. -Wno-conversion HAS BEEN ADDED TO SUPPRESS
    # THESE BUT THE WARNINGS SHOULD REALLY BE FIXED. I FIXED THREE IN showpng.cpp.
    # These warnings do no appear when building with gcc or clang from the command
    # line. Presumably Xcode turns these warnings on by default.
#    add_compile_options( -W -Wall -Wextra -ansi -pedantic -Wno-unused-function -Wno-conversion $<$<BOOL:${LODEPNG_WERROR}>:-Werror>)
    add_compile_options( -W -Wall -Wextra -ansi -pedantic -Wno-unused-function $<$<BOOL:${LODEPNG_WERROR}>:-Werror>)
    add_compile_options( $<IF:$<CONFIG:Debug>,-O0$<SEMICOLON>-g,-O3> )
    if(EMSCRIPTEN)
        add_link_options( $<IF:$<CONFIG:Debug>,-gsource-map,-O3> )
    else()
        add_link_options( $<IF:$<CONFIG:Debug>,-g,-O3> )
    endif()
else()
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} not yet supported.")
endif()

add_executable(unittest
    lodepng.cpp
    lodepng_unittest.cpp
    lodepng_util.cpp
    CMakeLists.txt
)

add_executable(pngdetail
    lodepng.cpp
    pngdetail.cpp
    lodepng_util.cpp
    CMakeLists.txt
)

find_package(SDL2)

if(TARGET SDL2::SDL2)
  add_executable(showpng
      examples/example_sdl.cpp
      lodepng.cpp
      CMakeLists.txt
  )

  target_include_directories(
    showpng
  SYSTEM AFTER PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    # The directory made public by this target is the <SDL2> directory. As
    # example_sdl.cpp uses #include <SDL2/foo>, must use SDL_INCLUDE_DIRS
    # which includes the parent of <SDL2> as well as <SDL2>.
    #SDL2::SDL2
    ${SDL2_INCLUDE_DIRS}
  )

  target_link_libraries(
    showpng
  PUBLIC
    # Use SDL2_LIBRARIES because it includes SD2Lmain when on Windows. Using
    # the targets directly would require an if(WIN32).
    #SDL2::SDL2
    ${SDL2_LIBRARIES}
  )

  add_executable(benchmark
    lodepng.cpp
    lodepng_benchmark.cpp
    CMakeLists.txt
  )

  target_include_directories(
    benchmark
  SYSTEM AFTER PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Ditto include directory comment under showpng.
    #SDL2::SDL2
    ${SDL2_INCLUDE_DIRS}
  )

  target_link_libraries(
    benchmark
  PUBLIC
    # Ditto libraries comment under showpng.
    #SDL2::SDL2
    ${SDL2_LIBRARIES}
  )
else()
  message(NOTICE "SDL2 not found. Install SDL2 or vcpkg to configure and build targets showpng and benchmark.")
endif()

