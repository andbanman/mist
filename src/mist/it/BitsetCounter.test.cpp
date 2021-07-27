#include <boost/test/unit_test.hpp>

#include <stdexcept>

#include "Variable.hpp"
#include "it/BitsetCounter.hpp"

using namespace mist;

// test data
int num_vars = 10;

auto* data_a{ new Variable::data_t[6]{ 0, 1, 1, 0, 0, 1 } };
;
auto* data_b{ new Variable::data_t[6]{ 1, 1, 0, 0, 1, 0 } };
;

Variable::data_ptr da(data_a);
Variable::data_ptr db(data_b);

Variable variable_a(da, 6, 0, 2);
Variable variable_b(db, 6, 1, 2);
Variable variable_num_vars(db, 6, num_vars - 1, 2);
Variable variable_index_oor(db, 6, num_vars, 2);

BOOST_AUTO_TEST_CASE(BitsetCounter_default_constructor)
{
  it::BitsetCounter pdb1({ variable_a, variable_b });
}

BOOST_AUTO_TEST_CASE(BitsetCounter_count_1)
{

  Variable::tuple vars;
  vars.push_back(variable_a);
  it::BitsetCounter pdb(vars);

  it::Distribution pd0 = pdb.count(vars, { 0 });
  it::Distribution pd1 = pdb.count(vars);
  it::Distribution pd2 = pdb.count(variable_a);

  BOOST_TEST(pd0(std::vector<Variable::data_t>{ 0 }) == 3);
  BOOST_TEST(pd0(std::vector<Variable::data_t>{ 1 }) == 3);
  BOOST_TEST(pd1(std::vector<Variable::data_t>{ 0 }) == 3);
  BOOST_TEST(pd1(std::vector<Variable::data_t>{ 1 }) == 3);
  BOOST_TEST(pd2(std::vector<Variable::data_t>{ 0 }) == 3);
  BOOST_TEST(pd2(std::vector<Variable::data_t>{ 1 }) == 3);

  BOOST_TEST(pd0 == pd2);
  BOOST_TEST(pd1 == pd2);
}

BOOST_AUTO_TEST_CASE(BitsetCounter_count)
{

  Variable::tuple vars;
  vars.push_back(variable_a);
  vars.push_back(variable_b);
  it::BitsetCounter pdb(vars);

  it::Distribution pd = pdb.count(vars);

  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 0 }) == 1);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 1 }) == 2);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 1, 0 }) == 2);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 1, 1 }) == 1);

  vars.push_back(variable_b);
  pd = pdb.count(vars);

  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 0, 0 }) == 1);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 0, 1 }) == 0);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 1, 0 }) == 0);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 1, 1 }) == 2);
}

BOOST_AUTO_TEST_CASE(BitsetCounter_index_oor)
{

  Variable::tuple vars;
  // variable has index larger than the number of variables in the set, i.e.
  // out of range
  vars.push_back(variable_num_vars);
  BOOST_CHECK_THROW(it::BitsetCounter pdb(vars), it::BitsetCounterOutOfRange);
}

static it::Distribution
polymorphCount(it::Counter& pdc, Variable::tuple const& vars)
{
  return pdc.count(vars);
}

BOOST_AUTO_TEST_CASE(BitsetCounter_polymorph)
{

  Variable::tuple vars;
  vars.push_back(variable_b);
  vars.push_back(variable_a);
  it::BitsetCounter pdb(vars);

  it::Distribution pd = polymorphCount(pdb, vars);

  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 0 }) == 1);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 0, 1 }) == 2);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 1, 0 }) == 2);
  BOOST_TEST(pd(std::vector<Variable::data_t>{ 1, 1 }) == 1);
}
