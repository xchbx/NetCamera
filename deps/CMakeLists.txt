cmake_minimum_required(VERSION 3.5.0)

SET(CMAKE_INSTALL_PREFIX ".")
project(deps)

ADD_DEFINITIONS(-DBUILD_SHARED_LIBS=OFF)


add_subdirectory(elog)
add_subdirectory(webrtc)
add_subdirectory(libsrtp)
add_subdirectory(JThread)
add_subdirectory(jrtplib)
add_subdirectory(gsm)
add_subdirectory(tinyjpeg)
add_subdirectory(lpc)
