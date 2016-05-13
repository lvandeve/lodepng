cmake_minimum_required(VERSION 3.3.2)

project(LodePNG)

include(GenerateExportHeader)
find_package(SDL)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
file(GLOB_RECURSE SOURCES *.cpp)

set(LIBRARIES lodepng lodepng_util)
foreach(LIBRARY ${LIBRARIES})
  add_library(${LIBRARY} ${LIBRARY}.cpp)
  if (NOT LIBRARY STREQUAL "lodepng")
    target_link_libraries(${LIBRARY} lodepng)
  else ()
    generate_export_header(${LIBRARY})
  endif ()
  install(TARGETS ${LIBRARY}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endforeach()

string(LENGTH ${CMAKE_SOURCE_DIR} LENGTH_CMAKE_SOURCE_DIR)
set(SDL_DEPENDENT  example_opengl example_sdl lodepng_benchmark)
set(UTIL_DEPENDENT lodepng_unittest pngdetail)
enable_testing()

foreach(SOURCE ${SOURCES})
  string(SUBSTRING ${SOURCE} ${LENGTH_CMAKE_SOURCE_DIR} -1 EXECUTABLE_NAME)
  string(REPLACE ".cpp" "" EXECUTABLE_NAME ${EXECUTABLE_NAME})
  string(REPLACE "/examples/" "" EXECUTABLE_NAME ${EXECUTABLE_NAME})
  string(REPLACE "/" "" EXECUTABLE_NAME ${EXECUTABLE_NAME})

  if (NOT ${EXECUTABLE_NAME} IN_LIST LIBRARIES)
    set(LINK_LIBRARIES lodepng)
    if (EXECUTABLE_NAME IN_LIST UTIL_DEPENDENT)
      list(APPEND LINK_LIBRARIES lodepng_util)
    endif()
    if (EXECUTABLE_NAME IN_LIST SDL_DEPENDENT)
      if (NOT SDL_FOUND)
	continue()
      endif()
      list(APPEND LINK_LIBRARIES ${SDL_LIBRARY})
      list(APPEND INCLUDE_DIRS   ${SDL_INCLUDE_DIR})
    endif()

    add_executable(${EXECUTABLE_NAME} ${SOURCE})
    if (INCLUDE_DIRS)
      target_include_directories(${EXECUTABLE_NAME} PUBLIC ${INCLUDE_DIRS})
    endif()
    target_link_libraries(${EXECUTABLE_NAME} ${LINK_LIBRARIES})
    add_test(NAME ${EXECUTABLE_NAME} COMMAND $<TARGET_FILE:${EXECUTABLE_NAME}>)
    install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
  endif()
endforeach()

install(FILES ${CMAKE_BINARY_DIR}/lodepng_export.h lodepng.h lodepng_util.h DESTINATION include)

include(InstallRequiredSystemLibraries)
include(CPack)
