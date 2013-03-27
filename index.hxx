#pragma once

#include "storage.hxx"

class IndexCommand : public Request::CommandParser {
public:
  IndexCommand (const std::string & name, Storage & storage);
  void defaults ();
  void run ();

private:
  Storage &                storage_;
  std::vector<std::string> exclude_;
  bool                     diagnostics_;
};
