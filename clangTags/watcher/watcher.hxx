#pragma once

namespace ClangTags {
namespace Watcher {

/** @brief Background thread watching source files
 */
class Watcher {
public:
  virtual ~Watcher () {};

  /** @brief Schedule an update of the watched source files
   *
   * Calling this method asks the Watch thread to update its list of source
   * files to watch
   */
  virtual void update () = 0;
};
}
}
