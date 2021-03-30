#pragma once

#include "TupleConsumer.hpp"

namespace mist {
namespace algorithm {

class TupleProducer {
protected:
    int tuple_size;
    TupleProducer(int tuple_size) : tuple_size(tuple_size) { };
public:
    virtual ~TupleProducer() { };
    virtual void start() = 0;
    virtual void registerConsumer(TupleConsumer &consumer) = 0;
};

} // algorithm
} // mist
