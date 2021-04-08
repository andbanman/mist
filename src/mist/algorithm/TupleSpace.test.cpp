#include <boost/test/unit_test.hpp>
#include <stdexcept>

#include "algorithm/TupleSpace.hpp"

using namespace mist;
using namespace algorithm;

BOOST_AUTO_TEST_CASE(simple_names) {
    TupleSpace ts(2);
    ts.set_names( { "v1", "v2", "v3" } );
    auto names = ts.names();
    BOOST_TEST(names[0] == "v1");
    BOOST_TEST(names[1] == "v2");
    BOOST_TEST(names[2] == "v3");
}

#if 0
BOOST_AUTO_TEST_CASE(simple_index_d2_n4) {
    TupleSpace ts(2);
    ts.addGroup({0,1})
    ts.addGroup({4,5})
    // default min = 0, max = 1
    auto tuples = TupleSpace::generateTuples(ts);
    BOOST_TEST(tuples[0] = { 0, 4 });
    BOOST_TEST(tuples[1] = { 0, 5 });
    BOOST_TEST(tuples[2] = { 1, 4 });
    BOOST_TEST(tuples[3] = { 1, 5 });
}
#endif
