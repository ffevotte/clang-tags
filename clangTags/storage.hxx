#pragma once

#include <json/json.h>
#include <boost/thread.hpp>

namespace ClangTags {
class Storage {
public:
  Storage ();
  ~Storage ();

  int setCompileCommand (const std::string & fileName,
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

  void addInclude (const int includedId, const int sourceId);

  void addInclude (const std::string & includedFile,
                   const std::string & sourceFile);

  // TODO use a structure instead of a large number of arguments
  void addTag (const std::string & usr,
               const std::string & kind,
               const std::string & spelling,
               const std::string & fileName,
               const int line1, const int col1, const int offset1,
               const int line2, const int col2, const int offset2,
               bool isDeclaration);

  struct Reference {
    std::string file;
    int line1;
    int line2;
    int col1;
    int col2;
    int offset1;
    int offset2;
    std::string kind;
    std::string spelling;

    Json::Value json () const {
      Json::Value json;
      json["file"]     = file;
      json["line1"]    = line1;
      json["line2"]    = line2;
      json["col1"]     = col1;
      json["col2"]     = col2;
      json["offset1"]  = offset1;
      json["offset2"]  = offset2;
      json["kind"]     = kind;
      json["spelling"] = spelling;
      return json;
    }
  };

  struct Definition {
    std::string usr;
    std::string file;
    int line1;
    int line2;
    int col1;
    int col2;
    std::string kind;
    std::string spelling;

    Json::Value json () const {
      Json::Value json;
      json["usr"]      = usr;
      json["file"]     = file;
      json["line1"]    = line1;
      json["line2"]    = line2;
      json["col1"]     = col1;
      json["col2"]     = col2;
      json["kind"]     = kind;
      json["spelling"] = spelling;
      return json;
    }
  };

  struct RefDef {
    Reference ref;
    Definition def;

    Json::Value json () const {
      Json::Value json;
      json["ref"] = ref.json();
      json["def"] = def.json();
      return json;
    }
  };

  std::vector<RefDef> findDefinition (const std::string fileName,
                                      int offset);

  std::vector<Reference> grep (const std::string usr);

  template <typename T>
  void getOption (const std::string & name, T & destination);

  void getOption (const std::string & name, std::vector<std::string> & destination);

private:
  void instantiateTemplates_ ();

  class Impl;
  Impl* impl_;

  typedef boost::lock_guard<boost::mutex> Guard;
  boost::mutex mutex_;
};
}
