#pragma once

#include "../Variable.hpp"

#include "Distribution.hpp"

namespace mist {
namespace it {

/** Abstract class. Generates a Probability Distribution from a Variable tuple
 */
class Counter
{
public:
  virtual ~Counter(){};
  virtual void count(Variable const&, Distribution&) = 0;
  virtual void count(Variable::tuple const&, Distribution&) = 0;
  virtual void count(Variable::tuple const&,
                     Variable::indexes const&,
                     Distribution&) = 0;
};

} // it
} // mist
