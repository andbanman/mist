#include <cmath>
#include <stdexcept>

#include "it/EntropyCalculator.hpp"
#include "it/VectorCounter.hpp"

using namespace mist;
using namespace mist::it;

EntropyCalculator::EntropyCalculator(Variable::tuple const& vars,
                                     cache_ptr_type cache)
  : vars(vars)
  , counter(0)
  , cache(cache)
{}

EntropyCalculator::EntropyCalculator(Variable::tuple const& vars,
                                     counter_ptr_type counter,
                                     cache_ptr_type cache)
  : vars(vars)
  , counter(counter)
  , cache(cache)
{}

EntropyCalculator::EntropyCalculator(Variable::tuple const& vars,
                                     counter_ptr_type counter,
                                     std::vector<cache_ptr_type>& caches)
  : vars(vars)
  , counter(counter)
{
  init_caches(caches);
}

EntropyCalculator::EntropyCalculator(Variable::tuple const& vars,
                                     counter_ptr_type counter)
  : vars(vars)
  , counter(counter)
{}

EntropyCalculator::EntropyCalculator(Variable::tuple const& vars)
  : vars(vars)
{
  this->counter = counter_ptr_type(new VectorCounter());
}

void
EntropyCalculator::init_caches(std::vector<cache_ptr_type>& caches)
{
  auto num_caches = caches.size();
  if (num_caches >= 1) {
    this->cache1d = caches[0];
  }
  if (num_caches >= 2) {
    this->cache2d = caches[1];
  }
  if (num_caches >= 3) {
    this->cache3d = caches[2];
  }
}

entropy_type
EntropyCalculator::entropy_it_distribution(Distribution const& pd)
{
  entropy_type entropy = 0.0;
  for (auto& prob : pd) {
    entropy = (prob) ? entropy + prob * std::log2(prob) : entropy;
  }
  return (entropy) ? -entropy : entropy;
};

entropy_type
EntropyCalculator::entropy_cache(tuple_t const& tuple, cache_ptr_type& cache)
{
  if (cache) {
    try {
      return *cache->get(tuple);
    } catch (std::out_of_range& e) {
      counter->count(vars, tuple, dist);
      dist.normalize();
      auto entropy = entropy_it_distribution(dist);
      try {
        cache->put(tuple, entropy);
      } catch (std::bad_alloc& e) {
        // out of memory, continue on
      }
      return entropy;
    }
  } else {
    this->counter->count(vars, tuple, dist);
    dist.normalize();
    return entropy_it_distribution(dist);
  }
}

entropy_type
EntropyCalculator::entropy(tuple_t const& tuple)
{
  if (this->cache) {
    // unified cache
    return entropy_cache(tuple, this->cache);
  } else {
    // multilevel cache
    auto size = tuple.size();
    switch (size) {
      case 1:
        return entropy_cache(tuple, this->cache1d);
      case 2:
        return entropy_cache(tuple, this->cache2d);
      case 3:
        return entropy_cache(tuple, this->cache3d);
      default:
        throw EntropyCalculatorException("entropy",
                                         "Unsupported tuple size " +
                                           std::to_string(size) +
                                           ", valid range [1,3]");
    }
  }
}
