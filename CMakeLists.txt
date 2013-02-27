cmake_minimum_required(VERSION 2.6)
project(DepletionSolver)

include (CheckIncludeFile)
include (CheckFunctionExists)
include (CheckLibraryExists)

add_definitions (-Wall)
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
  getopt/getopt.cxx)

add_executable(clang-tags-def
  findDefinition.cxx)
target_link_libraries(clang-tags-def clang getopt++)

add_executable(clang-tags-index
  index.cxx)
target_link_libraries(clang-tags-index clang getopt++)


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
  DESTINATION  bin
)
