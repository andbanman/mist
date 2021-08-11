//
// Variable - Permutation integration tests
//
#include <boost/test/unit_test.hpp>

#include "Permutation.hpp"
#include "Variable.hpp"

using namespace mist;

BOOST_AUTO_TEST_CASE(Permute_Variable_direct)
{
  auto* data_0{ new Variable::data_t[3]{ 1, 2, 3 } };
  ;
  auto* data_1{ new Variable::data_t[3]{ 1, 3, 2 } };
  auto* data_2{ new Variable::data_t[3]{ 2, 1, 3 } };
  Variable::data_ptr p_0(data_0);
  Variable::data_ptr p_1(data_1);
  Variable::data_ptr p_2(data_2);

  Variable v0(p_0, 3, 0, 3);
  Variable v1(p_1, 3, 0, 3);
  Variable v2(p_2, 3, 0, 3);

  Variable v = v0.deepCopy();

  BOOST_ASSERT(v == v0);
  BOOST_TEST(permutation::next(v));
  BOOST_ASSERT(v == v1);
  BOOST_TEST(permutation::next(v));
  BOOST_ASSERT(v == v2);
}

BOOST_AUTO_TEST_CASE(Permute_Variable_copy)
{
  Permutation<int> perm = { 2, 1, 0 };
  auto* data_3{ new Variable::data_t[3]{ 1, 2, 3 } };
  ;
  auto* data_3_exp{ new Variable::data_t[3]{ 3, 2, 1 } };

  Variable::data_ptr p3(data_3);
  Variable::data_ptr p3_exp(data_3_exp);
  Variable original(p3, 3, 0, 3);
  Variable expected(p3_exp, 3, 0, 3);
  Variable permuted = original.deepCopy();

  perm.apply(original, permuted);
  BOOST_ASSERT(permuted == expected);
}
