# clang-tags

`clang-tags` is a C / C++ source code indexing tool. Unlike many other indexing
tools, `clang-tags` relies on the [clang](http://clang.llvm.org) compiler (via
the libclang interface) to analyse and index the source code base.

Its main features are:
- generating
  [compilation databases](http://clang.llvm.org/docs/JSONCompilationDatabase.html)
  in a build-system-agnostic way,
- indexing a C/C++ project source files,
- finding the definition location of an identifier in a source file,
- finding all uses of a definition in the project sources.

Please see the [user manual](http://ffevotte.github.com/clang-tags) for a
complete description of `clang-tags`' features and a usage documentation.

## Installing

### Requirements

- `libclang` (>= 3.0)
- `python` (>= 2.3)
  - a version newer than 2.7 is recommended to benefit from the more recent `argparse` module.

### Building

`clang-tags` uses [`CMake`](http://www.cmake.org/) as a build system.

A complete build and test process could for example look like this:

```
git clone https://github.com/ffevotte/clang-tags.git src
mkdir build && cd build
cmake ../src
make
make test
```

### Testing and using without installing

It is possible to use `clang-tags` immediately after the build, without properly installing it on
the system. Two environment setup scripts are generated during the build to help in this respect:

- **setup the shell environment**: `env.sh` appropriately sets the required environment variables,
  such as `PATH`:

    ```
    source env.sh
    ```

- **setup script for Emacs**: `env.el` sets the required environment variables and loads the
  `clang-tags.el` library. You can either:
  - load it interactively: `M-x load-file RET /path/to/build/env.el RET`
  - or put the following in your Emacs initialization file (`~/.emacs.d/init.el` or `~/.emacs`):

        ```lisp
        (load-file "/path/to/build/env.el")
        ```

### Installing

Running `make install` in the build directory should install `clang-tags`, but this has not been
tested much yet. For now, it is safer to follow the instructions in the previous section.



## Getting started

All details are in the [user manual](http://ffevotte.github.com/clang-tags), but
here are a few steps which should get you started. If anything goes wrong,
please consult the documentation.

0. source the environment:

    ```
    ~> source path/to/build/env.sh
    ~> echo $PATH
    /usr/local/bin:/usr/bin:/bin:path/to/clang-tags/src:path/to/clang-tags/build
    ```

1. build the compilation database (in this example, we'll index the sources of `clang-tags` itself):

    ```
    ~> cd path/to/clang-tags/build
    build> make clean
    build> clang-tags trace make
    [ 33%] Building CXX object CMakeFiles/getopt++.dir/getopt/getopt.cxx.o
    Linking CXX static library libgetopt++.a
    [ 33%] Built target getopt++
    [ 66%] Building CXX object CMakeFiles/clang-tags-def.dir/findDefinition.cxx.o
    Linking CXX executable clang-tags-def
    [ 66%] Built target clang-tags-def
    [100%] Building CXX object CMakeFiles/clang-tags-index.dir/index.cxx.o
    Linking CXX executable clang-tags-index
    [100%] Built target clang-tags-index
    ```

2. index the source files:

    ```
    build> clang-tags index --emacs-conf ../src/
    src/getopt/getopt.cxx...          1.27s.
    src/findDefinition.cxx...         0.34s.
    src/index.cxx...                  0.33s.
    ```

3. find the definition location of the identifier located in `index.cxx` at
   offset 750:

    ```
    build> clang-tags find-def ../src/index.cxx 902
    -- cursor -- DeclRefExpr cursor
       /local00/home/H55056/local/perso/projets/git/clang-tags/src/index.cxx:17-17:23-40: VarDecl cursor
       USR: c:index.cxx@405@F@indexFile#$@SA@CXCursor#S0_#*v#@cursor

    -- cursor.location() -- CallExpr location
       /local00/home/H55056/local/perso/projets/git/clang-tags/src/clang/cursor.hxx:67-69:20-5: CXXMethod location
       USR: c:@N@Clang@C@Cursor@F@location#1

    -- cursor.location().expansionLocation() -- CallExpr expansionLocation
       /local00/home/H55056/local/perso/projets/git/clang-tags/src/clang/sourceLocation.hxx:31-51:20-5: CXXMethod expansionLocation
       USR: c:@N@Clang@C@SourceLocation@F@expansionLocation#1
    ```

4. find all uses of the `Cursor::location()` method (identified by its USR, as
   given in the second result of `clang-tags find-def` above):

    ```
    build> clang-tags grep 'c:@N@Clang@C@Cursor@F@location#1' | sort -u
    clang/cursor.hxx:67:    SourceLocation location () const {
    findDefinition.cxx:12:  const Clang::SourceLocation location (cursor.location());
    findDefinition.cxx:135:    Clang::SourceLocation target = cursor.location();
    findDefinition.cxx:32:    const Clang::SourceLocation::Position begin = cursorDef.location().expansionLocation();
    findDefinition.cxx:51:  const Clang::SourceLocation location (cursor.location());
    index.cxx:30:  const Clang::SourceLocation::Position begin = cursor.location().expansionLocation();
    ```


## See also

### Similar tools

- [Bear](https://github.com/rizsotto/Bear): a tool to generate compilation databases.
- [clang-ctags](https://github.com/drothlis/clang-ctags): a similar project
  using libclang to generate `ctags`/`etags` index files.
- clangd ([proposal][clangd_proposal] / [design document][clangd_design]):
  a Clang-based IDE-like service (still in design phase AFAIK).

[clangd_proposal]: http://lists.cs.uiuc.edu/pipermail/cfe-dev/2012-June/022028.html
[clangd_design]:   https://github.com/chandlerc/llvm-designs/blob/master/ClangService.rst

### Clang documentation

- [libclang API documentation][libclang]
- Eli Bendersky's [blog post on C++ parsing][bendersky] using libclang's Python bindings
- Doug Gregor's [presentation on libclang][gregor]
- Manuel Klimek's [presentation on clang-based IDE-like tools][klimek]

[libclang]:  http://clang.llvm.org/doxygen/group__CINDEX.html
[bendersky]: http://eli.thegreenplace.net/2011/07/03/parsing-c-in-python-with-clang/
[gregor]:    http://llvm.org/devmtg/2010-11/Gregor-libclang.pdf
[klimek]:    http://llvm.org/devmtg/2012-04-12/Slides/Manuel_Klimek.pdf


## Contributing

If you make improvements to this code or have suggestions, please do not
hesitate to fork the repository or submit bug reports on
[github](https://github.com/ffevotte/clang-tags). The repository's URL is:

    https://github.com/ffevotte/clang-tags.el.git


## License

Copyright (C) 2013 François Févotte.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.



The JsonCpp library is bundled with clang-tags' sources.
It is copyright (c) 2007-2010 Baptiste Lepilleur, and is released under the
terms of the MIT licence, see `dist/json-cpp/LICENSE`.
