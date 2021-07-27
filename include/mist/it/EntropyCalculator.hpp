#pragma once

#include <memory>

#include "../cache/Cache.hpp"
#include "Counter.hpp"
#include "Distribution.hpp"
#include "Entropy.hpp"

namespace mist {
namespace it {

class EntropyCalculator
{
public:
  using cache_type = cache::Cache<entropy_type>;
  using cache_ptr_type = std::shared_ptr<cache_type>;
  using counter_type = Counter;
  using counter_ptr_type = std::shared_ptr<counter_type>;
  using tuple_t = Variable::indexes;

private:
  Variable::tuple vars;
  counter_ptr_type counter;
  // keep a distribution "buffer" to avoid thrashing malloc/free
  it::Distribution dist;
  // TODO simpler caches
  cache_ptr_type cache = 0;
  cache_ptr_type cache1d = 0;
  cache_ptr_type cache2d = 0;
  cache_ptr_type cache3d = 0;

  void init_caches(std::vector<cache_ptr_type>& caches);
  static entropy_type entropy_it_distribution(Distribution const& pd);
  entropy_type entropy_cache(tuple_t const& tuple, cache_ptr_type& cache);

public:
  EntropyCalculator(Variable::tuple const& vars, cache_ptr_type cache);
  EntropyCalculator(Variable::tuple const& vars,
                    counter_ptr_type counter,
                    cache_ptr_type cache);
  EntropyCalculator(Variable::tuple const& vars,
                    counter_ptr_type counter,
                    std::vector<cache_ptr_type>& caches);
  EntropyCalculator(Variable::tuple const& vars, counter_ptr_type counter);
  EntropyCalculator(Variable::tuple const& vars);
  entropy_type entropy(tuple_t const& tuple);
};

class EntropyCalculatorException : public std::exception
{
private:
  std::string msg;

public:
  EntropyCalculatorException(std::string const& method, std::string const& msg)
    : msg("EntropyCalculator::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};
} // it
} // mist
