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

The [quick start guide](http://ffevotte.github.com/clang-tags/quickStart.html)
is a good place to get a quick look at `clang-tags` various features.

Please see the [user manual](http://ffevotte.github.com/clang-tags) for a
complete description of `clang-tags` features and a usage documentation.


## Installing

The [installation guide](http://ffevotte.github.com/clang-tags/install.html)
should provide all information to build and install `clang-tags`.


## Getting started

The [quick start guide](http://ffevotte.github.com/clang-tags/quickStart.html)
should get you started in a few minutes. It is also a good starting point to get
a quick overview of the features `clang-tags` provides.

To go a step further, all details are in the
[user manual](http://ffevotte.github.com/clang-tags).



## See also

### Similar tools

- [Bear][]: a tool to generate compilation databases.
- [clang-ctags][]: a similar project
  using `libclang` to generate `ctags`/`etags` index files.
- [rtags][]: another C/C++ code indexer based on `libclang`. `rtags` is
  much more advanced and featureful than `clang-tags`, its only (minor) drawback
  being its long list of dependencies.
- clangd ([proposal][clangd_proposal] / [design document][clangd_design]):
  a Clang-based IDE-like service (still in design phase AFAIK).

[Bear]: https://github.com/rizsotto/Bear
[clang-ctags]: https://github.com/drothlis/clang-ctags
[rtags]: https://github.com/Andersbakken/rtags
[clangd_proposal]: http://lists.cs.uiuc.edu/pipermail/cfe-dev/2012-June/022028.html
[clangd_design]: https://github.com/chandlerc/llvm-designs/blob/master/ClangService.rst

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


A doxygen-generated documentation targeted at developers is available
[here](http://ffevotte.github.com/clang-tags/doxygen). Please do not hesitate to
consult it, should you need help implementing in `clang-tags` the killer feature
you need.

### Contributors

- [Cameron White](https://github.com/cameronwhite) largely improved SQLite
  performances


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
