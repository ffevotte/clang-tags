#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <list>
#include <vector>

namespace Request {
  class KeyParserBase {
  public:
    KeyParserBase (std::string name)
      : name_ (name)
    {}

    virtual ~KeyParserBase () {}

    const std::string & name () const {
      return name_;
    }

    std::string display () const {
      std::ostringstream oss;

      std::string keyDesc = name_;
      if (metavar_ != "")
        keyDesc += " " + metavar_;

      oss << std::setw(20) << std::left << keyDesc;

      if (description_ != "")
        oss << " " << description_;

      if (default_ != "")
        oss << " (default: " << default_ << ")";

      return oss.str();
    }

    KeyParserBase * metavar (std::string metavar) {
      metavar_ = metavar;
      return this;
    }

    KeyParserBase * description (std::string description) {
      description_ = description;
      return this;
    }

    virtual void parse (std::istream & stream) = 0;

  private:
    const std::string name_;
    std::string description_;
    std::string metavar_;

  protected:
    std::string default_;
  };

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

    virtual void parse (std::istream & stream) {
      stream >> std::boolalpha >> destination_;
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
      std::ostringstream defaultStr;
      defaultStr << "[";
      auto it = destination.begin();
      auto end = destination.end();
      for ( ; it != end ; ++it) {
        defaultStr << " " << *it;
      }
      defaultStr << " ]";
      default_ = defaultStr.str();
    }

    virtual void parse (std::istream & stream) {
      T val;
      stream >> std::boolalpha >> val;
      destination_.push_back (val);
    }

  private:
    Vector & destination_;
  };

  class CommandParser {
  public:
    typedef std::list<KeyParserBase*> KeyList;

    CommandParser (std::string name, std::string description = "")
      : name_ (name),
        description_ (description)
    { }

    virtual ~CommandParser () {
      auto it = keys_.begin();
      auto end = keys_.end();
      for ( ; it != end ; ++it) {
        delete (*it);
      }
    }

    virtual void run () = 0;
    virtual void defaults () {}

    const std::string & name () {
      return name_;
    }

    void help () const {
      std::cerr << description_ << std::endl;

      std::cerr << "Keys:" << std::endl;
      auto it = keys_.begin();
      auto end = keys_.end();
      for ( ; it != end ; ++it) {
        std::cerr << "  " << (*it)->display() << std::endl;
      }
    }

    std::string display () {
      std::ostringstream oss;
      oss << std::setw(20) << std::left << name_
          << " " << description_;
      return oss.str();
    }

    void parse (std::istream & stream) {
      defaults ();
      do {
        std::cout << prompt_ << std::flush;
        std::string line;
        std::getline (stream, line);

        if (line == "") {
          break;
        }

        std::istringstream input (line);
        std::string key;
        input >> key;

        if (key == "help") {
          help();
          continue;
        }

        auto it = keys_.begin();
        auto end = keys_.end();
        bool found = false;
        for ( ; it != end ; ++it) {
          KeyParserBase & keyParser = **it;
          if (key == keyParser.name()) {
            keyParser.parse (input);
            found = true;
            break;
          }
        }

        if (! found)
          std::cerr << "Unknown key: `" << key << "'" << std::endl;

      } while (! stream.eof());
      run ();
    }

    CommandParser & add (KeyParserBase * key) {
      keys_.push_back (key);
      return *this;
    }

  private:
    const std::string name_;
    KeyList keys_;
    std::string description_;

  protected:
    std::string prompt_;
  };

  class Parser {
  public:
    typedef std::list<CommandParser*> CommandList;

    Parser (std::string description = "")
      : description_ (description)
    { }

    ~Parser () {
      auto it = commands_.begin();
      auto end = commands_.end();
      for ( ; it != end ; ++it) {
        delete *it;
      }
    }

    Parser & prompt (std::string p) {
      prompt_ = p;
      return *this;
    }

    Parser & add (CommandParser * command) {
      commands_.push_back (command);
      return *this;
    }

    void help () {
      std::cerr << description_ << std::endl;

      std::cerr << "Commands:" << std::endl;
      auto it = commands_.begin();
      auto end = commands_.end();
      for ( ; it != end ; ++it) {
        std::cerr << "  " << (*it)->display() << std::endl;
      }
    }

    void parse (std::istream & stream) {
      do {
        std::cout << prompt_ << std::flush;

        std::string line;
        std::getline (stream, line);

        if (line == "") {
          break;
        }

        std::istringstream input (line);
        std::string command;
        input >> command;

        if (command == "help") {
          input >> command;
          help();
          continue;
        }

        auto it = commands_.begin();
        auto end = commands_.end();
        bool found = false;
        for ( ; it != end ; ++it) {
          CommandParser & commandParser = **it;
          if (command == commandParser.name()) {
            commandParser.parse (stream);
            found = true;
            continue;
          }
        }

        if (! found) {
          std::cerr << "Unknown command: `" << command << "'" << std::endl;
        }

      } while (! stream.eof());
    }

  private:
    CommandList commands_;
    std::string description_;
    std::string prompt_;
  };

  template <typename T>
  Key<T>* key (std::string name, T & destination) {
    return new Key<T> (name, destination);
  }
}
