#pragma once

#include "storage.hxx"

#include <string>
#include <iostream>

namespace ClangTags {

class Config
{
public:
  Config (Storage & storage);

  struct Args {
    bool        get;
    std::string name;
    std::string value;
  };
  void operator() (const Args & args, std::ostream & cout);

private:
  Storage & storage_;
};
}
