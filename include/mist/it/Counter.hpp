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
  virtual Distribution count(Variable const& var) = 0;
  virtual Distribution count(Variable::tuple const&) = 0;
  virtual Distribution count(Variable::tuple const&,
                             Variable::indexes const& indexes) = 0;
};

} // it
} // mist
