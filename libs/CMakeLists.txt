cmake_minimum_required(VERSION 3.5.0)

project(NetCamLib)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++11")
SET(CMAKE_SOURCE_DIR .)
SET(CMAKE_INSTALL_PREFIX ".")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


ADD_SUBDIRECTORY(JThread)
ADD_SUBDIRECTORY(jrtplib)
add_subdirectory(libsrtp)
add_subdirectory(tinyjpeg)
add_subdirectory(lpc)
