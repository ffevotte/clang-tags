#include "request.hxx"
#include "storage.hxx"
#include "index.hxx"

#include <iostream>
#include <fstream>
#include <vector>

#include <jsoncpp/json/json.h>

class CompilationDatabaseCommand : public Request::CommandParser
{
public:
  CompilationDatabaseCommand (const std::string & name, Storage & storage)
    : Request::CommandParser (name, "Create a compilation database"),
      storage_ (storage)
  {
    prompt_ = "compilationDB> ";
  }

  void run () {
    Json::Value root;
    Json::Reader reader;

    std::ifstream json ("compile_commands.json");
    bool ret = reader.parse (json, root);
    if ( !ret ) {
      std::cerr  << "Failed to parse configuration\n"
                 << reader.getFormattedErrorMessages();
    }

    for (unsigned int i=0 ; i<root.size() ; ++i) {
      std::string fileName = root[i]["file"].asString();
      std::string directory = root[i]["directory"].asString();

      std::vector<std::string> clArgs;
      std::istringstream command (root[i]["command"].asString());
      std::string arg;
      std::getline (command, arg, ' ');
      do {
        std::getline (command, arg, ' ');
        clArgs.push_back (arg);
      } while (! command.eof());

      storage_.setCompileCommand (fileName, directory, clArgs);
    }
  }

private:
  Storage & storage_;
};

int main () {
  Storage storage;
  Request::Parser p;
  p .prompt ("clang-dde> ")
    .add (new CompilationDatabaseCommand ("compilationDatabase", storage))
    .add (new IndexCommand ("index", storage))
    .parse (std::cin);

  return EXIT_SUCCESS;
}
