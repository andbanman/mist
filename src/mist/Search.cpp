#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "Search.hpp"
#include "algorithm/TupleSpace.hpp"
#include "algorithm/Worker.hpp"
#include "io/DataMatrix.hpp"
#include "io/MapOutputStream.hpp"
#include "it/BitsetCounter.hpp"
#include "it/Entropy.hpp"
#include "it/EntropyCalculator.hpp"
#include "it/SymmetricDelta.hpp"

using namespace mist;

using data_ptr = std::shared_ptr<io::DataMatrix>;
using output_stream_ptr = std::shared_ptr<io::OutputStream>;
using file_stream_ptr = std::shared_ptr<io::FileOutputStream>;
using map_stream_ptr = std::shared_ptr<io::MapOutputStream>;
using measure_ptr = std::shared_ptr<it::Measure>;
using cache_ptr = it::EntropyCalculator::cache_ptr_type;
using entropy_calc_ptr = std::shared_ptr<it::EntropyCalculator>;

std::string
Search::version()
{
  return MIST_VERSION;
}

enum struct probability_algorithms : int
{
  vector,
  bitset
};

struct thread_config
{
  measure_ptr measure;
  output_stream_ptr output_stream;
  std::vector<cache_ptr> caches;
  entropy_calc_ptr calculator;
};

struct Search::impl
{
  // structures
  data_ptr data;
  file_stream_ptr file_output;
  measure_ptr measure;
  std::string measure_str;
  std::vector<cache_ptr> shared_caches;
  std::vector<map_stream_ptr> mem_outputs;
  std::vector<thread_config> threads;

  // config
  unsigned long cache_size_bytes = 0;
  algorithm::TupleSpace::index_t tuple_limit = 0;
  bool use_cache = true;
  bool full_output = false;
  bool in_memory_output = true;
  // whether this Search is participating in a parallel search
  bool parallel_search = false;
  int ranks;
  int start_rank;
  int total_ranks;
  int tuple_size;
  probability_algorithms probability_algorithm;
  std::string probability_algorithm_str;
  std::string outfile;
  algorithm::TupleSpace tuple_space;
};

Search::Search()
  : pimpl(std::make_unique<impl>())
{
  // structures
  pimpl->measure = measure_ptr(new it::SymmetricDelta());
  pimpl->measure_str = "SymmetricDelta";
  pimpl->data = 0;

  // default config
  pimpl->ranks = std::thread::hardware_concurrency();
  pimpl->total_ranks = pimpl->ranks;
  pimpl->start_rank = 0;
  pimpl->tuple_size = 2;
  pimpl->probability_algorithm = probability_algorithms::vector;
  pimpl->probability_algorithm_str = "Vector"; // TODO enumerate elswhere
};

Search::Search(const Search& other)
  : pimpl(std::make_unique<impl>())
{
  *this->pimpl = *other.pimpl;
};

Search::Search(Search&&) = default;
Search&
Search::operator=(Search&&) = default;
Search::~Search() {}

void
Search::set_measure(std::string const& measure)
{
  std::string test(measure);
  transform(test.begin(), test.end(), test.begin(), ::tolower);

  if (test == "symmetricdelta") {
    pimpl->measure = measure_ptr(new it::SymmetricDelta());
    pimpl->measure_str = "SymmetricDelta";
  } else if (test == "entropy") {
    pimpl->measure = measure_ptr(new it::EntropyMeasure());
    pimpl->measure_str = "JointEntropy";
  } else {
    throw SearchException("set_measure",
                          "Invalid measure: " + measure +
                            ", allowed: [SymmetricDelta,Entropy]");
  }
}

std::string
Search::get_measure()
{
  return pimpl->measure_str;
}

void
Search::set_probability_algorithm(std::string const& algorithm)
{
  std::string test(algorithm);
  transform(test.begin(), test.end(), test.begin(), ::tolower);

  if (test == "bitset") {
    pimpl->probability_algorithm = probability_algorithms::bitset;
    pimpl->probability_algorithm_str = "Bitset";
  } else if (test == "vector") {
    pimpl->probability_algorithm = probability_algorithms::vector;
    pimpl->probability_algorithm_str = "Vector";
  } else {
    throw SearchException("set_probability_algorithm",
                          "Invalid probability algorithm : " + algorithm +
                            ", allowed: [bitset, vector]");
  }
}
std::string
Search::get_probability_algorithm()
{
  return pimpl->probability_algorithm_str;
}

