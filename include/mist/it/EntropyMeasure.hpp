#pragma once

#include "../Variable.hpp"

#include "Measure.hpp"
#include "EntropyCalculator.hpp"

namespace mist {
namespace it {

class EntropyMeasure : public Measure {
public:
    EntropyMeasure() { };
    ~EntropyMeasure() { };

    result_type compute(EntropyCalculator &ecalc, Variable::indexes const& tuple) const;
    void recomputeLastIndex(EntropyCalculator &ecalc, Variable::indexes const& tuple, result_type & sub) const;

    enum struct sub_calc_1d : int {
        entropy0,
        size
    };

    enum struct sub_calc_2d : int {
        entropy01,
        size
    };

    enum struct sub_calc_3d {
        entropy012,
        size
    };
};

class EntropyMeasureException : public std::exception {
private:
    std::string msg;
public:
    EntropyMeasureException(std::string const& method, std::string const& msg) :
        msg("EntropyMeasure::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // it
} // mist
