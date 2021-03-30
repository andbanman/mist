#include <boost/test/unit_test.hpp>
#include <stdexcept>

#include "it/Distribution.hpp"

using namespace mist;

BOOST_AUTO_TEST_CASE(ProbabilityDistribution_constructor) {
    it::Distribution pd_1d(std::vector<int>{10});
    it::Distribution pd_2d(std::vector<int>{1,1});
    it::Distribution pd_3d(std::vector<int>{2,4,6});

    BOOST_CHECK_THROW(
        it::Distribution pd_negative(std::vector<int>{-1}),
        std::exception
    );
}

BOOST_AUTO_TEST_CASE(ProbabilityDistribution_access) {
    it::Distribution pd(std::vector<int>{2,2});

    // uninitialized
    BOOST_TEST(pd(std::vector<int>{0, 0}) == 0);
    BOOST_TEST(pd(std::vector<int>{0, 1}) == 0);
    BOOST_TEST(pd(std::vector<int>{1, 0}) == 0);
    BOOST_TEST(pd(std::vector<int>{1, 1}) == 0);

    BOOST_TEST(pd.at(std::vector<int>{0, 0}) == 0);
    BOOST_TEST(pd.at(std::vector<int>{0, 1}) == 0);
    BOOST_TEST(pd.at(std::vector<int>{1, 0}) == 0);
    BOOST_TEST(pd.at(std::vector<int>{1, 1}) == 0);

    // out of range
    BOOST_CHECK_THROW(pd.at(std::vector<int>{1, 2}), std::out_of_range);
    BOOST_CHECK_THROW(pd.at(std::vector<int>{1, 2, 3}), std::out_of_range);
    BOOST_CHECK_THROW(pd.at(std::vector<int>{2, 1}) = 0, std::out_of_range);

    // containers
    pd(std::vector<int>{0, 0}) = 2;
    BOOST_TEST(pd(std::vector<int>{0, 0}) == 2);
    BOOST_TEST(pd.at(std::vector<int>{0, 0}) == 2);

    pd(std::array<int,2>{0, 1}) = -1;
    BOOST_TEST(pd(std::array<int,2>{0, 1}) == (it::DistributionData) -1);
    BOOST_TEST(pd.at(std::array<int,2>{0, 1}) == (it::DistributionData) -1);

    // plain data
    int indexes[2] = {0, 0};
    pd(2, indexes) = 3; BOOST_TEST(pd(2, indexes) == 3);
}

BOOST_AUTO_TEST_CASE(ProbabilityDistribution_increment) {
    it::Distribution pd(std::vector<int>{10});
    ++pd(std::vector<int>{1});
    BOOST_TEST(pd(std::vector<int>{1}) == 1);
    BOOST_TEST(pd.at(std::vector<int>{1}) == 1);
    pd(std::vector<int>{1})++;
    BOOST_TEST(pd(std::vector<int>{1}) == 2);
    BOOST_TEST(pd.at(std::vector<int>{1}) == 2);
}
