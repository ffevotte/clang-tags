#pragma once

#include "cache.hxx"

#include <string>

namespace ClangTags {
class Complete {
public:
  Complete (Storage::Interface & storage,
            Cache & cache);

  struct Args {
    std::string fileName;
    int         line;
    int         column;
  };
  void operator() (Args & args, std::ostream & cout);

private:
  Storage::Interface & storage_;
  Cache & cache_;
};
}
