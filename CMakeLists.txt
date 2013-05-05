cmake_minimum_required(VERSION 2.6)
project(clang-tags)

include (CheckIncludeFile)
include (CheckFunctionExists)
include (CheckLibraryExists)

add_definitions (-Wall -std=c++11)
#add_definitions (-g -O1)
add_definitions (-O3)


# Check for libclang

check_include_file (clang-c/Index.h LIBCLANG_DEV)
if (NOT LIBCLANG_DEV)
  message (FATAL_ERROR "Unable to find libclang headers")
endif (NOT LIBCLANG_DEV)

set (CMAKE_REQUIRED_LIBRARIES -lclang)
check_function_exists (clang_disposeString LIBCLANG)
if (NOT LIBCLANG)
  message (FATAL_ERROR "Unable to find libclang")
endif (NOT LIBCLANG)

check_function_exists (clang_getExpansionLocation
  HAVE_CLANG_GETEXPANSIONLOCATION)


# Check for libsqlite3

check_include_file (sqlite3.h LIBSQLITE3_DEV)
if (NOT LIBSQLITE3_DEV)
  message (FATAL_ERROR "Unable to find libsqlite3 headers")
endif (NOT LIBSQLITE3_DEV)

set (CMAKE_REQUIRED_LIBRARIES -lsqlite3)
check_function_exists (sqlite3_open LIBSQLITE3)
if (NOT LIBSQLITE3)
  message (FATAL_ERROR "Unable to find libsqlite3")
endif (NOT LIBSQLITE3)



configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h")
include_directories ("${PROJECT_BINARY_DIR}")


add_library (jsoncpp
  dist/json-cpp/jsoncpp.cpp)
include_directories ("${PROJECT_SOURCE_DIR}/dist/json-cpp")


add_library (clang++
  libclang++/index.cxx
  libclang++/translationUnit.cxx
  libclang++/sourceLocation.cxx
  libclang++/cursor.cxx)


add_library (sqlite++
  sqlite++/database.cxx)


add_executable(druide
  main.cxx
  request.cxx
  compilationDatabase.cxx
  index.cxx
  findDefinition.cxx
  complete.cxx)
target_link_libraries(druide sqlite3 sqlite++ clang clang++ jsoncpp)


configure_file (
  "${PROJECT_SOURCE_DIR}/env.sh.in"
  "${PROJECT_BINARY_DIR}/env.sh")
configure_file (
  "${PROJECT_SOURCE_DIR}/env.el.in"
  "${PROJECT_BINARY_DIR}/env.el")

install (
  PROGRAMS     clang-tags
  DESTINATION  bin)



enable_testing()

set (ENV env PATH=${PROJECT_SOURCE_DIR}:${PATH})

add_test (
  NAME    clang-tags-index-runs
  COMMAND clang-tags-index -h)

add_test (
  NAME    clang-tags-def-runs
  COMMAND clang-tags-def -h)

add_test (
  NAME    clang-tags-runs
  COMMAND ${ENV} clang-tags -h)

add_test (
  NAME    clang-tags-scan
  COMMAND ${ENV} clang-tags scan ${PROJECT_SOURCE_DIR} -- -I.)

add_test (
  NAME    clang-tags-index
  COMMAND ${ENV} clang-tags index)

add_test (
  NAME    clang-tags-update
  COMMAND ${ENV} clang-tags update)

add_test (
  NAME    clang-tags-find-def
  COMMAND ${ENV} clang-tags find-def ${PROJECT_SOURCE_DIR}/findDefinition.cxx 376)
set_tests_properties (clang-tags-find-def PROPERTIES
  PASS_REGULAR_EXPRESSION c:@N@LibClang@C@Cursor@F@location\#1)

add_test (
  NAME    clang-tags-find-def-r
  COMMAND ${ENV} clang-tags find-def -r ${PROJECT_SOURCE_DIR}/findDefinition.cxx 376)
set_tests_properties (clang-tags-find-def PROPERTIES
  PASS_REGULAR_EXPRESSION c:@N@LibClang@C@Cursor@F@location\#1)

add_test (
  NAME    clang-tags-grep
  COMMAND ${ENV} clang-tags grep c:@N@LibClang@C@Cursor@F@location\#1)
set_tests_properties (clang-tags-grep PROPERTIES
  PASS_REGULAR_EXPRESSION findDefinition.cxx:59:)
