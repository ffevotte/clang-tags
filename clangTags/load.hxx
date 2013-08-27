#pragma once

#include "storage.hxx"
#include "update.hxx"

#include <string>
#include <iostream>

namespace ClangTags {

class Load
{
public:
  Load (Storage::Interface & storage,
        Update & update);
  ~Load ();

  struct Args {
    std::string fileName;
  };
  void operator() (Args & args, std::ostream & cout);

private:
  Storage::Interface & storage_;
  Update & update_;
  char* cwd_;
};


inline Load::~Load () {
  delete[] cwd_;
}

}
