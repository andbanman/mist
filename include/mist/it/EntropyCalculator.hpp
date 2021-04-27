#pragma once

#include <memory>

#include "Counter.hpp"
#include "Distribution.hpp"
#include "../cache/Cache.hpp"

namespace mist {
namespace it {

using Entropy = double;

class EntropyCalculator {
public:
    // TODO split pd-cache and entropy-cache
    using cache_type = cache::Cache<Entropy>;
    using cache_ptr_type = std::shared_ptr<cache_type>;
    using counter_type = Counter;
    using counter_ptr_type = std::shared_ptr<counter_type>;
    using tuple_type = Variable::indexes;

private:
    Variable::tuple vars;
    counter_ptr_type counter;
    //TODO simpler caches
    cache_ptr_type cache = 0;
    cache_ptr_type cache1d = 0;
    cache_ptr_type cache2d = 0;
    cache_ptr_type cache3d = 0;

    void init_caches(std::vector<cache_ptr_type> &caches);
    static Entropy entropy_it_distribution(Distribution const& pd);
    Entropy entropy_cache(tuple_type const& tuple, cache_ptr_type &cache);

public:
    EntropyCalculator(Variable::tuple const& vars, cache_ptr_type cache);
    EntropyCalculator(Variable::tuple const& vars, counter_ptr_type counter, cache_ptr_type cache);
    EntropyCalculator(Variable::tuple const& vars, counter_ptr_type counter, std::vector<cache_ptr_type> &caches);
    EntropyCalculator(Variable::tuple const& vars, counter_ptr_type counter);
    EntropyCalculator(Variable::tuple const& vars);
    Entropy entropy(tuple_type const& tuple);
};

class EntropyCalculatorException : public std::exception {
private:
    std::string msg;
public:
    EntropyCalculatorException(std::string const& method, std::string const& msg) :
        msg("EntropyCalculator::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};
} // it
} // mist
