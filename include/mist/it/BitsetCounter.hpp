#pragma once

#include <boost/dynamic_bitset.hpp>

#include "../Variable.hpp"

#include "Counter.hpp"

namespace mist {
namespace it {

using Bitset = boost::dynamic_bitset<unsigned long long>;
using BitsetVariable = std::vector<Bitset>;
using BitsetTable = std::vector<BitsetVariable>;

/** Generates a ProbabilityDistribution from a Variable tuple.
 *
 * Recasts each Variable as an array of bitsets, one for each bin value.
 * Computes the ProbabilityDistribution using bitwise AND operation and bit
 * counting algorithm.
 */
class BitsetCounter : public Counter {
public:
    BitsetCounter(Variable::tuple const& all_vars);
    ~BitsetCounter() {};
    Distribution count(Variable const& var);
    Distribution count(Variable::tuple const& vars);
    Distribution count(Variable::tuple const& vars, Variable::indexes const& indexes);
private:
    BitsetTable bits;
};

class BitsetCounterOutOfRange : public std::out_of_range {
public:
    BitsetCounterOutOfRange(std::string const& method, int index, int max) :
        out_of_range("BitsetCounter::" + method + " : Variable index " + std::to_string(index) + " out of bitset table range, valid range [0," + std::to_string(max) + "]") { };
};

} // it
} // mist
