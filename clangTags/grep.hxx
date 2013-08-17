#pragma once

#include "storage.hxx"

namespace ClangTags {
class Grep {
public:
  Grep (Storage & storage);

  struct Args {
    std::string usr;
  };
  void operator() (const Args & args, std::ostream & cout);

private:
  Storage storage_;
};
}
