#pragma once

#include <memory>
#include <stdexcept>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include "Cache.hpp"

using namespace boost::multi_index;

namespace mist {
namespace cache {

class MRUOutOfRange : public std::out_of_range {
public:
    MRUOutOfRange(std::string const& method, std::string const& key) :
        out_of_range("Map::" + method + " : No item with key " + key) { };
;
};

/** Fixed sized associative cache with least recently added eviction.
 */
template<class V>
class MRU : public Cache<V> {

    //
    // Need mutable pair type for multi_index
    //
    // Based on: https://www.boost.org/doc/libs/1_65_0/libs/multi_index/example/serialization.cpp
	template <typename T1,typename T2>
	struct mutable_pair
	{
	    typedef T1 first_type;
	    typedef T2 second_type;

	    mutable_pair():first(T1()),second(T2()){}
	    mutable_pair(const T1& f,const T2& s):first(f),second(s){}
	    mutable_pair(const std::pair<T1,T2>& p):first(p.first),second(p.second){}

	    T1         first;
	    mutable T2 second;
	};

	typedef mutable_pair<K, V> Element;

    typedef multi_index_container<
        Element,
        indexed_by<
			sequenced<>,
		    hashed_unique< member<Element, K, &Element::first> > >
    > item_list;

public:
    typedef typename item_list::iterator iterator;

    MRU() : max_num_items(0) {};
    MRU(std::size_t max_size) : max_num_items(max_size) {};
    MRU(MRU const& other) : max_num_items(other.max_num_items) {
        this->il = other.il;
    };
    MRU& operator=(MRU const& other) {
        this->il = other.il;
        this->max_num_items = other.max_num_items;
    }

    bool has(K const& key) {
        auto it = boost::multi_index::get<1>(il).find(key);
        return (it != boost::multi_index::get<1>(il).end());
    }

    std::pair<K, V> put(K const& key, V const& val)
    {
        std::pair<iterator, bool> p = il.push_front(Element(key, val));

        if (!p.second) {
            // XXX this code never reached if program correctly does not process
            // same tuple twice
            // TODO remove this block and rename class AssociativeFIFO
            il.relocate(il.begin(), p.first);
            il.begin()->second = val;
        }
        else if (il.size() > max_num_items){
            // keep the length <= max_num_items
            ++this->_evictions;
            auto evicted = std::make_pair(il.back().first, il.back().second);
            il.pop_back();
            return evicted;
        }

        return std::make_pair(K(), V());
    }

    std::shared_ptr<V> get(K const& key) {
        auto it = boost::multi_index::get<1>(il).find(key);
        if (it != boost::multi_index::get<1>(il).end()) {
            //TODO: update order to make true LRU
            // approximate by just moving it ahead one position?
            ++this->_hits;
            // empty shared pointer doesn't own the object
            return std::shared_ptr<V>(std::shared_ptr<V>(), &it->second);
        } else {
            ++this->_misses;
            throw MRUOutOfRange("get", this->key_to_string(key));
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
    item_list il;
    std::size_t max_num_items;
};

} // cache
} // mist
