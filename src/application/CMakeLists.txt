
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../")
include_directories($ENV{JRTPLIB_INC})
include_directories($ENV{EMIPLIB_INC})
MESSAGE(STATUS "*************************${CMAKE_CURRENT_BINARY_DIR}")
MESSAGE(STATUS "*************************$ENV{JTHREAD_INC}")
MESSAGE(STATUS "*************************$ENV{JRTPLIB_INC}")
#link_directories($ENV{JTHREAD_LIB})
#link_directories($ENV{JRTPLIB_LIB})
#link_directories($ENV{EMIPLIB_LIB})
foreach(IDX avsession feedbackexample multiplesoundfileplayer simplechain soundfileplayer soundrecorder pushtotalk
	    pushtotalk2 soundvolume audiosession)
	add_executable(${IDX} ${IDX}.cpp)
	target_link_libraries(${IDX} 
		emiplib.a
		jthread.a
		libjrtp.a
		)
endforeach(IDX)

