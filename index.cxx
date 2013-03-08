#include "clang/translationUnit.hxx"
#include "clang/cursor.hxx"
#include "clang/sourceLocation.hxx"

#include "getopt/getopt.hxx"

#include "findDefinition.hxx"
#include "sourceFile.hxx"

#include <clang-c/Index.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

CXChildVisitResult indexFile (CXCursor rawCursor,
                              CXCursor rawParent, //unused
                              CXClientData client_data)
{
  std::map<std::string, bool> & includedFiles = *((std::map<std::string, bool>*) client_data);
  const Clang::Cursor cursor (rawCursor);
  const Clang::Cursor cursorDef (cursor.referenced());

  // Skip non-reference cursors
  if (cursorDef.isNull()) {
    return CXChildVisit_Recurse;
  }

  const std::string usr = cursorDef.USR();
  if (usr == "") {
    return CXChildVisit_Recurse;
  }

  const Clang::SourceLocation::Position begin = cursor.location().expansionLocation();

  if (begin.file == "") {
    return CXChildVisit_Continue;
  }

  if (begin.file.compare(0, 4, "/usr") == 0) {
    return CXChildVisit_Continue;
  }

  includedFiles[begin.file] = true;

  const Clang::SourceLocation::Position end = cursor.end().expansionLocation();
  SourceFile sourceFile (begin.file);

  std::cout << usr << " "
            << begin.file << ":" << begin.line << ":"
            << sourceFile.line (begin.line) << std::endl;

  return CXChildVisit_Recurse;
}

int main(int argc, char *argv[]) {

  // Command-line arguments handling
  Getopt args (argc, argv,
               "Usage: %c FILE [options] -- CLANG ARGS\n\n"
               "  Index identifiers in a C++ source file.\n\n"
               "  FILE      \t Source file to examine\n"
               "  CLANG ARGS\t Clang command-line arguments");
  args.add ("help",           'h', 0, "Show this help");
  args.add ("no-diagnostics", 'D', 0, "Don't print compiler diagnostics");

  // Get optional arguments
  try {
    args.get ();
  }
  catch (...) {
    std::cerr << std::endl << args.usage();
    return (EXIT_FAILURE);
  }

  if (args["help"] == "true") {
    std::cerr << args.usage();
    return (EXIT_SUCCESS);
  }

  if (args.argc() == 0) {
    std::cerr << std::endl << args.usage();
    return (EXIT_FAILURE);
  }

  Clang::TranslationUnit tu (args.argc(), args.argv());
  Clang::Cursor top (tu);

  typedef std::map<std::string, bool>  map;
  map includedFiles;

  // Print clang diagnostics if requested
  if (args["no-diagnostics"] == "") {
    for (unsigned int N = tu.numDiagnostics(),
           i = 0 ; i < N ; ++i) {
      std::cerr << tu.diagnostic (i) << std::endl << std::endl;
    }
  }

  clang_visitChildren (top.raw(), indexFile, &includedFiles);

  return EXIT_SUCCESS;
}
