#pragma once
#include <clang-c/Index.h>
#include <string>

namespace LibClang {
  class TranslationUnit;
  class SourceLocation;

  class Cursor {
  public:
    Cursor (CXCursor raw);
    Cursor (const TranslationUnit & tu);
    Cursor (const TranslationUnit & tu,
            const std::string & fileName, const unsigned int offset);

    const CXCursor & raw () const;
    bool isNull () const;
    bool isUnexposed () const;
    bool isDeclaration () const;
    Cursor referenced () const;
    std::string kindStr () const;
    std::string spelling () const;
    std::string USR () const;
    SourceLocation location () const;
    SourceLocation end () const;

  private:
    CXCursor cursor_;
  };
}
