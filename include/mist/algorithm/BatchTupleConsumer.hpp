#pragma once

#include <memory>

#include "TupleConsumer.hpp"
#include "BatchQueue.hpp"

namespace mist {
namespace algorithm {

class BatchTupleConsumer : public TupleConsumer {
private:
    using queue_ptr = std::shared_ptr<BatchQueue>;
    queue_ptr queue;

public:
    BatchTupleConsumer(entropy_calc_ptr const& calculator, output_stream_ptr const& out, measure_ptr const& measure)
        : TupleConsumer(calculator, out, measure) { };

    BatchTupleConsumer(entropy_calc_ptr const& calculator, measure_ptr const& measure)
        : TupleConsumer(calculator, measure) { };

    BatchTupleConsumer(BatchTupleConsumer const& other) : TupleConsumer(other.calculator, other.out, other.measure) { };

    BatchTupleConsumer(entropy_calc_ptr const& calculator, output_stream_ptr const& out, measure_ptr const& measure, bool output_all)
        : TupleConsumer(calculator, out, measure, output_all) { };

    BatchTupleConsumer(entropy_calc_ptr const& calculator, measure_ptr const& measure, bool output_all)
        : TupleConsumer(calculator, measure, output_all) { };

    BatchTupleConsumer(BatchTupleConsumer const& other, bool output_all) : TupleConsumer(other.calculator, other.out, other.measure, output_all) { };

    ~BatchTupleConsumer() { };

    void set_queue(queue_ptr const& queue) {
        this->queue = queue;
    }

    void start();
};

} // algorithm
} // mist
