#include "sourceFile.hxx"

#include "libclang++/index.hxx"
#include "libclang++/translationUnit.hxx"
#include "libclang++/sourceLocation.hxx"
#include "libclang++/cursor.hxx"
#include "libclang++/visitor.hxx"

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

class FindDefinition : public LibClang::Visitor<FindDefinition>
{
public:
  FindDefinition (const LibClang::SourceLocation & targetLocation)
    : targetLocation_ (targetLocation)
  {}

  CXChildVisitResult visit (LibClang::Cursor cursor,
                            LibClang::Cursor parent)
  {
    const LibClang::SourceLocation location (cursor.location());

    // Skip unexposed cursor kinds
    if (cursor.isUnexposed()) {
      return CXChildVisit_Recurse;
    }

    if (location == targetLocation_) {
      displayCursor (cursor);
    }

    return CXChildVisit_Recurse;
  }

private:
  const LibClang::SourceLocation & targetLocation_;
};

bool findDefinition () {
  std::string fileName;
  int offset;
  std::vector<std::string> args;
  bool printDiagnostics = true;
  bool mostSpecific = false;

  while (true) {
    std::cout << "find-def> " << std::flush;

    std::string line;
    std::getline (std::cin, line);

    if (std::cin.eof()) {
      std::cerr << "Exiting..." << std::endl;
      return false;
    }

    std::istringstream input (line);
    std::string key;
    input >> key;

    if (key == "") {
      break;
    }

    if (key == "fileName") {
      input >> fileName;
      continue;
    }

    if (key == "offset") {
      input >> offset;
      continue;
    }

    if (key == "arg") {
      std::string arg;
      input >> arg;
      args.push_back (arg);
      continue;
    }

    std::cerr << "Unknown key: '" << key << "'" << std::endl;
  }

  LibClang::Index index;
  LibClang::TranslationUnit tu = index.parse (args);

  // Print clang diagnostics if requested
  if (printDiagnostics) {
    for (unsigned int N = tu.numDiagnostics(),
           i = 0 ; i < N ; ++i) {
      std::cerr << tu.diagnostic (i) << std::endl << std::endl;
    }
  }

  // Print cursor definition
  LibClang::Cursor cursor (tu, fileName.c_str(), offset);
  if (mostSpecific) {
    displayCursor (cursor);
  }
  else {
    LibClang::SourceLocation target = cursor.location();
    FindDefinition findDef (target);
    findDef.visitChildren (tu.cursor());
  }

  return true;
}

int main () {
  while (true) {
    std::cout << "clang-tags> " << std::flush;

    std::string line;
    std::getline (std::cin, line);
    std::istringstream input (line);

    std::string command;
    input >> command;

    bool ok = true;
    if (command == "") {
      /* do nothing */
    }
    else if (command == "find-definition") {
      ok = findDefinition ();
    } else {
      std::cerr << "Unknown command: '" << command << "'" << std::endl;
    }

    if (!ok || std::cin.eof()) {
      std::cout << "\nExiting..." << std::endl;
      break;
    }
  }

  return EXIT_SUCCESS;
}
