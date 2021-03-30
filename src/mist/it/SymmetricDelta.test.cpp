
#include <boost/test/unit_test.hpp>
#include <stdexcept>

#include "io/DataMatrix.hpp" //TODO: don't cross namespace!
#include "it/EntropyCalculator.hpp"
#include "it/SymmetricDelta.hpp"

using namespace mist;

int test_data[21] = {
    0, 1, 1, 0, 1, 1, 1,
    0, 0, 0, 1, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1
};
int n = 3;
int m = 7;
io::DataMatrix test_matrix(test_data, n, m);
it::EntropyCalculator ec(test_matrix.variables());

// expected values
double e0 = 0.863120568566631;
double e1 = 0.863120568566631;
double e2 = 0.9852281360342515;
double e01 = 1.6644977792004614;
double e02 = 1.8423709931771086;
double e12 = 1.8423709931771088;
double e012 = 2.2359263506290326;
auto I01 = e0 + e1 - e01;
auto I02 = e0 + e2 - e02;
auto I12 = e1 + e2 - e12;
auto I012 = e0 + e1 + e2 - e01 - e02 - e12 + e012;
auto D0 = I012 - I12;
auto D1 = I012 - I02;
auto D2 = I012 - I01;
auto DD = D0 * D1 * D2;

double tolerance = 0.00000000001;

BOOST_AUTO_TEST_CASE(SymmetricDelta_constructor_default) {
    it::SymmetricDelta sym;
}

BOOST_AUTO_TEST_CASE(SymmetricDelta_compute_measure, * boost::unit_test::tolerance(tolerance)) {
    it::SymmetricDelta sym;

    auto res2 = sym.compute(ec, {0,1});
    BOOST_TEST(res2.back() == I01);

    auto res3 = sym.compute(ec, {0,1,2});
    BOOST_TEST(res3.back() == DD);
}

BOOST_AUTO_TEST_CASE(SymmetricDelta_compute_measure_polymorph, * boost::unit_test::tolerance(tolerance)) {
    it::Measure *measure;
    it::SymmetricDelta sym;
    measure = &sym;

    auto res2 = measure->compute(ec, {0,1});
    BOOST_TEST(res2.back() == I01);

    auto res3 = measure->compute(ec, {0,1,2});
    BOOST_TEST(res3.back() == DD);
}

BOOST_AUTO_TEST_CASE(SymmetricDelta_compute_subcalc, * boost::unit_test::tolerance(tolerance)) {
    it::SymmetricDelta sym;

    auto res2 = sym.compute(ec, {0,1});
    BOOST_TEST(res2[(int) it::SymmetricDelta::sub_calc_2d::entropy0] == e0);
    BOOST_TEST(res2[(int) it::SymmetricDelta::sub_calc_2d::entropy1] == e1);
    BOOST_TEST(res2[(int) it::SymmetricDelta::sub_calc_2d::entropy01] == e01);

    auto res3 = sym.compute(ec, {0,1,2});
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::entropy0] == e0);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::entropy1] == e1);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::entropy2] == e2);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::entropy01] == e01);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::entropy02] == e02);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::entropy12] == e12);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::entropy012] == e012);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::jointInfo01] == I01);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::jointInfo02] == I02);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::jointInfo12] == I12);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::diffInfo0] == D0);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::diffInfo1] == D1);
    BOOST_TEST(res3[(int) it::SymmetricDelta::sub_calc_3d::diffInfo2] == D2);
}

BOOST_AUTO_TEST_CASE(SymmetricDelta_compute_completion, * boost::unit_test::tolerance(tolerance)) {
    it::SymmetricDelta sym;
    auto res2 = sym.compute(ec, {0,1});
    sym.recomputeLastIndex(ec, {0,2}, res2);
    BOOST_TEST(res2.back() == I02);
    BOOST_TEST(res2[(int) it::SymmetricDelta::sub_calc_2d::entropy0] == e0);
    BOOST_TEST(res2[(int) it::SymmetricDelta::sub_calc_2d::entropy1] == e2);
    BOOST_TEST(res2[(int) it::SymmetricDelta::sub_calc_2d::entropy01] == e02);
}
