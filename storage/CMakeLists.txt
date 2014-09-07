# Main CMakeList.txt
# Minimum CMake version 2.8
# 
# category 			CMakeList Files
# package 			MAIN_DIR/Bolt.Storage.Driver.Azure
# author			Gamunu Bandara
# copyright 		All rights reserved
# version 1.0.0 	{Version Major.Version Minor.Version Patch}
# Last Updated 		27.2.2014 11:39
# TODO: Find a way to add precompiled headers


CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)

# about this project
# ------------------
PROJECT(driverazure)

# Fix: Virtual memory range for PCH exceeded
#---------------------------------------------
IF(MSVC)
STRING(REGEX REPLACE "/Zm[0-9]+ *" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm500" CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" CACHE STRING "" FORCE)
ENDIF()

# Additonal include directories
#-------------------------------
INCLUDE_DIRECTORIES( driver.azure/include ../casablanca/include)

FIND_LIBRARY(CASABLANCA_LIBRARY
    NAMES casablanca
    HINTS "$ENV{BOLTLIBROOT}"
)

# Source files list
#-------------------
SET(SOURCES	
	driver.azure/src/async_semaphore.cpp
	driver.azure/src/authentication.cpp
	driver.azure/src/basic_types.cpp
	driver.azure/src/blob_request_factory.cpp
	driver.azure/src/blob_response_parsers.cpp
	driver.azure/src/cloud_blob.cpp
	driver.azure/src/cloud_blob_client.cpp
	driver.azure/src/cloud_blob_container.cpp
	driver.azure/src/cloud_blob_directory.cpp
	driver.azure/src/cloud_blob_istreambuf.cpp
	driver.azure/src/cloud_blob_ostreambuf.cpp
	driver.azure/src/cloud_blob_shared.cpp
	driver.azure/src/cloud_block_blob.cpp
	driver.azure/src/cloud_client.cpp
	driver.azure/src/cloud_core.cpp
	driver.azure/src/cloud_page_blob.cpp
	driver.azure/src/cloud_queue.cpp
	driver.azure/src/cloud_queue_client.cpp
	driver.azure/src/cloud_queue_message.cpp
	driver.azure/src/cloud_storage_account.cpp
	driver.azure/src/cloud_table.cpp
	driver.azure/src/cloud_table_client.cpp
	driver.azure/src/entity_property.cpp
	driver.azure/src/hash_windows.cpp
	driver.azure/src/logging_windows.cpp
	driver.azure/src/mime_multipart_helper.cpp
	driver.azure/src/navigation.cpp
	driver.azure/src/operation_context.cpp
	driver.azure/src/protocol_json.cpp
	driver.azure/src/protocol_xml.cpp
	driver.azure/src/queue_request_factory.cpp
	driver.azure/src/request_factory.cpp
	driver.azure/src/request_result.cpp
	driver.azure/src/response_parsers.cpp
	driver.azure/src/retry_policies.cpp
	driver.azure/src/shared_access_signature.cpp
	driver.azure/src/table_query.cpp
	driver.azure/src/table_request_factory.cpp
	driver.azure/src/table_response_parsers.cpp
	driver.azure/src/util.cpp
	driver.azure/src/util_windows.cpp
	driver.azure/src/xmlhelpers.cpp
)

# create library
ADD_LIBRARY(driverazure SHARED ${SOURCES})
SET_TARGET_PROPERTIES(driverazure PROPERTIES
     COMPILE_DEFINITIONS "DRIVERAZURE_DLL;_USRDLL;_UNICODE;UNICODE")
# Add Linker targets
TARGET_LINK_LIBRARIES(driverazure rpcrt4.lib xmllite.lib bcrypt.lib ${CASABLANCA_LIBRARY})

# about this project
# ------------------
PROJECT(boltazure)

# Additonal Include directories
#---------------------------------------
INCLUDE_DIRECTORIES( azure/include driver.azure/include ../casablanca/include ../global/includes)

# Source files list
SET(SOURCES	
	azure/src/azure_connection.cpp
	azure/src/azure_table.cpp
	azure/src/azure_query.cpp
	azure/src/azure_entity.cpp
)


#create shared library
ADD_LIBRARY(boltazure SHARED ${SOURCES})
SET_TARGET_PROPERTIES(boltazure PROPERTIES COMPILE_DEFINITIONS BOLTAZURE_DLL)
#ADD_DEPENDENCIES (boltazure driverazure)
TARGET_LINK_LIBRARIES(boltazure driverazure ${CASABLANCA_LIBRARY})


PROJECT(boltmysql)
FIND_PACKAGE(Boost REQUIRED)

# set environment 
#-------------------
FIND_LIBRARY(MYSQLCPPCONN_LIBRARY
    NAMES mysqlcppconn
    HINTS "$ENV{MYSQLLIBDIR}"
)

FIND_LIBRARY(MYSQL_LIBS
	NAMES libmysql
	HINTS "$ENV{MYSQL_LIBS}"
	)


#-----------------------------
# Boost libraries.
#----------------------------

set(Boost_INCLUDE_DIR $ENV{BOOSTROOT})
set(Boost_LIBRARY_DIR $ENV{BOOST_LIBRARYDIR})

FIND_PACKAGE(Boost COMPONENTS REQUIRED)

# Link Directories
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS} $ENV{MYSQLOPT})

INCLUDE_DIRECTORIES ( mysql/include $ENV{MYSQLINCLUDEDIR} ${Boost_INCLUDE_DIRS}  ../casablanca/include ../global/includes)

# Source files list
SET(SOURCES	
	mysql/src/mysql_connection.cpp
	mysql/src/mysql_table.cpp
	mysql/src/mysql_query.cpp
	mysql/src/mysql_delete.cpp
	mysql/src/mysql_entity.cpp
	mysql/src/mysql_property.cpp
	mysql/src/mysql_database.cpp
)


# build library
ADD_LIBRARY (boltmysql SHARED ${SOURCES})
SET_TARGET_PROPERTIES(boltmysql PROPERTIES COMPILE_DEFINITIONS "BOLTMYSQL_DLL;_CRT_SECURE_NO_WARNINGS;")
TARGET_LINK_LIBRARIES(boltmysql ${CASABLANCA_LIBRARY} ${MYSQL_LIBS} ${MYSQLCPPCONN_LIBRARY})