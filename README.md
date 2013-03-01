# clang-tags

`clang-tags` is a C / C++ source code indexing tool. Unlike many other indexing
tools, `clang-tags` relies on the [clang](http://clang.llvm.org) compiler (via
the libclang interface) to analyse and index the source code base.


## How to install


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


## How to use

### Creating the index

#### From a `cmake` project

`CMake` (since version 2.8.5) supports the generation of
[JSON compilation databases](http://clang.llvm.org/docs/JSONCompilationDatabase.html) listing all
necessary information to build the project:

```
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ../src
```

`clang-tags` can use this information to build the index:

```
clang-tags index json compile_commands.json
```

#### Scanning `*.cxx` files

For relatively simple projects, it can be sufficient to simply scan the top sources directory to
find all `*.c` or `*.cxx` files, and additionally provide `clang-tags` with a set of command-line
arguments necessary for `clang` to parse these files. For example:

```
clang-tags index scan ../src -- -I.
```


### Using the Emacs UI

First, load the package using `M-x load-file RET path/to/clang-tags.el RET`

You can then activate `clang-tags-mode` for the C and C++ source files in your project.

#### Find the definition of the symbol at point

While in a `clang-tags-mode` buffer, you can use `clang-tags` to find the location of the definition
of the symbol under point by pressing `M-.`.

The list of relevant definitions is presented in a buffer, where pressing `RET` will take you to the
location of the definition.

#### Find all uses of a definition in the source base

After having looked for a definition of the symbol under point, and while in the definitions list
buffer, press `M-,` to list all uses of the current definition in the source code base.

Results are presented in a `grep-mode` buffer.


## See also

- [libclang API documentation](http://clang.llvm.org/doxygen/group__CINDEX.html)
- [Doug Gregor's presentation on libclang](http://llvm.org/devmtg/2010-11/Gregor-libclang.pdf)
- [clang-ctags](https://github.com/drothlis/clang-ctags): a similar project
  using libclang to generate `ctags`/`etags` index files.


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
