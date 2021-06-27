#pragma once

#include <string>

#include "../Variable.hpp"

#include "EntropyCalculator.hpp"

namespace mist {
namespace it {

class Measure {
public:
    using data_type = double;
    using result_type = std::vector<data_type>;

    virtual ~Measure() {};

    /**
     * Compute the information theory measure with the computation ecalc for
     * the given variables.
     *
     * @return final result
     */
    virtual result_type compute(EntropyCalculator &ecalc, Variable::indexes const& tuple) const = 0;

    /**
     * Compute the information theory measure with the the given variables,
     * using pre-computed entropies. Only useful for measures that use entropy
     * sub calculations.
     */
    virtual result_type compute(EntropyCalculator &ecalc, Variable::indexes const& tuple, Entropy const& entropy) const = 0;

    /**
     * Return a comma-separated header string corresponding to the full results
     * @param d tuple size
     * @param full_output whether header should include all subcalculation names
     * @return header string
     */
    virtual std::string header(int d, bool full_output) const = 0;
};

} // it
} // mist
