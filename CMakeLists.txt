cmake_minimum_required(VERSION 3.1)

project(librg_node VERSION 1.0.0)

add_subdirectory(node_modules/librg.c)

include(NodeJS.cmake)
nodejs_init()

add_nodejs_module(${PROJECT_NAME}
  ${CMAKE_SOURCE_DIR}/src/stdafx.h
  ${CMAKE_SOURCE_DIR}/src/module.h
  ${CMAKE_SOURCE_DIR}/src/module.cpp
)

target_link_libraries(${PROJECT_NAME} librg)