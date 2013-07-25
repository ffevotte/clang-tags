#include "request.hxx"

namespace Request {
  template <>
  void setValue<bool> (const Json::Value & json, bool & destination) {
    destination = json.asBool();
  }

  template <>
  void setValue<std::string> (const Json::Value & json, std::string &
                              destination) {
    destination = json.asString();
  }
}
