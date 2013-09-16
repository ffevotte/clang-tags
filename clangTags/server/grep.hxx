#pragma once

#include "request/request.hxx"
#include "clangTags/storage.hxx"

namespace ClangTags {
namespace Server {

class Grep : public Request::CommandParser {
public:
  Grep (Storage & storage);
  void defaults ();
  void run (std::ostream & cout);

private:
  struct Args {
    std::string usr;
  };
  Args args_;

  Storage & storage_;
};
}
}
