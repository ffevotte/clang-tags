#pragma once

#include <iostream>
#include <sstream>
#include <vector>

namespace Request {
  template <typename REQUEST_READER, typename KEY_READER>
  class Composite;

  template <typename VAL>
  class KeyReader;

  struct Input {
    Input (std::istream & stream)
      : stream (stream)
    { }

    std::istream & stream;
  };

  template <typename REQUEST_READER>
  struct RequestReader {
    REQUEST_READER & getVR () {
      return static_cast<REQUEST_READER&> (*this);
    }

    REQUEST_READER & operator>> (Input input) {
      do {
        std::string inputLine;
        std::getline (input.stream, inputLine);

        if (inputLine == "") {
          break;
        }

        std::istringstream line (inputLine);
        std::string key;
        line >> key;

        getVR().set (key, line);
      } while (!input.stream.eof());

      return getVR();
    }

    template <typename VAL>
    Composite<REQUEST_READER, KeyReader<VAL> > operator>> (KeyReader<VAL> kr) {
      return Composite<REQUEST_READER, KeyReader<VAL> > (getVR(), kr);
    }
  };


  struct Empty : RequestReader<Empty> {
    void set (std::string key, std::istream & stream) {
      std::cerr << "Error: unknown key '" << key << "'" << std::endl;
    }
  };

  template <typename REQUEST_READER, typename KEY_READER>
  class Composite
    : public RequestReader<Composite<REQUEST_READER, KEY_READER> >
  {
  public:
    Composite (REQUEST_READER rr, KEY_READER kr)
      : rr_ (rr), kr_ (kr)
    { }

    virtual void set (std::string key, std::istream & stream) {
      if (key == kr_.key()) {
        kr_.read(stream);
      } else {
        rr_.set (key, stream);
      }
    }

  private:
    REQUEST_READER rr_;
    KEY_READER     kr_;
  };


  template <typename VAL>
  class KeyReaderBase {
  public:
    KeyReaderBase (const std::string & key, VAL & val)
      : key_ (key),
        val_ (val)
    { }

    std::string & key () { return key_; }

  protected:
    std::string key_;
    VAL & val_;
  };

  template <typename VAL>
  struct KeyReader : public KeyReaderBase<VAL>
  {
    KeyReader (const std::string & key, VAL & val)
      : KeyReaderBase<VAL> (key, val)
    { }

    void read (std::istream & stream) {
      stream >> this->val_;
    }
  };

  template <>
  struct KeyReader<std::vector<std::string> >
    : public KeyReaderBase<std::vector<std::string> >
  {
    typedef std::vector<std::string>  VAL;

    KeyReader (const std::string & key, VAL & val)
      : KeyReaderBase<VAL> (key, val)
    { }

    void read (std::istream & stream) {
      std::string item;
      stream >> item;
      val_.push_back (item);
    }
  };



  inline Empty reader () { return Empty(); }

  template <typename VAL>
  KeyReader<VAL> key (const std::string & key, VAL & val) {
    return KeyReader<VAL> (key, val);
  }

  inline Input read (std::istream & stream) {
    return Input (stream);
  }
}
