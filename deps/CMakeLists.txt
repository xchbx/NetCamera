cmake_minimum_required(VERSION 3.5.0)

project(deps)

find_package(Threads)
set(JTHREAD_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/deps/jthread/src")
set(JTHREAD_LIBRARIES "${CMAKE_SOURCE_DIR}/build/lib")

ADD_DEFINITIONS(-DBUILD_SHARED_LIBS=OFF)

add_subdirectory(elog)
add_subdirectory(webrtc)
add_subdirectory(jthread)
add_subdirectory(jrtplib)
add_subdirectory(libsrtp)
add_subdirectory(emiplib)
