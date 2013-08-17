#pragma once

#include "cache.hxx"

#include <string>

namespace ClangTags {
class Complete {
public:
  Complete (Cache & cache);

  struct Args {
    std::string fileName;
    int         line;
    int         column;
  };
  void operator() (Args & args, std::ostream & cout);

private:
  Cache & cache_;
};
}
