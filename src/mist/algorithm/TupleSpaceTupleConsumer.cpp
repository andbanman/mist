#include "algorithm/TupleSpaceTupleConsumer.hpp"
#include "it/Measure.hpp"

using namespace mist;
using namespace mist::algorithm;

void TupleSpaceTupleConsumer::start() {

    TupleSpaceQueueElement element;

    while (this->queue->check_and_pop(element)) {
        auto& groupTuple = element.groupTuple;
        auto& workingTuple = element.workingTuple;
        auto size = groupTuple.size();
        int pos = size - 1;

        // find the appropriate start by tuple up to this point
        // if the same group has appeared, then must advance the variable index
        int start = 0;
        for (int ii = 0; ii < pos; ii++) {
            if (groupTuple[ii] == groupTuple[pos])
                start = std::max(start, workingTuple[ii]);
        }
        //last position in the group
        int end = tupleSpace.getVariableGroup(groupTuple[pos]).size();

        // translate working tuple into tuple of variable indexes
        TupleSpace::tuple_type tuple(size);
        for (int ii = 0; ii < pos; ii++)
            tuple[ii] = tupleSpace.getVariableGroup(groupTuple[ii])[workingTuple[ii]];

        // initial computation
        tuple[pos] = tupleSpace.getVariableGroup(groupTuple[pos])[start];
        auto result = this->measure->compute(*this->calculator, tuple);
        if (this->out) {
            if (this->output_all) {
                this->out->push(tuple, result);
            } else {
                this->out->push(tuple, it::Measure::result_type(result.end() - 1, result.end()));
            }
        }

        // completion computations
        for (int ii = start+1; ii < end; ii++) {
            tuple[pos] = tupleSpace.getVariableGroup(groupTuple[pos])[ii];
            this->measure->recomputeLastIndex(*this->calculator, tuple, result);
            if (this->out) {
                if (this->output_all) {
                    this->out->push(tuple, result);
                } else {
                    this->out->push(tuple, it::Measure::result_type(result.end() - 1, result.end()));
                }
            }
        }
    }
}
