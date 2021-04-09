#include "algorithm/TupleSpaceTupleProducer.hpp"
#include "Variable.hpp"

using namespace mist;
using namespace mist::algorithm;

// groupTuple   list of variable groups that define the tuple
// workingTuple tuple containing position of variable in corresponding group
// pos          working position in the tuple
void TupleSpaceTupleProducer::queueTuples(TupleSpace::tuple_type const& groupTuple, TupleSpace::tuple_type & workingTuple, int pos) { 
    auto size = groupTuple.size();
    if (pos >= size) {
        // done, queue it up
        if (queue) {
            // translate tuple of variable positions in group to the variable indexes themselves
            TupleSpace::tuple_type variableTuple(size);
            for (int ii = 0; ii < size; ii++)
                variableTuple[ii] = tupleSpace.getVariableGroup(groupTuple[ii])[workingTuple[ii]];
            //XXX: temp using batch for testing purpose
            std::vector<TupleSpace::tuple_type> batch;
            batch.push_back(variableTuple);
            queue->push(batch);
        }
    } else {
        // find the appropriate start by tuple up to this point
        // if the same group has appeared, then must advance the variable index
        int start = 0;
        for (int ii = 0; ii < pos; ii++) {
            if (groupTuple[ii] == groupTuple[pos])
                start = std::max(start, workingTuple[ii]);
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
        auto batchconsumer = dynamic_cast<BatchTupleConsumer*>(&consumer);
        batchconsumer->set_queue(this->queue);
    } catch (std::bad_cast &e) {
        throw TupleSpaceTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to BatchTupleConsumer");
    }
}
