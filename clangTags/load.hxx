#pragma once

#include "storage.hxx"
#include "watch.hxx"

#include <string>
#include <iostream>

namespace ClangTags {

class Load
{
public:
  Load (Storage & storage,
        Watch & watch);
  ~Load ();

  struct Args {
    std::string fileName;
  };
  void operator() (Args & args, std::ostream & cout);

private:
  Storage & storage_;
  Watch   & watch_;
  char*     cwd_;
};


inline Load::~Load () {
  delete[] cwd_;
}

}
