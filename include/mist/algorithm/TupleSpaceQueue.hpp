#pragma once

#include <vector>

#include "Queue.hpp"
#include "TupleSpace.hpp"

namespace mist {
namespace algorithm {

struct TupleSpaceQueueElement {
    TupleSpace::tuple_type groupTuple;
    TupleSpace::tuple_type workingTuple;
};

using TupleSpaceQueue = Queue<TupleSpaceQueueElement>;

} // algorithm
} // mist
