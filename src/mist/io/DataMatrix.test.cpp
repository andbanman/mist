
#include <boost/test/unit_test.hpp>

#include "io/DataMatrix.hpp"

using namespace mist;
using namespace mist::io;

BOOST_AUTO_TEST_CASE(DataMatrix_constructor_zeros)
{
  std::size_t n = 5;
  std::size_t m = 10;
  std::size_t b = 2;
  DataMatrix test_data(n, m, b);
}

BOOST_AUTO_TEST_CASE(DataMatrix_constructor_array)
{
  int n = 3;
  int m = 2;
  DataMatrix::data_t test_data[6] = { 0, 1, 2, 3, 4, 5 };
  DataMatrix test_matrix(test_data, n, m);

  int kk = 0;
  for (int ii = 0; ii < n; ii++) {
    for (int jj = 0; jj < m; jj++) {
      // std::cout << ii << "," << jj << "," << test_matrix.get_variable(ii)[jj] <<
      // std::endl;
      BOOST_TEST(test_matrix.get_variable(ii)[jj] == kk);
      kk++;
    }
  }
}

BOOST_AUTO_TEST_CASE(DataMatrix_constructor_array2)
{
  DataMatrix::data_t test_data[12] = { 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0 };

  DataMatrix test_matrix0(test_data, 1, 12);
  DataMatrix test_matrix1(test_data, 2, 6);
  DataMatrix test_matrix2(test_data, 3, 4);
  DataMatrix test_matrix3(test_data, 4, 3);
  DataMatrix test_matrix4(test_data, 6, 2);
  DataMatrix test_matrix5(test_data, 12, 1);
}
