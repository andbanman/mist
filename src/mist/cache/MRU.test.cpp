#include <string>

#include <boost/test/unit_test.hpp>

#include "cache/MRU.hpp"

using namespace mist;

// see https://www.boost.org/doc/libs/1_65_0/libs/multi_index/doc/tutorial/techniques.html#emulate_assoc_containers
// TODO: make CacheMRU.test.cpp

BOOST_AUTO_TEST_CASE(CacheMRU_test) {
    cache::MRU<std::string> cache(3);

    cache.put({0}, "hello");
    cache.put({1}, "world");
    cache.put({2}, "over");
    cache.put({0}, "under");
    cache.put({0}, "behind");

    std::string res = "";
    // expect "behindoverworld"
    auto it = cache.begin();
    while (it != cache.end()) {
        res += it->second;
        it++;
    }

    BOOST_TEST(res == "behindoverworld");
}
