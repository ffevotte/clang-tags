#pragma once

#include <clang-c/Index.h>
#include <memory>

#include "unsavedFiles.hxx"

namespace LibClang {
  class Index;
  class SourceLocation;
  class Cursor;

  class TranslationUnit {
  public:
    TranslationUnit (CXTranslationUnit tu);

    void reparse ();
    void reparse (UnsavedFiles & unsaved);

    SourceLocation getLocation (const char* fileName, unsigned int offset);
    Cursor cursor () const;

    unsigned int numDiagnostics ();
    std::string diagnostic (unsigned int i);

    const CXTranslationUnit & raw () const;

  private:
    struct TranslationUnit_ {
      CXTranslationUnit translationUnit_;
      TranslationUnit_ (CXTranslationUnit tu) : translationUnit_ (tu) {}
      ~TranslationUnit_ () { clang_disposeTranslationUnit (translationUnit_); }
    };
    std::shared_ptr<TranslationUnit_> translationUnit_;
  };
}
