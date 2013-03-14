#pragma once

#include <clang-c/Index.h>
#include <memory>

namespace LibClang {
  class TranslationUnit;

  class Index {
  public:
    Index ();
    ~Index ();
    TranslationUnit parse (int argc, char **argv) const;
    const CXIndex & raw() const;

  private:
    std::shared_ptr<CXIndex> index_;
  };
}
