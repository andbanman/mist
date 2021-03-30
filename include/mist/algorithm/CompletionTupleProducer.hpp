#pragma once

#include <memory>

#include "CompletionQueue.hpp"
#include "CompletionTupleConsumer.hpp"
#include "TupleProducer.hpp"

namespace mist {
namespace algorithm {

class CompletionTupleProducer : public TupleProducer {
private:
    using queue_ptr = std::shared_ptr<CompletionQueue>;
    queue_ptr queue;
    int nvar;
    void start_d2();
    void start_d3();
public:
    CompletionTupleProducer(int tuple_size, int nvar)
        : TupleProducer(tuple_size), nvar(nvar) {
        this->queue = queue_ptr(new CompletionQueue);
    };
    ~CompletionTupleProducer() { };

    void registerConsumer(TupleConsumer & consumer);
    void start();
};

class CompletionTupleProducerException : public std::exception {
private:
    std::string msg;
public:
    CompletionTupleProducerException(std::string const& method, std::string const& msg) :
        msg("CompletionTupleProducer::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // algorithm
} // mist
