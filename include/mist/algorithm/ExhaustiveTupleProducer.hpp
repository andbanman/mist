#pragma once

#include <memory>

#include "BatchQueue.hpp"
#include "ExhaustiveQueue.hpp"
#include "ExhaustiveTupleConsumer.hpp"
#include "TupleProducer.hpp"

namespace mist {
namespace algorithm {

/** Produce an exhaustive set of tuples from a set of variables to be processed
 * by ExhaustiveTupleConsumer
 *
 */
class ExhaustiveTupleProducer : public TupleProducer {
private:
    using compl_queue_ptr = std::shared_ptr<ExhaustiveQueue>;
    using batch_queue_ptr = std::shared_ptr<BatchQueue>;
    compl_queue_ptr compl_queue;
    batch_queue_ptr batch_queue;
    int nvar;
    // TODO hide these
    void start_batch_d1();
    void start_batch_d2();
    void start_batch_d3();
    void start_compl_d2();
    void start_compl_d3();
public:
    ExhaustiveTupleProducer(int tuple_size, int nvar, algorithm alg);
    ExhaustiveTupleProducer(int tuple_size, int nvar);
    ~ExhaustiveTupleProducer() { };
    void registerConsumer(TupleConsumer & consumer);
    void start();
};

class ExhaustiveTupleProducerException : public std::exception {
private:
    std::string msg;
public:
    ExhaustiveTupleProducerException(std::string const& method, std::string const& msg) :
        msg("ExhaustiveTupleProducer::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // algorithm
} // mist
