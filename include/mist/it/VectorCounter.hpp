#pragma once

#include "Counter.hpp"

namespace mist {
namespace it {

/**
 * Generates a ProbabilityDistribution from a Variable tuple.
 *
 * Counts using standard algorithm.
 */
class VectorCounter : public Counter {
public:
    VectorCounter() {};
    ~VectorCounter() {};
    Distribution count(Variable const& var);
    Distribution count(Variable::tuple const& vars);
    Distribution count(Variable::tuple const& vars, Variable::indexes const& indexes);
};

class VectorCounterException : public std::exception {
private:
    std::string msg;
public:
    VectorCounterException(std::string const& method, std::string const& msg) :
        msg("Variable::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // it
} // mist
