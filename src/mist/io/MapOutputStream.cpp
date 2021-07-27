#include "io/MapOutputStream.hpp"
#include <stdexcept>

using namespace mist;
using namespace mist::io;

MapOutputStream::MapOutputStream()
  : OutputStream(mutex_ptr(new mutex_type)){};
MapOutputStream::~MapOutputStream(){};

void
MapOutputStream::push(std::size_t tuple_no, tuple_type const& tuple, result_type const& result)
{
  results[tuple] = result;
}
void
MapOutputStream::push(std::size_t tuple_no, tuple_type const& tuple, it::entropy_type result)
{
  throw std::runtime_error("Don't use push on single outputs, as impletemented you will thrash on memory allocations");
  //results[tuple] = { result }; //XXX malloc thrashing!
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
