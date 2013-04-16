#pragma once

#include "sqlite++/database.hxx"
#include "sqlite++/statement.hxx"

#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <sstream>

#include "json/json.h"

class Storage {
public:
  Storage ()
    : db_ (".ct.sqlite2")
  {
    db_.execute ("CREATE TABLE IF NOT EXISTS files ("
                 "  id      INTEGER PRIMARY KEY,"
                 "  name    TEXT,"
                 "  indexed INTEGER"
                 ")");
    db_.execute ("CREATE TABLE IF NOT EXISTS commands ("
                 "  fileId     INTEGER REFERENCES files(id),"
                 "  directory  TEXT,"
                 "  args       TEXT"
                 ")");
    db_.execute ("CREATE TABLE IF NOT EXISTS includes ("
                 "  sourceId   INTEGER REFERENCES files(id),"
                 "  includedId INTEGER REFERENCES files(id)"
                 ")");
    db_.execute ("CREATE TABLE IF NOT EXISTS tags ("
                 "  fileId   INTEGER REFERENCES files(id),"
                 "  usr      TEXT,"
                 "  kind     TEXT,"
                 "  line1    INTEGER,"
                 "  col1     INTEGER,"
                 "  offset1  INTEGER,"
                 "  line2    INTEGER,"
                 "  col2     INTEGER,"
                 "  offset2  INTEGER,"
                 "  isDecl   BOOLEAN"
                 ")");
    db_.execute ("CREATE TABLE IF NOT EXISTS options ( "
                 "  name   TEXT, "
                 "  value  TEXT "
                 ")");
  }

  int setCompileCommand (const std::string & fileName,
                         const std::string & directory,
                         const std::vector<std::string> & args) {
    int fileId = addFile_ (fileName);
    addInclude (fileId, fileId);

    db_.prepare ("DELETE FROM commands "
                 "WHERE fileId=?")
      .bind (fileId)
      .step();

    db_.prepare ("INSERT INTO commands VALUES (?,?,?)")
      .bind (fileId)
      .bind (directory)
      .bind (serialize_ (args))
      .step();

    return fileId;
  }

  void getCompileCommand (const std::string & fileName,
                          std::string & directory,
                          std::vector<std::string> & args) {
    int fileId = fileId_ (fileName);
    Sqlite::Statement stmt
      = db_.prepare ("SELECT commands.directory, commands.args "
                     "FROM includes "
                     "INNER JOIN commands ON includes.sourceId = commands.fileId "
                     "WHERE includes.includedId = ?")
      .bind (fileId);

    switch (stmt.step()) {
    case SQLITE_DONE:
      throw std::runtime_error ("no compilation command for file `"
                                + fileName + "'");
      break;
    default:
      std::string serializedArgs;
      stmt >> directory >> serializedArgs;
      deserialize_ (serializedArgs, args);
    }
  }

  std::string nextFile () {
    Sqlite::Statement stmt
      = db_.prepare ("SELECT included.name, included.indexed, source.name, "
                     "       count(source.name) AS sourceCount "
                     "FROM includes "
                     "INNER JOIN files AS source ON source.id = includes.sourceId "
                     "INNER JOIN files AS included ON included.id = includes.includedId "
                     "GROUP BY included.id "
                     "ORDER BY sourceCount ");
    while (stmt.step() == SQLITE_ROW) {
      std::string includedName;
      int indexed;
      std::string sourceName;
      stmt >> includedName >> indexed >> sourceName;

      struct stat fileStat;
      stat (includedName.c_str(), &fileStat);
      int modified = fileStat.st_mtime;

      if (modified > indexed) {
        return sourceName;
      }
    }

    return "";
  }

  void cleanIndex () {
    db_.execute ("DELETE FROM tags");
    db_.execute ("UPDATE files SET indexed = 0");
  }

  void beginIndex () {
    db_.execute ("BEGIN TRANSACTION");
  }

  void endIndex () {
    db_.execute ("END TRANSACTION");
  }

  bool beginFile (const std::string & fileName) {
    int fileId = addFile_ (fileName);

    int indexed;
    {
      Sqlite::Statement stmt
        = db_.prepare ("SELECT indexed FROM files WHERE id = ?")
        .bind (fileId);
      stmt.step();
      stmt >> indexed;
    }

    struct stat fileStat;
    stat (fileName.c_str(), &fileStat);
    int modified = fileStat.st_mtime;

    if (modified > indexed) {
      db_.prepare ("DELETE FROM tags WHERE fileId=?").bind (fileId).step();
      db_.prepare ("DELETE FROM includes WHERE sourceId=?").bind (fileId).step();
      db_.prepare ("UPDATE files "
                   "SET indexed=? "
                   "WHERE id=?")
        .bind (modified)
        .bind (fileId)
        .step();
      return true;
    } else {
      return false;
    }
  }

  void addInclude (const int includedId,
                   const int sourceId)
  {
    int res = db_.prepare ("SELECT * FROM includes "
                           "WHERE sourceId=? "
                           "  AND includedId=?")
      .bind (sourceId) .bind (includedId)
      .step ();
    if (res == SQLITE_DONE) { // No matching row
      db_.prepare ("INSERT INTO includes VALUES (?,?)")
        .bind (sourceId) . bind (includedId)
        .step();
    }
  }

