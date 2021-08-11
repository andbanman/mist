
#include "binomial.hpp"
#include "cache/Flat2D.hpp"

using namespace mist;
using namespace mist::cache;

static inline std::size_t
index(std::size_t n, std::size_t i, std::size_t j)
{
  return ((n*(n-1))/2) - ((n-i)*((n-i)-1))/2 + j - i - 1;
}

Flat2D::Flat2D()
{
}

Flat2D::Flat2D(std::size_t nvar)
{
  this->data.resize(binomial(nvar, 2));
  this->data.assign(nvar, DOUBLE_UNSET);
}

bool
Flat2D::has(key_type const& key)
{
  return (data[index(data.size(), key[0], key[1])] != DOUBLE_UNSET);
}

void
Flat2D::put(key_type const& key, val_type const& val)
{
  this->data[index(data.size(), key[0], key[1])] = val;
}

Flat2D::val_type
Flat2D::get(key_type const& key)
{
  auto ii = index(data.size(), key[0], key[1]);
  if (this->data[ii] != DOUBLE_UNSET) {
    this->_hits++;
    return this->data[ii];
  } else {
    this->_misses++;
    throw Flat2DOutOfRange("get", this->key_to_string(key));
  }
}

std::size_t
Flat2D::size()
{
  return data.size();
}

std::size_t
Flat2D::bytes()
{
  return data.size() * sizeof(val_type);
}
