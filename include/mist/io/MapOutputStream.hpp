#pragma once

#include <map>

#include "OutputStream.hpp"

namespace mist {
namespace io {

class MapOutputStream : public OutputStream
{
public:
  using map_type = std::map<tuple_type, result_type>;
  MapOutputStream();
  ~MapOutputStream();

  void push(tuple_type const& tuple, result_type const& result);
  void combine(MapOutputStream const& other);
  map_type const& get_results();

private:
  map_type results;
};

} // io
} // mist
