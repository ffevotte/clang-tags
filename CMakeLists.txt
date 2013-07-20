cmake_minimum_required(VERSION 2.6)
project(clang-tags)

include (CheckFunctionExists)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_definitions (-Wall -std=c++11)
if ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
  add_definitions (-O3)
else ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
  add_definitions (-g -O1)
endif ("${CMAKE_BUILD_TYPE}" STREQUAL Release)


# Check for Boost (system + asio)
find_package (Boost
  REQUIRED
  COMPONENTS system
)
include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Boost_LIBRARIES})


# Check for jsoncpp
find_package (Libjsoncpp REQUIRED)
include_directories (${LIBJSONCPP_INCLUDE_DIRS})
link_directories (${LIBJSONCPP_LIBRARY_DIRS})
set(LIBS ${LIBS} ${LIBJSONCPP_LIBRARIES})


# Check for libclang
find_package (Libclang REQUIRED)
include_directories (${Libclang_INCLUDE_DIRS})
link_directories (${Libclang_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Libclang_LIBRARIES})

set (CMAKE_REQUIRED_LIBRARIES -lclang)
check_function_exists (clang_getExpansionLocation
  HAVE_CLANG_GETEXPANSIONLOCATION)


# Check for libsqlite3
find_package (Libsqlite3 REQUIRED)
include_directories (${Libsqlite3_INCLUDE_DIRS})
link_directories (${Libsqlite3_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Libsqlite3_LIBRARIES})


# Check for socat
find_package (Socat REQUIRED)



configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h")
include_directories ("${PROJECT_BINARY_DIR}")


add_library (clang++
  libclang++/index.cxx
  libclang++/translationUnit.cxx
  libclang++/sourceLocation.cxx
  libclang++/cursor.cxx)
set (LIBS ${LIBS} clang++)


add_library (sqlite++
  sqlite++/database.cxx)
set (LIBS ${LIBS} sqlite++)


add_library (getopt++
  getopt++/getopt.cxx)
set (LIBS ${LIBS} getopt++)


add_executable(clang-tags-server
  main.cxx
  request.cxx
  compilationDatabase.cxx
  index.cxx
  findDefinition.cxx
  grep.cxx
  complete.cxx)
target_link_libraries(clang-tags-server ${LIBS})


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