void
Search::set_outfile(std::string const& filename)
{
  // Thread copies of FileOutputStream should be constructed from
  // this object so they share an underlying file stream
  pimpl->outfile = filename;
  pimpl->in_memory_output = false;
}
std::string
Search::get_outfile()
{
  return pimpl->outfile;
}

#if BOOST_PYTHON_EXTENSIONS
//
// Returns python numpy ndarray of variable indexes with the result set
//
np::ndarray
Search::python_get_results()
{
  if (pimpl->mem_outputs.empty()) {
    return np::zeros(p::make_tuple(0, 0), np::dtype::get_builtin<double>());
  }

  // infer results dimensions
  auto a_result = pimpl->mem_outputs.front()->get_results().begin();
  int num_variables = a_result->first.size();
  int num_results = a_result->second.size();

  // determine number of tuples collected in the output streams
  long num_tuples = 0;
  for (auto& ptr : pimpl->mem_outputs) {
    num_tuples += ptr->get_results().size();
  }

  // initialize the output ndarray
  np::ndarray ret =
    np::zeros(p::make_tuple(num_tuples, num_variables + num_results),
              np::dtype::get_builtin<double>());

  // copy results into output ndarray
  int ii = 0;
  for (auto& ptr : pimpl->mem_outputs) {
    auto& results = ptr->get_results();
    for (auto& item : results) {
      // fill in variable index tuple
      for (int jj = 0; jj < num_variables; jj++) {
        ret[ii][jj] = (double)item.first[jj];
      }
      // fill in results
      for (int jj = 0; jj < num_results; jj++) {
        ret[ii][num_variables + jj] = (double)item.second[jj];
      }
      ii++;
    }
  }

  return ret;
}
#endif

// Returns a copy of the results collected into one data structure
io::MapOutputStream::map_type
Search::get_results()
{
  io::MapOutputStream::map_type ret;
  if (pimpl->mem_outputs.empty()) {
    return ret;
  }
  for (auto& ptr : pimpl->mem_outputs) {
    auto& results = ptr->get_results();
    ret.insert(results.begin(), results.end());
  }
  return ret;
}

void
Search::set_tuple_size(int size)
{
  if (size < 2 || size > 3) {
    throw SearchException("set_tuple_size",
                          "Invalid tuple size " + std::to_string(size) +
                            ", valid range is [2,3]");
  }
  pimpl->tuple_size = size;
}
int
Search::get_tuple_size()
{
  return pimpl->tuple_size;
}

void
Search::set_tuple_space(algorithm::TupleSpace const& ts)
{
  pimpl->tuple_space = ts;
  pimpl->tuple_size = ts.tupleSize();
}
algorithm::TupleSpace
Search::get_tuple_space()
{
  return pimpl->tuple_space;
}

void
Search::set_tuple_limit(long limit)
{
  pimpl->tuple_limit = limit;
}
long
Search::get_tuple_limit()
{
  return pimpl->tuple_limit;
}

void
Search::set_ranks(int ranks)
{
  pimpl->ranks = (!ranks) ? std::thread::hardware_concurrency() : ranks;
}
int
Search::get_ranks()
{
  return pimpl->ranks;
}

void
Search::set_start_rank(int start_rank)
{
  pimpl->start_rank = start_rank;
}
int
Search::get_start_rank()
{
  return pimpl->ranks;
}

void
Search::set_total_ranks(int total_ranks)
{
  pimpl->total_ranks = total_ranks;
  pimpl->parallel_search = true;
}
int
Search::get_total_ranks()
{
  return pimpl->total_ranks;
}

void
Search::set_output_intermediate(bool full)
{
  pimpl->full_output = full;
}
bool
Search::get_output_intermediate()
{
  return pimpl->full_output;
}

void
Search::set_cache_enabled(bool enabled)
{
  pimpl->use_cache = enabled;
}
bool
Search::get_cache_enabled()
{
  return pimpl->use_cache;
}

