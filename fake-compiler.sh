#!/bin/bash
COMPILER=$(basename $0)
COMPILER_PATH=$(which -a $COMPILER \
    | tail -n -1 \
    | head -n  1 )

# Add the command-line to the compilation database
clang-tags add -- $COMPILER "$@"

# Forward the command-line to the real compiler
$COMPILER_PATH "$@"
