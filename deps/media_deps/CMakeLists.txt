cmake_minimum_required(VERSION 3.5.0)

project(media_deps)

find_package(Threads)
set(JTHREAD_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/deps/media_deps/JThread/src" "${PROJECT_BINARY_DIR}/JThread/src")
set(JTHREAD_LIBRARIES "${CMAKE_SOURCE_DIR}/build/lib")

MESSAGE(STATUS "JTHREAD INC----${JTHREAD_INCLUDE_DIRS}")
MESSAGE(STATUS "JTHREAD LIB----${JTHREAD_LIBRARIES}")

ADD_DEFINITIONS(-DBUILD_SHARED_LIBS=OFF)

add_subdirectory(JThread)
add_subdirectory(jrtplib)
add_subdirectory(libsrtp)
add_subdirectory(emiplib)
