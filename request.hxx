#include <json/json.h>

#include <sstream>
#include <iomanip>
#include <map>
#include <vector>

namespace Request {
  class KeyParserBase {
  public:
    KeyParserBase (std::string name)
      : name_ (name),
        required_ (false)
    {}

    virtual ~KeyParserBase () {}

    const std::string & name () const {
      return name_;
    }

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

    KeyParserBase * metavar (std::string metavar) {
      metavar_ = metavar;
      return this;
    }

    KeyParserBase * description (std::string description) {
      description_ = description;
      return this;
    }

    KeyParserBase * required (bool req = true) {
      required_ = req;
      return this;
    }

    virtual bool isVector () const {
      return false;
    }

    virtual void parse (std::istream & cin) = 0;

    virtual void set (const Json::Value & json) = 0;

  private:
    const std::string name_;
    std::string description_;
    std::string metavar_;
    bool required_;

  protected:
    std::string default_;
  };

  template <typename T>
  void setValue (const Json::Value & json, T & destination) {
    std::istringstream iss (json.asString());
    iss >> destination;
  }

  template <>
  void setValue<bool> (const Json::Value & json, bool & destination);

  template <>
  void setValue<std::string> (const Json::Value & json,
                              std::string & destination);


  template <typename T>
  class Key : public KeyParserBase {
  public:
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

  template <typename T>
  class Key<std::vector<T> > : public KeyParserBase {
  public:
    typedef std::vector<T>  Vector;

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

  class CommandParser {
  public:
    typedef std::map<std::string, KeyParserBase*> KeyMap;

    CommandParser (std::string name, std::string description = "")
      : name_ (name),
        description_ (description)
    { }

    virtual ~CommandParser () {
      auto it = keys_.begin();
      auto end = keys_.end();
      for ( ; it != end ; ++it) {
        delete (it->second);
      }
    }

    virtual void run (std::ostream & cout) = 0;
    virtual void defaults () {}

    const std::string & name () {
      return name_;
    }

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

    void display (std::ostream & cout) {
      cout << std::setw(20) << std::left << name_
           << " " << description_ << std::endl;
    }

    void parse (std::istream & cin, std::ostream & cout) {
      defaults();
      do {
        cout << prompt_ << std::flush;
        std::string line;
        std::getline (cin, line);

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

    CommandParser & add (KeyParserBase * key) {
      keys_[key->name()] = (key);
      return *this;
    }

  private:
    const std::string name_;
    KeyMap keys_;

  protected:
    std::string prompt_;
    std::string description_;
  };

  class Parser {
  public:
    typedef std::map<std::string, CommandParser*> CommandMap;

    Parser (std::string description = "")
      : description_ (description)
    { }

    ~Parser () {
      auto it = commands_.begin();
      auto end = commands_.end();
      for ( ; it != end ; ++it) {
        delete it->second;
      }
    }

    Parser & prompt (std::string p) {
      prompt_ = p;
      return *this;
    }

    Parser & add (CommandParser * command) {
      commands_[command->name()] = command;
      return *this;
    }

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

    void parse (std::istream & cin, std::ostream & cout) {
      do {
        cout << prompt_ << std::flush;

        std::string line;
        std::getline (cin, line);

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
            it->second->parse (cin, cout);
        } else {
          cout << "Unknown command: `" << command << "'" << std::endl;
        }

      } while (! cin.eof());
    }

    void parseJson (const Json::Value & request, std::ostream & cout) {
      std::string command = request["command"].asString();
      CommandMap::const_iterator it = commands_.find (command);
      if (it != commands_.end()) {
        it->second->parseJson (request, cout);
      } else {
        cout << "Unknown command: `" << command << "'" << std::endl;
      }
    }

  private:
    CommandMap  commands_;
    std::string description_;
    std::string prompt_;
  };

  template <typename T>
  Key<T>* key (std::string name, T & destination) {
    return new Key<T> (name, destination);
  }
}
