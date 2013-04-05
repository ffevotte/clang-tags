#pragma once

#include <sstream>
#include <fstream>
#include <vector>

namespace LibClang {

  class UnsavedFiles {
  public:
    void add (const std::string sourcePath, const std::string bufferPath)
    {
      const int bufsize = 4096;
      char *buf = new char[bufsize];

      std::ostringstream contents;
      std::ifstream buffer (bufferPath);
      do {
        buffer.get (buf, bufsize, 0);
        contents << buf;
      } while (! buffer.eof());

      delete[] buf;

      sourcePath_.push_back (sourcePath);
      contents_.push_back (contents.str());
      unsavedFile_.resize (sourcePath_.size());

      CXUnsavedFile & unsavedFile = unsavedFile_.back();
      unsavedFile.Filename = sourcePath_.back().c_str();
      unsavedFile.Contents = contents_.back().c_str();
      unsavedFile.Length   = contents_.back().size();
    }

    unsigned int size () const {
      return sourcePath_.size();
    }

    CXUnsavedFile * begin () {
      return &(unsavedFile_[0]);
    }

  private:
    std::vector<std::string>   sourcePath_;
    std::vector<std::string>   contents_;
    std::vector<CXUnsavedFile> unsavedFile_;
  };
}
