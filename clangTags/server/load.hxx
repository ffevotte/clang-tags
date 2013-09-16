#pragma once

#include "request/request.hxx"

#include "clangTags/storage.hxx"
#include "clangTags/indexer/indexer.hxx"

#include <string>
#include <iostream>

namespace ClangTags {
namespace Server {

class Load : public Request::CommandParser {
public:
  Load (Storage & storage,
        Indexer::Indexer & indexer);
  ~Load ();

  void defaults ();
  void run (std::ostream & cout);

private:
  struct Args {
    std::string fileName;
  };
  Args args_;

  Storage & storage_;
  Indexer::Indexer & indexer_;
  char* cwd_;
};
}
}
