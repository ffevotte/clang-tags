#pragma once

#include "libclang++/libclang++.hxx"
#include "storage.hxx"
#include <unistd.h>

namespace ClangTags {

class Cache {
public:
  LibClang::TranslationUnit & translationUnit (Storage & storage,
                                               std::string fileName) {
    std::string directory;
    std::vector<std::string> clArgs;
    storage.getCompileCommand (fileName, directory, clArgs);

    // chdir() to the correct directory
    // (whether we need to parse the TU for the first time or reparse it)
    chdir (directory.c_str());

    auto it = tu_.find (fileName);
    if (it == tu_.end()) {
      LibClang::TranslationUnit tu = index_.parse (clArgs);
      return tu_.insert (std::make_pair (fileName, tu)).first->second;
    } else {
      it->second.reparse();
      return it->second;
    }
  }

private:
  LibClang::Index index_;
  std::map<std::string, LibClang::TranslationUnit> tu_;
};

}
