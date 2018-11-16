project(lodepng)

set (SOURCES
     lodepng.cpp
     lodepng_util.cpp
     pngdetail.cpp
)

include_directories(${CMAKE_CURRENT_DIRECTORY})

add_library(lodepng STATIC ${SOURCES})
