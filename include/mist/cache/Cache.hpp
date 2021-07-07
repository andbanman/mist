#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <utility>

#include "../Variable.hpp"

namespace mist {
namespace cache {

using K = Variable::indexes;

/** Cache interface
 */
template<class V>
class Cache
{
public:
  virtual ~Cache(){};

  /** Test that key is in table
   */
  virtual bool has(K const&) = 0;

  // TODO: getter function that both Mem and IO can use.
  // for large size V, use pointers instead
  // virtual V get(K const&) = 0;

  /** Insert value at key.
   * An element will be removed if the table size would
   * be exceeded and returned for handling.
   */
  virtual std::pair<K, V> put(K const&, V const&) = 0;

  /** Return value at key.
   *
   * @except out_of_range Key not in table
   */
  virtual std::shared_ptr<V> get(K const&) = 0;

  /** Number of entries in table
   */
  virtual std::size_t size() = 0;

  /** Size in bytes of table
   */
  virtual std::size_t bytes() = 0;

  /** Number of cache hits
   */
  std::size_t hits() { return this->_hits; }

  /** Number of cache misses
   */
  std::size_t misses() { return this->_misses; }

  /** Number of cache evictions
   */
  std::size_t evictions() { return this->_evictions; }

protected:
  // TODO atomic types for thread safety
  std::size_t _hits = 0;
  std::size_t _misses = 0;
  std::size_t _evictions = 0;

  // helper to print out keys
  std::string key_to_string(K const& key)
  {
    std::string ret = "[";
    size_t size = key.size();
    for (size_t ii = 0; ii < size - 1; ii++)
      ret += std::to_string(key[ii]) + ",";
    ret += std::to_string(key.back()) + "]";
    return ret;
  }
};

}; // cache
}; // mist
