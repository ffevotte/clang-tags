#pragma once

#include "storage.hxx"
#include "libclang++/libclang++.hxx"
#include <map>
#include <iostream>

class Application {
public:
  Application (Storage & storage)
    : storage_ (storage)
  {
    const size_t size = 4096;
    cwd_ = new char[size];
    if (getcwd (cwd_, size) == NULL) {
      // FIXME: correctly handle this case
     throw std::runtime_error ("Not enough space to store current directory name.");
    }
  }

  ~Application () {
    delete[] cwd_;
  }


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
    std::string directory;
    std::vector<std::string> clArgs;
    storage_.getCompileCommand (fileName, directory, clArgs);

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

  Storage & storage_;
  LibClang::Index index_;
  std::map<std::string, LibClang::TranslationUnit> tu_;
  char* cwd_;
};
