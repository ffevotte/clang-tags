#include <json/json.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

/** @defgroup request Request
    @brief Requests handling library

This module handles requests composed of:
- a command, and
- a set of named and typed arguments (refered to as @em keys in the following)

In the following, we will take @c "repeat" to be a command taking two keys:
- @c "times" of integer type, and
- @c "input" of string type.

Commands can be formatted in one of two ways:
- plain text, e.g:
  @verbatim
  repeat
  times 5
  input foo
  @endverbatim

- JSON value, e.g:
  @verbatim
  {
    "command": "repeat",
    "times":   5,
    "input":   "foo"
  }
  @endverbatim

This module helps parsing such requests and define appropriate C++ functors to
handle commands. For example in this case, handling this request would output 5
times the string "foo".

Additionally, a request parser understands (only in plain-text form) an "help"
command displaying help messages :
- general help, listing available commands:
  @verbatim
  help
  @endverbatim

- help on a specific command, listing available keys:
  @verbatim
  help COMMAND
  @endverbatim

See @ref test_request.cxx for an example showing how to use this module. Here
is a example interactive session for an application defining the @c "repeat" command as above:

@verbatim
Command> help
Example application
Commands:
  help                 Display this help
  help COMMAND         Display help about COMMAND
  repeat               Repeat provided input

Command> help repeat
Repeat provided input

Arguments:
  input STRING
      message to repeat
      required

  times N
      number of times to repeat
      default: 2

Command> repeat
Repeat> times 5
Repeat> input foo
Repeat>
foo
foo
foo
foo
foo
Command> ^D
@endverbatim

*/

namespace Request {
  /** @brief Base class for key parsers
   *
   * A key is a named parameter taking a typed value.
   *
   * @ingroup request
   */
  class KeyParserBase {
  public:
    /** @brief Constructor
     *
     * @param name  key name
     */
    KeyParserBase (std::string name)
      : name_ (name),
        required_ (false)
    {}

    // Destructor
    virtual ~KeyParserBase () {}

    /** @brief Get the key name
     *
     * @return the key name, as a string
     */
    const std::string & name () const {
      return name_;
    }

    /** @brief Display help about the key
     *
     * Display the key name, along with indications about its usage. Such
     * indications include, when available:
     * - expected values
     * - a description of the key meaning
     * - a default value taken if the key is not provided in the request
     * - whether the key is optional or required
     * - whether the key can take multiple values
     *
     * @param cout  I/O stream where help will be printed
     */
    void display (std::ostream & cout) const {
      cout << "  " << name_ << " " << metavar_ << std::endl;

      if (description_ != "")
        cout << "      " << description_ << std::endl;

      if (isVector())
        cout << "      multiple values allowed" << std::endl;

      if (default_ != "")
        cout << "      default: " << default_ << std::endl;

      if (required_)
        cout << "      required" << std::endl;

      cout << std::endl;
    }

    /** @brief Set the meta variable name for the key
     *
     * The meta variable name is a generic name used as a placeholder for the
     * value taken by the key in the documentation. For example, the meta
     * variable name for a "fileName" key could be "PATH", indicating that the
     * value provided to the key should be a filesystem path. The meta variable
     * name is included in the help message printed by the display() method.
     *
     * Returns the key itself to allow chaining mutator calls.
     *
     * @param metavar  new value for the meta variable name
     *
     * @return  the key itself
     */
    KeyParserBase * metavar (std::string metavar) {
      metavar_ = metavar;
      return this;
    }

    /** @brief Set the description string for the key
     *
     * The description string is included in the help message output by the
     * display() method.
     *
     * Returns the key itself to allow chaining mutator calls.
     *
     * @param description  new value for the key description
     *
     * @return  the key itself
     */
    KeyParserBase * description (std::string description) {
      description_ = description;
      return this;
    }

    /** @brief Sets whether the key is optional or required
     *
     * If a required key is missing in a request, an error message will be
     * output. Optional keys can take default values when missing in the
     * request.
     *
     * Returns the key itself to allow chaining mutator calls.
     *
     * @param req  @c true if the key should be required
     *
     * @return  the key itself
     */
    KeyParserBase * required (bool req = true) {
      required_ = req;
      return this;
    }

