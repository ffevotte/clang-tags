#pragma once
#include <clang-c/Index.h>
#include <string>

namespace LibClang {
  class SourceLocation {
  public:
    struct Position {
      std::string file;
      unsigned int line;
      unsigned int column;
      unsigned int offset;
    };

    SourceLocation (CXSourceLocation raw);
    bool operator== (const SourceLocation & other) const;
    const CXSourceLocation & raw () const;
    const Position expansionLocation () const;

  private:
    CXSourceLocation location_;
  };
}
