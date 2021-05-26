#pragma once

#include <memory>

#include "TupleConsumer.hpp"
#include "TupleSpaceQueue.hpp"

namespace mist {
namespace algorithm {

class TupleSpaceTupleConsumer : public TupleConsumer {
private:
    using queue_ptr = std::shared_ptr<TupleSpaceQueue>;
    queue_ptr queue;
    TupleSpace tupleSpace;

public:
    TupleSpaceTupleConsumer(entropy_calc_ptr const& calculator, output_stream_ptr const& out, measure_ptr const& measure, TupleSpace const& tupleSpace) : TupleConsumer(calculator, out, measure), tupleSpace(tupleSpace) { };

    TupleSpaceTupleConsumer(entropy_calc_ptr const& calculator, measure_ptr const& measure, TupleSpace const& tupleSpace) : TupleConsumer(calculator, measure), tupleSpace(tupleSpace) { };

    TupleSpaceTupleConsumer(TupleSpaceTupleConsumer const& other) : TupleConsumer(other.calculator, other.out, other.measure), tupleSpace(other.tupleSpace) { };

    TupleSpaceTupleConsumer(entropy_calc_ptr const& calculator, output_stream_ptr const& out, measure_ptr const& measure, TupleSpace const& tupleSpace, bool output_all) : TupleConsumer(calculator, out, measure, output_all), tupleSpace(tupleSpace) { };

    TupleSpaceTupleConsumer(entropy_calc_ptr const& calculator, measure_ptr const& measure, TupleSpace const& tupleSpace, bool output_all) : TupleConsumer(calculator, measure, output_all), tupleSpace(tupleSpace) { };

    TupleSpaceTupleConsumer(TupleSpaceTupleConsumer const& other, bool output_all) : TupleConsumer(other.calculator, other.out, other.measure, output_all), tupleSpace(other.tupleSpace) { };

    ~TupleSpaceTupleConsumer() { };

    void set_queue(queue_ptr const& queue) {
        this->queue = queue;
    };

    void start();
};

} // algorithm
} // mist
