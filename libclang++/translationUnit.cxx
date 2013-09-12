#include "translationUnit.hxx"
#include "index.hxx"
#include "sourceLocation.hxx"
#include "cursor.hxx"

namespace LibClang {
  TranslationUnit::TranslationUnit (CXTranslationUnit tu)
    : translationUnit_ (new TranslationUnit_ (tu))
  { }

  void TranslationUnit::reparse () {
    clang_reparseTranslationUnit (raw(), 0, 0,
                                  clang_defaultReparseOptions(raw()));
  }

  void TranslationUnit::reparse (UnsavedFiles & unsaved) {
    clang_reparseTranslationUnit (raw(),
                                  unsaved.size(), unsaved.begin(),
                                  clang_defaultReparseOptions(raw()));
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

  unsigned long TranslationUnit::memoryUsage () const {
    CXTUResourceUsage usage = clang_getCXTUResourceUsage (raw());
    unsigned long total = 0;

    for (unsigned int i = 0; i < usage.numEntries; ++i)
    {
      const CXTUResourceUsageEntry & entry = usage.entries[i];
      total += entry.amount;
    }

    clang_disposeCXTUResourceUsage(usage);
    return total;
  }

  const CXTranslationUnit & TranslationUnit::raw () const {
    return translationUnit_->translationUnit_;
  }
}
