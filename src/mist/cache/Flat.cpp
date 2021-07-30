#include "cache/Flat.hpp"

using namespace mist;
using namespace mist::cache;

std::size_t
Flat::get_index(key_type const& indexes)
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

Flat::Flat()
{
};

Flat::Flat(std::size_t nvar, std::size_t dimension)
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

Flat::Flat(std::size_t nvar, std::size_t dimension, std::size_t maxmem)
{
  std::size_t stride = 1;
  for (int ii = 0; ii < dimension; ii++) {
    this->strides.push_back(stride);
    stride *= nvar;
  }
  unsigned long maxsize = maxmem / sizeof(val_type);
  unsigned long size = (stride > maxsize) ? maxsize : stride;
  this->data.resize(size);
  // TODO hardcoded for DOUBLE types, but should use template
  this->data.assign(size, DOUBLE_UNSET);
};

// TODO template for value types
bool
Flat::has(key_type const& key)
{
  return (data[get_index(key)] != DOUBLE_UNSET);
}

void
Flat::put(key_type const& key, val_type const& val)
{
  auto index = get_index(key);
  this->data[index] = val;
}

Flat::val_type
Flat::get(key_type const& key)
{
  auto index = get_index(key);
  if (this->data[index] != DOUBLE_UNSET) {
    this->_hits++;
    // shared pointer that doesn't own the object
    return this->data[index];
  } else {
    this->_misses++;
    throw FlatOutOfRange("get", this->key_to_string(key));
  }
}

std::size_t
Flat::size()
{
  return data.size();
}

std::size_t
Flat::bytes()
{
  return data.size() * sizeof(val_type);
}
