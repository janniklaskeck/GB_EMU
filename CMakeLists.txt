cmake_minimum_required(VERSION 3.16.1 FATAL_ERROR)

project(gbemu)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

###############################################################################
# Set build features
set(CMAKE_BUILD_TYPE Debug)

###############################################################################
include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

include(CMakePrintHelpers)

###############################################################################
# Check headers
set(INCLUDES "")
macro(ck_check_include_file header var)
  check_include_files("${INCLUDES};${header}" ${var})
  if(${var})
    set(INCLUDES ${INCLUDES} ${header})
  endif(${var})
endmacro(ck_check_include_file)

ck_check_include_file("stdlib.h" HAVE_STDLIB_H)

###############################################################################
# Check for integer types
# (The following are used in check.h. Regardless if they are used in
# the project, they will need to be checked in order to use Check).
check_type_size(intmax_t INTMAX_T)
check_type_size(uintmax_t UINTMAX_T)

check_type_size(pid_t PID_T)
if(NOT HAVE_PID_T)
  if(WIN32)
    set(pid_t "int")
  else(WIN32)
    MESSAGE(FATAL_ERROR "pid_t doesn't exist on this platform?")
  endif(WIN32)
endif(NOT HAVE_PID_T)

if(WIN32)
  set(SDL2_VERSION "2.30.0")
  set(SDL2-TTF_VERSION "2.22.0")
  set(SDL2-IMAGE_VERSION "2.8.2")

  set(SDL2_DIR "${PROJECT_SOURCE_DIR}/ext/SDL2-${SDL2_VERSION}")
  set(SDL2_INCLUDE_DIRS "${SDL2_DIR}/include;${SDL2_DIR}/include/SDL2")

  set(SDL2_LIBRARIES "${SDL2_DIR}/lib/x64/SDL2.lib;${SDL2_DIR}/lib/x64/SDL2main.lib")

  string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

  set(SDL2_TTF_DIR "${PROJECT_SOURCE_DIR}/ext/SDL2_ttf-${SDL2-TTF_VERSION}")
  set(SDL2_TTF_INCLUDE_DIRS "${SDL2_TTF_DIR}/include")

  set(SDL2_TTF_LIBRARIES "${SDL2_TTF_DIR}/lib/x64/SDL2_ttf.lib")

  string(STRIP "${SDL2_TTF_LIBRARIES}" SDL2_TTF_LIBRARIES)

  cmake_print_variables(SDL2_INCLUDE_DIRS)
else()

endif()

###############################################################################
# Generate "config.h" from "cmake/config.h.cmake"
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DHAVE_CONFIG_H)
set(CONFIG_HEADER ${CMAKE_CURRENT_BINARY_DIR}/config.h)


###############################################################################
# Subdirectories
add_subdirectory(lib)
add_subdirectory(gbemu)
add_subdirectory(tests)

###############################################################################
# Unit tests
enable_testing()
add_test(NAME check_gbe COMMAND check_gbe)

