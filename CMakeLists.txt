# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.1)

project(lodepng)

# We build using c++11
set(CMAKE_CXX_STANDARD 11)

# Set optimization flags
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Project's name
project(lodepng)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}")

# build a static library
add_library(lodepng STATIC ${CMAKE_SOURCE_DIR}/lodepng.cpp)

add_executable(example_4bit_palette ${CMAKE_SOURCE_DIR}/examples/example_4bit_palette.cpp)
target_link_libraries(example_4bit_palette "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_4bit_palette PROPERTIES OUTPUT_NAME "example_4bit_palette")
add_dependencies(example_4bit_palette lodepng)
add_executable(example_bmp2png ${CMAKE_SOURCE_DIR}/examples/example_bmp2png.cpp)
target_link_libraries(example_bmp2png "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_bmp2png PROPERTIES OUTPUT_NAME "example_bmp2png")
add_dependencies(example_bmp2png lodepng)
add_executable(example_decode ${CMAKE_SOURCE_DIR}/examples/example_decode.cpp)
target_link_libraries(example_decode "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_decode PROPERTIES OUTPUT_NAME "example_decode")
add_dependencies(example_decode lodepng)
add_executable(example_encode ${CMAKE_SOURCE_DIR}/examples/example_encode.cpp)
target_link_libraries(example_encode "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_encode PROPERTIES OUTPUT_NAME "example_encode")
add_dependencies(example_encode lodepng)
add_executable(example_encode_type ${CMAKE_SOURCE_DIR}/examples/example_encode_type.cpp)
target_link_libraries(example_encode_type "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_encode_type PROPERTIES OUTPUT_NAME "example_encode_type")
add_dependencies(example_encode_type lodepng)
add_executable(example_gzip ${CMAKE_SOURCE_DIR}/examples/example_gzip.cpp)
target_link_libraries(example_gzip "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_gzip PROPERTIES OUTPUT_NAME "example_gzip")
add_dependencies(example_gzip lodepng)
add_executable(example_optimize_png ${CMAKE_SOURCE_DIR}/examples/example_optimize_png.cpp)
target_link_libraries(example_optimize_png "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_optimize_png PROPERTIES OUTPUT_NAME "example_optimize_png")
add_dependencies(example_optimize_png lodepng)
add_executable(example_png2bmp ${CMAKE_SOURCE_DIR}/examples/example_png2bmp.cpp)
target_link_libraries(example_png2bmp "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_png2bmp PROPERTIES OUTPUT_NAME "example_png2bmp")
add_dependencies(example_png2bmp lodepng)
add_executable(example_png_info ${CMAKE_SOURCE_DIR}/examples/example_png_info.cpp)
target_link_libraries(example_png_info "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_png_info PROPERTIES OUTPUT_NAME "example_png_info")
add_dependencies(example_png_info lodepng)
add_executable(example_reencode ${CMAKE_SOURCE_DIR}/examples/example_reencode.cpp)
target_link_libraries(example_reencode "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
set_target_properties(example_reencode PROPERTIES OUTPUT_NAME "example_reencode")
add_dependencies(example_reencode lodepng)

#target_link_libraries(bsort-main "${LIBRARY_OUTPUT_PATH}/liblodepng.a")
#set_target_properties(bsort-main PROPERTIES OUTPUT_NAME "bsort")
#add_dependencies(bsort-main bsort)
