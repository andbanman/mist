#pragma once

#include <memory>
#include <stdexcept>

#ifdef BOOST_PYTHON_EXTENSIONS
#include <boost/predef/version.h>
#if BOOST_PREDEF_VERSION < BOOST_VERSION_NUMBER(1, 6, 0)
#include <boost/numpy.hpp>
#else
#include <boost/python/numpy.hpp>
#endif
#endif

#include "mist.hpp"

#ifdef BOOST_PYTHON_EXTENSIONS
namespace p = boost::python;
#if BOOST_PREDEF_VERSION < BOOST_VERSION_NUMBER(1, 6, 0)
namespace np = boost::numpy;
#else
namespace np = boost::python::numpy;
#endif
#endif

namespace mist {

/** Main user interface for mist runtime.
 *
 * CPP and Python users instantiate this class, load data, and optionally call
 * various configuration methods to define the computation. Computations begin
 * with compute(). Maintains state in between runs, such as intermediate value
 * caches for improved performance.
 */
class Search {
private:
    class impl;
    //std::experimental::propagate_const<std::unique_ptr<impl>> pimpl;
    std::unique_ptr<impl> pimpl;

    bool cacheInvalid();
    void primeCaches();
    void configureThreads();

public:
    Search();
    Search(Search&&);
    Search(const Search&);
    Search& operator=(Search&&);
    ~Search();

    //
    // Configuration toggles
    //

    /** Set the IT Measure to be computed.
     *
     * - Entropy : Compute only combined entropy.
     * - SymmetricDelta (default) : A novel symmetric measure of shared
     *   information. See Sakhanenko, Galas in the literature.
     */
    void set_measure(std::string const& measure);

    /** Set the tuple sharing algorithm between thread
     *
     * See algorithm::TupleProducer for list of availabled algorithms.
     */
    void set_tuple_algorithm(std::string const& tuple_algorithm);

    /** Set the serach space type
     *
     * - Exhaustive (default): include all unique combinations of variables in
     *   the tuple search space.
     * - TupleSpace : include only tuples defined by algorithm::TupleSpace, see Search::set_tuple_space.
     */
    void set_search_type(std::string const& search_type);

    /** Set the algorithm for generating probability distributions.
     *
     * - Vector (default) : Process each Variable as a vector. Gives best
     *   performance when Variable size is small or when there are many value
     *   bins.
     * - Bitset : Convert each distinct Variable value into a bitset to
     *   leverage bitwise operations. Gives best performance when Variable size
     *   is large and the number of value bins is small.
     *
     * Performance of each algorithm depends strongly on the problem, i.e. the
     * data, and potentially also on the system. After the number of threads,
     * this parameter has the largest effect on runtime since distribution
     * generation dominates the computation.
     */
    void set_probability_algorithm(std::string const& algorithm);

    /** Set output CSV file.
     */
    void set_outfile(std::string const& filename);

    /** Set number of concurrent threads.
     *
     * In the producer-consumer work sharing model, the number of threads
     * includes all producers and consumers. The number of threads has the
     * greatest effect on runtime.
     */
    void set_threads(int num_consumers);

    /** Set the number of Variables to include in each IT measure computation.
     */
    void set_tuple_size(int size);

    /** Set the custom tuple space for the next computation
     *
     * Side effects: sets the thread algorithm to TupleSpace so that the tuple
     * space becomes effective immediately.
     */
    void set_tuple_space(algorithm::TupleSpace const& ts);

    /** Include all subcalculations in the output
     */
    void full_output();

    /** Enable caching intermediate results for individual Variables.
     */
    void enable_cache_d1();

    /** Enable caching intermediate results for Variable sub-tuples size 2.
     */
    void enable_cache_d2();

    /**  Disable caching intermediate results for individual Variables.
     */
    void disable_cache_d1();

    /** Disable caching intermediate results for Variable sub-tuples size 2.
     */
    void disable_cache_d2();

    /** Load Data from CSV or tab-separated file.
     *
     * Each column in file is intepreted as a Variable, with each row a
     * instance.
     *
     * @param filename path to file
     * @pre each row has an equal number of columns.
     */
    void load_file(std::string const& filename);

#if BOOST_PYTHON_EXTENSIONS
    /** Load Data from Python Numpy::ndarray.
     *
     * Data is loaded into the library following a zero-copy guarantee.
     *
     * @param np ndarray
     * @pre Array is NxM matrix of the expected dtype and C memory layout.
     */
    void load_ndarray(np::ndarray const& np);
#endif

    /** Begin computation.
     *
     * Compute the configured IT measure for all Variable tuples in the
     * configured search space.
     */
    void compute();

    /** Print cache statistics for each cache in each thread to stdout.
     */
    void printCacheStats();

    /** Return the Search library Version string
     */
    std::string version();
};

class SearchException : public std::exception {
private:
    std::string msg;
public:
    SearchException(std::string const& method, std::string const& msg) :
        msg("Search::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // mist
