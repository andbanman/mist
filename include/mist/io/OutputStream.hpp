#pragma once

#include <fstream>
#include <memory>
#include <mutex>

#include "../Variable.hpp"
#include "../it/Measure.hpp"
#include "it/Entropy.hpp"

namespace mist {
namespace io {

class OutputStream
{
protected:
  using result_type = it::Measure::result_type;
  using data_t = it::Measure::data_t;
  using mutex_type = std::mutex;
  using mutex_ptr = std::shared_ptr<mutex_type>;
  using tuple_type = Variable::indexes;

  mutex_ptr m;
  OutputStream(std::shared_ptr<std::mutex> const& m)
    : m(m){};

public:
  virtual ~OutputStream(){};
  virtual void push(std::size_t tuple_no, tuple_type const& tuple, result_type const& result) = 0;
  virtual void push(std::size_t tuple_no, tuple_type const& tuple, it::entropy_type result) = 0;
  // virtual void push(tuple_type const& tuple, measure_type) = 0;
};

} // io
} // mist
