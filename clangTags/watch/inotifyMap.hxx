#include <string>
#include <map>

namespace ClangTags {
namespace Watch {

class InotifyMap {
public:
  void add (const std::string & fileName, int wd);
  std::string fileName (int wd);
  bool contains (const std::string & fileName);

private:
  std::map<std::string, int> wd_;
  std::map<int, std::string> file_;
};
}
}
