#pragma once

#include <memory>

#include "ExhaustiveQueue.hpp"
#include "TupleConsumer.hpp"

namespace mist {
namespace algorithm {

class ExhaustiveTupleConsumer : public TupleConsumer {
private:
    using queue_ptr = std::shared_ptr<ExhaustiveQueue>;
    queue_ptr queue;
    int nvar;

public:
    ExhaustiveTupleConsumer(entropy_calc_ptr const& calculator, output_stream_ptr const& out, measure_ptr const& measure, int nvar) : TupleConsumer(calculator, out, measure), nvar(nvar) { };

    ExhaustiveTupleConsumer(entropy_calc_ptr const& calculator, measure_ptr const& measure, int nvar) : TupleConsumer(calculator, measure), nvar(nvar) { };

    ExhaustiveTupleConsumer(ExhaustiveTupleConsumer const& other) : TupleConsumer(other.calculator, other.out, other.measure), nvar(other.nvar) { };

    ~ExhaustiveTupleConsumer() { };

    void set_queue(queue_ptr const& queue) {
        this->queue = queue;
    };

    void start();
};

} // algorithm
} // mist
