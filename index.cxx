#include "libclang++/index.hxx"
#include "libclang++/translationUnit.hxx"
#include "libclang++/cursor.hxx"
#include "libclang++/sourceLocation.hxx"
#include "libclang++/visitor.hxx"

#include "getopt++/getopt.hxx"

#include "request.hxx"

#include "util.hxx"

#include "index.hxx"

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>



class Indexer : public LibClang::Visitor<Indexer> {
public:
  Indexer (const std::string & fileName,
           const std::vector<std::string> & exclude,
           Storage & storage)
    : sourceFile_ (fileName),
      exclude_    (exclude),
      storage_    (storage)
  {
    needsUpdate_[fileName] = storage.beginFile (fileName);
  }

  CXChildVisitResult visit (LibClang::Cursor cursor,
                            LibClang::Cursor parent)
  {
    const LibClang::Cursor cursorDef (cursor.referenced());

    // Skip non-reference cursors
    if (cursorDef.isNull()) {
      return CXChildVisit_Recurse;
    }

    const std::string usr = cursorDef.USR();
    if (usr == "") {
      return CXChildVisit_Recurse;
    }

    const LibClang::SourceLocation::Position begin = cursor.location().expansionLocation();
    const String fileName = begin.file;

    if (fileName == "") {
      return CXChildVisit_Continue;
    }

    { // Skip excluded paths
      auto it  = exclude_.begin();
      auto end = exclude_.end();
      for ( ; it != end ; ++it) {
        if (fileName.startsWith (*it)) {
          return CXChildVisit_Continue;
        }
      }
    }

    if (needsUpdate_.count(fileName) == 0) {
      needsUpdate_[fileName] = storage_.beginFile (fileName);
      storage_.addInclude (fileName, sourceFile_);
    }

    if (needsUpdate_[fileName]) {
      const LibClang::SourceLocation::Position end = cursor.end().expansionLocation();
      storage_.addTag (usr, cursor.kindStr(), fileName,
                       begin.line, begin.column, begin.offset,
                       end.line,   end.column,   end.offset,
                       cursor.isDeclaration());
    }

    return CXChildVisit_Recurse;
  }

private:
  const std::string              & sourceFile_;
  const std::vector<std::string> & exclude_;
  Storage                        & storage_;
  std::map<std::string, bool>      needsUpdate_;
};


void index (Storage & storage) {
  std::vector<std::string> exclude;
  bool diagnostics (true);
  Request::reader () >> Request::key ("exclude", exclude)
                     >> Request::key ("diagnostics", diagnostics)
                     >> Request::read (std::cin);

  storage.cleanIndex();
  storage.beginIndex();

  std::cerr << std::endl
            << "-- Indexing project" << std::endl;
  Timer totalTimer;

  std::string fileName;
  while ((fileName = storage.nextFile()) != "") {
    std::cerr << fileName << ":" << std::endl
              << "  parsing..." << std::flush;
    Timer timer;

    std::string directory;
    std::vector<std::string> args;
    storage.getCompileCommand (fileName, directory, args);

    LibClang::Index index;
    LibClang::TranslationUnit tu = index.parse (args);

    std::cerr << "\t" << timer.get() << "s." << std::endl;
    timer.reset();

    // Print clang diagnostics if requested
    if (diagnostics) {
      for (unsigned int N = tu.numDiagnostics(),
             i = 0 ; i < N ; ++i) {
        std::cerr << tu.diagnostic (i) << std::endl << std::endl;
      }
    }

    std::cerr << "  indexing..." << std::flush;
    LibClang::Cursor top (tu);
    Indexer indexer (fileName, exclude, storage);
    indexer.visitChildren (top);
    std::cerr << "\t" << timer.get() << std::endl;
  }
  std::cerr << totalTimer.get() << "s." << std::endl;

  storage.endIndex();
}
