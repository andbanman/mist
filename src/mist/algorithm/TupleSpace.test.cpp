#include <boost/test/unit_test.hpp>
#include <stdexcept>

#include "algorithm/TupleSpace.hpp"

using namespace mist;
using namespace algorithm;

class Counter : public TupleSpaceTraverser {
public:
  void process_tuple(TupleSpace::count_t tuple_no, TupleSpace::tuple_t const& tuple){ this->count++; };
  void process_tuple_entropy(TupleSpace::count_t tuple_no, TupleSpace::tuple_t const& tuple, it::Entropy const& e) { this->count++; };
  TupleSpace::index_t count = 0;
};

BOOST_AUTO_TEST_CASE(simple_names)
{
  TupleSpace ts;
  ts.set_names({ "v1", "v2", "v3" });
  auto names = ts.names();
  BOOST_TEST(names[0] == "v1");
  BOOST_TEST(names[1] == "v2");
  BOOST_TEST(names[2] == "v3");
}

BOOST_AUTO_TEST_CASE(traverse_equal_count)
{
  TupleSpace ts;
  ts.addVariableGroup("A", {0,1,2,3,4});
  ts.addVariableGroup("B", {5,6});
  ts.addVariableGroupTuple({0,1});
  Counter cntr;
  ts.traverse(cntr);
  BOOST_TEST(ts.count_tuples() == cntr.count);
  BOOST_TEST(ts.count_tuples() == 10);
}

BOOST_AUTO_TEST_CASE(traverse_equal_count2)
{
  TupleSpace ts(10, 2);
  Counter cntr;
  ts.traverse(cntr);
  BOOST_TEST(ts.count_tuples() == cntr.count);
  BOOST_TEST(ts.count_tuples() == 45);
}

// tests that we are in the regime of uint64, otherwise there would be overflow
BOOST_AUTO_TEST_CASE(count_large)
{
  TupleSpace ts(1e5, 2);
  BOOST_TEST(ts.count_tuples() == 4999950000);
}

//BOOST_AUTO_TEST_CASE(count_large1)
//{
//  TupleSpace ts(1e6, 3);
//  BOOST_TEST(ts.count_tuples() == 166666166667000000);
//}

//BOOST_AUTO_TEST_CASE(count_large2)
//{
//  TupleSpace ts(1e7, 2);
//  BOOST_TEST(ts.count_tuples() == 49999995000000);
//}

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
