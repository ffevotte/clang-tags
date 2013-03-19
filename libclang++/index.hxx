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
    ~Index ();
    TranslationUnit parse (int argc, char const *const *const argv) const;
    TranslationUnit parse (const std::vector<std::string> & args) const;
    const CXIndex & raw() const;

  private:
    std::shared_ptr<CXIndex> index_;
  };
}