    /** @brief Tell whether the key takes more than one value
     *
     * A scalar key can take only one argument. If it is present multiple times
     * in the request, only the last value will be used. A vector key takes
     * multiple values : each occurence of the key in the request adds the
     * correponding value to a list. In JSON-formatted requests, vector keys are
     * associated to an array of values.
     *
     * @return  @c true if the key takes multiple values, @c false otherwise
     */
    virtual bool isVector () const {
      return false;
    }

    /** @brief Parse an input stream and handle the key value
     *
     * Parse the key value in an input text stream and store it. This method is
     * used for plain-text requests. Values associated to vector keys are appended
     * to the list of previously read values.
     *
     * @param cin  input stream
     */
    virtual void parse (std::istream & cin) = 0;

    /** @brief Retrieve the key value from a JSON value and handle it
     *
     * This method is used for JSON-formatted requests.
     *
     * @param json  JSON request
     */
    virtual void set (const Json::Value & json) = 0;

  protected:
    /** @brief Description of the default key value
     *
     * Used for the help message
     */
    std::string default_;

  private:
    const std::string name_;
    std::string description_;
    std::string metavar_;
    bool required_;
  };

  template <typename T>
  void setValue (const Json::Value & json, T & destination) {
    std::istringstream iss (json.asString());
    iss >> destination;
  }

  /** @brief Scalar key parser
   *
   * Parses a key and stores a single value in a destination variable.
   *
   * @param T  type of associated value
   *
   * @ingroup request
   */
  template <typename T>
  class Key : public KeyParserBase {
  public:
    /** @brief Constructor
     *
     * @param name  key name
     * @param destination  reference to a variable where the key value
     *                     will be stored
     *
     * @return
     */
    Key (std::string name, T & destination)
      : KeyParserBase (name),
        destination_ (destination)
    {
      std::ostringstream defaultStr;
      defaultStr << std::boolalpha << destination;
      default_ = defaultStr.str();
    }

    virtual void parse (std::istream & cin) {
      cin >> std::boolalpha >> destination_;
    }

    virtual void set (const Json::Value & json) {
      setValue (json, destination_);
    }

  private:
    T & destination_;
  };

  /** @brief Vector key parser
   *
   * Parses a key and stores multiple values in a destination vector.
   *
   * @param T  type of value stored
   *
   * @ingroup request
   */
  template <typename T>
  class Key<std::vector<T> > : public KeyParserBase {
  public:
    /** @brief Type of container in which values are stored */
    typedef std::vector<T>  Vector;

    /** @brief Constructor
     *
     * @param name  key name
     * @param destination  reference to a vector where key values
     *                     will be stored
     */
    Key (std::string name, Vector & destination)
      : KeyParserBase (name),
        destination_ (destination)
    {
      Json::Value defaults;

      auto it = destination.begin();
      auto end = destination.end();
      for ( ; it != end ; ++it) {
        defaults.append (*it);
      }

      default_ = Json::FastWriter().write (defaults);
    }

    virtual bool isVector () const {
      return true;
    }

    virtual void parse (std::istream & cin) {
      T val;
      cin >> std::boolalpha >> val;
      destination_.push_back (val);
    }

    virtual void set (const Json::Value & json) {
      destination_.resize (json.size());
      for (int i = 0 ; i<json.size() ; ++i) {
        setValue (json[i], destination_[i]);
      }
    }

  private:
    Vector & destination_;
  };


  /** @brief Base class for command parsers
   *
   * Parses the keys associated to a command, and run it.
   *
   * Custom commands should be implemented through user-defined functors
   * deriving from CommandParser.
   *
   * @snippet test_request.cxx CommandParser
   * @ingroup request
   */
  class CommandParser {
  public:
    /** @brief Constructor
     *
     * @param name         Command name
     * @param description  Command description
     */
    CommandParser (std::string name, std::string description = "")
      : prompt_ (name + "> "),
        description_ (description),
        name_ (name)
    { }

    virtual ~CommandParser () {
      auto it = keys_.begin();
      auto end = keys_.end();
      for ( ; it != end ; ++it) {
        delete (it->second);
      }
    }

