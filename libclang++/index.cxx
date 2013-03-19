#include "index.hxx"
#include "translationUnit.hxx"

namespace LibClang {
  Index::Index ()
    : index_ (new CXIndex (clang_createIndex (0, 0)))
  { }

  Index::~Index () {
    clang_disposeIndex (raw());
  }

  TranslationUnit Index::parse (int argc, char const *const *const argv) const {
    return clang_parseTranslationUnit (raw(), 0,
                                       argv, argc,
                                       0, 0,
                                       CXTranslationUnit_None);
  }

  TranslationUnit Index::parse (const std::vector<std::string> & args) const {
    std::vector<const char*> args_c;
    auto i   = args.begin();
    auto end = args.end();
    for ( ; i != end ; ++i) {
      args_c.push_back (i->c_str());
    }

    return parse (args_c.size(), &(args_c[0]));
  }

  const CXIndex & Index::raw () const {
    return *index_;
  }
}
