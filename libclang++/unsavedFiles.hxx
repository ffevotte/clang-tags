#pragma once

#include <sstream>
#include <fstream>
#include <vector>

namespace LibClang {

  /** @brief Set of in-memory buffers storing up-to-date contents for unsaved files
   *
   * This class essentially is a vector of libclang's @c CXUnsavedFile buffers.
   */
  class UnsavedFiles {
  public:
    /** @brief Store updated content for a source file
     *
     * Add an unsaved file to the list, associating it with updated contents
     * read from a temporary filesystem path (potentially a pipe or in-memory
     * filesystem).
     *
     * @param sourcePath  path to the source file
     * @param bufferPath  path to the up-to-date contents
     */
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

    /** @brief Get the size of the unsaved files set
     *
     * @return number of unsaved files
     */
    unsigned int size () const {
      return sourcePath_.size();
    }

    /** @brief Get a C-like array of unsaved files
     *
     * @return C pointer to the first unsaved file
     */
    CXUnsavedFile * begin () {
      return &(unsavedFile_[0]);
    }

  private:
    std::vector<std::string>   sourcePath_;
    std::vector<std::string>   contents_;
    std::vector<CXUnsavedFile> unsavedFile_;
  };
}
