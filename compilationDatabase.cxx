#include "application.hxx"
#include "json/json.h"
#include <fstream>

void Application::compilationDatabase (CompilationDatabaseArgs & args,
                                       std::ostream & cout) {
  Json::Value root;
  Json::Reader reader;

  std::ifstream json (args.fileName);
  bool ret = reader.parse (json, root);
  if ( !ret ) {
    cout  << "Failed to parse compilation database `" << args.fileName << "'\n"
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
