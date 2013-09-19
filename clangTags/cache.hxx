#pragma once

#include "libclang++/libclang++.hxx"
#include "libclang++/translationUnitCache.hxx"
#include "storage.hxx"
#include <unistd.h>

namespace ClangTags {

class Cache {
public:
  Cache (unsigned int cacheLimit)
    : tu_ (cacheLimit)
  {}

  LibClang::TranslationUnit & translationUnit (Storage & storage,
                                               std::string fileName) {
    std::string directory;
    std::vector<std::string> clArgs;
    storage.getCompileCommand (fileName, directory, clArgs);

    // chdir() to the correct directory
    // (whether we need to parse the TU for the first time or reparse it)
    chdir (directory.c_str());

    if (!tu_.contains (fileName)) {
      LibClang::TranslationUnit tu = index_.parse (clArgs);
      tu_.insert (fileName, tu);
      return tu_.get (fileName);
    } else {
      LibClang::TranslationUnit & tu = tu_.get (fileName);
      tu.reparse();
      return tu;
    }
  }

private:
  LibClang::Index index_;
  LibClang::TranslationUnitCache tu_;
};

}
