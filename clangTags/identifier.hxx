#pragma once

#include <json/json.h>

namespace ClangTags {
/** @addtogroup clangTags
 *  @{
 */

/** @brief Symbol in the source code
 */
struct Identifier {
  /** @brief Location of an identifier in the source code
   */
  struct Reference {
    std::string file;           /**< @brief Source file path */
    int line1;                  /**< @brief Line number of the identifier beginning */
    int line2;                  /**< @brief Line number of the identifier end */
    int col1;                   /**< @brief Column number of the identifier beginning */
    int col2;                   /**< @brief Column number of the identifier end */
    int offset1;                /**< @brief Offset of the identifier beginning */
    int offset2;                /**< @brief Offset of the identifier end */
    std::string kind;           /**< @brief Symbol kind */
    std::string spelling;       /**< @brief Identifier name */

    /** @brief Serialize to a JSON object
     *
     * @return the reference properties, serialized as a Json::Value
     */
    Json::Value json () const {
      Json::Value json;
      json["file"]     = file;
      json["line1"]    = line1;
      json["line2"]    = line2;
      json["col1"]     = col1;
      json["col2"]     = col2;
      json["offset1"]  = offset1;
      json["offset2"]  = offset2;
      json["kind"]     = kind;
      json["spelling"] = spelling;
      return json;
    }
  };

  /** @brief Location of the definition of a symbol in the source code
   */
  struct Definition {
    std::string usr;            /**< @brief Unified Symbol Resolution */
    std::string file;           /**< @brief Source file path */
    int line1;                  /**< @brief Line number of the definition beginning */
    int line2;                  /**< @brief Line number of the definition end */
    int col1;                   /**< @brief Column number of the definition beginning */
    int col2;                   /**< @brief Column number of the definition end */
    std::string kind;           /**< @brief Symbol kind */
    std::string spelling;       /**< @brief Symbol name */

    /** @brief Serialize to a JSON object
     *
     * @return the definition properties, serialized as a Json::Value
     */
    Json::Value json () const {
      Json::Value json;
      json["usr"]      = usr;
      json["file"]     = file;
      json["line1"]    = line1;
      json["line2"]    = line2;
      json["col1"]     = col1;
      json["col2"]     = col2;
      json["kind"]     = kind;
      json["spelling"] = spelling;
      return json;
    }
  };

  Reference  ref;  /**< @brief Location of the identifier in the source code */
  Definition def;  /**< @brief Location of the symbol definition */

  /** @brief Serialize to a JSON object
   *
   * @return the identifier properties, serialized as a Json::Value
   */
  Json::Value json () const {
    Json::Value json;
    json["ref"] = ref.json();
    json["def"] = def.json();
    return json;
  }
};
/** @} */
}
