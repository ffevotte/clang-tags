#include "libclang++/index.hxx"
#include "libclang++/translationUnit.hxx"
#include "libclang++/cursor.hxx"
#include "libclang++/sourceLocation.hxx"

#include "getopt++/getopt.hxx"

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>

inline
bool startsWith (const std::string & str, const std::string & prefix)
{
  return str.compare(0, prefix.size(), prefix) == 0;
}

CXChildVisitResult indexFile (CXCursor rawCursor,
                              CXCursor rawParent, //unused
                              CXClientData client_data)
{
  const LibClang::Cursor cursor (rawCursor);
  const LibClang::Cursor cursorDef (cursor.referenced());
  const Getopt & options = *((Getopt*)client_data);

  // Skip non-reference cursors
  if (cursorDef.isNull()) {
    return CXChildVisit_Recurse;
  }

  const std::string usr = cursorDef.USR();
  if (usr == "") {
    return CXChildVisit_Recurse;
  }

  const LibClang::SourceLocation::Position begin = cursor.location().expansionLocation();

  if (begin.file == "") {
    return CXChildVisit_Continue;
  }

  {
    typedef std::vector<std::string> Container;
    const Container & exclude = options.getAll("exclude");
    Container::const_iterator it = exclude.begin();
    Container::const_iterator end = exclude.end();
    for ( ; it != end ; ++it) {
      if (startsWith (begin.file, *it)) {
        return CXChildVisit_Continue;
      }
    }
  }

  const LibClang::SourceLocation::Position end = cursor.end().expansionLocation();

  std::cout << usr                    << std::endl
            << cursor.kindStr ()      << std::endl
            << begin.file             << std::endl
            << begin.line             << std::endl
            << begin.column           << std::endl
            << begin.offset           << std::endl
            << end.line               << std::endl
            << end.column             << std::endl
            << end.offset             << std::endl
            << cursor.isDeclaration() << std::endl
            << "--"                   << std::endl;

  return CXChildVisit_Recurse;
}

int main(int argc, char *argv[]) {

  // Command-line arguments handling
  Getopt args (argc, argv,
               "Usage: %c [options] FILE -- CLANG ARGS\n\n"
               "  Index identifiers in a C++ source file.\n\n"
               "  FILE      \t Source file to examine\n"
               "  CLANG ARGS\t Clang command-line arguments");
  args.add ("help",           'h', 0, "Show this help");
  args.add ("no-diagnostics", 'D', 0, "Don't print compiler diagnostics");
  args.add ("exclude",        'e', 1, "Exclude path", "PATH");

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

  LibClang::Index index;
  LibClang::TranslationUnit tu = index.parse (args.argc(), args.argv());
  LibClang::Cursor top (tu);

  // Print clang diagnostics if requested
  if (args["no-diagnostics"] == "") {
    for (unsigned int N = tu.numDiagnostics(),
           i = 0 ; i < N ; ++i) {
      std::cerr << tu.diagnostic (i) << std::endl << std::endl;
    }
  }

  clang_visitChildren (top.raw(), indexFile, &args);

  return EXIT_SUCCESS;
}
