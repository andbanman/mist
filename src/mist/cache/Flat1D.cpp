#include "cache/Flat1D.hpp"

using namespace mist;
using namespace mist::cache;

Flat1D::Flat1D()
{
};

Flat1D::Flat1D(std::size_t nvar)
{
  this->data.resize(nvar);
  this->data.assign(nvar, DOUBLE_UNSET);
};

// TODO template for value types
bool
Flat1D::has(key_type const& key)
{
  return (data[key[0]] != DOUBLE_UNSET);
}

void
Flat1D::put(key_type const& key, val_type const& val)
{
  this->data[key[0]] = val;
}

Flat1D::val_type
Flat1D::get(key_type const& key)
{
  auto index = key[0];
  if (this->data[index] != DOUBLE_UNSET) {
    this->_hits++;
    return this->data[index];
  } else {
    this->_misses++;
    throw Flat1DOutOfRange("get", this->key_to_string(key));
  }
}

std::size_t
Flat1D::size()
{
  return data.size();
}

std::size_t
Flat1D::bytes()
{
  return data.size() * sizeof(val_type);
}
