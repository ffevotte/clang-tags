#include "libclang++/libclang++.hxx"
#include "getopt++/getopt.hxx"

#include "request.hxx"
#include "util.hxx"
#include "application.hxx"

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
    storage_.addInclude (fileName, fileName);
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



void Application::index (IndexArgs & args) {
  std::cerr << std::endl
            << "-- Indexing project" << std::endl;
  storage_.setOption ("exclude", args.exclude);
  storage_.cleanIndex();

  updateIndex (args);
}

void Application::update (IndexArgs & args) {
  std::cerr << std::endl
            << "-- Updating index" << std::endl;
  args.exclude = storage_.getOption ("exclude", Storage::Vector());

  updateIndex (args);
}

void Application::updateIndex (IndexArgs & args) {
  Timer totalTimer;

  storage_.beginIndex();
  std::string fileName;
  while ((fileName = storage_.nextFile()) != "") {
    std::cerr << fileName << ":" << std::endl
              << "  parsing..." << std::flush;
    Timer timer;

    std::string directory;
    std::vector<std::string> clArgs;
    storage_.getCompileCommand (fileName, directory, clArgs);

    LibClang::Index index;
    LibClang::TranslationUnit tu = index.parse (clArgs);

    std::cerr << "\t" << timer.get() << "s." << std::endl;
    timer.reset();

    // Print clang diagnostics if requested
    if (args.diagnostics) {
      for (unsigned int N = tu.numDiagnostics(),
             i = 0 ; i < N ; ++i) {
        std::cerr << tu.diagnostic (i) << std::endl << std::endl;
      }
    }

    std::cerr << "  indexing..." << std::flush;
    LibClang::Cursor top (tu);
    Indexer indexer (fileName, args.exclude, storage_);
    indexer.visitChildren (top);
    std::cerr << "\t" << timer.get() << std::endl;
  }
  storage_.endIndex();
  std::cerr << totalTimer.get() << "s." << std::endl;
}
