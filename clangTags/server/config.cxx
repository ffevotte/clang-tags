#include "config.hxx"

namespace ClangTags {
namespace Server {

Config::Config (Storage & storage)
  : Request::CommandParser ("config", "Get/set clang-tags configuration"),
    storage_ (storage)
{
  prompt_ = "config> ";
  defaults();

  using Request::key;
  add (key ("get", args_.get)
       ->metavar ("true|false")
       ->description ("if true, get the option value. Otherwise, set it"));
  add (key ("option", args_.name)
       ->metavar ("NAME")
       ->description ("option name"));
  add (key ("value", args_.value)
       ->metavar ("JSON_VAL")
       ->description ("JSON-encoded option value"));
}

void Config::defaults () {
  args_.get = false;
}

void Config::run (std::ostream & cout) {
  if (args_.get) {
    std::string value;
    storage_.getOption (args_.name, value);
    cout << value << std::endl;
  } else {
    storage_.setOption (args_.name, args_.value);
  }
}
}
}
