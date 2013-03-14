#include "sourceFile.hxx"

#include "libclang++/index.hxx"
#include "libclang++/translationUnit.hxx"
#include "libclang++/sourceLocation.hxx"
#include "libclang++/cursor.hxx"

#include "getopt++/getopt.hxx"

#include <iostream>
#include <cstdlib>


void displayCursor (LibClang::Cursor cursor)
{
  const LibClang::SourceLocation location (cursor.location());
  const LibClang::Cursor cursorDef = cursor.referenced();

  if (cursorDef.isNull()) {
    return;
  }

  // Display cursor
  {
    const LibClang::SourceLocation::Position begin = location.expansionLocation();
    const LibClang::SourceLocation::Position end = cursor.end().expansionLocation();
    SourceFile sourceFile (begin.file);

    std::cout << "-- " << sourceFile.substring (begin.offset, end.offset) << " -- "
              << cursor.kindStr() << " " << cursor.spelling()
              << std::endl;
  }

  // Display referenced cursor
  {
    const LibClang::SourceLocation::Position begin = cursorDef.location().expansionLocation();
    const LibClang::SourceLocation::Position end = cursorDef.end().expansionLocation();

    std::cout << "   "
              << begin.file << ":"
              << begin.line << "-" << end.line << ":"
              << begin.column << "-" << end.column-1 << ": "
              << cursorDef.kindStr() << " " << cursorDef.spelling()
              << std::endl << "   USR: " << cursorDef.USR()
              << std::endl << std::endl;
  }
}

CXChildVisitResult findDefinition (CXCursor rawCursor,
                                   CXCursor rawParent, //unused
                                   CXClientData client_data)
{
  LibClang::Cursor cursor (rawCursor);
  const LibClang::SourceLocation & targetLocation = *((LibClang::SourceLocation*)client_data);
  const LibClang::SourceLocation location (cursor.location());

  // Skip unexposed cursor kinds
  if (cursor.isUnexposed()) {
    return CXChildVisit_Recurse;
  }

  if (location == targetLocation) {
    displayCursor (cursor);
  }

  return CXChildVisit_Recurse;
}

int main (int argc, char **argv) {
  // Command-line arguments handling
  Getopt args (argc, argv,
               "Usage: %c FILE OFFSET [options] -- COMPILER ARGUMENTS\n\n"
               "  Find the place of definition of an identifier in a C++ source file.\n\n"
               "  FILE      \t Source file to examine\n"
               "  OFFSET    \t Position in the file (in bytes)\n"
               "  CLANG ARGS\t Clang command-line arguments\n"
               "            \t   (e.g. '-I . -std=c++11 main.cxx')\n\n");
  args.add ("help",           'h', 0, "Show this help");
  args.add ("most-specific",  's', 0, "Show only the most specific reference");
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

  // Get first argument: FILE
  std::string fileName = args.shift();
  if (fileName == "") {
    std::cerr << "Missing required argument FILE."
              << std::endl << std::endl
              << args.usage();
    return (EXIT_FAILURE);
  }

  // Get second argument: OFFSET
  int offset;
  {
    std::string offsetStr = args.shift();
    if (offsetStr == "") {
      std::cerr << "Missing required argument OFFSET."
                << std::endl << std::endl
                << args.usage();
      return (EXIT_FAILURE);
    }
    if (! fromString (offset, offsetStr)) {
      std::cerr << "Invalid offset: " << offsetStr
                << std::endl << std::endl
                << args.usage();
      return (EXIT_FAILURE);
    }
  }

  LibClang::Index index;
  LibClang::TranslationUnit tu = index.parse (args.argc(), args.argv());

  // Print clang diagnostics if requested
  if (args["no-diagnostics"] == "") {
    for (unsigned int N = tu.numDiagnostics(),
           i = 0 ; i < N ; ++i) {
      std::cerr << tu.diagnostic (i) << std::endl << std::endl;
    }
  }

  // Print cursor definition
  LibClang::Cursor cursor (tu, fileName.c_str(), offset);
  if (args["most-specific"] == "true") {
    displayCursor (cursor);
  }
  else {
    LibClang::SourceLocation target = cursor.location();
    clang_visitChildren (tu.cursor().raw(), findDefinition, &target);
  }

  return EXIT_SUCCESS;
}
