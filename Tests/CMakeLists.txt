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


# about this project
# ------------------
project(testboltmysql)

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
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})


# Additonal Include directories
#---------------------------------------
INCLUDE_DIRECTORIES( ../bolt/server/include ../casablanca/include auth/include ../storage/mysql/include ../storage/azure/include ../storage/driver.azure/include ../global/includes ${Boost_INCLUDE_DIRS})

# Source files list
SET(SOURCES	
	src/boltmysql/boltmysql.cpp
    #src/server/mysql_handler.cpp


    #Metadata class
    .#./bolt/server/src/metadata.cpp
	../global/src/configuration.cpp
)

#create executable
ADD_EXECUTABLE(testboltmysql ${SOURCES})
set_target_properties(testboltmysql PROPERTIES COMPILE_DEFINITIONS "_UNICODE;UNICODE;_CRT_SECURE_NO_WARNINGS")
TARGET_LINK_LIBRARIES(testboltmysql
                      ${Boost_LIBRARIES}
                      ${CASABLANCA_LIBRARY} ${BOLT_MYSQL} ${BOLT_AZURE} ${DRIVER_AZURE})