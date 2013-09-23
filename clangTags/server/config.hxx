#pragma once

#include "request/request.hxx"

#include "clangTags/storage.hxx"

#include <string>
#include <iostream>

namespace ClangTags {
namespace Server {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Configure clang-tags options
 *
 * This class provides a parser for the @c "config" command, which gets/sets
 * clang-tags options.
 */
class Config : public Request::CommandParser {
public:
  /** @brief Constructor
   *
   * @param storage @ref Storage instance used to store options
   */
  Config (Storage & storage);

  void defaults ();

  void run (std::ostream & cout);

private:
  struct Args {
    bool        get;
    std::string name;
    std::string value;
  };
  Args args_;

  Storage & storage_;
};
/** @} */
}
}
