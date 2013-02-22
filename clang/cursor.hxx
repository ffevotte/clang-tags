#pragma once
#include <clang-c/Index.h>
#include "translationUnit.hxx"
#include "sourceLocation.hxx"

namespace Clang {

  class Cursor {
  public:
    Cursor (CXCursor raw)
      : cursor_ (raw)
    { }

    Cursor (const TranslationUnit & tu)
      : cursor_ (clang_getTranslationUnitCursor (tu.raw()))
    { }

    Cursor (const TranslationUnit & tu,
            const std::string & fileName, const unsigned int offset)
      : cursor_ (clang_getCursor
                 (tu.raw(),
                  clang_getLocationForOffset (tu.raw(),
                                              clang_getFile (tu.raw(), fileName.c_str()),
                                              offset)))
    { }

    const CXCursor & raw () const { return cursor_; };

    bool isNull () const {
      return clang_Cursor_isNull (raw());
    }

    bool isUnexposed () const {
      return clang_isUnexposed (clang_getCursorKind(raw()));
    }

    Cursor referenced () const {
      return clang_getCursorReferenced (raw());
    }

    std::string kindStr () const {
      CXCursorKind kind = clang_getCursorKind (raw());
      CXString kindSpelling = clang_getCursorKindSpelling (kind);
      std::string res = clang_getCString (kindSpelling);
      clang_disposeString (kindSpelling);
      return res;
    }

    std::string spelling () const {
      CXString cursorSpelling = clang_getCursorSpelling (raw());
      std::string res = clang_getCString (cursorSpelling);
      clang_disposeString (cursorSpelling);
      return res;
    }

    std::string USR () const {
      CXString cursorUSR = clang_getCursorUSR (raw());
      std::string res = clang_getCString (cursorUSR);
      clang_disposeString (cursorUSR);
      return res;
    }

    SourceLocation location () const {
      return clang_getCursorLocation (raw());
    }

    SourceLocation end () const {
      CXSourceRange extent = clang_getCursorExtent (raw());
      return clang_getRangeEnd (extent);
    }

  private:
    CXCursor cursor_;
  };
}
