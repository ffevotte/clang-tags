#include "request.hxx"
#include "storage.hxx"
#include "index.hxx"

#include <iostream>
#include <fstream>
#include <vector>

#include <jsoncpp/json/json.h>

void compilationDatabase (Storage & storage) {
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

    storage.setCompileCommand (fileName, directory, clArgs);
  }
}

int main () {
  Storage storage;

  do {
    std::cout << "clang-dde> " << std::flush;

    std::string inputLine;
    std::getline (std::cin, inputLine);

    if (inputLine == "") {
      continue;
    }

    std::istringstream line (inputLine);
    std::string command;
    line >> command;

    if (command == "compilationDatabase") {
      compilationDatabase (storage);
      continue;
    }

    if (command == "index") {
      index (storage);
      continue;
    }

    std::cerr << "Unknown command: '" << command << "'" << std::endl;

  } while (! std::cin.eof());

  return EXIT_SUCCESS;
}
