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

class FlatException : public std::exception
{
private:
  std::string msg;

public:
  FlatException(std::string const& method, std::string const& msg)
    : msg("Flat::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

class FlatOutOfRange : public std::out_of_range
{
public:
  FlatOutOfRange(std::string const& method, std::string const& key)
    : out_of_range("Flat::" + method + " : key " + key + " out of range")
  {}
};

/** Fixed sized associative cache
 */
class Flat : public Cache
{
public:
  using key_type = K;
  using val_type = it::entropy_type;

  Flat();
  Flat(std::size_t nvar, std::size_t dimension);
  Flat(std::size_t nvar, std::size_t dimension, std::size_t maxmem);
  bool has(key_type const& key);
  void put(key_type const& key, val_type const& val);
  val_type get(key_type const& key);
  std::size_t size();
  std::size_t bytes();

private:
  std::size_t get_index(key_type const& indexes);
  std::vector<std::size_t> strides;
  std::vector<val_type> data;
};

} // cache
} // mist
