#pragma once

#include <fstream>

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
    return res;
  }

  std::string line (const unsigned int lineno) {
    seekg (0);
    std::string line;
    for (unsigned int i=0 ; i<lineno ; ++i) {
      std::getline (*this, line);
    }

    return line;
  }
};
