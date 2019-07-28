SET(NetCamera_SOURCES
	EchoCanceller.h
    EchoCanceller.cpp
    PackageQueue.h
    PackageQueue.cpp
    test.cpp
	)

#add_definitions(-std=c++11)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/elog)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/webrtc/independent_aec/src)

ADD_EXECUTABLE(NetCamera ${NetCamera_SOURCES})
TARGET_LINK_LIBRARIES(NetCamera elog pthread aec)