void
Search::set_cache_size_bytes(unsigned long size)
{
  pimpl->cache_size_bytes = size;
}
unsigned long
Search::get_cache_size_bytes()
{
  return pimpl->cache_size_bytes;
}

void
Search::_load_file(std::string const& filename, bool rowmajor)
{
  // TODO invalidate previous results
  pimpl->data = data_ptr(new io::DataMatrix(filename, rowmajor));
  if (!pimpl->data) {
    throw SearchException(
      "load_file", "Failed to create DataMatrix from file '" + filename + "'");
  }
}

void
Search::load_file(std::string const& filename)
{
  _load_file(filename, true);
}

void
Search::load_file_row_major(std::string const& filename)
{
  _load_file(filename, true);
}

void
Search::load_file_column_major(std::string const& filename)
{
  _load_file(filename, false);
}

#if BOOST_PYTHON_EXTENSIONS
void
Search::load_ndarray(np::ndarray const& np)
{
  // TODO invalidate previous results
  pimpl->data = data_ptr(new io::DataMatrix(np));
  if (!pimpl->data) {
    throw SearchException("load_ndarray",
                          "Failed to create DataMatrix from ndarray");
  }
}
#endif

void
Search::printCacheStats()
{
  int thread_no = 0;
  for (auto const& thread : pimpl->threads) {
    int cache_no = 1;
    if (!thread.caches.empty()) {
      for (auto cache : thread.caches) {
        if (cache) {
          std::cerr << "Thread " << thread_no;
          std::cerr << " cache " << cache_no;
          std::cerr << " hits " << cache->hits();
          std::cerr << " misses " << cache->misses();
          std::cerr << " evictions " << cache->evictions();
          std::cerr << std::endl;
        }
        cache_no++;
      }
    }
    thread_no++;
  }
}

entropy_calc_ptr
makeEntropyCalc(probability_algorithms const& type,
                Variable::tuple const& variables,
                std::vector<cache_ptr>& caches)
{
  it::EntropyCalculator* calc = 0;
  switch (type) {
    case probability_algorithms::vector:
      calc = new it::EntropyCalculator(
        variables,
        std::shared_ptr<it::VectorCounter>(new it::VectorCounter()),
        caches);
      break;
    case probability_algorithms::bitset:
      calc = new it::EntropyCalculator(
        variables,
        std::shared_ptr<it::BitsetCounter>(new it::BitsetCounter(variables)),
        caches);
      break;
  }
  return entropy_calc_ptr(calc);
}

void
Search::init_caches()
{
  auto entropy_measure = measure_ptr(new it::EntropyMeasure());
  int nvar = pimpl->data->get_nvar();
  int num_thread = pimpl->ranks;
  int ncache = (pimpl->tuple_size > 2) ? 2 : 1;
  auto variables = pimpl->data->variables();
  auto total_ranks =
    (pimpl->parallel_search) ? pimpl->total_ranks : pimpl->ranks;
  auto start_rank = pimpl->start_rank;

  pimpl->shared_caches.resize(ncache);

  // By taking each dimension on its own we prevent any two threads from
  // seeing the same tuple. Only safe for pre-sized caches, e.g. Flat, that
  // are thread-safe for unique puts. For other cache types use a single
  // worker.

  // fill caches
  for (int cc = 0; cc < ncache; cc++) {
    int d = cc + 1;
    try {
      if (pimpl->cache_size_bytes) {
        pimpl->shared_caches[cc] =
          cache_ptr(new cache::Flat<it::entropy_type>(nvar, d, pimpl->cache_size_bytes));
      } else {
        pimpl->shared_caches[cc] =
          cache_ptr(new cache::Flat<it::entropy_type>(nvar, d));
      }
    } catch (std::bad_alloc const& ba) {
      // cannot allocate this cache, stop the cache init
      break;
    }
    std::vector<algorithm::Worker> workers(num_thread);
    std::vector<std::thread> threads(num_thread - 1);
    std::vector<cache_ptr> caches = { pimpl->shared_caches[cc] };
    auto ts = algorithm::TupleSpace(nvar, d); //TODO: make it closer to the real TupleSpace ...
    auto calc =
      makeEntropyCalc(pimpl->probability_algorithm, variables, caches);

    for (int ii = 0; ii < num_thread; ii++) {
      workers[ii] = algorithm::Worker(
        start_rank + ii, total_ranks, 0, ts, calc, {}, entropy_measure);
    }
    for (int ii = 0; ii < num_thread - 1; ii++) {
      threads[ii] = std::thread(&algorithm::Worker::start, workers[ii]);
    }
    workers.back().start();
    for (auto& thread : threads) {
      thread.join();
    }
  }
}

