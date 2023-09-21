cmake_minimum_required(VERSION 3.0)

project(nvenc-loader)

add_library(nvencloader STATIC nvenc_loader.c)

message(${CMAKE_LIBRARY_ARCHITECTURE})

target_include_directories(nvencloader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

