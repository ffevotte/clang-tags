#include "load.hxx"
#include "json/json.h"

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdexcept>

namespace ClangTags {

Load::Load (Storage & storage,
            Watch   & watch)
  : storage_ (storage),
    watch_ (watch)
{
  const size_t size = 4096;
  cwd_ = new char[size];
  if (getcwd (cwd_, size) == NULL) {
    // FIXME: correctly handle this case
    throw std::runtime_error ("Not enough space to store current directory name.");
  }
}

void Load::operator() (Args & args,
                       std::ostream & cout) {
  // Change back to the original WD (in case `index` or `update` would have
  // changed it)
  chdir (cwd_);

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

    cout << "  " << fileName << std::endl;
    storage_.setCompileCommand (fileName, directory, clArgs);
  }

  watch_.update();
}
}
