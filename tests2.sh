#!/bin/bash

set -x
export CLANG_TAGS_TEST=1

rm -f .ct.sqlite

cd A
make clean
clang-tags trace make
cd ..

clang-tags load A/compile_commands.json
clang-tags index


cd B
make clean
clang-tags trace make
cd ..

clang-tags load B/compile_commands.json
clang-tags update

clang-tags find-def B/main.cxx 50
clang-tags grep 'c:@C@A@F@display#1'
