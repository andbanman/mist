#pragma once

#include <memory>
#include <stdexcept>

#include "BatchQueue.hpp"
#include "TupleProducer.hpp"
#include "TupleSpace.hpp"
#include "TupleSpaceQueue.hpp"
#include "TupleSpaceTupleConsumer.hpp"

namespace mist {
namespace algorithm {

/** Produce tuples from a TupleSpace to be processed by TupleSpaceTupleConsumer
 */
class TupleSpaceTupleProducer : public TupleProducer {
private:
    using compl_queue_ptr = std::shared_ptr<TupleSpaceQueue>;
    using batch_queue_ptr = std::shared_ptr<BatchQueue>;
    TupleSpace tupleSpace;
    compl_queue_ptr compl_queue;
    batch_queue_ptr batch_queue;
    void queueTuplesCompl(TupleSpace::tuple_type const& groupTuple, TupleSpace::tuple_type & workingTuple, int pos);
    void queueTuplesBatch(TupleSpace::tuple_type const& groupTuple, TupleSpace::tuple_type & workingTuple, int pos, std::vector<TupleSpace::tuple_type> &batch);

public:
    TupleSpaceTupleProducer(TupleSpace const& tupleSpace);
    TupleSpaceTupleProducer(TupleSpace const& tupleSpace, algorithm alg);
    TupleSpaceTupleProducer(TupleSpace const& tupleSpace, int batchsize);
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
