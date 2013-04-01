#include "index.hxx"
#include "translationUnit.hxx"

namespace LibClang {
  Index::Index ()
    : index_ (new Index_ (clang_createIndex (0, 0)))
  { }

  TranslationUnit Index::parse (int argc, char const *const *const argv) const {
    return clang_createTranslationUnitFromSourceFile (raw(), 0,
                                                      argc, argv,
                                                      0, 0);
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
    return index_->index_;
  }
}
