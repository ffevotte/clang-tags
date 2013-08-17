#pragma once

#include "storage.hxx"
#include "cache.hxx"
#include "libclang++/libclang++.hxx"
#include <string>

namespace ClangTags {

class FindDefinition {
public:
  FindDefinition (Storage & storage,
                  Cache   & cache);

  struct Args {
    std::string fileName;
    int         offset;
    bool        diagnostics;
    bool        mostSpecific;
    bool        fromIndex;
  };
  void operator() (Args & args, std::ostream & cout);

  static void displayRefDef (const Storage::RefDef & refDef,
                             std::ostream & cout);

  static void outputRefDef (const Storage::RefDef & refDef,
                            std::ostream & cout);

  static void displayCursor (LibClang::Cursor cursor,
                             std::ostream & cout);

private:
  void fromIndex_  (Args & args, std::ostream & cout);
  void fromSource_ (Args & args, std::ostream & cout);

  Storage & storage_;
  Cache   & cache_;
};
}
