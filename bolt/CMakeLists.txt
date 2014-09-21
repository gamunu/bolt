# Main CMakeList.txt
# Minimum CMake version 2.8
# 
# category 			CMakeList Files
# package 			MAIN_DIR/Bolt
# author			Gamunu Bandara
# copyright 		All rights reserved
# version 1.1.0 	{Version Major.Version Minor.Version Patch}
# Last Updated 		29.6.2014 12:02

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

project(boltauth)

# set environment 
#-------------------
find_library(MYSQLCPPCONN_LIBRARY
    NAMES mysqlcppconn
    HINTS "$ENV{MYSQLLIBDIR}"
)

find_library(MYSQL_LIBS
	NAMES libmysql
	HINTS "$ENV{MYSQL_LIBS}"
	)

find_library(CASABLANCA_LIBRARY
    NAMES casablanca
    HINTS "$ENV{BOLTLIBROOT}"
)

find_library(BOLT_MYSQL
    NAMES boltmysql
    HINTS "$ENV{BOLTLIBROOT}"
)

find_library(BOLT_AZURE
    NAMES boltazure
    HINTS "$ENV{BOLTLIBROOT}"
)

find_library(DRIVER_AZURE
    NAMES driverazure
    HINTS "$ENV{BOLTLIBROOT}"
)

#-----------------------------
# Boost libraries.
#----------------------------

set(Boost_INCLUDE_DIR $ENV{BOOSTROOT})
set(Boost_LIBRARY_DIR $ENV{BOOST_LIBRARYDIR})

FIND_PACKAGE(Boost COMPONENTS REQUIRED)

# Link Directories
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} $ENV{MYSQLOPT})
# Additonal Include directories
INCLUDE_DIRECTORIES ( auth/include  $ENV{MYSQLINCLUDEDIR}  ${Boost_INCLUDE_DIRS} ../casablanca/include  ../global/includes)

# Source files list
SET(SOURCES	
	auth/src/connection.cpp
	auth/src/user.cpp
	auth/src/authenticate.cpp
	auth/src/signature.cpp
	auth/src/permissions.cpp
	../global/src/configuration.cpp
)

# build library
ADD_LIBRARY (boltauth SHARED ${SOURCES})
set_target_properties(boltauth PROPERTIES COMPILE_DEFINITIONS "BOLTAUTH_DLL;_CRT_SECURE_NO_WARNINGS")
TARGET_LINK_LIBRARIES(boltauth ${MYSQLCPPCONN_LIBRARY} ${MYSQL_LIBS} ${CASABLANCA_LIBRARY})

# about this project
# ------------------
project(boltserver)

# Additonal Include directories
#---------------------------------------
INCLUDE_DIRECTORIES( server/include ../casablanca/include auth/include ../storage/mysql/include ../storage/azure/include ../storage/driver.azure/include ../global/includes ${Boost_INCLUDE_DIRS})

# Source files list
SET(SOURCES	
	server/src/main.cpp
	server/src/dispatch.cpp
	server/src/mhttppost.cpp
	server/src/mhttpdelete.cpp
	server/src/ahttppost.cpp
	server/src/ahttpdelete.cpp
	server/src/mysql_handler.cpp
	server/src/azure_handler.cpp
	server/src/whole_handler.cpp
	server/src/url_utils.cpp
	server/src/header_utils.cpp
	server/src/winservice_base.cpp
	server/src/winservice_installer.cpp
	server/src/winbservice.cpp
	server/src/metadata.cpp
	../global/src/configuration.cpp
)

#create executable
ADD_EXECUTABLE(boltserver ${SOURCES})
set_target_properties(boltserver PROPERTIES COMPILE_DEFINITIONS "_UNICODE;UNICODE;_CRT_SECURE_NO_WARNINGS")
ADD_DEPENDENCIES (boltserver boltauth)
TARGET_LINK_LIBRARIES(boltserver boltauth ${CASABLANCA_LIBRARY} ${BOLT_MYSQL} ${BOLT_AZURE} ${DRIVER_AZURE})