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
include_directories (${Libjsoncpp_INCLUDE_DIRS})
link_directories (${Libjsoncpp_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Libjsoncpp_LIBRARIES})


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


function (ct_add_test name)
  set (CMD ":")
  set (CMD "${CMD} \\\n&& export PATH=\${PATH}:${PROJECT_SOURCE_DIR}")
  set (CMD "${CMD} \\\n&& export PATH=\${PATH}:${PROJECT_SOURCE_DIR}/tests")
  set (CMD "${CMD} \\\n&& export PATH=\${PATH}:${PROJECT_BINARY_DIR}")
  set (CMD "${CMD} \\\n&& export CLANG_TAGS_TEST=1")
  set (CMD "${CMD} \\\n&& set -e")
  set (CMD "${CMD} \\\n&& set -o pipefail")
  set (CMD "${CMD} \\\n&& mkdir -p ${PROJECT_BINARY_DIR}/tests")
  set (CMD "${CMD} \\\n&& cd ${PROJECT_BINARY_DIR}/tests")
  set (CMD "${CMD} \\\n&& exec >${name}.out")
  foreach (command ${ARGN})
    set (CMD "${CMD} \\\n&& ${command}")
  endforeach (command)
  add_test (
    NAME ${name}
    COMMAND bash -c "${CMD}")
endfunction (ct_add_test)

enable_testing()

ct_add_test (ct-trace
  "rm -rf src build"
  "cp -R ${PROJECT_SOURCE_DIR}/tests/src ."
  "mkdir -p ${PROJECT_BINARY_DIR}/tests/build"
  "(cd ${PROJECT_BINARY_DIR}/tests/build; ln -s ../src/Makefile .)"

  "cd build"
  "ct-trace"
  "set -x"
  "test -r compile_commands.json"
  "grep main.cxx compile_commands.json"
)

ct_add_test (ct-index
  "cd build"
  "ct-index"
  "set -x"
  "test -r .ct.sqlite"
)
set_tests_properties (ct-index PROPERTIES DEPENDS ct-trace)

ct_add_test (ct-find-def-r
  "cd build"
  "ct-find-def-r | tee output"
  "set -x"
  "grep -q 'main.cxx:15-17' output"
  "grep -q 'c:@S@MyClass>#d@F@display#' output"
)
set_tests_properties (ct-find-def-r PROPERTIES DEPENDS ct-trace)

ct_add_test (ct-find-def-i
  "cd build"
  "ct-find-def-i | tee output"
  "set -x"
  "grep -q 'main.cxx:21-23' output"
  "grep -q 'c:@S@MyClass>#I@F@display#' output"
)
set_tests_properties (ct-find-def-r PROPERTIES DEPENDS ct-index)

ct_add_test (ct-grep
  "cd build"
  "ct-grep | tee output"
  "set -x"
  "grep -q 'main.cxx:33' output"
)
set_tests_properties (ct-grep PROPERTIES DEPENDS ct-index)
