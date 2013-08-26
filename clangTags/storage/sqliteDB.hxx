#pragma once

#include "clangTags/storage.hxx"

namespace ClangTags {
namespace Storage {

/** @brief Sqlite3 storage backend
 *
 * This class implements the @ref Storage::Interface "storage interface" using
 * an Sqlite3 database.
 */
class SqliteDB : public Interface {
public:
  SqliteDB();
  ~SqliteDB();

  void setCompileCommand (const std::string & fileName,
                         const std::string & directory,
                         const std::vector<std::string> & args);

  void getCompileCommand (const std::string & fileName,
                          std::string & directory,
                          std::vector<std::string> & args);

  std::vector<std::string> listFiles ();

  std::string nextFile ();

  void beginIndex ();

  void endIndex ();

  bool beginFile (const std::string & fileName);

  void addInclude (const std::string & includedFile,
                   const std::string & sourceFile);

  void addTag (const std::string & usr,
               const std::string & kind,
               const std::string & spelling,
               const std::string & fileName,
               const int line1, const int col1, const int offset1,
               const int line2, const int col2, const int offset2,
               bool isDeclaration);

  std::vector<ClangTags::Identifier> findDefinition (const std::string fileName,
                                                 int offset);

  std::vector<ClangTags::Identifier::Reference> grep (const std::string usr);

  void getOption (const std::string & name, std::string & destination);
  void getOption (const std::string & name, bool & destination);
  void getOption (const std::string & name, std::vector<std::string> & destination);

  void setOption (const std::string & name, const std::string & value);
  void setOptionDefault (const std::string & name, const std::string & value);

private:
  class Impl;
  Impl* impl_;

  typedef boost::lock_guard<boost::mutex> Guard;
  boost::mutex mutex_;
};
}
}
