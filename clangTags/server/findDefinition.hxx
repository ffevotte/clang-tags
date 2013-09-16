#pragma once

#include "libclang++/libclang++.hxx"
#include "request/request.hxx"

#include "clangTags/storage.hxx"
#include "clangTags/cache.hxx"

#include <string>

namespace ClangTags {
namespace Server {

class FindDefinition : public Request::CommandParser {
public:
  FindDefinition (Storage & storage,
                  Cache & cache);

  void defaults ();

  void run (std::ostream & cout);

  static void prettyPrint (const ClangTags::Identifier & identifier,
                           std::ostream & cout);

  static void output (const ClangTags::Identifier & identifier,
                      std::ostream & cout);

  static void output (LibClang::Cursor cursor,
                      std::ostream & cout);

private:
  void fromIndex_  (std::ostream & cout);
  void fromSource_ (std::ostream & cout);

  struct Args {
    std::string fileName;
    int         offset;
    bool        diagnostics;
    bool        mostSpecific;
    bool        fromIndex;
  };
  Args args_;

  Storage & storage_;
  Cache & cache_;
};
}
}
