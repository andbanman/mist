#pragma once

#include <memory>
#include <stdexcept>
#include <map>

#include "Cache.hpp"

namespace mist {
namespace cache {

class MapOutOfRange : public std::out_of_range {
public:
    MapOutOfRange(std::string const& method, std::string const& key) :
        out_of_range("Map::" + method + " : No item with key " + key) { };
};

/** Dynamically-expanding associative cache.
 */
template<class V>
class Map : public Cache<V> {
public:
    typedef typename std::map<std::size_t,V> item_list;
    typedef typename item_list::iterator iterator;

    Map() : numvars(0), dimensions(0) {};
    Map(int numvars, int dimensions) : numvars(numvars), dimensions(dimensions) {};

    bool has(K const& key) {
        auto it = il.find(tuple_to_key(key));
        return (it != il.end());
    }

    std::pair<K, V> put(K const& key, V const& val)
    {
        std::pair<iterator, bool> p = il.emplace(tuple_to_key(key), val);
        return std::make_pair(K(), V());
    }

    std::shared_ptr<V> get(K const& key) {
        auto it = il.find(tuple_to_key(key));
        if (it != il.end()) {
            ++this->_hits;
            // shared pointer that doesn't own the object
            return std::shared_ptr<V>(std::shared_ptr<V>(), &it->second);
        } else {
            ++this->_misses;
            throw MapOutOfRange("get", this->key_to_string(key));
        }
    }

    std::size_t size() {
        return il.size();
    }

    std::size_t bytes() {
        return 0; //TODO
    }

    iterator begin() { return il.begin(); }
    iterator end() { return il.end(); }

private:
    // Map of tuple index vector has much slower lookup. Convert to the flat
    // index
    std::size_t tuple_to_key(K const& tuple) {
        std::size_t index = 0;
        std::size_t factor = 1;
        std::size_t size = tuple.size();
        for (int ii = 0; ii < size; ii++) {
            index += factor * tuple[ii];
            factor *= numvars;
        }
        // fill in with full index to make unique with all tuples
        for (int ii = size; ii < dimensions; ii++) {
            index += factor * numvars;
            factor *= numvars;
        }
        return index;
    }
    item_list il;
    int numvars;
    int dimensions;
};

} // cache
} // mist
