cmake_minimum_required(VERSION 3.0)

# the name of the target operating system
#
INCLUDE(CMakeForceCompiler) 

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_VERSION   1)

set(CMAKE_CXX_COMPILER     	g++)
set(CMAKE_C_COMPILER     	gcc)
set(CMAKE_ASM_COMPILER     	as)
set(CMAKE_LINKER           	ld)
set(CMAKE_OBJCOPY     	   	objcopy)
set(CMAKE_OBJDUMP          	objdump)
set(CMAKE_SIZE             	size)
# project
project(serial C)

#enable_language(ASM-ATT)
set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
# Bring the headers into the project
include_directories(include)
# The file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "${PROJECT_PATH}/src/*.c")
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_PATH}/bin")
set(CMAKE_BINARY_DIR "${PROJECT_PATH}/bin")

# set(CMAKE_C_FLAGS "-Wall -Werror -std=c99 -O0" CACHE INTERNAL "c compiler flags")
set(CMAKE_C_FLAGS "-Wall -std=c11 -O0" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS "-Werror -Wall std=c++11 -O0 " CACHE INTERNAL "cxx compiler flags")
#set(CMAKE_ASM_FLAGS "")

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
add_executable(${PROJECT_NAME} ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} serialport readline cs50)

set(LIST_FILE ${PROJECT_SOURCE_DIR}/bin/${PROJECT_NAME}.list)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_OBJDUMP} -D $<TARGET_FILE:${PROJECT_NAME}> >${LIST_FILE}
COMMAND ${CMAKE_SIZE} -A -x -t $<TARGET_FILE:${PROJECT_NAME}>
COMMENT "Building ${LIST_FILE}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
