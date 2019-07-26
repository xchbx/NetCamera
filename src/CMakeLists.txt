SET(NetCamera_SOURCES 
	main.cc
	)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/deps/elog)

ADD_EXECUTABLE(NetCamera ${NetCamera_SOURCES})
TARGET_LINK_LIBRARIES(NetCamera elog pthread)
