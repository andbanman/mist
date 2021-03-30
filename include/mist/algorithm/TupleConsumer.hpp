#pragma once

#include <memory>

#include "../io/OutputStream.hpp"
#include "../it/EntropyCalculator.hpp"
#include "../it/Measure.hpp"

namespace mist {
namespace algorithm {

class TupleConsumer {
public:
    using entropy_calc_ptr  = std::shared_ptr<it::EntropyCalculator>;
    using output_stream_ptr = std::shared_ptr<io::OutputStream>;
    using measure_ptr       = std::shared_ptr<it::Measure>;

    virtual ~TupleConsumer() { };
    virtual void start() = 0;

protected:
    entropy_calc_ptr calculator;
    output_stream_ptr out;
    measure_ptr measure;
    bool output_all;

    TupleConsumer(entropy_calc_ptr const& calculator, output_stream_ptr const& out, measure_ptr const& measure) :
        calculator(calculator),
        out(out),
        measure(measure),
        output_all(false)
    {};

    TupleConsumer(entropy_calc_ptr const& calculator, measure_ptr const& measure) :
        calculator(calculator),
        out(0),
        measure(measure),
        output_all(false)
    {};
};

} // algorithm
} // mist
