cmake_minimum_required(VERSION 2.6)
project(DepletionSolver)

include_directories (.)

add_definitions (-Wall)
add_definitions (-g -O1)
#add_definitions (-O3)

add_library (getopt
  getopt/getopt.cxx)

add_executable(clang-tags-def
  findDefinition.cxx)
target_link_libraries(clang-tags-def clang getopt)

add_executable(clang-tags-index
  index.cxx)
target_link_libraries(clang-tags-index clang getopt)