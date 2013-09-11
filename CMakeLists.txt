cmake_minimum_required(VERSION 2.6)
project(clang-tags)

include (CheckFunctionExists)
include (CheckIncludeFile)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_definitions (-Wall -std=c++11)
if ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
  add_definitions (-O3)
else ("${CMAKE_BUILD_TYPE}" STREQUAL Release)
  add_definitions (-g -O1)
endif ("${CMAKE_BUILD_TYPE}" STREQUAL Release)

include_directories (${CMAKE_SOURCE_DIR})


# Check for Boost (system + asio)
find_package (Boost
  REQUIRED
  COMPONENTS system thread
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

set (CMAKE_REQUIRED_LIBRARIES ${Libclang_LIBRARIES})
check_function_exists (clang_getExpansionLocation
  HAVE_CLANG_GETEXPANSIONLOCATION)


# Check for libsqlite3
find_package (Libsqlite3 REQUIRED)
include_directories (${Libsqlite3_INCLUDE_DIRS})
link_directories (${Libsqlite3_LIBRARY_DIRS})
set(LIBS ${LIBS} ${Libsqlite3_LIBRARIES})


# Check for socat
find_package (Socat REQUIRED)


# Check for strace
find_package (Strace REQUIRED)


# Check for python
find_package (PythonInterp REQUIRED)


# Check for inotify
check_include_file (sys/inotify.h HAVE_INOTIFY)
if (HAVE_INOTIFY)
  list (APPEND CLANG_TAGS_SERVER_SOURCES
    clangTags/watch/inotify.cxx
    clangTags/watch/inotifyMap.cxx)
endif (HAVE_INOTIFY)


# Manage a stack of source directories
macro (ct_push_dir DIR)
  list (INSERT CT_DIR_STACK 0 ${DIR})
  list (GET CT_DIR_STACK 0 CT_DIR)
  message (STATUS "Scanning directory ${CT_DIR}")
endmacro (ct_push_dir)
set (CT_DIR ".")
set (CT_DIR_STACK ${CT_DIR})

macro (ct_pop_dir)
  list (REMOVE_AT CT_DIR_STACK 0)
  list (GET CT_DIR_STACK 0 CT_DIR)
endmacro (ct_pop_dir)


configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h")
include_directories ("${PROJECT_BINARY_DIR}")


include ("sqlite++/CMakeLists.txt")
include ("libclang++/CMakeLists.txt")
include ("getopt++/CMakeLists.txt")
include ("request/CMakeLists.txt")
include ("util/CMakeLists.txt")

list (APPEND CLANG_TAGS_SERVER_SOURCES
  main.cxx
  MT/stream.cxx
  clangTags/storage/sqliteDB.cxx
  clangTags/update/thread.cxx
  clangTags/update/index.cxx
  clangTags/load.cxx
  clangTags/findDefinition.cxx
  clangTags/complete.cxx
  clangTags/grep.cxx
  clangTags/config.cxx)
add_executable (clang-tags-server ${CLANG_TAGS_SERVER_SOURCES})
target_link_libraries (clang-tags-server ${LIBS})


function (ct_template path)
  configure_file (
    "${PROJECT_SOURCE_DIR}/${path}"
    "${PROJECT_BINARY_DIR}/${path}")
endfunction (ct_template)
ct_template ("env.sh")
ct_template ("env.el")


# Fake compilers
function (ct_fake_compiler name)
  configure_file (
    "${PROJECT_SOURCE_DIR}/fake-compiler.sh"
    "${PROJECT_BINARY_DIR}/fake-compiler/${name}")
endfunction (ct_fake_compiler)
ct_fake_compiler ("gcc")
ct_fake_compiler ("g++")
ct_fake_compiler ("clang")
ct_fake_compiler ("clang++")


# Generate documentation
ct_template ("Doxyfile")
ct_template ("doc/export-org")
ct_template ("doc/generate.el")
ct_template ("doc/setup.org")
ct_template ("doc/index.org")
ct_template ("doc/quickStart.org")
ct_template ("doc/install.org")
add_custom_target (doc-doxygen COMMAND doxygen)
add_custom_target (doc-org     COMMAND ./doc/export-org)
add_custom_target (doc DEPENDS doc-doxygen doc-org)
  


install (
  PROGRAMS     clang-tags
  DESTINATION  bin)
install (
  TARGETS      clang-tags-server
  DESTINATION  bin)
install (
  FILES        clang-tags.el
  DESTINATION  share/emacs/site-lisp/)
install (
  FILES        arg2opt.py
  DESTINATION  share/pyshared/)

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

ct_add_test (ct-help
  "ct-help"
)

ct_add_test (setup
  "rm -rf src build"
  "cp -R ${PROJECT_SOURCE_DIR}/tests/src ."
  "mkdir -p ${PROJECT_BINARY_DIR}/tests/build"
  "(cd ${PROJECT_BINARY_DIR}/tests/build; ln -s ../src/Makefile .)"
)

ct_add_test (ct-trace
  "cd build"
  "unset MAKEFLAGS"
  "unset MAKELEVEL"
  "ct-trace"
  "set -x"
  "test -r compile_commands.json"
  "grep -q 'main.cxx' compile_commands.json"
)
set_tests_properties (ct-trace PROPERTIES DEPENDS setup)

ct_add_test (ct-load
  "cd build"
  "ct-load"
  "set -x"
  "test -r .ct.sqlite"
)
set_tests_properties (ct-load PROPERTIES DEPENDS ct-trace)

ct_add_test (ct-index
  "cd build"
  "ct-index"
  "set -x"
  "test -r .ct.sqlite"
)
set_tests_properties (ct-index PROPERTIES DEPENDS ct-load)

ct_add_test (ct-find-def-r
  "cd build"
  "ct-find-def-r | tee output"
  "set -x"
  "grep -q 'main.cxx:15-17' output"
  "grep -q 'c:@S@MyClass>#d@F@display#' output"
)
set_tests_properties (ct-find-def-r PROPERTIES DEPENDS ct-load)

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
