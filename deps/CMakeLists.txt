cmake_minimum_required(VERSION 3.5.0)

project(deps)

ADD_DEFINITIONS(-DBUILD_SHARED_LIBS=OFF)

add_subdirectory(elog)
add_subdirectory(webrtc)
