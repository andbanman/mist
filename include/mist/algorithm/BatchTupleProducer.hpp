#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "TupleProducer.hpp"
#include "BatchQueue.hpp"
#include "BatchTupleConsumer.hpp"

namespace mist {
namespace algorithm {

class BatchTupleProducer : public TupleProducer {
private:
    using queue_ptr = std::shared_ptr<BatchQueue>;
    queue_ptr queue;
    int nvar;
    int batchsize;
    void start_d1();
    void start_d2();
    void start_d3();

public:
    BatchTupleProducer(int tuple_size, int nvar, int batchsize)
        : TupleProducer(tuple_size), nvar(nvar), batchsize(batchsize) {
        this->queue = queue_ptr(new BatchQueue);
    };
    BatchTupleProducer(int tuple_size, int nvar)
        : TupleProducer(tuple_size), nvar(nvar), batchsize(10) {
        this->queue = queue_ptr(new BatchQueue);
    };
    ~BatchTupleProducer() { };

    void registerConsumer(TupleConsumer & consumer);
    void start();
};

class BatchTupleProducerException : public std::exception {
private:
    std::string msg;
public:
    BatchTupleProducerException(std::string const& method, std::string const& msg) :
        msg("BatchTupleProducer::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // algorithm
} // mist
