#pragma once

#include "storage.hxx"
#include "libclang++/libclang++.hxx"
#include <map>
#include <iostream>

class Application {
public:
  Application (Storage & storage)
    : storage_ (storage)
  { }


  struct CompilationDatabaseArgs {
    std::string fileName;
  };
  void compilationDatabase (CompilationDatabaseArgs & args, std::ostream & cout);


  struct IndexArgs {
    std::vector<std::string> exclude;
    bool                     diagnostics;
  };
  void index (IndexArgs & args, std::ostream & cout);
  void update (IndexArgs & args, std::ostream & cout);


  struct FindDefinitionArgs {
    std::string fileName;
    int         offset;
    bool        diagnostics;
    bool        mostSpecific;
    bool        fromIndex;
  };
  void findDefinition (FindDefinitionArgs & args, std::ostream & cout);


  struct GrepArgs {
    std::string usr;
  };
  void grep (const GrepArgs & args, std::ostream & cout);


  struct CompleteArgs {
    std::string fileName;
    int         line;
    int         column;
  };
  void complete (CompleteArgs & args, std::ostream & cout);


private:
  void updateIndex_ (IndexArgs & args, std::ostream & cout);
  void findDefinitionFromIndex_  (FindDefinitionArgs & args, std::ostream & cout);
  void findDefinitionFromSource_ (FindDefinitionArgs & args, std::ostream & cout);

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
