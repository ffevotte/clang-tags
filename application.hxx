#pragma once

#include "storage.hxx"
#include "libclang++/libclang++.hxx"
#include "libclang++/translationUnitCache.hxx"
#include <iostream>

class Application {
public:
  Application (Storage & storage, unsigned int cacheLimit)
    : storage_ (storage),
      tu_ (cacheLimit)
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

  Storage & storage_;
  LibClang::Index index_;
  LibClang::TranslationUnitCache tu_;
  char* cwd_;
};
