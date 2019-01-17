
foreach(example
    4bit_palette
    bmp2png
    decode
    encode
    gzip
    #opengl
    optimize_png
    png2bmp
    png_info
    reencode
    #sdl
  )
  add_executable(example_${example}
    example_${example}.cpp)
  target_link_libraries(example_${example} PRIVATE lodepng)
endforeach()

# SDL2 Dependency
find_package(SDL2 CONFIG REQUIRED)
if (TARGET SDL2::SDL2)
  message(STATUS "using TARGET SDL2::SDL2")
else()
  message(STATUS "no TARGET SDL2::SDL2, or SDL2, using variables")
endif()
foreach(example
    #opengl
    #sdl
  )
  if (TARGET SDL2::SDL2)
    target_link_libraries(example_${example} PRIVATE SDL2::SDL2)
  else()
    target_include_directories(example_${example} PRIVATE $<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>)
    target_link_libraries(example_${example} PRIVATE ${SDL2_LIBRARIES})
  endif()
endforeach()
