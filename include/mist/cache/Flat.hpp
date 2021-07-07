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

/** Fixed sized associative cache with least recently added eviction.
 */
template<class V>
class Flat : public Cache<V>
{
public:
  using key_type = K;
  using val_type = V;

  Flat(){};

  Flat(std::size_t nvar, std::size_t dimension)
  {
    std::size_t stride = 1;
    for (int ii = 0; ii < dimension; ii++) {
      this->strides.push_back(stride);
      stride *= nvar;
    }
    this->data.resize(stride);
    // TODO hardcoded for DOUBLE types, but should use template
    this->data.assign(stride, DOUBLE_UNSET);
  };

  // TODO template for value types
  bool has(key_type const& key)
  {
    return (data[get_index(key)] != DOUBLE_UNSET);
  }

  std::pair<key_type, val_type> put(key_type const& key, val_type const& val)
  {
    auto index = get_index(key);
    auto oldval = this->data[index];
    this->data[index] = val;
    return std::make_pair(key, oldval);
  }

  std::shared_ptr<V> get(key_type const& key)
  {
    auto index = get_index(key);
    if (this->data[get_index(key)] != DOUBLE_UNSET) {
      this->_hits++;
      // shared pointer that doesn't own the object
      return std::shared_ptr<V>(std::shared_ptr<V>(),
                                &this->data[get_index(key)]);
    } else {
      this->_misses++;
      throw FlatOutOfRange("get", this->key_to_string(key));
    }
  }

  std::size_t size() { return data.size(); }

  std::size_t bytes() { return data.size() * sizeof(val_type); }

private:
  std::size_t get_index(key_type const& indexes)
  {
    auto dimension = indexes.size();
    if (dimension > strides.size())
      throw FlatException("get_index",
                          "Invalid tuple dimension for Flat array");

    auto index = indexes[0];
    for (int ii = 1; ii < dimension; ii++)
      index += indexes[ii] * this->strides[ii];

    return index;
  }

  std::vector<std::size_t> strides;
  std::vector<val_type> data;
};

} // cache
} // mist
