#pragma once

#include "request/request.hxx"
#include "clangTags/cache.hxx"

#include <string>

namespace ClangTags {
namespace Server {
class Complete : public Request::CommandParser {
public:
  Complete (Storage & storage,
            Cache & cache);

  void defaults ();
  void run (std::ostream & cout);

private:
  struct Args {
    std::string fileName;
    int         line;
    int         column;
  };
  Args args_;

  Storage & storage_;
  Cache & cache_;
};
}
}
