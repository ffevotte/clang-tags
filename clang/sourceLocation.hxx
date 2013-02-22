#pragma once
#include <clang-c/Index.h>
#include <string>
#include <stdlib.h>

namespace Clang {
  class SourceLocation {
  public:
    SourceLocation (CXSourceLocation raw)
      : location_ (raw)
    { }

    bool operator== (const SourceLocation & other) const
    {
      return clang_equalLocations (raw(), other.raw());
    }

    const CXSourceLocation & raw () const {
      return location_;
    }

    struct Position {
      std::string file;
      unsigned int line;
      unsigned int column;
      unsigned int offset;
    };

    const Position expansionLocation () const {
      Position res;
      CXFile file;
      clang_getExpansionLocation (raw(), &file, &res.line, &res.column, &res.offset);
      CXString fileName = clang_getFileName (file);
      if (clang_getCString (fileName)) {
        char * canonicalPath = realpath (clang_getCString (fileName), NULL);
        res.file = canonicalPath;
        free(canonicalPath);
      }
      clang_disposeString (fileName);
      return res;
    }

  private:
    CXSourceLocation location_;
  };
}