    /** @brief Parse an input stream and handle key arguments
     *
     * Parse all key arguments provided to the command, and handle them. Also
     * @ref run the command and output results to the provided stream. This
     * method is used for plain-text requests.
     *
     * @param cin   input stream, where the request will be read
     * @param cout  output stream
     *
     * @sa parseJson()
     */
    void parse (std::istream & cin, std::ostream & cout, bool echo = false) {
      defaults();
      do {
        cout << prompt_ << std::flush;
        std::string line;
        std::getline (cin, line);

        if (echo) {
          cout << line << std::endl;
        }

        if (line == "") {
          break;
        }

        std::istringstream input (line);
        std::string key;
        input >> key;

        KeyMap::const_iterator it = keys_.find(key);
        if (it != keys_.end()) {
          it->second->parse (input);
        } else {
          cout << "Unknown key: `" << key << "'" << std::endl;
        }

      } while (! cin.eof());
      run (cout);
    }

    /** @brief Parse an input stream and handle key arguments
     *
     * Parse all key arguments provided to the command, and handle them. Also
     * @ref run the command and output results to the provided stream. This
     * method is used for JSON-formatted requests.
     *
     * @param request  JSON request
     * @param cout     output stream
     *
     * @sa parseJson()
     */
    void parseJson (const Json::Value & request, std::ostream & cout) {
      defaults();

      auto it = keys_.begin();
      auto end = keys_.end();
      for ( ; it != end ; ++it){
        Json::Value arg = request[it->first];
        if (! arg.isNull()) {
          it->second->set (arg);
        }
      }

      run(cout);
    }

  protected:
    /** @brief Get the command name
     *
     * @return the command name, as a string
     */
    const std::string & name () {
      return name_;
    }

    /** @brief Set the command description
     *
     * This description will be used for help messages.
     *
     * @param description  command description
     */
    void setDescription (std::string description) {
      description_ = description;
    }

    /** @brief Set default values for keys
     *
     * This method should be specialized to specify default key values.
     */
    virtual void defaults () {}

    /** @brief Run the command
     *
     * This typically is a user-defined function. It should use key values and
     * write results on the provided output stream.
     *
     * @param cout  output stream for results
     */
    virtual void run (std::ostream & cout) = 0;

    /** @brief Add a @ref KeyParserBase "key parser" to the list of parameters
     *
     * @param key  key parser
     *
     * @return the command parser itself
     */
    CommandParser & add (KeyParserBase * key) {
      keys_[key->name()] = (key);
      return *this;
    }

    /** @brief Prompt used for interactive plain-text requests
     */
    std::string prompt_;

  private:
    /** @brief Display a detailed help about the command
     *
     * Display the command name, along with indications about is use, which
     * include when available:
     * - the command description
     * - description of the command keys (as output by KeyParserBase::display)
     *
     * @param cout  output stream where help will be printed
     *
     * @sa display()
     */
    void help (std::ostream & cout) const {
      cout << description_ << std::endl
           << std::endl
           << "Arguments:" << std::endl;
      auto it = keys_.begin();
      auto end = keys_.end();
      for ( ; it != end ; ++it) {
        it->second->display(cout);
      }
    }

    /** @brief Display a short help about the command
     *
     * Display the command name, along with its description when available.
     *
     * @param cout  output stream where help will be printed
     *
     * @sa help()
     */
    void display (std::ostream & cout) {
      cout << std::setw(20) << std::left << name_
           << " " << description_ << std::endl;
    }

    typedef std::map<std::string, KeyParserBase*> KeyMap;

    std::string description_;
    const std::string name_;
    KeyMap keys_;

    friend class Parser;
  };

  /** @brief Request parser
   *
   * Parses a request to identify the command, and run it.
   *
   * @snippet test_request.cxx Parser
   * @ingroup request
   */
  class Parser {
  public:
    /** @brief Constructor
     *
     * @param description  global description for the request parser
     */
    Parser (std::string description = "")
      : description_ (description),
        echo_ (false)
    { }

    ~Parser () {
      auto it = commands_.begin();
      auto end = commands_.end();
      for ( ; it != end ; ++it) {
        delete it->second;
      }
    }

    /** @brief Set a global prompt
     *
     * This prompt will be used at the top level for interactive plain-text
     * requests.
     *
     * @param p   prompt string
     *
     * @return  the parser itself
     */
    Parser & prompt (std::string p) {
      prompt_ = p;
      return *this;
    }

