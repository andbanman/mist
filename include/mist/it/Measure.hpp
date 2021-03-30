#pragma once

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
     * Compute the information theory measure with the EntropyCalculator for
     * the given variables, re-using intermdiate results for all variables
     * except the last Variable. The result struct is completed with new
     * ecalculations.
     */
    virtual void recomputeLastIndex(EntropyCalculator &ecalc, Variable::indexes const& tuple, result_type & sub) const = 0;
};

} // it
} // mist
