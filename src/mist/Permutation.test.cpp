#include <boost/test/unit_test.hpp>

#include <iostream>
#include <stdexcept>

#include "Permutation.hpp"

using namespace mist;

template <typename T>
static void print(Permutation<T> const& perm) {
    std::cout << perm.to_string() << std::endl;
}

BOOST_AUTO_TEST_CASE(Permuation_constructor) {
    Permutation<int> perm;
    Permutation<double> p2;
    Permutation<int> p3 = {0, 1, 2, 3};
}

BOOST_AUTO_TEST_CASE(Permuation_lexigraphic_2) {
    Permutation<int> perm = {0, 1};
    Permutation<int> e1 = {1, 0};
    Permutation<int> e2 = {0, 1};
    BOOST_TEST(perm.next());
    BOOST_TEST(perm == e1);
    BOOST_TEST(!perm.next()); // reached end / beginning
    BOOST_TEST(perm == e2);
}

BOOST_AUTO_TEST_CASE(Permuation_lexigraphic_2_static) {
    std::vector<int> e  = {1, 0};
    std::vector<int> e0 = {0, 1};
    std::vector<int> e1 = {1, 0};
    BOOST_TEST(e == e1);
    BOOST_TEST(!permutation::next(e)); BOOST_TEST(e == e0);
    BOOST_TEST(permutation::next(e));  BOOST_TEST(e == e1);
}

BOOST_AUTO_TEST_CASE(Permuation_lexigraphic_3_full) {
    // starting with ordered permutation go all the way around
    Permutation<int> perm = {0, 1, 2};
    Permutation<int> e0 = {0, 1, 2};
    Permutation<int> e1 = {0, 2, 1};
    Permutation<int> e2 = {1, 0, 2};
    Permutation<int> e3 = {1, 2, 0};
    Permutation<int> e4 = {2, 0, 1};
    Permutation<int> e5 = {2, 1, 0};
    BOOST_TEST(perm.next());  BOOST_TEST(perm == e1);
    BOOST_TEST(perm.next());  BOOST_TEST(perm == e2);
    BOOST_TEST(perm.next());  BOOST_TEST(perm == e3);
    BOOST_TEST(perm.next());  BOOST_TEST(perm == e4);
    BOOST_TEST(perm.next());  BOOST_TEST(perm == e5);
    BOOST_TEST(!perm.next()); BOOST_TEST(perm == e0);
}

BOOST_AUTO_TEST_CASE(Permuation_lexigraphic_3_full_static) {
    // starting with ordered permutation go all the way around
    Permutation<int> e  = {0, 1, 2};
    Permutation<int> e0 = {0, 1, 2};
    Permutation<int> e1 = {0, 2, 1};
    Permutation<int> e2 = {1, 0, 2};
    Permutation<int> e3 = {1, 2, 0};
    Permutation<int> e4 = {2, 0, 1};
    Permutation<int> e5 = {2, 1, 0};
    BOOST_TEST(e == e0);
    BOOST_TEST(permutation::next(e));  BOOST_TEST(e == e1);
    BOOST_TEST(permutation::next(e));  BOOST_TEST(e == e2);
    BOOST_TEST(permutation::next(e));  BOOST_TEST(e == e3);
    BOOST_TEST(permutation::next(e));  BOOST_TEST(e == e4);
    BOOST_TEST(permutation::next(e));  BOOST_TEST(e == e5);
    BOOST_TEST(!permutation::next(e)); BOOST_TEST(e == e0);
}

BOOST_AUTO_TEST_CASE(Permuation_lexigraphic_3_random_static) {
    // starting with ordered permutation go all the way around
    Permutation<int> e  = {0, 1, 2};
    Permutation<int> permutations[6];
    permutations[0] = {0, 1, 2};
    permutations[1] = {0, 2, 1};
    permutations[2] = {1, 0, 2};
    permutations[3] = {1, 2, 0};
    permutations[4] = {2, 0, 1};
    permutations[5] = {2, 1, 0};

    BOOST_TEST(e == permutations[0]);

    // get random permutation
    std::random_device rd;
    std::mt19937 gen(rd());
    permutation::random(e, gen);

    // check if it's valid
    bool valid_permutation = false;
    for (auto p : permutations) {
        if (p == e) {
            valid_permutation = true;
            break;
        }
    }
    BOOST_ASSERT(valid_permutation);
}

BOOST_AUTO_TEST_CASE(Permuation_lexigraphic_3_partial) {
    // starting with unordered permutation, go around util ordered
    Permutation<int> perm = {0, -1, 2};
    Permutation<int> e1 = {0, 2, -1};
    Permutation<int> e2 = {2, -1, 0};
    Permutation<int> e3 = {2, 0, -1};
    Permutation<int> e4 = {-1, 0, 2};
    BOOST_TEST(perm.next());  BOOST_TEST(perm == e1);
    BOOST_TEST(perm.next());  BOOST_TEST(perm == e2);
    BOOST_TEST(perm.next());  BOOST_TEST(perm == e3);
    BOOST_TEST(!perm.next());  BOOST_TEST(perm == e4);
}

BOOST_AUTO_TEST_CASE(Permuation_apply_char) {
    std::vector<char> test1 = {'t','a','s','k'};
    Permutation<int> perm = {1, 2, 0, 3};
    auto test1_p = perm.apply(test1);
    BOOST_TEST(test1_p == (std::vector<char> {'a','s','t','k'}));
}

BOOST_AUTO_TEST_CASE(Permuation_apply_bool) {
    std::vector<bool> test1 = {0,1,1,1,0,0};
    std::vector<bool> exp1 =  {0,1,1,0,1,0};
    Permutation<int> perm = {5,3,1,0,2,4};
    auto test1_p = perm.apply(test1);
    BOOST_TEST(test1_p == exp1);
}