  void addInclude (const std::string & includedFile,
                   const std::string & sourceFile) {
    int includedId = fileId_ (includedFile);
    int sourceId   = fileId_ (sourceFile);
    if (includedId == -1 || sourceId == -1)
      throw std::runtime_error ("Cannot add inclusion for unknown files `"
                                + includedFile + "' and `" + sourceFile + "'");
    addInclude (includedId, sourceId);
  }

  void addTag (const std::string & usr,
               const std::string & kind,
               const std::string & fileName,
               const int line1, const int col1, const int offset1,
               const int line2, const int col2, const int offset2,
               bool isDeclaration) {
    int fileId = fileId_ (fileName);
    if (fileId == -1) {
      return;
    }

    Sqlite::Statement stmt =
      db_.prepare ("SELECT * FROM tags "
                   "WHERE fileId=? "
                   "  AND usr=?"
                   "  AND offset1=?"
                   "  AND offset2=?")
      .bind (fileId).bind (usr).bind (offset1).bind (offset2);
    if (stmt.step() == SQLITE_DONE) { // no matching row
      db_.prepare ("INSERT INTO tags VALUES (?,?,?,?,?,?,?,?,?,?)")
        .bind(fileId) .bind(usr)  .bind(kind)
        .bind(line1)  .bind(col1) .bind(offset1)
        .bind(line2)  .bind(col2) .bind(offset2)
        .bind(isDeclaration)
        .step();
    }
  }

  struct Reference {
    std::string usr;
    int offset1;
    int offset2;
    std::string kind;
    std::string defFile;
    int defLine1;
    int defLine2;
    int defCol1;
    int defCol2;
  };
  std::vector<Reference> findDefinition (const std::string fileName,
                       int offset) {
    int fileId = fileId_ (fileName);
    Sqlite::Statement stmt =
      db_.prepare ("SELECT ref.usr, ref.offset1, ref.offset2, ref.kind, "
                   "       files.name, def.line1, def.line2, def.col1, def.col2 "
                   "FROM tags AS ref "
                   "INNER JOIN tags AS def ON def.usr = ref.usr "
                   "INNER JOIN files ON def.fileId = files.id "
                   "WHERE def.isDecl = 1 "
                   "  AND ref.fileId = ?  "
                   "  AND ref.offset1 <= ? "
                   "  AND ref.offset2 >= ? "
                   "ORDER BY (ref.offset2 - ref.offset1)")
      .bind (fileId)
      .bind (offset)
      .bind (offset);

    std::vector<Reference> ret;
    while (stmt.step() == SQLITE_ROW) {
      Reference ref;
      stmt >> ref.usr >> ref.offset1 >> ref.offset2 >> ref.kind
           >> ref.defFile >> ref.defLine1 >> ref.defLine2
           >> ref.defCol1 >> ref.defCol2;
      ret.push_back(ref);
    }
    return ret;
  }

  void setOption (const std::string & name, const std::string & value) {
    db_.prepare ("DELETE FROM options "
                 "WHERE name = ?")
      .bind (name)
      .step();

    db_.prepare ("INSERT INTO options "
                 "VALUES (?, ?)")
      .bind (name)
      .bind (value)
      .step();
  }

  void setOption (const std::string & name, const std::vector<std::string> & value) {
    setOption (name, serialize_(value));
  }


  std::string getOption (const std::string & name) {
    Sqlite::Statement stmt =
      db_.prepare ("SELECT value FROM options "
                   "WHERE name = ?")
      .bind (name);

    std::string ret;
    if (stmt.step() == SQLITE_ROW) {
      stmt >> ret;
    } else {
      throw std::runtime_error ("No stored value for option: `" + name + "'");
    }

    return ret;
  }

  struct Vector {};
  std::vector<std::string> getOption (const std::string & name, const Vector & v) {
    std::vector<std::string> ret;
    deserialize_ (getOption (name), ret);
    return ret;
  }

private:
  int fileId_ (const std::string & fileName) {
    Sqlite::Statement stmt
      = db_.prepare ("SELECT id FROM files WHERE name=?")
      .bind (fileName);

    int id = -1;
    if (stmt.step() == SQLITE_ROW) {
      stmt >> id;
    }

    return id;
  }

  int addFile_ (const std::string & fileName) {
    int id = fileId_ (fileName);
    if (id == -1) {
      db_.prepare ("INSERT INTO files VALUES (NULL, ?, 0)")
        .bind (fileName)
        .step();

      id = db_.lastInsertRowId();
    }
    return id;
  }

  std::string serialize_ (const std::vector<std::string> & v) {
    Json::Value json;
    auto it = v.begin();
    auto end = v.end();
    for ( ; it != end ; ++it) {
      json.append (*it);
    }

    Json::FastWriter writer;
    return writer.write (json);
  }

  void deserialize_ (const std::string & s, std::vector<std::string> & v) {
    Json::Value json;
    Json::Reader reader;
    if (! reader.parse (s, json)) {
      throw std::runtime_error (reader.getFormattedErrorMessages());
    }

    for (int i=0 ; i<json.size() ; ++i) {
      v.push_back (json[i].asString());
    }
  }

  Sqlite::Database db_;
};
