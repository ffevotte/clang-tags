#pragma once

#include "storage.hxx"
#include "clangTags/update/thread.hxx"

#include <string>
#include <iostream>

namespace ClangTags {

class Load
{
public:
  Load (Storage::Interface & storage,
        Update::Thread & update);
  ~Load ();

  struct Args {
    std::string fileName;
  };
  void operator() (Args & args, std::ostream & cout);

private:
  Storage::Interface & storage_;
  Update::Thread & update_;
  char* cwd_;
};


inline Load::~Load () {
  delete[] cwd_;
}

}
