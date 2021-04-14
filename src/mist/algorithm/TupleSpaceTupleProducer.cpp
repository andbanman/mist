#include "algorithm/TupleSpaceTupleProducer.hpp"
#include "Variable.hpp"

using namespace mist;
using namespace mist::algorithm;

// groupTuple   list of variable groups that define the tuple
// workingTuple tuple containing position of variable in corresponding group
// pos          working position in the tuple
void TupleSpaceTupleProducer::queueTuples(TupleSpace::tuple_type const& groupTuple, TupleSpace::tuple_type & workingTuple, int pos) {
    auto size = groupTuple.size();
    if (pos >= size - 1) {
        // done, queue it up
        if (queue) {
            TupleSpaceQueueElement element;
            element.groupTuple = groupTuple;
            element.workingTuple = workingTuple;
            queue->push(element);
        }
    } else {
        // find the appropriate start by tuple up to this point
        // if the same group has appeared, then must advance the variable index
        int start = 0;
        for (int ii = 0; ii < pos; ii++) {
            if (groupTuple[ii] == groupTuple[pos])
                start = std::max(start, workingTuple[ii] + 1);
        }
        //last position in the group
        int end = tupleSpace.getVariableGroup(groupTuple[pos]).size();
        for (int ii = start; ii < end; ii++) {
            workingTuple[pos] = ii;
            queueTuples(groupTuple, workingTuple, pos+1);
        }
    }
}

void TupleSpaceTupleProducer::start() {
    for (auto const& groupTuple : tupleSpace.getVariableGroupTuples()) {
        tuple_size = groupTuple.size();
        TupleSpace::tuple_type workingTuple(tuple_size);
        queueTuples(groupTuple, workingTuple, 0);
    }
    this->queue.get()->finish();
}

void TupleSpaceTupleProducer::registerConsumer(TupleConsumer & consumer) {
    try {
        auto tsconsumer = dynamic_cast<TupleSpaceTupleConsumer*>(&consumer);
        tsconsumer->set_queue(this->queue);
    } catch (std::bad_cast &e) {
        throw TupleSpaceTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to TupleSpaceTupleConsumer");
    }
}
