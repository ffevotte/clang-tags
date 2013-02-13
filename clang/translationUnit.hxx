#pragma once

#include <clang-c/Index.h>
#include "sourceLocation.hxx"

namespace Clang {
  class TranslationUnit {
  public:
    TranslationUnit (int argc, char **argv)
      : index_ (clang_createIndex (0, 0)),
        translationUnit_ (clang_parseTranslationUnit (index_, 0,
                                                      argv, argc,
                                                      0, 0,
                                                      CXTranslationUnit_None))
    { }

    ~TranslationUnit () {
      clang_disposeTranslationUnit (translationUnit_);
      clang_disposeIndex (index_);
    }

    SourceLocation getLocation (const char* fileName, unsigned int offset) {
      CXFile topFile = clang_getFile (translationUnit_, fileName);
      CXSourceLocation target = clang_getLocationForOffset (translationUnit_,
                                                            topFile,
                                                            offset);
      return target;
    }

    unsigned int numDiagnostics () {
      return clang_getNumDiagnostics (raw());
    }

    std::string diagnostic (unsigned int i) {
      CXDiagnostic diagnostic = clang_getDiagnostic (raw(), i);
      CXString string = clang_formatDiagnostic (diagnostic,
                                                clang_defaultDiagnosticDisplayOptions());
      std::string res = clang_getCString (string);
      clang_disposeString (string);
      clang_disposeDiagnostic (diagnostic);
      return res;
    }

    const CXTranslationUnit & raw () const {
      return translationUnit_;
    }

  private:
    CXIndex index_;
    CXTranslationUnit translationUnit_;
  };
}
