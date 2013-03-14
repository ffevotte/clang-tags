#include "index.hxx"
#include "translationUnit.hxx"

namespace LibClang {
  Index::Index ()
    : index_ (new CXIndex (clang_createIndex (0, 0)))
  { }

  Index::~Index () {
    clang_disposeIndex (raw());
  }

  TranslationUnit Index::parse (int argc, char **argv) const {
    return clang_parseTranslationUnit (raw(), 0,
                                       argv, argc,
                                       0, 0,
                                       CXTranslationUnit_None);
  }

  const CXIndex & Index::raw () const {
    return *index_;
  }
}
