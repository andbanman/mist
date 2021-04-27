#include "algorithm/TupleSpaceTupleProducer.hpp"
#include "algorithm/BatchTupleConsumer.hpp"
#include "Variable.hpp"

using namespace mist;
using namespace mist::algorithm;

TupleSpaceTupleProducer::TupleSpaceTupleProducer(TupleSpace const& tupleSpace)
    : TupleProducer(0, algorithm::completion, 10), tupleSpace(tupleSpace) {
    this->batch_queue = batch_queue_ptr(new BatchQueue);
    this->compl_queue = compl_queue_ptr(new TupleSpaceQueue);
};

TupleSpaceTupleProducer::TupleSpaceTupleProducer(TupleSpace const& tupleSpace, algorithm alg)
    : TupleProducer(0, alg), tupleSpace(tupleSpace) {
    this->batch_queue = batch_queue_ptr(new BatchQueue);
    this->compl_queue = compl_queue_ptr(new TupleSpaceQueue);
};

TupleSpaceTupleProducer::TupleSpaceTupleProducer(TupleSpace const& tupleSpace, int batch_size)
    : TupleProducer(0, algorithm::completion, batch_size), tupleSpace(tupleSpace) {
    this->batch_queue = batch_queue_ptr(new BatchQueue);
    this->compl_queue = compl_queue_ptr(new TupleSpaceQueue);
};

// groupTuple   list of variable groups that define the tuple
// workingTuple tuple containing position of variable in corresponding group
// pos          working position in the tuple
void TupleSpaceTupleProducer::queueTuplesCompl(TupleSpace::tuple_type const& groupTuple, TupleSpace::tuple_type & workingTuple, int pos) {
    auto size = groupTuple.size();
    if (pos >= size - 1) {
        // done, queue it up
        if (compl_queue) {
            TupleSpaceQueueElement element;
            element.groupTuple = groupTuple;
            element.workingTuple = workingTuple;
            compl_queue->push(element);
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
            queueTuplesCompl(groupTuple, workingTuple, pos+1);
        }
    }
}

void TupleSpaceTupleProducer::queueTuplesBatch(TupleSpace::tuple_type const& groupTuple, TupleSpace::tuple_type & workingTuple, int pos, std::vector<TupleSpace::tuple_type> &batch) {
    auto size = groupTuple.size();
    if (pos >= size) {
        // translate working tuple into tuple of variable indexes
        if (batch_queue) {
            TupleSpace::tuple_type tuple(size);
            for (int ii = 0; ii < pos; ii++)
                tuple[ii] = tupleSpace.getVariableGroup(groupTuple[ii])[workingTuple[ii]];
            batch.push_back(tuple);
            if (batch.size() >= batch_size) {
                batch_queue->push(batch);
                batch.clear();
            }
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
            queueTuplesBatch(groupTuple, workingTuple, pos+1, batch);
        }
    }
}

void TupleSpaceTupleProducer::start() {
    std::vector<TupleSpace::tuple_type> batch;

    switch(alg) {
    case algorithm::completion:
        for (auto const& groupTuple : tupleSpace.getVariableGroupTuples()) {
            tuple_size = groupTuple.size();
            TupleSpace::tuple_type workingTuple(tuple_size);
            queueTuplesCompl(groupTuple, workingTuple, 0);
        }
        this->compl_queue.get()->finish();
        break;
    case algorithm::batch:
        for (auto const& groupTuple : tupleSpace.getVariableGroupTuples()) {
            tuple_size = groupTuple.size();
            TupleSpace::tuple_type workingTuple(tuple_size);
            queueTuplesBatch(groupTuple, workingTuple, 0, batch);
        }
        if (!batch.empty())
            batch_queue->push(batch);
        this->batch_queue.get()->finish();
        break;
    default:
        throw TupleSpaceTupleProducerException("start", "Unknown TupleProducer::algorithm type");
    }
}

void TupleSpaceTupleProducer::registerConsumer(TupleConsumer & consumer) {
    switch (this->alg) {
    case algorithm::batch:
        try {
            auto batchconsumer = dynamic_cast<BatchTupleConsumer*>(&consumer);
            if (!batchconsumer)
                throw TupleSpaceTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to BatchTupleConsumer");
            batchconsumer->set_queue(this->batch_queue);
        } catch (std::bad_cast &e) {
            throw TupleSpaceTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to BatchTupleConsumer");
        }
        break;
    case algorithm::completion:
        try {
            auto completionconsumer = dynamic_cast<TupleSpaceTupleConsumer*>(&consumer);
            if (!completionconsumer)
                throw TupleSpaceTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to BatchTupleConsumer");
            completionconsumer->set_queue(this->compl_queue);
        } catch (std::bad_cast &e) {
            throw TupleSpaceTupleProducerException("registerConsumer", "Failed to cast TupleConsumer to TupleSpaceTupleConsumer");
        }
        break;
    default:
        throw TupleSpaceTupleProducerException("registerConsumer", "Unknown TupleProducer::algorithm type");
    }
}
