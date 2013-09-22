#pragma once

#include "identifier.hxx"
#include "sqlite++/transaction.hxx"

namespace ClangTags {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Interface for persistant storage
 *
 * This interface describes the operations needed for setting/getting persistant
 * data needed for clang-tags:
 * - compilation database
 */
class Storage {
public:
  Storage ();
  ~Storage ();

  
  /** @name Options management
   *  @{
   */

  /** @brief Retrieve the value for an option
   *
   * @param[in]  name         option name
   * @param[out] destination  destination where the value will be stored
   */
  void getOption (const std::string & name, std::string & destination);

  /** @brief Retrieve the value for an option
   *
   * @param[in]  name         option name
   * @param[out] destination  destination where the value will be stored
   */
  void getOption (const std::string & name, bool & destination);

  /** @brief Retrieve the value for an option
   *
   * @param[in]  name         option name
   * @param[out] destination  destination where the value will be stored
   */
  void getOption (const std::string & name,
                  std::vector<std::string> & destination);

  /** @brief Store an option value
   *
   * @param name  option name
   * @param value option value
   */
  void setOption (const std::string & name, const std::string & value);

  /** @brief Store a default value for an option
   *
   * If the option already has a value, it is kept. Otherwise, the default value
   * is stored.
   *
   * @param name  option name
   * @param value option value
   */
  void setOptionDefault (const std::string & name, const std::string & value);

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
  std::vector<std::string> listFiles ();

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
  void getCompileCommand (const std::string & fileName,
                          std::string & directory,
                          std::vector<std::string> & args);

  /** @brief Store the compilation command associated to a source file
   *
   * @param fileName   absolute path of the source file
   * @param directory  compilation directory
   * @param args       compilation command
   */
  void setCompileCommand (const std::string & fileName,
                          const std::string & directory,
                          const std::vector<std::string> & args);

  /** @brief Retrieve the next file needing to be indexed
   *
   * These are files which changed on the filesystem after their last indexing.
   * Source files are returned first, before header files.
   *
   * @return absolute path to the source file.
   */
  std::string nextFile ();

  /** @brief Declare the inclusion of a header from a source file
   *
   * @param includedFile absolute path to the header file
   * @param sourceFile   absolute path to the source file
   */
  void addInclude (const std::string & includedFile,
                   const std::string & sourceFile);

  /** @} */


  
  /** @name Indexing
   *  @{
   */

  /** @brief Begin indexing a source file
   *
   * This method should be called before beginning indexing a source file.
   *
   * @param fileName  absolute path to the source file
   *
   * @return @c true if the file needs indexing (i.e. changed on the filesystem
   *         since its last indexing)
   */
  bool beginFile (const std::string & fileName);

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
  void addTag (const std::string & usr,
               const std::string & kind,
               const std::string & spelling,
               const std::string & fileName,
               const int line1, const int col1, const int offset1,
               const int line2, const int col2, const int offset2,
               bool isDeclaration);

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
  std::vector<ClangTags::Identifier> findDefinition (const std::string fileName,
                                                     int offset);

  /** @brief Retrieve all source location referring to a given symbol
   *
   * @param usr  Unified Symbol Resolution
   *
   * @return vector of @ref Identifier::Reference "symbol references"
   */
  std::vector<ClangTags::Identifier::Reference> grep (const std::string usr);

  /** @} */

  
  /** @brief Begin an SQL transaction
   *
   * This method returns an Sqlite::Transaction object, whose lifetime
   * determines the SQL transaction.
   *
   * @return an Sqlite::Transaction object associated to the database
   */
  Sqlite::Transaction beginTransaction ();

private:
  class Impl;
  Impl *impl_;
};
/** @} */
}
