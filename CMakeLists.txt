# Auth: Alberto Taiuti, Github usr: snowzufer

cmake_minimum_required(VERSION 2.8)

project(lode-png)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(lodepng
  ${CMAKE_CURRENT_SOURCE_DIR}/lodepng.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lodepng_util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/pngdetail.cpp)

set(LODEPNG_LIBRARY lodepng PARENT_SCOPE)
