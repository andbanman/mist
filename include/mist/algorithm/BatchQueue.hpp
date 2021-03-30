#pragma once

#include <vector>

#include "../Variable.hpp"
#include "Queue.hpp"

namespace mist {
namespace algorithm {

using BatchQueue = Queue<std::vector<Variable::indexes>>;

} // algorithm
} // mist
