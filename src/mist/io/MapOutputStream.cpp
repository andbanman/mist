#include "io/MapOutputStream.hpp"

using namespace mist;
using namespace mist::io;

MapOutputStream::MapOutputStream()
  : OutputStream(mutex_ptr(new mutex_type)){};
MapOutputStream::~MapOutputStream(){};

void
MapOutputStream::push(tuple_type const& tuple, result_type const& result)
{
  results[tuple] = result;
}

MapOutputStream::map_type const&
MapOutputStream::get_results()
{
  return results;
}

void
MapOutputStream::combine(MapOutputStream const& other)
{
  this->results.insert(other.results.begin(), other.results.end());
}