//
// Run full algoirithm as configured
//
void
Search::start()
{
  // sanity checks
  if (!pimpl->data) {
    throw SearchException("start",
                          "No data loaded, use load_file or load_ndarray.");
  }
  if (!pimpl->measure) {
    throw SearchException("start", "No IT Measure selected, use set_measure.");
  }
  if (pimpl->parallel_search && pimpl->total_ranks < pimpl->ranks) {
    throw SearchException(
      "start", "ranks for this Search cannot be greater than total_ranks.");
  }

  int nvar = pimpl->data->get_nvar();
  int tuple_size = pimpl->tuple_size;
  int num_thread =
    (!pimpl->ranks) ? std::thread::hardware_concurrency() : pimpl->ranks;
  auto variables = pimpl->data->variables();
  auto total_ranks =
    (pimpl->parallel_search) ? pimpl->total_ranks : pimpl->ranks;
  auto start_rank = pimpl->start_rank;

  // load default tuplespace if one has not been set yet
  if (!pimpl->tuple_space.tupleSize()) {
    pimpl->tuple_space = algorithm::TupleSpace(nvar, tuple_size);
  }

  // initialize output file stream
  if (!pimpl->outfile.empty()) {
    auto header = pimpl->measure->header(tuple_size, pimpl->full_output);
    pimpl->file_output =
      file_stream_ptr(new io::FileOutputStream(pimpl->outfile, header));
    if (!pimpl->file_output) {
      throw SearchException("start",
                            "Failed to create FileOutputStream from file '" +
                              pimpl->outfile + "'");
    }
  }

  // Only use caches for measures that use intermediate entropies
  if (pimpl->use_cache && pimpl->measure->full_entropy()) {
    init_caches();
  }

  if (pimpl->in_memory_output) {
    pimpl->mem_outputs.clear();
    pimpl->mem_outputs.resize(num_thread);
  }
  std::vector<algorithm::Worker> workers(num_thread);
  std::vector<std::thread> threads(num_thread - 1);
  auto calc = makeEntropyCalc(
    pimpl->probability_algorithm, variables, pimpl->shared_caches);
  // Create Workers
  for (int ii = 0; ii < num_thread; ii++) {
    // Configure output streams. Each worker gets separate output streams
    // to avoid collision (single stream coordinated by mutex is too slow).
    std::vector<output_stream_ptr> out_streams;
    if (pimpl->in_memory_output) {
      pimpl->mem_outputs[ii] = map_stream_ptr(new io::MapOutputStream());
      out_streams.push_back(pimpl->mem_outputs[ii]);
    }
    if (pimpl->file_output) {
      out_streams.push_back(std::shared_ptr<io::OutputStream>(
        new io::FileOutputStream(*pimpl->file_output)));
    }
    workers[ii] = algorithm::Worker(start_rank + ii,
                                    total_ranks,
                                    pimpl->tuple_limit,
                                    pimpl->tuple_space,
                                    calc,
                                    out_streams,
                                    pimpl->measure);
    workers[ii].output_all = pimpl->full_output;
  }

  // Start child ranks
  for (int ii = 0; ii < num_thread - 1; ii++) {
    threads[ii] = std::thread(&algorithm::Worker::start, workers[ii]);
  }
  workers.back().start();

  // join other threads
  for (auto& thread : threads) {
    thread.join();
  }

  // cleanup output file stream
  pimpl->in_memory_output = true;
  pimpl->file_output = 0;
  pimpl->outfile = "";
}

#if BOOST_PYTHON_EXTENSIONS
np::ndarray
Search::python_start()
{
  start();
  return python_get_results();
}
#endif
