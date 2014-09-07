# Main CMakeList.txt
# Minimum CMake version 2.6
# 
# category 			CMakeList Files
# package 			MAIN_DIR/Casablanca
# author			Gamunu Bandara
# copyright 		All rights reserved
# version 1.0.0 	{Version Major.Version Minor.Version Patch}
# Last Updated 		23.2.2014 02:20
# TODO: Find a way to add precompiled headers


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

# about this project
# ------------------
project(casablanca)
SET(VERSION_MAJOR "1")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "0")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Include additonal linclude directories
INCLUDE_DIRECTORIES( include src/pch )

# Source files list
set(SOURCES
    src/json/json.cpp
    src/json/json_parsing.cpp
    src/json/json_serialization.cpp
    src/utilities/asyncrt_utils.cpp
    src/uri/uri.cpp
    src/uri/uri_builder.cpp
    src/uri/uri_parser.cpp
    src/http/common/http_msg.cpp
    src/http/common/http_helpers.cpp
    src/http/client/http_msg_client.cpp
    src/http/listener/http_listener.cpp
    src/http/listener/http_msg_listen.cpp
    src/http/listener/http_server_api.cpp
    src/utilities/base64.cpp
    src/http/client/http_win7.cpp
    src/http/listener/http_windows_server.cpp
    src/streams/windows/fileio.cpp
    src/streams/windows/ioscheduler.cpp
    src/utilities/dllmain.cpp
    src/json/json.cpp
    src/json/json_parsing.cpp
    src/json/json_serialization.cpp
    #src/pplx/pplx.cpp
    #src/pplx/windows/pplxwin.cpp
    )

# Add definitions
ADD_DEFINITIONS (
    -D_ASYNCRT_EXPORT
    -D_PPLX_EXPORT
    -D_MBCS
    -D_USRDLL
    )

set(EXTRALINKS Httpapi.lib Winhttp.dll)

# build library

ADD_LIBRARY(casablanca SHARED ${SOURCES})


target_link_libraries(casablanca
  ${CMAKE_THREAD_LIBS_INIT}
  ${EXTRALINKS}
  )