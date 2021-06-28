#pragma once

#include "../Variable.hpp"

#include "Measure.hpp"
#include "EntropyCalculator.hpp"

namespace mist {
namespace it {

class SymmetricDelta : public Measure {
public:
    SymmetricDelta() { };
    ~SymmetricDelta() { };

    result_type compute(EntropyCalculator &ecalc, Variable::indexes const& tuple) const;
    result_type compute(EntropyCalculator &ecalc, Variable::indexes const& tuple, Entropy const& e) const;
    std::string header(int d, bool full_output) const;

    enum struct sub_calc_2d : int {
        entropy0, entropy1, entropy01,
        symmetric_mist,
        size
    };

    enum struct sub_calc_3d {
        entropy0, entropy1, entropy2, entropy01, entropy02, entropy12, entropy012,
        jointInfo01, jointInfo02, jointInfo12, jointInfo012,
        diffInfo0, diffInfo1, diffInfo2,
        symmetric_mist,
        size
    };

    bool full_entropy() const { return true; };
};

class SymmetricDeltaException : public std::exception {
private:
    std::string msg;
public:
    SymmetricDeltaException(std::string const& method, std::string const& msg) :
        msg("SymmetricDelta::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // it
} // mist
