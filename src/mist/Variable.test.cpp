#include <boost/test/unit_test.hpp>

#include <iostream>
#include <stdexcept>

#include "Variable.hpp"

using namespace mist;

// test data
Variable::data_t* null;
auto* data_size6_bin2_a{ new Variable::data_t[6]{ 0, 1, 1, 0, 0, 1 } };
;
auto* data_size6_bin2_b{ new Variable::data_t[6]{ 1, 1, 0, 0, 1, 0 } };
;

// make shared pointers in global scope so the data doesn't get free'd
Variable::data_ptr pa(data_size6_bin2_a);
Variable::data_ptr pb(data_size6_bin2_b);

BOOST_AUTO_TEST_CASE(Variable_constructor_null_excep)
{
  BOOST_CHECK_THROW(Variable v_null(Variable::data_ptr(null), 6, 0, 2),
                    std::exception);
}

BOOST_AUTO_TEST_CASE(Variable_constructor_many)
{

  Variable va;
  BOOST_REQUIRE_NO_THROW(va = Variable(pa, 5, 0, 2));

  BOOST_TEST(va.size() == 5);
  BOOST_TEST(va.index() == 0);
  BOOST_TEST(va.bins() == 2);

  BOOST_REQUIRE_NO_THROW(Variable vb(pb, 5, 1, 2));
}

BOOST_AUTO_TEST_CASE(Variable_desctructor)
{
  {
    auto* data{ new Variable::data_t[6]{ 0, 1, 1, 0, 0, 1 } };
    ;
    auto* data2{ new Variable::data_t[6]{ 0, 1, 1, 0, 0, 1 } };
    ;
    Variable::data_ptr ptr(data);
    Variable::data_ptr ptr2(data2);
    Variable v1(ptr, 6, 0, 1);
    Variable v2(ptr2, 6, 0, 1);
    Variable v3(ptr2, 6, 0, 1);
  }
  // go out of scope
  BOOST_TEST(true);
}

BOOST_AUTO_TEST_CASE(Variable_access)
{
  // note our range is less than what's possible
  Variable va(pa, 5, 0, 2);

  for (int ii = 0; ii < 5; ii++) {
    BOOST_TEST(va[ii] == data_size6_bin2_a[ii]);
  }

  BOOST_CHECK_THROW(va.at(6), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(Variable_equality)
{
  Variable va(pa, 5, 0, 2);
  Variable vb(pb, 5, 1, 2);
  BOOST_ASSERT(va == va);
  BOOST_ASSERT(va != vb);
  // TODO more cases
}

BOOST_AUTO_TEST_CASE(Variable_copy)
{
  Variable va(pa, 5, 0, 2);
  Variable vb(va);
  Variable vc = va;
  Variable vd = va.deepCopy();
  BOOST_ASSERT(va == vb);
  BOOST_ASSERT(vb == vc);
  BOOST_ASSERT(vd == va);
}

BOOST_AUTO_TEST_CASE(Variable_swap)
{
  Variable va(pa, 5, 0, 2);
  Variable vb(pb, 5, 1, 2);
  Variable vc(pb, 5, 1, 2);
  BOOST_ASSERT(va != vb);
  BOOST_ASSERT(vb == vc);
  std::swap(va, vb);
  BOOST_ASSERT(va != vb);
  BOOST_ASSERT(va == vc);
}

BOOST_AUTO_TEST_CASE(Variable_move)
{
  Variable va(pa, 5, 0, 2);
  Variable vb = va;
  Variable vc = std::move(va);
  // XXX variable invalid after move
  BOOST_ASSERT(vb == vc);
  BOOST_ASSERT(va != vc);
}

BOOST_AUTO_TEST_CASE(Variable_iterator)
{
  auto* data{ new Variable::data_t[6]{ -2, -1, 0, 1, 2, 3 } };
  ;
  Variable::data_ptr ptr(data);
  Variable va(ptr, 5, 0, 4);
  int ii = 0;
  for (auto x : va) {
    BOOST_TEST(x == data[ii]);
    ii++;
  }
}
