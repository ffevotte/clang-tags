#include "translationUnit.hxx"
#include "index.hxx"
#include "sourceLocation.hxx"
#include "cursor.hxx"

namespace LibClang {
  TranslationUnit::TranslationUnit (CXTranslationUnit tu)
    : translationUnit_ (new CXTranslationUnit (tu))
  { }

  TranslationUnit::~TranslationUnit () {
    clang_disposeTranslationUnit (raw());
  }

  SourceLocation TranslationUnit::getLocation (const char* fileName, unsigned int offset) {
    CXFile topFile = clang_getFile (raw(), fileName);
    CXSourceLocation target = clang_getLocationForOffset (raw(),
                                                          topFile,
                                                          offset);
    return target;
  }

  Cursor TranslationUnit::cursor () const {
    return Cursor (*this);
  }

  unsigned int TranslationUnit::numDiagnostics () {
    return clang_getNumDiagnostics (raw());
  }

  std::string TranslationUnit::diagnostic (unsigned int i) {
    CXDiagnostic diagnostic = clang_getDiagnostic (raw(), i);
    CXString string = clang_formatDiagnostic (diagnostic,
                                              clang_defaultDiagnosticDisplayOptions());
    std::string res = clang_getCString (string);
    clang_disposeString (string);
    clang_disposeDiagnostic (diagnostic);
    return res;
  }

  const CXTranslationUnit & TranslationUnit::raw () const {
    return *translationUnit_;
  }
}
