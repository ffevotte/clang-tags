cmake_minimum_required(VERSION 2.6)
project(clang-tags)

include (CheckIncludeFile)
include (CheckFunctionExists)
include (CheckLibraryExists)

add_definitions (-Wall -std=c++11)
#add_definitions (-g -O1)
add_definitions (-O3)

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

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h")
include_directories ("${PROJECT_BINARY_DIR}")


add_library (getopt++
  getopt++/getopt.cxx)

add_library (libclang++
  libclang++/index.cxx
  libclang++/translationUnit.cxx
  libclang++/sourceLocation.cxx
  libclang++/cursor.cxx)

add_executable(clang-tags-def
  findDefinition.cxx)
target_link_libraries(clang-tags-def clang getopt++ libclang++)

add_executable(clang-tags-index
  index.cxx)
target_link_libraries(clang-tags-index clang getopt++ libclang++)


configure_file (
  "${PROJECT_SOURCE_DIR}/env.sh.in"
  "${PROJECT_BINARY_DIR}/env.sh")
configure_file (
  "${PROJECT_SOURCE_DIR}/env.el.in"
  "${PROJECT_BINARY_DIR}/env.el")

install (
  TARGETS      clang-tags-def clang-tags-index
  DESTINATION  bin
)

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
