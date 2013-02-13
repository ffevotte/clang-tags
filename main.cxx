#include <clang-c/Index.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "clang/translationUnit.hxx"
#include "clang/cursor.hxx"
#include "clang/sourceLocation.hxx"

#include "getopt/getopt.hxx"

void displayCursor (Clang::Cursor cursor)
{
  const Clang::SourceLocation location (cursor.location());
  const Clang::Cursor cursorDef = cursor.referenced();

  if (cursorDef.isNull()) {
    return;
  }

  // Display cursor
  {
    const Clang::SourceLocation::Position begin = location.expansionLocation();
    const Clang::SourceLocation::Position end = cursor.end().expansionLocation();

    std::cout << "-- " << begin.file << ":"
              << begin.line << "-" << end.line << ":"
              << begin.column << "-" << end.column-1 << ":" << std::endl
              << cursor.kindStr() << " " << cursor.spelling()
              << std::endl;
  }

  // Display referenced cursor
  {
    const Clang::SourceLocation::Position begin = cursorDef.location().expansionLocation();
    const Clang::SourceLocation::Position end = cursorDef.end().expansionLocation();

    std::cout << "   "
              << begin.file << ":"
              << begin.line << "-" << end.line << ":"
              << begin.column << "-" << end.column-1 << ": "
      // << "(" << cursorDef.USR() << ")" << std::endl << "\t"
              << cursorDef.kindStr() << " " << cursorDef.spelling()
              << std::endl << std::endl;
  }
}

CXChildVisitResult findDefinition (CXCursor rawCursor,
                                   CXCursor rawParent, //unused
                                   CXClientData client_data)
{
  Clang::Cursor cursor (rawCursor);
  const Clang::SourceLocation & targetLocation = *((Clang::SourceLocation*)client_data);
  const Clang::SourceLocation location (cursor.location());

  // Skip unexposed cursor kinds
  if (cursor.isUnexposed()) {
    return CXChildVisit_Recurse;
  }

  if (location == targetLocation) {
    displayCursor (cursor);
  }

  return CXChildVisit_Recurse;
}

int main(int argc, char *argv[]) {

  // Command-line arguments handling
  Getopt opt (argc, argv,
              "FILE OFFSET -- CLANG ARGUMENTS\n\n"
              "Find the place of definition of an identifier in a C++ source file.\n\n"
              "Required arguments:\n"
              "  FILE      \t Source file to examine\n"
              "  OFFSET    \t Cursor position (in bytes)\n"
              "  CLANG ARGS\t Clang command-line arguments\n"
              "            \t   (e.g. '-I . -std=c++11 main.cxx')");
  opt.add ("help",          'h', 0, "Show this help");
  opt.add ("most-specific", 's', 0, "Show only the most specific reference");
  opt.add ("debug",         'D', 0, "Print debug information");

  // Get optional arguments
  try {
    opt.get ();
  }
  catch (...) {
    std::cerr << std::endl << opt.usage();
    return (EXIT_FAILURE);
  }

  if (opt["help"] == "true") {
    std::cerr << opt.usage();
    return (EXIT_SUCCESS);
  }

  // Get required arguments
  char * fileName = argv[optind];
  int offset;
  if (! fromString (offset, argv[optind+1])) {
    std::cerr << "Invalid offset: " << argv[optind+1]
              << std::endl << std::endl
              << opt.usage();
    return (EXIT_FAILURE);
  }

  optind += 2;
  argc -= optind;
  argv += optind;

  // Print debug info on CL arguments if requested
  if (opt["debug"] == "true") {
    std::cerr << "file:\t" << fileName << std::endl;
    std::cerr << "offset:\t" << offset << std::endl;
    std::cerr << "most-specific:\t" << opt["most-specific"] << std::endl;
    std::cerr << "debug:\t" << opt["debug"] << std::endl;
    std::cerr << "clang CL:";

    for (int i=0 ; i < argc ; ++i) {
      std::cerr << " " << argv[i];
    }
    std::cerr << std::endl << std::endl;
  }


  Clang::TranslationUnit tu (argc, argv);


  // Print clang diagnostics
  for (unsigned int N = tu.numDiagnostics(),
         i = 0 ; i < N ; ++i) {
    std::cerr << tu.diagnostic (i) << std::endl << std::endl;
  }

  // Print cursor definition
  if (opt["most-specific"] == "true") {
    Clang::Cursor cursor (tu, fileName, offset);
    displayCursor (cursor);
  }
  else {
    Clang::Cursor cursor (tu);
    Clang::SourceLocation target = tu.getLocation (fileName, offset);
    clang_visitChildren (cursor.raw(), findDefinition, &target);
  }

  return 0;
}
