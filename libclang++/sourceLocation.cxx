#include "sourceLocation.hxx"
#include "config.h"
#include <stdlib.h>

namespace LibClang {
  SourceLocation::SourceLocation (CXSourceLocation raw)
    : location_ (raw)
  { }

  bool SourceLocation::operator== (const SourceLocation & other) const
  {
    return clang_equalLocations (raw(), other.raw());
  }

  const CXSourceLocation & SourceLocation::raw () const {
    return location_;
  }

  const SourceLocation::Position SourceLocation::expansionLocation () const {
    Position res;
    CXFile file;

#ifdef HAVE_CLANG_GETEXPANSIONLOCATION
    // This is the newer libclang API
    clang_getExpansionLocation (raw(), &file, &res.line, &res.column, &res.offset);
#else
    // Has been deprecated
    clang_getInstantiationLocation (raw(), &file, &res.line, &res.column, &res.offset);
#endif

    CXString fileName = clang_getFileName (file);
    if (clang_getCString (fileName)) {
      char * canonicalPath = realpath (clang_getCString (fileName), NULL);
      res.file = canonicalPath;
      free(canonicalPath);
    }
    clang_disposeString (fileName);
    return res;
  }
}
