# clang-tags

`clang-tags` is a C / C++ source code indexing tool. Unlike many other indexing
tools, `clang-tags` relies on the [clang](http://clang.llvm.org) compiler (via
the libclang interface) to analyse and index the source code base.

Its main features are:
- generating
  [compilation databases](http://clang.llvm.org/docs/JSONCompilationDatabase.html)
  in a build-system-agnostic way
- indexing a C/C++ project source files
- finding the definition location of an identifier in a source file
- finding all uses of a definition in the project sources

Please see the [user manual](http://ffevotte.github.com/clang-tags) for a
complete description of `clang-tags`' features and a usage documentation.

## Installing

### Requirements

- `libclang` (>= 3.0)
- `python` (>= 2.3)
  - a version newer than 2.7 is recommended to benefit from the more recent `argparse` module.


### Build

`clang-tags` uses [`CMake`](http://www.cmake.org/) as a build system.

A complete build process could for example look like this:

```
git clone https://github.com/ffevotte/clang-tags.git src
mkdir build && cd build
cmake ../src
```


## Getting started

All details are in the [user manual](http://ffevotte.github.com/clang-tags), but
here are a few steps which should get you started. If anything goes wrong,
please consult the documentation.

1. build the compilation database:

   ```
   ~> cd path/to/your/build/directory
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
   offset 902:

   ```
   build> clang-tags find-def ../src/index.cxx 902
   -- cursor.location().expansionLocation() -- CallExpr expansionLocation
      src/clang/sourceLocation.hxx:31-51:20-5: CXXMethod expansionLocation
      USR: c:@N@Clang@C@SourceLocation@F@expansionLocation#1

   -- cursor.location() -- CallExpr location
      src/clang/cursor.hxx:63-65:20-5: CXXMethod location
      USR: c:@N@Clang@C@Cursor@F@location#1

   -- cursor -- DeclRefExpr cursor
      src/index.cxx:21-21:23-40: VarDecl cursor
      USR: c:index.cxx@557@F@indexFile#$@SA@CXCursor#S0_#*v#@cursor
   ```

4. find all uses of the `Cursor::location()` method (identified by its USR, as
   given in the second result of `clang-tags find-def` above):

   ```
   build> clang-tags grep 'c:@N@Clang@C@Cursor@F@location#1'
   src/findDefinition.cxx:13:  const Clang::SourceLocation location(cursor.location());
   src/findDefinition.cxx:33:    const Clang::SourceLocation::Position begin = cursorDef.location().expansionLocation();
   src/findDefinition.cxx:52:  const Clang::SourceLocation location (cursor.location());
   src/findDefinition.cxx:131:    Clang::SourceLocation target = cursor.location();
   src/clang/cursor.hxx:63:    SourceLocation location () const {
   src/index.cxx:34:  const Clang::SourceLocation::Position begin = cursor.location().expansionLocation();
   ```


## See also

- [Bear](https://github.com/rizsotto/Bear): a tool to generate compilation databases.
- [clang-ctags](https://github.com/drothlis/clang-ctags): a similar project
  using libclang to generate `ctags`/`etags` index files.
- clangd
  ([proposal](http://lists.cs.uiuc.edu/pipermail/cfe-dev/2012-June/022028.html)
  /
  [design document](https://github.com/chandlerc/llvm-designs/blob/master/ClangService.rst)):
  a Clang-based IDE-like service (still in design phase AFAIK).


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
