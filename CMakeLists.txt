cmake_minimum_required(VERSION 2.8)
project(lodepng)

find_package(SDL)

set(CMAKE_CXX_FLAGS "-W -Wall -Wextra -ansi -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(.)
add_library(lodepng STATIC lodepng.cpp)
install(TARGETS lodepng)

add_executable(unittest lodepng_util.cpp lodepng_unittest.cpp)
target_link_libraries(unittest lodepng)

add_executable(pngdetail lodepng_util.cpp pngdetail.cpp)
target_link_libraries(pngdetail lodepng)
install(TARGETS pngdetail)

if (SDL_FOUND)
	add_executable(benchmark lodepng_benchmark.cpp)
	target_include_directories(benchmark PRIVATE ${SDL_INCLUDE_DIRS})
	target_link_libraries(benchmark lodepng ${SDL_LIBRARY})

	add_executable(showpng examples/example_sdl.cpp)
	target_include_directories(benchmark PRIVATE ${SDL_INCLUDE_DIRS})
	target_link_libraries(showpng lodepng ${SDL_LIBRARY})
	install(TARGETS showpng)
endif(SDL_FOUND)
