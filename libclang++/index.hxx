#pragma once

#include <clang-c/Index.h>
#include <memory>
#include <vector>
#include <string>

namespace LibClang {
  class TranslationUnit;

  class Index {
  public:
    Index ();
    TranslationUnit parse (int argc, char const *const *const argv) const;
    TranslationUnit parse (const std::vector<std::string> & args) const;
    const CXIndex & raw() const;

  private:
    struct Index_ {
      CXIndex index_;
      Index_ (CXIndex index) : index_ (index) {}
      ~Index_ () { clang_disposeIndex (index_); }
    };

    std::shared_ptr<Index_> index_;
  };
}
