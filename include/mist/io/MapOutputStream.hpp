#pragma once

#include <map>

#include "OutputStream.hpp"
#include "it/Entropy.hpp"

namespace mist {
namespace io {

class MapOutputStream : public OutputStream
{
public:
  using map_type = std::map<tuple_type, result_type>;
  MapOutputStream();
  ~MapOutputStream();

  void push(std::size_t tuple_no, tuple_type const& tuple, result_type const& result);
  void push(std::size_t tuple_no, tuple_type const& tuple, it::entropy_type result);
  void combine(MapOutputStream const& other);
  map_type const& get_results();

private:
  map_type results;
};

} // io
} // mist
