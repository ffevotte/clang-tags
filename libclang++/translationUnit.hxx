#pragma once

#include <clang-c/Index.h>
#include <memory>

namespace LibClang {
  class Index;
  class SourceLocation;
  class Cursor;

  class TranslationUnit {
  public:
    TranslationUnit (const Index & index, int argc, char **argv);
    TranslationUnit (CXTranslationUnit tu);
    ~TranslationUnit ();

    SourceLocation getLocation (const char* fileName, unsigned int offset);
    Cursor cursor () const;

    unsigned int numDiagnostics ();
    std::string diagnostic (unsigned int i);

    const CXTranslationUnit & raw () const;

  private:
    std::shared_ptr<CXTranslationUnit> translationUnit_;
  };
}
