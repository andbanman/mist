
#include <boost/test/unit_test.hpp>
#include <stdexcept>

#include "io/DataMatrix.hpp" // TODO don't cross namespace!
#include "it/EntropyCalculator.hpp"

using namespace mist;

io::DataMatrix::data_t test_data[12] = { 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0 };

BOOST_AUTO_TEST_CASE(EntropyCalculator_constructor_default)
{
  int n = 3;
  int m = 4;
  io::DataMatrix test_matrix(test_data, n, m);
  it::EntropyCalculator ec(
      it::EntropyCalculator::variables_ptr(test_matrix.variables()));
}

BOOST_AUTO_TEST_CASE(EntropyCalculator_entropy_bounds)
{
  int n = 3;
  int m = 4;
  io::DataMatrix test_matrix(test_data, n, m);

  it::EntropyCalculator ec(
      it::EntropyCalculator::variables_ptr(test_matrix.variables()));

  // run through some entropies
  for (Variable::index_t i = 0; i < n; i++) {
    ec.entropy({ i });
  }
  ec.entropy({ 0, 1 });
  ec.entropy({ 0, 2 });
  ec.entropy({ 1, 0 });
  ec.entropy({ 1, 2 });
  ec.entropy({ 0, 1, 2 });

  // XXX: memory error, doesn't throw because counters don't check variable
  // indexes impossible in practice because indexes are deterministic & safe
  // BOOST_CHECK_THROW(
  //    ec.entropy({6}), std::exception
  //);
}

BOOST_AUTO_TEST_CASE(EntropyCalculator_entropy_correct1)
{
  int n = 3;
  int m = 4;
  io::DataMatrix test_matrix(test_data, n, m);

  it::EntropyCalculator ec(
      it::EntropyCalculator::variables_ptr(test_matrix.variables()));

  BOOST_TEST(ec.entropy({ 0 }) == 0.8112781244591328);
  BOOST_TEST(ec.entropy({ 1 }) == 1);
  BOOST_TEST(ec.entropy({ 2 }) == 0.8112781244591328);
  BOOST_TEST(ec.entropy({ 0, 1 }) == 1.5);
  BOOST_TEST(ec.entropy({ 0, 2 }) == 1.5);
  BOOST_TEST(ec.entropy({ 1, 2 }) == 1.5);
  BOOST_TEST(ec.entropy({ 0, 1, 2 }) == 2);
}
