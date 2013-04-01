#pragma once

#include <fstream>
#include <sstream>

class SourceFile : public std::ifstream {
public:
  SourceFile (const std::string & fileName)
    : std::ifstream (fileName.c_str())
  { }

  std::string substring (const unsigned int offset1, const unsigned int offset2) {
    unsigned int len = offset2 - offset1;
    char * buffer = new char [len+1];

    seekg (offset1);
    read (buffer, len);
    buffer[len] = 0;

    std::string res (buffer);
    delete[] buffer;

    return shorten_ (res, 42);
  }

  std::string line (const unsigned int lineno) {
    seekg (0);
    std::string line;
    for (unsigned int i=0 ; i<lineno ; ++i) {
      std::getline (*this, line);
    }

    return line;
  }

private:
  static std::string shorten_ (const std::string & s, const unsigned int sizeMax) {
    std::istringstream iss (s);

    std::string word;
    iss >> word;
    std::string res = word;
    while (! iss.eof()) {
      iss >> word;
      res += " " + word;

      if (res.size() > sizeMax) {
        res = res.substr(0, sizeMax-3);
        res += "...";
        break;
      }
    }

    return res;
  }

};
