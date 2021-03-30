#pragma once

#include <memory>

#include "CompletionQueue.hpp"
#include "TupleConsumer.hpp"

namespace mist {
namespace algorithm {

class CompletionTupleConsumer : public TupleConsumer {
private:
    using queue_ptr = std::shared_ptr<CompletionQueue>;
    queue_ptr queue;
    int nvar;

public:
    CompletionTupleConsumer(entropy_calc_ptr const& calculator, output_stream_ptr const& out, measure_ptr const& measure, int nvar) : TupleConsumer(calculator, out, measure), nvar(nvar) { };

    CompletionTupleConsumer(entropy_calc_ptr const& calculator, measure_ptr const& measure, int nvar) : TupleConsumer(calculator, measure), nvar(nvar) { };

    CompletionTupleConsumer(CompletionTupleConsumer const& other) : TupleConsumer(other.calculator, other.out, other.measure), nvar(other.nvar) { };

    ~CompletionTupleConsumer() { };

    void set_queue(queue_ptr const& queue) {
        this->queue = queue;
    };

    void start();
};

} // algorithm
} // mist
