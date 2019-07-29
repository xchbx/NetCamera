# Install script for directory: /home/parallels/work/NetCamera/deps/media_deps/emiplib/src

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/emiplib" TYPE FILE FILES
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipcompat.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipmessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipmediamessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/miperrorbase.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipfeedback.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipversion.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/miprawvideomessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipdebug.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipencodedaudiomessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipsystemmessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipencodedvideomessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/miptime.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipcomponent.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipcomponentchain.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipaudiomessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/miprtpmessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipvideomessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/mipcomponentalias.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/miprawaudiomessage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/core/miptypes_win.h"
    "/home/parallels/work/NetCamera/build/deps/media_deps/emiplib/src/core/mipconfig.h"
    "/home/parallels/work/NetCamera/build/deps/media_deps/emiplib/src/core/miptypes.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipalsainput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipjackinput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipdirectshowcapture.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipsndfileinput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipaudiorecorderinput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipv4l2input.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipfrequencygenerator.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipwavinput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipaudiofileinput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipwinmminput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipyuv420fileinput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/input/mipopenslesandroidinput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipalsaoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipsdlaudiooutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipjackoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipesdoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipopenaloutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipalsaoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipmessagedumper.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipqt5output.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipsndfileoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipvideoframestorage.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipaudiotrackoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipwavoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipwinmmoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mippulseoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipqt5audiooutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/output/mipopenslesandroidoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/timer/mipinterchaintimer.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/timer/mipaveragetimer.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/timer/mippusheventtimer.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/mixer/mipaudiomixer.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/mixer/mipmediabuffer.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/mixer/mipvideomixer.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/io/mipossinputoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/io/mippainputoutput.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtppacketdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpl16encoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpcomponent.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpulawencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpalawdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpgsmdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtplpcencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtph263encoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpspeexencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpl16decoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpulawdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtplpcdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpvideoencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtph263decoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpspeexdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpdummydecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpalawencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpgsmencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpvideodecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpsilkdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpsilkencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpopusencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpopusdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transmission/miprtpjpegdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipaudio3dbase.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/miphrirlisten.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipaudiosplitter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipspeexechocanceller.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipaudiodistancefade.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/miphrirbase.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipaudiofilter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipavcodecframeconverter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipsamplingrateconverter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipsampleencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/transform/mipyuv420framecutter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipspeexencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipalawencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipgsmencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipavcodecdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipulawdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipavcodecencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/miplpcdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipulawencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipalawdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipspeexdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipgsmdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/miplpcencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/miptinyjpegdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipsilkdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipsilkencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipopusencoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/codec/mipopusdecoder.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/util/mipoutputmessagequeue.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/util/mipoutputmessagequeuewithstate.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/util/mipoutputmessagequeuesimple.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/components/util/mipoutputmessagequeuewithstatesimple.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/sessions/mipaudiosession.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/sessions/mipvideosession.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/miprtpsynchronizer.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/mipstreambuffer.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/mipsignalwaiter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/mipwavwriter.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/miprtppacketgrouper.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/mipdirectorybrowser.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/mipresample.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/mipwavreader.h"
    "/home/parallels/work/NetCamera/deps/media_deps/emiplib/src/util/mipspeexutil.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libemip.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/home/parallels/work/NetCamera/build/lib/libemip.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libemip.so.1.2.1"
      "$ENV{DESTDIR}/usr/local/lib/libemip.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libemip.so.1.2.1;/usr/local/lib/libemip.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/parallels/work/NetCamera/build/lib/libemip.so.1.2.1"
    "/home/parallels/work/NetCamera/build/lib/libemip.so"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libemip.so.1.2.1"
      "$ENV{DESTDIR}/usr/local/lib/libemip.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

