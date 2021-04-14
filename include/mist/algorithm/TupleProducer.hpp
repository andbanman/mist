#pragma once

#include "TupleConsumer.hpp"

namespace mist {
namespace algorithm {

class TupleProducer {
public:
    /** Set the working-sharing algorithm between TupleProducer and consumers.
     *
     * - Batch : Allows configurable payload sizes between threads. Provided
     *   for completion and experimentation.
     * - Completion (default) : Reduces the amount of repeated work and is the
     *   fastest in most use cases. The "completion" optimization creates
     *   partial tuples to be completed by the consumer. This allows the
     *   subcalculations of one tuple to be shared by the next tuple.
     */
    enum struct algorithm : int {
        batch,
        completion,
        size
    };
protected:
    int tuple_size;
    int batch_size;
    algorithm alg;
    TupleProducer(int tuple_size, algorithm alg) : tuple_size(tuple_size), batch_size(10), alg(alg) { };
    TupleProducer(int tuple_size, algorithm alg, int batch_size) : tuple_size(tuple_size), batch_size(batch_size), alg(alg) { };
    TupleProducer(int tuple_size, int batch_size) : tuple_size(tuple_size), batch_size(batch_size), alg(algorithm::batch) { };
public:
    virtual ~TupleProducer() { };
    virtual void start() = 0;
    virtual void registerConsumer(TupleConsumer &consumer) = 0;
};

} // algorithm
} // mist
