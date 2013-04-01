#pragma once

#include "storage.hxx"
#include "libclang++/libclang++.hxx"
#include <map>

class Application {
public:
  Application (Storage & storage)
    : storage_ (storage)
  { }


  struct CompilationDatabaseArgs {
    std::string fileName;
  };
  void compilationDatabase (CompilationDatabaseArgs & args);

  struct IndexArgs {
    std::vector<std::string> exclude;
    bool                     diagnostics;
  };
  void index (IndexArgs & args);
  void update (IndexArgs &args);
  void updateIndex (IndexArgs & args);

  struct FindDefinitionArgs {
    std::string fileName;
    int         offset;
    bool        printDiagnostics;
    bool        mostSpecific;
  };
  void findDefinition (FindDefinitionArgs & args);

private:
  LibClang::TranslationUnit & translationUnit_ (std::string fileName) {
    auto it = tu_.find (fileName);
    if (it == tu_.end()) {
      std::string directory;
      std::vector<std::string> clArgs;
      storage_.getCompileCommand (fileName, directory, clArgs);

      LibClang::TranslationUnit tu = index_.parse (clArgs);
      return tu_.insert (std::make_pair (fileName, tu)).first->second;
    } else {
      it->second.reparse();
      return it->second;
    }
  }

  Storage & storage_;
  LibClang::Index index_;
  std::map<std::string, LibClang::TranslationUnit> tu_;
};
