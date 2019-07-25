cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_CXX_STANDARD 14)

project(elog)

include_directories(.)


file(GLOB_RECURSE FILES_SOURCES *.c)
file(GLOB_RECURSE FILES_INCLUDE *.h *.hpp)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${FILES_INCLUDE} ${FILES_SOURCES})
target_link_libraries(${PROJECT_NAME})

