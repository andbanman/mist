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

class Flat1DException : public std::exception
{
private:
  std::string msg;

public:
  Flat1DException(std::string const& method, std::string const& msg)
    : msg("Flat1D::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

class Flat1DOutOfRange : public std::out_of_range
{
public:
  Flat1DOutOfRange(std::string const& method, std::string const& key)
    : out_of_range("Flat1D::" + method + " : key " + key + " out of range")
  {}
};

/** Fixed sized associative cache
 */
class Flat1D : public Cache
{
public:
  using key_type = K;
  using val_type = V;

  Flat1D();
  Flat1D(std::size_t nvar);
  Flat1D(std::size_t nvar, std::size_t size);
  bool has(key_type const& key);
  void put(key_type const& key, val_type const& val);
  val_type get(key_type const& key);
  std::size_t size();
  std::size_t bytes();

private:
  std::vector<val_type> data;
};

} // cache
} // mist
