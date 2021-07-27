#include <boost/test/unit_test.hpp>

#include "Variable.hpp"
#include "it/VectorCounter.hpp"

using namespace mist;

// test data
auto* data_many_bin2_a{ new Variable::data_t[6]{ 0, 1, 1, 0, 0, 1 } };
;
auto* data_many_bin2_b{ new Variable::data_t[6]{ 1, 1, 0, 0, 1, 0 } };
;

Variable variable_many_bin2_a(Variable::data_ptr(data_many_bin2_a), 6, 0, 2);
Variable variable_many_bin2_b(Variable::data_ptr(data_many_bin2_b), 6, 1, 2);

BOOST_AUTO_TEST_CASE(VectorCounter_default_constructor)
{
  it::VectorCounter pdv;
}

BOOST_AUTO_TEST_CASE(BitsetCounter_count_1)
{
  it::VectorCounter pdb;

  Variable::tuple vars;
  vars.push_back(variable_many_bin2_a);

  it::Distribution pd0 = pdb.count(vars, { 0 });
  it::Distribution pd1 = pdb.count(vars);
  it::Distribution pd2 = pdb.count(variable_many_bin2_a);

  BOOST_TEST(pd0(std::vector<Variable::data_t>{ 0 }) == 3);
  BOOST_TEST(pd0(std::vector<Variable::data_t>{ 1 }) == 3);
  BOOST_TEST(pd1(std::vector<Variable::data_t>{ 0 }) == 3);
  BOOST_TEST(pd1(std::vector<Variable::data_t>{ 1 }) == 3);
  BOOST_TEST(pd2(std::vector<Variable::data_t>{ 0 }) == 3);
  BOOST_TEST(pd2(std::vector<Variable::data_t>{ 1 }) == 3);

  BOOST_TEST(pd0 == pd2);
  BOOST_TEST(pd1 == pd2);
}

BOOST_AUTO_TEST_CASE(VectorCounter_count_bins_2)
{
  it::VectorCounter pdv;

  Variable::tuple vars;
  vars.push_back(variable_many_bin2_a);
  vars.push_back(variable_many_bin2_b);

  it::Distribution pd = pdv.count(vars);

  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 0 }) == 1);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 1 }) == 2);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 1, 0 }) == 2);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 1, 1 }) == 1);

  vars.push_back(variable_many_bin2_b);
  pd = pdv.count(vars);

  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 0, 0 }) == 1);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 0, 1 }) == 0);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 1, 0 }) == 0);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 1, 1 }) == 2);
}

static it::Distribution
polymorphCount(it::Counter& pdc, Variable::tuple const& vars)
{
  return pdc.count(vars);
}

BOOST_AUTO_TEST_CASE(VectorCounter_polymorph)
{
  it::VectorCounter pdv;

  Variable::tuple vars;
  vars.push_back(variable_many_bin2_b);
  vars.push_back(variable_many_bin2_a);

  it::Distribution pd = polymorphCount(pdv, vars);

  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 0 }) == 1);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 1 }) == 2);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 1, 0 }) == 2);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 1, 1 }) == 1);
}
