#pragma once

#include "storage.hxx"

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

  struct FindDefinitionArgs {
    std::string fileName;
    int         offset;
    bool        printDiagnostics;
    bool        mostSpecific;
  };
  void findDefinition (FindDefinitionArgs & args);

private:
  Storage & storage_;
};
