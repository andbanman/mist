#pragma once

#include <memory>
#include <stdexcept>

#include "TupleProducer.hpp"
#include "TupleSpace.hpp"
#include "TupleSpaceQueue.hpp"
#include "TupleSpaceTupleConsumer.hpp"

namespace mist {
namespace algorithm {

/** Produce tuples from a TupleSpace to be processed by TupleSpaceTupleConsumer
 *
 * Uses the "completion" algorithm (see CompletionTupleProducer) As such,
 * variable groups and group tuples should be written with a mind toward load
 * balancing, i.e. the last group in the tuple should be as large as possible
 * to maximize the optimization, but there should be enough partial tuples to
 * keep each thread in the work pool busy.
 */
class TupleSpaceTupleProducer : public TupleProducer {
private:
    using queue_ptr = std::shared_ptr<TupleSpaceQueue>;
    TupleSpace tupleSpace;
    queue_ptr queue;
    int batchsize;

    void queueTuples(TupleSpace::tuple_type const& groupTuple, TupleSpace::tuple_type & workingTuple, int pos);

public:
    TupleSpaceTupleProducer(TupleSpace const& tupleSpace)
        : TupleProducer(0), tupleSpace(tupleSpace), batchsize(10) {
        this->queue = queue_ptr(new TupleSpaceQueue);
    };
    TupleSpaceTupleProducer(TupleSpace const& tupleSpace, int batchsize)
        : TupleProducer(0), tupleSpace(tupleSpace), batchsize(batchsize) {
        this->queue = queue_ptr(new TupleSpaceQueue);
    };
    ~TupleSpaceTupleProducer() { };

    void registerConsumer(TupleConsumer & consumer);
    void start();

    // return tuples generated according to TupleSpace
    // for large TupleSpaces this may overflow
    std::vector<Variable::indexes> getTuples();
    // return first N tuples generated according to TupleSpace
    std::vector<Variable::indexes> getTuples(int n);
    // return number of tuples generated according to TupleSpace
    int countTuples();
};

class TupleSpaceTupleProducerException : public std::exception {
private:
    std::string msg;
public:
    TupleSpaceTupleProducerException(std::string const& method, std::string const& msg) :
        msg("TupleSpaceTupleProducer::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // algorithm
} // mist
