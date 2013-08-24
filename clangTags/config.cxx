#include "config.hxx"

namespace ClangTags {
Config::Config (Storage & storage)
  : storage_ (storage)
{}

void Config::operator() (const Args & args, std::ostream & cout) {
  if (args.get) {
    std::string value;
    storage_.getOption (args.name, value);
    cout << value << std::endl;
  } else {
    storage_.setOption (args.name, args.value);
  }
}
}
