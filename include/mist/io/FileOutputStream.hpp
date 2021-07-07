#pragma once

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

#include <stdio.h>

#include "OutputStream.hpp"

#define BUFFER_MAX_SIZE_DEFAULT 1024

// the maximum string length of a double to string conversion
#define DOUBLE_BUFFER_MAX_SIZE 64

namespace mist {
namespace io {

class FileOutputStream : public OutputStream
{
public:
  using buffer_type = std::vector<char>;
  using buffer_ptr = std::unique_ptr<buffer_type>;
  using file_type = std::ofstream;
  using file_ptr = std::shared_ptr<file_type>;
  using size_type = std::size_t;

private:
  file_ptr file;
  buffer_ptr buffer;
  buffer_type double_strbuf;
  size_type buffer_max_size;
  size_type buffer_cur_size;
  std::string filename;
  std::string header;

  void direct_write(std::string const& ss);
  void buffered_write(std::string const& ss);
  void init();

public:
  FileOutputStream(std::string const& filename);
  FileOutputStream(std::string const& filename, size_type buffer_max_size);
  FileOutputStream(FileOutputStream const& other);
  FileOutputStream(std::string const& filename, std::string const& header);
  FileOutputStream(std::string const& filename,
                   std::string const& header,
                   size_type buffer_max_size);
  FileOutputStream(FileOutputStream const& other, std::string const& header);
  ~FileOutputStream();

  void push(tuple_type const& tuple, result_type const& result);
  std::string get_filename();
};

class FileOutputStreamException : public std::exception
{
private:
  std::string msg;

public:
  FileOutputStreamException(std::string const& method, std::string const& msg)
    : msg("FileOutputStream::" + method + ": " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};
} // io
} // mist
