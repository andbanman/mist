#pragma once

#include <algorithm>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace mist {

namespace permutation {

/** Permute to a random permutation.
 *
 * This method samples with replacement from all possible
 * permutations. Hence, there is a one in !n chance of obtaining
 * the same permutation you started with. Has time complexity O(n).
 *
 * @param gen Random number generator
 * @tparam G class meeting RandomNumberGenerator requirements
 * @tparam D class meeting RandomNumberDistribution requirements
 */
template<class T2, class G, class D = std::uniform_int_distribution<>>
void random(T2 &obj, G & gen) {
    int n = obj.size();

    // Fisher-Yates Shuffle algorithm
    for (std::size_t ii = n - 1; ii > 0; ii--) {
        D dist(0, ii);
        std::size_t jj = dist(gen);
        auto tmp_ii = obj[ii];
        obj[ii] = obj[jj];
        obj[jj] = tmp_ii;
    }
};

/** Permute to the next perumtation in the lexigraphic order.
 *
 * @return True if resulting permutation is not the ordered permutation.
 */
template <class T2>
static bool next(T2 &obj) {
    return std::next_permutation(obj.begin(), obj.end());
};

} // permutation

class PermutationException : public std::exception {
private:
    std::string msg;
public:
    PermutationException(std::string const& method, std::string const& msg) :
        msg("Permutation::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

 /**Permutation of any ordered class.
 */
template<class T>
class Permutation : public std::vector<T> {
public:
    // inherit constructors
    using std::vector<T>::vector;

     /** Apply permutation P to the source data.
     *
     * Time and space complexity O(n)
     *
     * @tparam S any ordered container
     * @return Copy of the permuted source array.
     */
    template<typename T2>
    std::vector<T2> apply(std::vector<T2> const& source) const {
        std::size_t size = source.size();

        if (size != this->size())
            throw PermutationException("apply", "Cannot apply Permutation (length " + std::to_string(this->size()) + ") to vector (length " + std::to_string(size) + ")");

        std::vector<T2> permuted(size);
        for (std::size_t ii = 0; ii < size; ii++)
            permuted[ii] = std::move(source[this->data()[ii]]);

        return permuted;
    };

    /** Permute source and save the result in dest
     */
    template<typename T2>
    void apply(T2 const& source, T2 & dest) const {
        std::size_t size = source.size();

        if (size != this->size())
            throw PermutationException("apply", "Cannot apply Permutation (length " + std::to_string(this->size()) + ") to vector (length " + std::to_string(size) + ")");
        if (size != dest.size())
            throw PermutationException("apply", "Size of src (" + std::to_string(size) + ") not equal dst (" + std::to_string(dest.size()) + ")");

        for (std::size_t ii = 0; ii < size; ii++)
            dest[ii] = std::move(source[this->data()[ii]]);
    };

    bool next() {
        return permutation::next(*this);
    };

    template<class G, class D = std::uniform_int_distribution<>>
    void random(G & gen) {
        permutation::random<Permutation, G, D>(*this, gen);
    };

    template<class D = std::uniform_int_distribution<>>
    void random() {
        std::random_device rd;
        std::mt19937 gen(rd());
        permutation::random<Permutation, std::mt19937, D>(*this, gen);
    };

    std::string to_string() {
        std::string s;
        int size = this->size();
        for (int ii = 0; ii < size; ii++) {
            s += std::to_string(this->data[ii]);
            if (ii < size - 1)
                s += ",";
        }
        return s;
    }
};

using IntegerPermutation = Permutation<int>;

} // mist
