#!/bin/bash

set -x
export CLANG_TAGS_TEST=1

cd A
make clean
clang-tags trace make
clang-tags load
clang-tags index
cd ..


cd B
make clean
clang-tags trace make
clang-tags load
clang-tags index
cd ..

cd B
clang-tags find-def main.cxx 50
clang-tags grep 'c:@C@A@F@display#1'
cd ../A
clang-tags grep 'c:@C@A@F@display#1'
