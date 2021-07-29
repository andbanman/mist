#pragma once

#include "io/DataMatrix.hpp"
#include "io/MapOutputStream.hpp"
#include "it/Entropy.hpp"
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
 * with start(). Maintains state in between runs, such as intermediate value
 * caches for improved performance.
 */
class Search
{
private:
  class impl;
  // std::experimental::propagate_const<std::unique_ptr<impl>> pimpl;
  std::unique_ptr<impl> pimpl;

  void init_caches();
  void _load_file(std::string const& filename, bool is_row_major);

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
  std::string get_measure();

  /** Set the minimum IT Measure value to keep in results.
   *
   * This option is most useful for dealing with very large TupleSpaces, the
   * results for which cannot be stored in memory or on disk.
   */
  void set_cutoff(it::entropy_type cutoff);
  it::entropy_type get_cutoff();

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
  std::string get_probability_algorithm();

  /** Set output CSV file.
   */
  void set_outfile(std::string const& filename);
  std::string get_outfile();

  /** Set number of concurrent ranks to use in this Search.
   *
   * A rank on a computation node is one execution thread. The default ranks
   * is the number of threads allowed by the node. Setting ranks to 0 causes
   * the system to use the maximum.
   */
  void set_ranks(int ranks);
  int get_ranks();

  /** Set the starting rank for this Search.
   *
   * A Mist search can run in parallel on multiple nodes in a system. For
   * each node, configure a Search with the starting rank, number of ranks
   * (ie threads) on the node, and total ranks among all nodes. In this way
   * you can divide the search space among nodes in the system.
   *
   * The starting rank is the zero-indexed rank number, valid over range
   * [0,total_ranks].
   *
   * @param rank Zero-indexed rank number
   */
  void set_start_rank(int rank);
  int get_start_rank();

  /** Set the total number of ranks among all participating Searches.
   *
   * Each thread on each node is counted as a rank. So the total_ranks is the
   * sum of configured ranks (threads) on each node.
   */
  void set_total_ranks(int ranks);
  int get_total_ranks();

  /** Set the number of Variables to include in each IT measure computation.
   */
  void set_tuple_size(int size);
  int get_tuple_size();

  /** Set the custom tuple space for the next computation
   *
   * Side effects: sets the thread algorithm to TupleSpace so that the tuple
   * space becomes effective immediately.
   */
  void set_tuple_space(algorithm::TupleSpace const& ts);
  algorithm::TupleSpace get_tuple_space();

  /** Set the maximum number of tuples to process. The default it 0, meaning
   * unlimited.
   */
  void set_tuple_limit(long limit);
  long get_tuple_limit();

  /** Include all subcalculations in the output
   */
  void set_output_intermediate(bool);
  bool get_output_intermediate();

  /** Enable caching intermediate entropy calculation
   */
  void set_cache_enabled(bool);
  bool get_cache_enabled();

  /** Set maximum size of entropy cache in bytes
   */
  void set_cache_size_bytes(unsigned long);
  unsigned long get_cache_size_bytes();

  /** Load Data from CSV or tab-separated file.
   *
   * @param filename path to file
   * @param is_row_major Set to true for row-major variables
   * @pre each row has an equal number of columns.
   */
  /** Load Data from CSV or tab-separated file.
   *
   * By defualt, the file is loaded in row-major order, i.e. each row is a variable.
   */
  void load_file(std::string const& filename);
  void load_file_row_major(std::string const& filename);
  void load_file_column_major(std::string const& filename);

#if BOOST_PYTHON_EXTENSIONS
  /** Load Data from Python Numpy::ndarray.
   *
   * Data is loaded into the library following a zero-copy guarantee.
   *
   * @param np ndarray
   * @pre Array is NxM matrix of the expected dtype and C memory layout.
   */
  void load_ndarray(np::ndarray const& np);

  /** Return a Numpy ndarray copy of all results
   */
  np::ndarray python_get_results();

  /** Start search.
   *
   * Compute the configured IT measure for all Variable tuples in the
   * configured search space. And return up to tuple_limit number of results.
   */
  np::ndarray python_start();
#endif

  /** Begin computation.
   *
   * Compute the configured IT measure for all Variable tuples in the
   * configured search space.
   */
  void start();

  /** Return a copy of all results
   */
  std::vector<it::entropy_type> const& get_results();

  /** Print cache statistics for each cache in each thread to stdout.
   */
  void printCacheStats();

  /** Return the Search library Version string
   */
  std::string version();
};

class SearchException : public std::exception
{
private:
  std::string msg;

public:
  SearchException(std::string const& method, std::string const& msg)
    : msg("Search::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

} // mist
