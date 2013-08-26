#pragma once

#include "identifier.hxx"
#include <json/json.h>
#include <boost/thread.hpp>

namespace ClangTags {
namespace Storage {

/** @brief Interface for persistant storage
 *
 * This interface describes the operations needed for setting/getting persistant
 * data needed for clang-tags:
 * - compilation database
 */
class Interface {
public:
  virtual ~Interface () {}

  
  /** @name Options management
   *  @{
   */

  /** @brief Retrieve the value for an option
   *
   * @param[in]  name         option name
   * @param[out] destination  destination where the value will be stored
   */
  virtual void getOption (const std::string & name, std::string & destination) = 0;

  /** @brief Retrieve the value for an option
   *
   * @param[in]  name         option name
   * @param[out] destination  destination where the value will be stored
   */
  virtual void getOption (const std::string & name, bool & destination) = 0;

  /** @brief Retrieve the value for an option
   *
   * @param[in]  name         option name
   * @param[out] destination  destination where the value will be stored
   */
  virtual void getOption (const std::string & name,
                          std::vector<std::string> & destination) = 0;

  /** @brief Store an option value
   *
   * @param name  option name
   * @param value option value
   */
  virtual void setOption (const std::string & name, const std::string & value) = 0;

  /** @brief Store a default value for an option
   *
   * If the option already has a value, it is kept. Otherwise, the default value
   * is stored.
   *
   * @param name  option name
   * @param value option value
   */
  virtual void setOptionDefault (const std::string & name, const std::string & value) = 0;

  /** @} */


  
  /** @name Compilation database management
   *  @{
   */

  /** @brief Retrieve the list of known source files
   *
   * These include both source files for which a compilation command is known,
   * and header files included from them.
   *
   * @return a vector of file paths
   */
  virtual std::vector<std::string> listFiles () = 0;

  /** @brief Retrieve the compilation command associated to a source file
   *
   * If the given file is a header file, return the compilation command
   * associated to a source file which includes it.
   *
   * @throw runtime_error if no compilation command exist for the given source
   *                      file.
   *
   * @param[in]  fileName   absolute path of the source file
   * @param[out] directory  compilation directory
   * @param[out] args       compilation command
   */
  virtual void getCompileCommand (const std::string & fileName,
                                  std::string & directory,
                                  std::vector<std::string> & args) = 0;

  /** @brief Store the compilation command associated to a source file
   *
   * @param fileName   absolute path of the source file
   * @param directory  compilation directory
   * @param args       compilation command
   */
  virtual void setCompileCommand (const std::string & fileName,
                                 const std::string & directory,
                                 const std::vector<std::string> & args) = 0;

  /** @brief Retrieve the next file needing to be indexed
   *
   * These are files which changed on the filesystem after their last indexing.
   * Source files are returned first, before header files.
   *
   * @return absolute path to the source file.
   */
  virtual std::string nextFile () = 0;

  /** @brief Declare the inclusion of a header from a source file
   *
   * @param includedFile absolute path to the header file
   * @param sourceFile   absolute path to the source file
   */
  virtual void addInclude (const std::string & includedFile,
                           const std::string & sourceFile) = 0;

  /** @} */


  
  /** @name Indexing
   *  @{
   */

  /** @brief Begin indexing source files
   *
   * This method should be called before beginning indexing source files.
   */
  virtual void beginIndex () = 0;

  /** @brief End indexing source files
   *
   * This method should be called after having indexed source files.
   */
  virtual void endIndex () = 0;

  /** @brief Begin indexing a source file
   *
   * This method should be called before beginning indexing a source file.
   *
   * @param fileName  absolute path to the source file
   *
   * @return @c true if the file needs indexing (i.e. changed on the filesystem
   *         since its last indexing)
   */
  virtual bool beginFile (const std::string & fileName) = 0;

  /** @brief Add a tag in the index
   *
   * @param usr           Unified Symbol Resolution
   * @param kind          symbol kind
   * @param spelling      symbol name
   * @param fileName      source file path
   * @param line1         line number of the symbol beginning
   * @param col1          column number of the symbol beginning
   * @param offset1       offset (in bytes since the file beginning) of the symbol beginning
   * @param line2         line number of the symbol end
   * @param col2          column number of the symbol end
   * @param offset2       offset (in bytes since the file beginning) of the symbol end
   * @param isDeclaration @c true if the current tag is a declaration/definition
   */
  // TODO use a structure instead of a large number of arguments
  virtual void addTag (const std::string & usr,
                       const std::string & kind,
                       const std::string & spelling,
                       const std::string & fileName,
                       const int line1, const int col1, const int offset1,
                       const int line2, const int col2, const int offset2,
                       bool isDeclaration) = 0;

  /** @} */


  
  /** @name Index querying
   *  @{
   */

  /** @brief Find the identifiers corresponding to a location in the source code
   *
   * @param fileName  absolute path of the source file
   * @param offset    position in the source file (offset in bytes since file beginning)
   *
   * @return vector of @ref Identifier "identifiers"
   */
  virtual std::vector<ClangTags::Identifier> findDefinition (const std::string fileName,
                                                         int offset) = 0;

  /** @brief Retrieve all source location referring to a given symbol
   *
   * @param usr  Unified Symbol Resolution
   *
   * @return vector of @ref Identifier::Reference "symbol references"
   */
  virtual std::vector<ClangTags::Identifier::Reference> grep (const std::string usr) = 0;

  /** @} */

};
}
}
