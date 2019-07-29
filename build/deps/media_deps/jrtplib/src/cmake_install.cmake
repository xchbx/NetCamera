# Install script for directory: /home/parallels/work/NetCamera/deps/media_deps/jrtplib/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/jrtplib3" TYPE FILE FILES
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpapppacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpbyepacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpcompoundpacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpcompoundpacketbuilder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcppacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcppacketbuilder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcprrpacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpscheduler.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpsdesinfo.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpsdespacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpsrpacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtcpunknownpacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpaddress.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpcollisionlist.h"
    "/home/parallels/work/NetCamera/build/deps/media_deps/jrtplib/src/rtpconfig.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpdebug.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpdefines.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtperrors.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtphashtable.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpinternalsourcedata.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpipv4address.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpipv4destination.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpipv6address.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpipv6destination.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpkeyhashtable.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtplibraryversion.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpmemorymanager.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpmemoryobject.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtppacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtppacketbuilder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtppollthread.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtprandom.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtprandomrand48.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtprandomrands.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtprandomurandom.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtprawpacket.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpsession.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpsessionparams.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpsessionsources.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpsourcedata.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpsources.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpstructs.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtptimeutilities.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtptransmitter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtptypes_win.h"
    "/home/parallels/work/NetCamera/build/deps/media_deps/jrtplib/src/rtptypes.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpudpv4transmitter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpudpv6transmitter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpbyteaddress.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpexternaltransmitter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpsecuresession.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpsocketutil.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpabortdescriptors.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtpselect.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtptcpaddress.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/rtptcptransmitter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/jrtplib/src/extratransmitters/rtpfaketransmitter.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libjrtp.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/home/parallels/work/NetCamera/build/lib/libjrtp.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libjrtp.so.3.11.1"
      "$ENV{DESTDIR}/usr/local/lib/libjrtp.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libjrtp.so.3.11.1;/usr/local/lib/libjrtp.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/parallels/work/NetCamera/build/lib/libjrtp.so.3.11.1"
    "/home/parallels/work/NetCamera/build/lib/libjrtp.so"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libjrtp.so.3.11.1"
      "$ENV{DESTDIR}/usr/local/lib/libjrtp.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

