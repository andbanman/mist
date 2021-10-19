#pragma once

#include <atomic>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Cache.hpp"

#include <float.h>
#define DOUBLE_UNSET DBL_MAX

namespace mist {
namespace cache {

class Flat2DException : public std::exception
{
private:
  std::string msg;

public:
  Flat2DException(std::string const& method, std::string const& msg)
    : msg("Flat2D::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

class Flat2DOutOfRange : public std::out_of_range
{
public:
  Flat2DOutOfRange(std::string const& method, std::string const& key)
    : out_of_range("Flat2D::" + method + " : key " + key + " out of range")
  {}
};

/** Fixed sized associative cache
 */
class Flat2D : public Cache
{
public:
  using key_type = K;
  using val_type = V;

  Flat2D();
  Flat2D(std::size_t nvar);
  Flat2D(std::size_t nvar, std::size_t size);
  bool has(key_type const& key);
  void put(key_type const& key, val_type const& val);
  val_type get(key_type const& key);
  std::size_t size();
  std::size_t bytes();

private:
  std::vector<val_type> data;
  std::size_t nvar;
};

} // cache
} // mist
