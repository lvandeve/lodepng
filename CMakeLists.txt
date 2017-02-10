cmake_minimum_required(VERSION 2.4.4)

project(lodepng)

#============================================================================
# LodePNG library
#============================================================================

set(PNG_PUBLIC_HDRS
	lodepng.h
	lodepng_util.h
)
set(PNG_PRIVATE_HDRS
	""
)
set(PNG_SRCS
	lodepng.cpp
	lodepng_util.cpp
)

add_library(lodepng STATIC ${PNG_SRCS} ${PNG_PUBLIC_HDRS} ${PNG_PRIVATE_HDRS})

#============================================================================
# Example binaries
#============================================================================

include_directories(".")

add_executable(example_decode examples/example_decode.cpp)
target_link_libraries(example_decode lodepng)

add_executable(example_encode examples/example_encode.cpp)
target_link_libraries(example_encode lodepng)

add_executable(example_bmp2png examples/example_bmp2png.cpp)
target_link_libraries(example_bmp2png lodepng)

add_executable(example_png2bmp examples/example_png2bmp.cpp)
target_link_libraries(example_png2bmp lodepng)

add_executable(example_png_info examples/example_png_info.cpp)
target_link_libraries(example_png_info lodepng)

add_executable(example_optimize_png examples/example_optimize_png.cpp)
target_link_libraries(example_optimize_png lodepng)

add_executable(example_4bit_palette examples/example_4bit_palette.cpp)
target_link_libraries(example_4bit_palette lodepng)

find_package(OpenGL QUIET)
if(OPENGL_FOUND)
	include_directories(${OPENGL_INCLUDE_DIR})
	add_definitions(${OPENGL_DEFINITIONS})
else()
	message("-- Can't find OpenGL. Continuing without it.")
endif()

find_package(SDL QUIET)
if(SDL_FOUND)
	include_directories(${SDL_INCLUDE_DIR})
	add_definitions(${SDL_DEFINITIONS})
else()
	message("-- Can't find SDL. Continuing without it.")
endif()

if(OPENGL_FOUND AND SDL_FOUND)
	add_executable(example_opengl examples/example_opengl.cpp)
	target_link_libraries(example_opengl lodepng ${OPENGL_LIBRARIES} ${SDL_LIBRARIES})

	add_executable(example_sdl examples/example_sdl.cpp)
	target_link_libraries(example_sdl lodepng ${OPENGL_LIBRARIES} ${SDL_LIBRARIES})
endif()
