#pragma once

#include "request/request.hxx"

#include "clangTags/storage.hxx"

#include <string>
#include <iostream>

namespace ClangTags {
namespace Server {
class Config : public Request::CommandParser {
public:
  Config (Storage & storage);

  void defaults ();
  void run (std::ostream & cout);

private:
  struct Args {
    bool        get;
    std::string name;
    std::string value;
  };
  Args args_;

  Storage & storage_;
};
}
}