    /** @brief Set echo
     *
     * When reading an plain-text request and echo is activated, read lines are
     * echoed after the prompt. This is moslty useful when reading a plain-text
     * request non-interactively (for exemple for testing purposes).
     *
     * @param activate   @c true to activate echo
     *
     * @return the parser itself
     */
    Parser & echo (bool activate = true) {
      echo_ = activate;
      return *this;
    }

    /** @brief Add a command parser
     *
     * @param command  pointer to a command parser
     *
     * @return  the parser itself
     */
    Parser & add (CommandParser * command) {
      commands_[command->name()] = command;
      return *this;
    }

    /** @brief Display help about the command parser
     *
     * Displays the command parser description, along with a list of all
     * recognised commands.
     *
     * @param cout  output stream where help will be displayed
     */
    void help (std::ostream & cout) {
      cout << description_ << std::endl
           << "Commands:" << std::endl
           << "  " << std::setw(20) << std::left << "help"
           << " Display this help" << std::endl
           << "  " << std::setw(20) << std::left << "help COMMAND"
           << " Display help about COMMAND" << std::endl;

      auto it = commands_.begin();
      auto end = commands_.end();
      for ( ; it != end ; ++it) {
        cout << "  ";
        it->second->display(cout);
      }
    }

    /** @brief Parse a stream of plain-text requests and run the associated
     *         commands
     *
     * @param cin   input stream where the request is read
     * @param cout  output stream where results are printed
     */
    void parse (std::istream & cin, std::ostream & cout) {
      do {
        cout << prompt_ << std::flush;

        std::string line;
        std::getline (cin, line);

        if (echo_) {
          cout << line << std::endl;
        }

        if (line == "") {
          continue;
        }

        std::istringstream input (line);
        std::string command;
        input >> command;

        bool helpRequested = false;
        if (command == "help") {
          input >> command;
          if (command == "help") {
            help(cout);
            continue;
          } else {
            helpRequested = true;
          }
        }

        CommandMap::const_iterator it = commands_.find (command);
        if (it != commands_.end()) {
          if (helpRequested)
            it->second->help (cout);
          else
            it->second->parse (cin, cout, echo_);
        } else {
          cout << "Unknown command: `" << command << "'" << std::endl;
        }

      } while (! cin.eof());
    }

    /** @brief Parse a stream of JSON requests and run the associated commands
     *
     * JSON requests must be separated by blank lines.
     *
     * @param cin      input stream where requests are read
     * @param cout     output stream where results are printed
     * @param verbose  if @c true, output progress information
     */
    void parseJson (std::istream & cin, std::ostream & cout, bool verbose=false) {
      if (verbose)
        std::cerr << "Receiving client request:" << std::endl;

      std::stringstream request;
      while (true) {
        std::string line;
        std::getline (cin, line);
        if (line == "")
          break;

        if (verbose)
          std::cerr << line << std::endl;

        request << line << std::endl;
      }

      Json::Value json;
      request >> json;

      if (verbose)
        std::cerr << "Processing request... ";
      cout << "Server response:" << std::endl << std::flush;

      std::string command = json["command"].asString();
      CommandMap::const_iterator it = commands_.find (command);
      if (it != commands_.end()) {
        it->second->parseJson (json, cout);
      } else {
        cout << "Unknown command: `" << command << "'" << std::endl;
      }

      if (verbose)
        std::cerr << "done." << std::endl << std::endl;
    }

  private:
    typedef std::map<std::string, CommandParser*> CommandMap;
    CommandMap  commands_;
    std::string description_;
    std::string prompt_;
    bool        echo_;
  };

  /** @brief Helper function to create @ref KeyParserBase "key parsers"
   *
   * Create a vector key parser when the provided destination is of type @c
   * std::vector<T>. Create a scalar key parser otherwise.
   *
   * @tparam T          type of key value
   * @param name        name of the key parameter
   * @param destination reference to a variable where key values will be stored
   *
   * @return pointer to a newly allocated key parser
   *
   * @ingroup request
   */
  template <typename T>
  Key<T>* key (std::string name, T & destination) {
    return new Key<T> (name, destination);
  }
}
